#include "LibInfo.h"
#include "PMReference.h"

// #include <AWObjectNameWidget.h>
// #include <AWObjectUsesWidget.h>
// #include <AWObjectUsedByWidget.h>
// #include <AWObjectChildrenWidget.h>

#include <DGraphicsSceneProxy.h>

#include "PMModel.h"
#include "PMTableBase.h"
#include "PMColumn.h"
#include "PMKey.h"
#include "PMReferenceColumn.h"

#include "PMDDLWidget.h"
#include "PMReferenceWidget.h"
#include "PMNotesWidget.h"
#include "PMRulesUsedWidget.h"

PMReference::PMReference( PMModel *pParent )
    : PMLine( pParent ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "PMReferenceColumn", ADMeta( QIcon(), "PMReferenceColumn", "Column" ) );
    mapMeta.insert( "PMReferenceColumnDisplay", ADMeta( QIcon(), "PMReferenceColumnDisplay", "ColumnDisplay" ) );

    // Begin floating text
    pointDeltaFloatingTextBegin = QPoint( 15, 15 );
    pFloatingTextBegin = nullptr;

    // End floating text
    pointDeltaFloatingTextEnd = QPoint( 15, 15 );
    pFloatingTextEnd = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMReference::~PMReference()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // DLine does this as well but we need to ensure that our version gets called
    doSourcesDelete();

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
 * \brief Catch case where entire line has moved so we can remove any refs to tables.
 * 
 * \note A similar intervention occurs when an EOL is moved. \sa PMReference::mouseReleaseHandle
 * \note A similar intervention occurs when a PMKey is deleted. 
 *  
 * \author pharvey (5/22/20)
 * 
 * \param pointDelta 
 */
void PMReference::setPosDelta( const QPointF &pointDelta )
{
    PMLine::setPosDelta( pointDelta );

    setKey( nullptr );
}

void PMReference::setMatch( const QString &s )
{
    if ( s == stringMatch ) return;
    stringMatch = s;
    setModified();
}

void PMReference::setOnDelete( const QString &s )
{
    if ( s == stringOnDelete ) return;
    stringOnDelete = s;
    setModified();
}

void PMReference::setOnUpdate( const QString &s )
{
    if ( s == stringOnUpdate ) return;
    stringOnUpdate = s;
    setModified();
}

void PMReference::setVisible( bool b ) 
{ 
    PMLine::setVisible( b );
    pFloatingTextBegin->setVisible( b ); 
    pFloatingTextEnd->setVisible( b ); 
}

void PMReference::setRole( CBD::EOLTypes n, const QString &s)  
{ 
    switch ( n )
    {
    case CBD::EOLBegin:
        if ( stringBeginRole == s ) return;
        stringBeginRole = s;
        pFloatingTextBegin->setPlainText( s );
        break;
    case CBD::EOLEnd:
        if ( stringEndRole == s ) return;
        stringEndRole = s;
        pFloatingTextEnd->setPlainText( s );
        break;
    case CBD::EOLNeither:
        return;
    }
    setModified();
}

void PMReference::setKey( PMKey *pKeyProposed )
{
    PMKey *pKeyExisting = getKey();
    if ( pKeyProposed == pKeyExisting ) return;

    // delete all ReferenceColumn's and remove ref to key
    if ( pKeyExisting )
    { 
        PMTableBase *pTable = (PMTableBase*)pKeyExisting->getParent( "PMTableBase" );
        // catch when PMKey adds/removes a column
        disconnect( pKeyExisting, SIGNAL(signalConnected( ADObject*, ADObject*, const QString &)), this, SLOT(slotKeyColumnConnected( ADObject*, ADObject*, const QString &)) );
        disconnect( pKeyExisting, SIGNAL(signalDisconnected( ADObject*, ADObject*, const QString &)), this, SLOT(slotKeyColumnDisconnected( ADObject*, ADObject*, const QString &)) );
        // catch when parent table deletes a PMKey or a PMColumn because we may be using them 
        disconnect( pTable, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotTableObjectDeleted(ADObject*)) );

        // get rid of the ReferenceColumns
        QList<ADObject*> l = getObjects( QString(), QString(), "PMReferenceColumn" );
        ADObject *pObject;
        foreach( pObject, l )
        {
            pObject->slotDelete();
        }

        ADObject::doDisconnect( pKeyExisting ); // we do not want the DLine ver
    }

    // create a ref?
    if ( pKeyProposed )
    {
        PMTableBase *pTable = (PMTableBase*)pKeyProposed->getParent( "PMTableBase" );
        // create a ref to key (avoiding DLine::doConnect)
        ADObject::doConnect( pKeyProposed );
        // create a ReferenceColumn for each column in the key
        QList<PMColumn*> l = pKeyProposed->getColumns();
        PMColumn *pColumn;
        foreach( pColumn, l )
        {
            PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)getObject( "Column" ); // friendly name so it works with derived classes
            Q_ASSERT(pReferenceColumn);
            pReferenceColumn->setParentColumn( pColumn );
        }
        // catch when PMKey adds/removes a column
        connect( pKeyProposed, SIGNAL(signalConnected( ADObject*, ADObject*, const QString &)), SLOT(slotKeyColumnConnected( ADObject*, ADObject*, const QString &)) );
        connect( pKeyProposed, SIGNAL(signalDisconnected( ADObject*, ADObject*, const QString &)), SLOT(slotKeyColumnDisconnected( ADObject*, ADObject*, const QString &)) );
        // catch when parent table deletes a PMKey or a PMColumn because we may be using them 
        connect( pTable, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotTableObjectDeleted(ADObject*)) );
    }

    setModified();
}

QPixmap PMReference::getIcon()
{
    return QPixmap( ":PM/Reference" );
}

ADObject *PMReference::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "PMReferenceColumn" )
        p = new PMReferenceColumn( this );
    else if ( stringClass == "PMReferenceColumnDisplay" )
        p = new PMReferenceColumnDisplay( this );

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}

AWDataWidget *PMReference::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = PMLine::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMReferenceWidget( this, p ), tr("Reference") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMReference::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Child table missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Parent table missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Child table missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Parent table missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Child table missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Parent table missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMReference", mapStrictness );

    // no child classes to add here so we are done
    return mapClasses;
}

AWPropWidget *PMReference::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = PMLine::getPropWidget( pWidgetParent );

    // remove 'Connector' added by DLine
    // - we can have an arrow on one end (when connected to parent) or nothing at all 
    //   (when connected to a child) and this will be set automatically
    pPropWidget->removeWidget( tr("Connector") );                                                                                 
    {                       
//        LMPropReferenceWidget *p = new LMPropReferenceWidget( (PMReference*)getModelObject(), pPropWidget );
//        pPropWidget->addItem( p, tr("Reference") );                                                            
    }                                                                                                                

    return pPropWidget;

}

PMKey *PMReference::getKey()
{
    QList<ADObjectReferenceKey> l = getUsesKeys( nullptr, QString(), QString(), "PMKey" );
    if ( !l.count() ) return nullptr;
    PMKey *pKey = (PMKey*)(l.first().pObject);
    return pKey;
}

PMReferenceColumn *PMReference::getReferenceColumn( PMColumn *pKeyColumn, bool bCreate )
{
    QList<ADObject*> l = getObjects( QString(), QString(), "PMReferenceColumn" );
    PMReferenceColumn *pReferenceColumn = nullptr;
    ADObject *pObject;
    foreach( pObject, l )
    {
        pReferenceColumn = (PMReferenceColumn*)pObject;
        if ( pReferenceColumn->getParentColumn() == pKeyColumn ) return pReferenceColumn;
    }

    if ( bCreate )
    { 
        pReferenceColumn = (PMReferenceColumn*)getObject( "Column" ); // friendly name so it works with derived classes
        Q_ASSERT(pReferenceColumn);
        pReferenceColumn->setParentColumn( pKeyColumn );
        return pReferenceColumn;
    }

    return nullptr;
}

/*!
    getRole
*/
QString PMReference::getRole( CBD::EOLTypes n )  
{ 
    switch ( n )
    {
    case CBD::EOLBegin:
        return stringBeginRole;
    case CBD::EOLEnd:
        return stringEndRole;
    case CBD::EOLNeither:
        break;
    }

    return QString();
}

/*!
    Return name of table at line end.
*/
QString PMReference::getParentTable()
{
    PMTableBase *ptable = getParentTableObject();

    if ( !ptable )
        return "";

    return ptable->getName();
}

/*!
    Return name of table at line begin.
*/
QString PMReference::getChildTable()
{
    PMTableBase *ptable = getChildTableObject();

    if ( !ptable )
        return "";

    return ptable->getName();
}

/*!
    Return table object at line end.
*/
PMTableBase * PMReference::getParentTableObject()
{
    DRectangle *p = getRectangle( CBD::EOLEnd );
    if ( !p ) return nullptr;

    if ( p->inherits( "PMTableBase" ) ) return (PMTableBase*)p;

    return nullptr;
}

/*!
    Return table object at line begin.
*/
PMTableBase * PMReference::getChildTableObject()
{
    DRectangle *p = getRectangle( CBD::EOLBegin );
    if ( !p ) return nullptr;

    if ( p->inherits( "PMTableBase" ) )
        return (PMTableBase*)p;

    return nullptr;
}

/*!
    getSQLCreate
    
    Generates ALTER TABLE statements for creating foreign key references/constraints. 
*/
QString PMReference::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeCreate() || !pModel->hasFeature( PMModel::FeatureReference, PMModel::FeatureSupportCreate ) ) return "";

    QString         stringSQL       = "";
    PMTableBase *   ptableChild     = getChildTableObject();                                                     
    PMTableBase *   ptableParent    = getParentTableObject();                                                    
                                                                                                                 
    if ( !ptableChild || !ptableParent || getName().isEmpty() )                                                  
        return stringSQL;                                                                                        
                                                                                                                 
    stringSQL += "ALTER TABLE " + ptableChild->getName() + " ADD ";                                              
    stringSQL += "CONSTRAINT " + getName() + "\n  ";                                                             
    stringSQL += "FOREIGN KEY ( ";                                                                               
                                                                                                                 
    QList<ADObject*> l = getObjects( QString(), QString(), "PMReferenceColumn" );
    ADObject *pObject;
    QString stringChildColumns  = "";                                                        
    QString stringParentColumns = "";                                                        
    foreach( pObject, l )
    {
        PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)pObject;
        PMColumn *pColumnParent = pReferenceColumn->getParentColumn();
        PMColumn *pColumnChild  = pReferenceColumn->getChildColumn();
                                                                         
        if ( !stringChildColumns.isEmpty() ) stringChildColumns += ",";
        stringChildColumns += ( pColumnChild ? pColumnChild->getName() : tr( "undefined" ) );
        if ( !stringParentColumns.isEmpty() ) stringParentColumns += ",";
        stringParentColumns += ( pColumnParent ? pColumnParent->getName() : tr( "undefined" ) );                                                
    }                                                                                                            
                                                                                                                 
    stringSQL += stringChildColumns + " )\n";                                                                    
    stringSQL += "   REFERENCES " + ptableParent->getName() + " ( ";                                             
    stringSQL += stringParentColumns + " )";                                                                     
                                                                                                                 
    QString stringOptions;                                                                                       
                                                                                                                 
    if ( !(getMatch().isEmpty()) ) stringOptions += " MATCH " + getMatch();                                                                 
    if ( !(getOnDelete().isEmpty()) ) stringOptions += " ON DELETE " + getOnDelete();                                                          
    if ( !(getOnUpdate().isEmpty()) ) stringOptions += " ON UPDATE " + getOnUpdate();                                                          
    if ( !stringOptions.isEmpty() ) stringSQL += "\n   " + stringOptions;                                                                    
                                                                                                                 
    stringSQL += ";\n";                                                                                          

    return stringSQL;
}

QString PMReference::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureReference, PMModel::FeatureSupportDrop ) ) return "";

    QString         stringSQL       = "";
    PMTableBase *   ptableChild     = getChildTableObject();        
    PMTableBase *   ptableParent    = getParentTableObject();       
                                                                    
    if ( !ptableChild || !ptableParent || getName().isEmpty() )     
        return stringSQL;                                           
                                                                    
    stringSQL += "ALTER TABLE " + ptableChild->getName() + " DROP ";
    stringSQL += "CONSTRAINT " + getName() + ";\n";                 

    return stringSQL;
}

QDomElement PMReference::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = PMLine::doSave( pdomDoc, pdomElemParent );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "FloatingTextBegin.x", pointDeltaFloatingTextBegin.x() );
    domElemThis.setAttribute( "FloatingTextBegin.y", pointDeltaFloatingTextBegin.y() );
    domElemThis.setAttribute( "FloatingTextEnd.x", pointDeltaFloatingTextEnd.x() );
    domElemThis.setAttribute( "FloatingTextEnd.y", pointDeltaFloatingTextEnd.y() );

    if ( !stringMatch.isEmpty() ) domElemThis.setAttribute( "Match", stringMatch );
    if ( !stringOnDelete.isEmpty() ) domElemThis.setAttribute( "OnDelete", stringOnDelete );
    if ( !stringOnUpdate.isEmpty() ) domElemThis.setAttribute( "OnUpdate", stringOnUpdate );

    return domElemThis;
}

bool PMReference::doLoad( QDomElement *pdomElemThis )
{
    PMLine::doLoad( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    stringMatch     = pdomElemThis->attribute( "Match", "" );
    stringOnDelete  = pdomElemThis->attribute( "OnDelete", "" );
    stringOnUpdate  = pdomElemThis->attribute( "OnUpdate", "" );

    pointDeltaFloatingTextBegin.setX( pdomElemThis->attribute( "FloatingTextBegin.x", "15" ).toInt() );
    pointDeltaFloatingTextBegin.setY( pdomElemThis->attribute( "FloatingTextBegin.y", "15" ).toInt() );
    pointDeltaFloatingTextEnd.setX( pdomElemThis->attribute( "FloatingTextEnd.x", "15" ).toInt() );
    pointDeltaFloatingTextEnd.setY( pdomElemThis->attribute( "FloatingTextEnd.y", "15" ).toInt() );

    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin );
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd );

    return true;
}

void PMReference::doPostLoad()
{
    PMLine::doPostLoad();

//    setFloatingText( CBD::EOLBegin );
//    setFloatingText( CBD::EOLEnd );

    // the default load has created the object hierarchy and the refs 
    // now - we take care of knowing when some events occur

    PMKey *pKey = getKey();
    if ( pKey )
    { 
        // catch when PMKey adds/removes a column
        connect( pKey, SIGNAL(signalConnected( ADObject*, ADObject*, const QString &)), SLOT(slotKeyColumnConnected( ADObject*, ADObject*, const QString &)) );
        connect( pKey, SIGNAL(signalDisconnected( ADObject*, ADObject*, const QString &)), SLOT(slotKeyColumnDisconnected( ADObject*, ADObject*, const QString &)) );
    }
    // catch when parent table deletes a PMKey or a PMColumn because we may be using them 
    PMTableBase *pTable = getParentTableObject();
    if ( pTable )
    { 
        connect( pTable, SIGNAL( signalDeleted( ADObject * ) ), this, SLOT( slotTableObjectDeleted( ADObject * ) ) );
    }
}

void PMReference::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nValueType);
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Child Entity?                                                                                                                  
    stringValidation= tr("Child table missing");                                                                        
    nSeverity       = getValidationSeverity( "PMReference", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getChildTableObject() )                                                                                                     
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Parent Entity?                                                                                                                 
    stringValidation= tr("Parent table missing");
    nSeverity       = getValidationSeverity( "PMReference", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getParentTableObject() )                                                                                                     
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    doLoadDefaults
    
    Apply the preferences for this object.
*/    
void PMReference::doLoadDefaults()
{
/*
    CBDSettings         settings;                                              
    QString             stringPath = "/CodeByDesign/LM/Defaults/Reference/";
    QPen                pen;                                                   
    QBrush              brush;                                                 
                                                                               
    pen.setColor( "#0000ff" );                                                 
    brush.setColor( Qt::white );                                               
    brush.setStyle( Qt::SolidPattern );                                        
                                                                               
    pen = settings.readLinePrefEntry( stringPath, pen );                       
    brush = settings.readFillPrefEntry( stringPath, brush);                    
                                                                               
    setPen(pen);                                                               
    setBrush( brush );                                                         
    setDefaultsLoaded( true );                                                 
*/
}

/*!
 * \brief Catch case where EOL has been moved and we may no longer be connected to same table(s).
 *  
 * We disassociate ourself from key/columns if we find that either table is changed. 
 * We could keep some of the info if just the Child end has been changed but we keep it simple 
 * and disassociate ourself from key/columns in both ends.
 *  
 * The refs remain if the EOL is moved from one point on the table to another (the table remains the same). 
 *  
 * \note A similar intervention occurs when the entire line is moved. \sa PMReference::setPosDelta
 * \note A similar intervention occurs when a PMKey is deleted. 
 *  
 * \author pharvey (5/22/20)
 * 
 * \param pHandle 
 * \param pEvent 
 */
void PMReference::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    PMLine::mouseReleaseHandle( pHandle, pointScene );

    // do we have a PMKey to worry about?
    PMKey *pKey = getKey();
    if ( !pKey ) return;

    // has parent table changed?
    // - here we use the keys parent (table) to see what we were using
    PMTableBase *   pTable  = getParentTableObject();           // current table (if any)
    if ( !pTable ) return setKey( nullptr );
    ADObject *      pObject = pKey->getParent( "ADObject" );    // previous table (must be one as we have a key ref)
    Q_ASSERT( pObject );
    if ( pTable != pObject ) return setKey( nullptr );

    // has child table changed?
    // - we have no way to get to the table we were using so we check by looking
    // up the ReferenceColumn in the new table - no match means that we are dealing with
    // a different table
    pTable = getChildTableObject();    // current table (if any)
    if ( !pTable ) return setKey( nullptr );
    QList<ADObject *> listReferenceColumns = getObjects( QString(), QString(), "PMReferenceColumn" );
    if ( listReferenceColumns.count() )
    { 
        PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)listReferenceColumns.first();
        if ( !pTable->getColumns().contains( pReferenceColumn->getChildColumn() ) ) return setKey( nullptr );
    }
}

void PMReference::slotKeyColumnConnected( ADObject *pUser, ADObject *pUsed, const QString &stringName )
{
    Q_UNUSED( stringName );
    if ( !pUser->inherits( "PMKey" ) ) return;
    PMKey *pKey = (PMKey*)pUser;
    if ( !pUsed->inherits( "PMColumn" ) ) return;
    PMColumn *pColumn = (PMColumn*)pUsed;
    Q_ASSERT( pKey == getKey() );

    PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)getObject( "Column" ); // friendly name so it works with derived classes
    Q_ASSERT(pReferenceColumn);
    pReferenceColumn->setParentColumn( pColumn );
}

void PMReference::slotKeyColumnDisconnected( ADObject *pUser, ADObject *pUsed, const QString &stringName )
{
    Q_UNUSED( stringName );
    if ( !pUser->inherits( "PMKey" ) ) return;
    PMKey *pKey = (PMKey*)pUser;
    if ( !pUsed->inherits( "PMColumn" ) ) return;
    PMColumn *pColumn = (PMColumn*)pUsed;
    Q_ASSERT( pKey == getKey() );

    // find referencecolumn and delete it
    QList<ADObject *> listReferenceColumns = getObjects( QString(), QString(), "PMReferenceColumn" );
    ADObject *p;
    foreach( p, listReferenceColumns )
    {
        PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)p;
        if ( pReferenceColumn->getParentColumn() == pColumn )
        {
            pReferenceColumn->slotDelete(); 
            return;
        }
    }
}

void PMReference::slotTableObjectDeleted( ADObject *pObject )
{
    if ( !pObject ) return;

    if ( pObject->inherits( "PMKey" ) )
    {
        PMKey *pKey = (PMKey*)pObject;
        if ( pKey == getKey() ) setKey( nullptr );
        return;
    }

    if ( pObject->inherits( "PMColumn" ) )
    { 
        PMColumn *pColumn = (PMColumn *)pObject;

        QList<ADObject *> listReferenceColumns = getObjects( QString(), QString(), "PMReferenceColumn" );
        ADObject *p;
        foreach( p, listReferenceColumns )
        {
            PMReferenceColumn *pReferenceColumn = (PMReferenceColumn*)p;
            if ( pReferenceColumn->getParentColumn() == pColumn )
            {
                pReferenceColumn->slotDelete(); 
                return;
            }
        }

        return;
    }
}

QPointF PMReference::getFloatingTextBeginPos()
{
    return getProxy()->mapToScene( polygon.first() );
}

QPointF PMReference::getFloatingTextEndPos()
{
    return getProxy()->mapToScene( polygon.last() );
}

void PMReference::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    // Begin floating text handle?
    if ( pHandle->getFunction() == DHandle::FunctionFloatingTextBegin )
    {
        // adjust delta and reposition
        pointDeltaFloatingTextBegin = pointScenePos - getFloatingTextBeginPos();
        pHandle->setPos( pointScenePos );
        pFloatingTextBegin->setPos( pointScenePos );
        return;
    }

    // End floating text handle?
    if ( pHandle->getFunction() == DHandle::FunctionFloatingTextEnd )
    {
        // adjust delta and reposition
        pointDeltaFloatingTextEnd = pointScenePos - getFloatingTextEndPos();
        pHandle->setPos( pointScenePos );
        pFloatingTextEnd->setPos( pointScenePos );
        return;
    }

    // Center floating text handle?
    if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
    {
        // adjust delta and reposition
        pointDeltaFloatingTextCenter = pointScenePos - getFloatingTextCenterPos();
        pHandle->setPos( pointScenePos );
        pFloatingTextCenter->setPos( pointScenePos );
        return;
    }

    // other handles
    {
        int nPoint = pHandle->getIndex();
        polygon[nPoint] = getProxy()->mapFromScene( pointScenePos );
        getProxy()->doPrepareGeometryChange();
        doHandlesSync();
        return;
    }
}

void PMReference::doHandlesCreate()
{
    //
    PMLine::doHandlesCreate();

    DHandle *pHandle;

    // Begin floating text
    pHandle = new DHandle( this, DHandle::FunctionFloatingTextBegin, DHandle::ConnectTypeNone, getFloatingTextBeginPos() + pointDeltaFloatingTextBegin );
    vectorHandles.append( pHandle );
    getProxy()->scene()->addItem( pHandle );

    // End floating text
    pHandle = new DHandle( this, DHandle::FunctionFloatingTextEnd, DHandle::ConnectTypeNone, getFloatingTextEndPos() + pointDeltaFloatingTextEnd );
    vectorHandles.append( pHandle );
    getProxy()->scene()->addItem( pHandle );
}

void PMReference::doHandlesSync()
{
    //
    PMLine::doHandlesSync();

    //
    DHandle *pHandle;
    foreach( pHandle, vectorHandles )
    {
        if ( pHandle->getFunction() == DHandle::FunctionFloatingTextBegin )
        {
           pHandle->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin ); 
           pFloatingTextBegin->setPos( pHandle->pos() );
        }
        else if ( pHandle->getFunction() == DHandle::FunctionFloatingTextEnd )
        {
           pHandle->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd ); 
           pFloatingTextEnd->setPos( pHandle->pos() );
        }
    }
}

void PMReference::doSourcesCreate()
{
    PMLine::doSourcesCreate();

    pFloatingTextCenter->setDefaultTextColor( font.color );

    // Begin floating text
    pFloatingTextBegin = new QGraphicsTextItem( "" );
    pFloatingTextBegin->setDefaultTextColor( font.color );
    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin );
    pFloatingTextBegin->setPlainText( getRole( CBD::EOLBegin ) );
    getDiagram()->getScene()->addItem( pFloatingTextBegin );

    // End floating text
    pFloatingTextEnd = new QGraphicsTextItem( "" );
    pFloatingTextEnd->setDefaultTextColor( font.color );
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd );
    pFloatingTextEnd->setPlainText( getRole( CBD::EOLEnd ) );
    getDiagram()->getScene()->addItem( pFloatingTextEnd );
}

void PMReference::doSourcesDelete()
{
    PMLine::doSourcesDelete();

    if ( pFloatingTextBegin )
    { 
        delete pFloatingTextBegin;
        pFloatingTextBegin = nullptr;
    }

    if ( pFloatingTextEnd )
    { 
        delete pFloatingTextEnd;
        pFloatingTextEnd = nullptr;
    }
}

void PMReference::doSourcesSync()
{
    PMLine::doSourcesSync();

    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin ); 
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd ); 
}


