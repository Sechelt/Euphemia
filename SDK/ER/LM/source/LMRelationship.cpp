#include "LibInfo.h"
#include "LMRelationship.h"

#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>
#include <DGraphicsSceneProxy.h>
#include <DGraphicsProxyItem.h>
#include <DLine.h>

#include "LMRule.h"
#include "LMModel.h" 
#include "LMRelationshipWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"
#include "LMLineWidget.h"

LMRelationship::LMRelationship( LMModel *pParent )
: LMLine( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // 
    bGenerate               = true;
    nDominant               = CBD::EOLNeither;
    ComposedFlagBegin       = ComposedNone;
    ComposedFlagEnd         = ComposedNone;
    stringBeginCardinality  = "0,n";
    stringEndCardinality    = "0,1";

    // we should always be Composed
    nStyleBegin = W::WLineEndComposed; 
    nStyleEnd = W::WLineEndComposed;

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

LMRelationship::~LMRelationship()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // done in DLine but will not call our version...
    // so we call our version here 
    doSourcesDelete();

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMRelationship::setVisible( bool b ) 
{ 
    LMLine::setVisible( b );
    pFloatingTextBegin->setVisible( b ); 
    pFloatingTextEnd->setVisible( b ); 
}


/*!
 * \brief True to Generate a reference in the Physical Model.
 * 
 * \author pharvey (11/27/19)
 * 
 * \param b 
 */
void LMRelationship::setGenerate( bool b )
{
    if ( bGenerate == b ) return;
    bGenerate = b;
    setModified();
}

/*!
 * \brief Indicates which direction to create reference when cardinality is 1,1.
 *  
 * This clarifies case when cardinality is 1-1 and is only useful for this case.  
 * The Dominant Entity will become the Parent Table in a PM. 
 *  
 * The drawing does not show this value. 
 *  
 * \author pharvey (11/27/19)
 * 
 * \param n 
 */
void LMRelationship::setDominant( CBD::EOLTypes n, bool b )
{
    if ( n == CBD::EOLBegin )
    { 
        if ( nDominant == CBD::EOLBegin && b ) return;
        if ( nDominant != CBD::EOLBegin && !b ) return;
        if ( b && (stringBeginCardinality != "1,1" || stringEndCardinality != "1,1" ) )
        {
            doMessage( "WARNING", tr("Setting an Entity as Dominant is only valid when Cardinality is 1-1.") );
            setModified(); // to get UI control set back to original value
            return;
        }
        if ( b ) nDominant = n;
        else nDominant = CBD::EOLNeither;
    }
    else if ( n == CBD::EOLEnd )
    { 
        if ( nDominant == CBD::EOLEnd && b ) return;
        if ( nDominant != CBD::EOLEnd && !b ) return;
        if ( b && (stringBeginCardinality != "1,1" || stringEndCardinality != "1,1" ) )
        {
            doMessage( "WARNING", tr("Setting an Entity as Dominant is only valid when Cardinality is 1-1.") );
            setModified(); // to get UI control set back to original value
            return;
        }
        if ( b ) nDominant = n;
        else nDominant = CBD::EOLNeither;
    }

    setModified();
    getProxy()->update();
}

/*!
 * \brief Indicates Entity is dependent upon the other Entity. Indicates Ordinality.
 *  
 * The drawing will show this as a short perpendicular line - on the OTHER end or 
 * a circle on the OTHER end (indicating Optional).
 *  
 * An Entity that is Identified by other Entity (Triangle on this end) is also Dependent on other Entity (line on other end).
 *  
 * \author pharvey (11/27/19)
 * 
 * \param n 
 */
void LMRelationship::setDependent( CBD::EOLTypes n, bool b )
{
    if ( n == CBD::EOLBegin )
    { 
        if ( b == ComposedFlagEnd.testFlag( ComposedPerpendicular ) ) return;
        if ( !b && ComposedFlagBegin.testFlag( ComposedTriangle ) )
        {
            doMessage( "WARNING", tr("Can not remove Dependent as Identified needs it.") );
            setModified(); // to get UI control set back to original value
            return;
        }
        ComposedFlagEnd.setFlag( ComposedPerpendicular, b );
        setModified();
        getProxy()->update();
    }
    else if ( n == CBD::EOLEnd )
    { 
        if ( b == ComposedFlagBegin.testFlag( ComposedPerpendicular ) ) return;
        if ( !b && ComposedFlagEnd.testFlag( ComposedTriangle ) )
        {
            doMessage( "WARNING", tr("Can not remove Dependent as Identified needs it.") );
            setModified(); // to get UI control set back to original value
            return;
        }
        ComposedFlagBegin.setFlag( ComposedPerpendicular, b );
        setModified();
        getProxy()->update();
    }
}

/*!
 * \brief Indicates that Entity is, at least partially, identified by the other Entity.
 *  
 * Implies Dependence upon other Entity. 
 *  
 * \author pharvey (5/13/20)
 * 
 * \param n 
 */
void LMRelationship::setIdentified( CBD::EOLTypes n, bool b )
{
    if ( n == CBD::EOLBegin )
    { 
        if ( b == ComposedFlagBegin.testFlag( ComposedTriangle ) ) return;
        if ( b && !ComposedFlagEnd.testFlag( ComposedPerpendicular ) )
        {
            ComposedFlagEnd.setFlag( ComposedPerpendicular );
            doMessage( "INFO", tr("Set Dependent as needed by Identified.") );
        }
        ComposedFlagBegin.setFlag( ComposedTriangle, b );
        setModified();
        getProxy()->update();
    }
    else if ( n == CBD::EOLEnd )
    { 
        if ( b == ComposedFlagEnd.testFlag( ComposedTriangle ) ) return;
        if ( b && !ComposedFlagBegin.testFlag( ComposedPerpendicular ) )
        {
            ComposedFlagBegin.setFlag( ComposedPerpendicular );
            doMessage( "INFO", tr("Set Dependent as needed by Identified.") );
        }
        ComposedFlagEnd.setFlag( ComposedTriangle, b );
        setModified();
        getProxy()->update();
    }
}

/*!
 * \brief Sets the Cardinality. 
 *  
 * - s should be in form of min,max where values for these must be >=0 or 'n' 
 * - ordinality should be = Optional if min = 0 
 * - will be appended to the Role text when displayed in diagram
 *  
 * \author pharvey (11/27/19)
 * 
 * \param n 
 * \param s 
 */
void LMRelationship::setCardinality( CBD::EOLTypes n, const QString &s )  
{ 
    if ( n == CBD::EOLNeither ) return;

    QStringList listCardinality  = s.split( ',' );
    if ( listCardinality.count() != 2 ) return;

    if ( n == CBD::EOLBegin )
    { 
        if ( listCardinality.at( 1 ) == "0" || listCardinality.at( 1 ) == "1" ) ComposedFlagBegin.setFlag( ComposedCrowsFoot, false );
        else ComposedFlagBegin.setFlag( ComposedCrowsFoot );
        stringBeginCardinality = s;
        setFloatingText( n );
        setModified();
        getProxy()->update();
    }
    else if ( n == CBD::EOLEnd )
    { 
        if ( listCardinality.at( 1 ) == "0" || listCardinality.at( 1 ) == "1" ) ComposedFlagEnd.setFlag( ComposedCrowsFoot, false );
        else ComposedFlagEnd.setFlag( ComposedCrowsFoot );
        stringEndCardinality = s; 
        setFloatingText( n );
        setModified();
        getProxy()->update();
    }
}

void LMRelationship::setRole( CBD::EOLTypes n, const QString &s)  
{ 
    if ( n == CBD::EOLBegin )
    { 
        if ( stringBeginRole == s ) return;
        stringBeginRole = s;
        setFloatingText( n );
        setModified();
    }
    else if ( n == CBD::EOLEnd )
    { 
        if ( stringEndRole == s ) return;
        stringEndRole = s;
        setFloatingText( n );
        setModified();
    }
}

/*!
    Return a 16x16 icon for use in the browser etc.
*/
QPixmap LMRelationship::getIcon()
{
    return QPixmap( ":LM/Relationship16x16" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties. 
 *  
 * We replace DLineWidget with LMLineWidget because DLineWidget allows edit of 'Connector' info 
 * with no regard for our rules.
 *  
 * \author pharvey (5/12/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *LMRelationship::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMLine::getObjectWidget( pWidgetParent );

    // replace DLineWidget with LMLineWidget
    p->removeByClass( "DLineWidget" );
    p->insertTab( 1, new LMLineWidget( this, p ), tr("Line") );
        
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->insertTab( 1, pWidget, tr("Notes") );
    }
    p->insertTab( 1, new LMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new LMRelationshipWidget( this, Qt::Horizontal, p ), tr("Relationship") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *LMRelationship::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = LMLine::getPropWidget( pWidgetParent );

    // replace 'Connector' added by DLine with 'Relationship'
    pPropWidget->removeWidget( tr("Connector") );                                                                                 
    pPropWidget->addWidget( tr("Relationship"), new LMRelationshipWidget( this, Qt::Vertical, pPropWidget ) );                                                            

    return pPropWidget;
}

bool LMRelationship::isDominant( CBD::EOLTypes n )
{
    return n == nDominant;
}

/*!
 * \brief Is Entity Dependent on other Entity.
 *  
 * Checks other end for ComposedPerpendicular (draw perpendicular line).
 *  
 * \author pharvey (5/13/20)
 * 
 * \param n 
 * 
 * \return bool 
 */
bool LMRelationship::isDependent( CBD::EOLTypes n )
{
    if ( n == CBD::EOLBegin ) return ComposedFlagEnd.testFlag( ComposedPerpendicular );
    return ComposedFlagBegin.testFlag( ComposedPerpendicular );
}

bool LMRelationship::isIdentified( CBD::EOLTypes n )
{
    if ( n == CBD::EOLBegin ) return ComposedFlagBegin.testFlag( ComposedTriangle );
    return ComposedFlagEnd.testFlag( ComposedTriangle );
}

/*!
    getCardinality
    
    Return Cardinality.
    
    example; "0,n"
*/

QString LMRelationship::getCardinality( CBD::EOLTypes n )  
{ 
    switch ( n )
    {
    case CBD::EOLBegin:
        return stringBeginCardinality; 
    case CBD::EOLEnd:
        return stringEndCardinality; 
    case CBD::EOLNeither:
        break;
    }

    return QString();
}

/*!
    getRole
*/
QString LMRelationship::getRole( CBD::EOLTypes n )  
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
    Return name of entity at line end.
*/
QString LMRelationship::getParentEntity()
{
    LMEntityBase *pentity = getParentEntityObject();

    if ( !pentity ) return "";

    return pentity->getName();
}

/*!
    Return name of entity at line begin.
*/
QString LMRelationship::getChildEntity()
{
    LMEntityBase *pentity = getChildEntityObject();

    if ( !pentity ) return "";

    return pentity->getName();
}

/*!
    Return entity object at line end.
*/
LMEntityBase * LMRelationship::getParentEntityObject()
{
    return (LMEntityBase*)getConnected( "End", QString(), "LMEntityBase" );
}

/*!
    Return entity object at line begin.
*/
LMEntityBase * LMRelationship::getChildEntityObject()
{
    return (LMEntityBase*)getConnected( "Begin", QString(), "LMEntityBase" );
}

ADValidationDatabase LMRelationship::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Child entity missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Parent entity missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Child entity missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Parent entity missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Child entity missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Parent entity missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "LMRelationship", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

/*!
    doSave
    
    Save ourself to the given XML.
*/    
QDomElement LMRelationship::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DLine::doSave( pdomDoc, pdomElemParent );

    doSaveLMObject( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "Generate", QString::number( isGenerate() ) );
    domElemThis.setAttribute( "Dominant", QString::number( nDominant ) );
    if ( !getCardinality( CBD::EOLBegin ).isEmpty() ) domElemThis.setAttribute( "BeginCardinality", getCardinality( CBD::EOLBegin ) );
    if ( !getCardinality( CBD::EOLEnd ).isEmpty() )   domElemThis.setAttribute( "EndCardinality", getCardinality( CBD::EOLEnd ) );
    if ( !getRole( CBD::EOLBegin ).isEmpty() )        domElemThis.setAttribute( "BeginRole", getRole( CBD::EOLBegin ) );
    if ( !getRole( CBD::EOLEnd ).isEmpty() )          domElemThis.setAttribute( "EndRole", getRole( CBD::EOLEnd ) );

    domElemThis.setAttribute( "BeginFlag", QString::number( int(ComposedFlagBegin) ) );
    domElemThis.setAttribute( "EndFlag", QString::number( int(ComposedFlagEnd) ) );

    domElemThis.setAttribute( "FloatingTextBegin.x", pointDeltaFloatingTextBegin.x() );
    domElemThis.setAttribute( "FloatingTextBegin.y", pointDeltaFloatingTextBegin.y() );
    domElemThis.setAttribute( "FloatingTextEnd.x", pointDeltaFloatingTextEnd.x() );
    domElemThis.setAttribute( "FloatingTextEnd.y", pointDeltaFloatingTextEnd.y() );

    return domElemThis;
}

/*!
    doLoad
    
    Load ourself from the given XML.
*/    
bool LMRelationship::doLoad( QDomElement *pdomElemThis )
{
    DLine::doLoad( pdomElemThis );

    doLoadLMObject( pdomElemThis );

    bGenerate               = pdomElemThis->attribute( "Generate", QString::number( bGenerate ) ).toInt();

    nDominant               = (CBD::EOLTypes)pdomElemThis->attribute( "Dominant", QString::number( nDominant ) ).toInt();

    ComposedFlagBegin       = (ComposedFlags)pdomElemThis->attribute( "BeginFlag", QString::number( int(ComposedFlagBegin)) ).toInt();
    ComposedFlagEnd         = (ComposedFlags)pdomElemThis->attribute( "EndFlag", QString::number( int(ComposedFlagEnd)) ).toInt();

    stringBeginCardinality  = pdomElemThis->attribute( "BeginCardinality" );
    stringEndCardinality    = pdomElemThis->attribute( "EndCardinality" );
    stringBeginRole         = pdomElemThis->attribute( "BeginRole" );
    stringEndRole           = pdomElemThis->attribute( "EndRole" );

    pointDeltaFloatingTextBegin.setX( pdomElemThis->attribute( "FloatingTextBegin.x", "15" ).toDouble() );
    pointDeltaFloatingTextBegin.setY( pdomElemThis->attribute( "FloatingTextBegin.y", "15" ).toDouble() );
    pointDeltaFloatingTextEnd.setX( pdomElemThis->attribute( "FloatingTextEnd.x", "15" ).toDouble() );
    pointDeltaFloatingTextEnd.setY( pdomElemThis->attribute( "FloatingTextEnd.y", "15" ).toDouble() );

    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin );
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd );

    return true;
}

/*!
 * \brief Validate ourself.
 * 
 * \author pharvey (11/29/19)
 * 
 * \param nStrictness 
 * \param listObjectPathItems 
 */
void LMRelationship::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;
                                                                                                                                      
    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         
                                                                                                                                      
    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Duplicate...                                                                                                                 
    stringValidation = tr("Name duplicate");                                                                         
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && getDuplicate( this, ADObject::FieldName ) )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    stringValidation = tr("Code duplicate");                                                                         
    nSeverity = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && getDuplicate( this, ADObject::FieldCode ) )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Child Entity?                                                                                                                  
    stringValidation= tr("Child entity missing");                                                                        
    nSeverity       = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getChildEntityObject() )                                                                                                     
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Parent Entity?                                                                                                                 
    stringValidation= tr("Parent entity missing");
    nSeverity       = getValidationSeverity( "LMRelationship", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getParentEntityObject() )                                                                                                     
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    doLoadDefaults
    
    Apply the preferences for this object.
*/    
void LMRelationship::doLoadDefaults()
{
/*
    CBDSettings         settings;                                              
    QString             stringPath = "/CodeByDesign/LM/Defaults/Relationship/";
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

void LMRelationship::doPostLoad()
{
    LMLine::doPostLoad();

    setFloatingText( CBD::EOLBegin );
    setFloatingText( CBD::EOLEnd );
}

void LMRelationship::setFloatingText( CBD::EOLTypes n )
{
    if ( n == CBD::EOLBegin )
    {
        pFloatingTextBegin->setPlainText( getRole( CBD::EOLBegin ) + "\n" + getCardinality( CBD::EOLBegin ) );
    }
    else if ( n == CBD::EOLEnd )
    {
        pFloatingTextEnd->setPlainText( getRole( CBD::EOLEnd ) + "\n" + getCardinality( CBD::EOLEnd ) );
    }
}

QPointF LMRelationship::getFloatingTextBeginPos()
{
    return getProxy()->mapToScene( polygon.first() );
}

QPointF LMRelationship::getFloatingTextEndPos()
{
    return getProxy()->mapToScene( polygon.last() );
}

void LMRelationship::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
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

void LMRelationship::doHandlesCreate()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    LMLine::doHandlesCreate();

    // last handle is a mid-line floating text handle
    // here we add two more floating text handles to the end of the list
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

void LMRelationship::doHandlesSync()
{
    //
    LMLine::doHandlesSync();

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

void LMRelationship::doSourcesCreate()
{
    LMLine::doSourcesCreate();

    pFloatingTextCenter->setDefaultTextColor( font.color );

    // Begin floating text
    pFloatingTextBegin = new QGraphicsTextItem( "" );
    pFloatingTextBegin->setDefaultTextColor( font.color );
    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin );
    setFloatingText( CBD::EOLBegin );
    getDiagram()->getScene()->addItem( pFloatingTextBegin );

    // End floating text
    pFloatingTextEnd = new QGraphicsTextItem( "" );
    pFloatingTextEnd->setDefaultTextColor( font.color );
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd );
    setFloatingText( CBD::EOLEnd );
    getDiagram()->getScene()->addItem( pFloatingTextEnd );
}

void LMRelationship::doSourcesDelete()
{
    LMLine::doSourcesDelete();

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

void LMRelationship::doSourcesSync()
{
    LMLine::doSourcesSync();

    pFloatingTextBegin->setPos( getFloatingTextBeginPos() + pointDeltaFloatingTextBegin ); 
    pFloatingTextEnd->setPos( getFloatingTextEndPos() + pointDeltaFloatingTextEnd ); 
}

void LMRelationship::doDrawBegin( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected )
{
    W::WLineEndStyles nStyle = getStyle( CBD::EOLBegin );

    switch ( nStyle )
    {
        case W::WLineEndArrow:
            doDrawArrow( pPainter, lineSegment );
            break;
        case W::WLineEndBall:
            doDrawBall( pPainter, lineSegment );
            break;
        case W::WLineEndComposed:
        {
            QPointF pointArrowTip( lineSegment.p1() );
            QPointF vectArrow = - lineSegment.p2() + pointArrowTip;

            doDrawComposedArrow( pPainter, 
                                 pointArrowTip.x(), pointArrowTip.y(), 
                                 vectArrow.x()/2, vectArrow.y()/2, 
                                 lineIntersected.p1().x(), lineIntersected.p1().y(),    // p1 of shape line we intersect
                                 lineIntersected.p2().x(), lineIntersected.p2().y(),    // p2 of shape line we intersect
                                 1.0f, 
                                 ComposedFlagBegin );
        }
            break;
        case W::WLineEndNone:
            break;
    };
}

void LMRelationship::doDrawEnd( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected )
{
    W::WLineEndStyles nStyle = getStyle( CBD::EOLEnd );

    switch ( nStyle )
    {
        case W::WLineEndArrow:
            doDrawArrow( pPainter, lineSegment );
            break;
        case W::WLineEndBall:
            doDrawBall( pPainter, lineSegment );
            break;
        case W::WLineEndComposed:
        {
            QPointF pointArrowTip( lineSegment.p1() );
            QPointF vectArrow = - lineSegment.p2() + pointArrowTip ;

            doDrawComposedArrow( pPainter, 
                                 pointArrowTip.x(), 
                                 pointArrowTip.y(), 
                                 vectArrow.x()/2, 
                                 vectArrow.y()/2, 
                                 lineIntersected.p1().x(), lineIntersected.p1().y(),  // p1 of shape line we intersect
                                 lineIntersected.p2().x(), lineIntersected.p2().y(),    // p2 of shape line we intersect
                                 1.0f, 
                                 ComposedFlagEnd );
        }
            break;
        case W::WLineEndNone:
            break;
    };
}

/*!
    Draw arrows with given parameters
 */
void LMRelationship::doDrawComposedArrow( QPainter *ppainter, float x, float y, float vecX, float vecY, float borderX1, float borderY1, float borderX2, float borderY2, float scale, ComposedFlags Flag, bool withLines ) 
{
    float m_l = 12.0f; // length
    float m_d = 9.0f; // circle diameter
    float length;
    float nvecX, nvecY; // normalized vectors
    double angle;
    float delta;
    int pointCount = 0, vecCount = 0;
    float _x;
    float _y;
    float savex = x;
    float savey = y;
 //   struct  vector * vectors;
 

    vecX = -vecX;
    vecY = -vecY;

    length = sqrt( vecX*vecX + vecY*vecY );

    if(!length)
        return;

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    // calculates angle of rotation
    if (length)
    {
        double Sin = vecY / length;
        double Cos = vecX / length;
        angle = asin(Sin);
        if (Cos < 0)
            angle = 3.14 - angle;
        else if (Sin < 0)
            angle = 2*3.14 + angle;
    }
    else
        angle = 0;

    angle = angle*180/M_PI;

    // Cardinality
    // - draw crows foot as needed 
    if ( Flag.testFlag( ComposedCrowsFoot ) )
    {
        // calculating two 'base' points on rectangle edge
        float x1, y1, x2, y2;

        // first point

        // (_x, _y) - vecotr which lies on edge
        _x = borderX1 - x;
        _y = borderY1 - y;
        length = sqrt(_x*_x + _y*_y);
        if (length)
        {
            // (x1, y1) - our first point.
            // dist. between (x, y) and (x1, y1) = 0.6*m_l
            x1 = (_x * m_l * 0.6)/length + x;
            y1 = (_y * m_l * 0.6)/length + y;
        }
        else
        {
            x1 = x;
            y1 = y;
        }

        // second point

        _x = borderX2 - x;
        _y = borderY2 - y;

        length = sqrt(_x*_x + _y*_y);
        if (length)
        {
            x2 = (_x * m_l * 0.6)/length + x;
            y2 = (_y * m_l * 0.6)/length + y;
        }
        else
        {
            x2 = x;
            y2 = y;
        }

        // delta is difference in length between left & right lines 
        // in normal(90 deg) case & other angles

        // vertical
        if (borderX1 == borderX2)
        {
            delta = 0.6*m_l*sin((360-angle) / 180 * 3.14);
            if (((360-angle) < 90) || ((360-angle) > 270))
                delta = -delta;
        }
        else
        {
            delta = 0.6*m_l*cos((360-angle) / 180 * 3.14);
            if ((360-angle) >= 180)
                delta = -delta;
        }

        // Many + Identified
        // draw triangle
        if ( Flag.testFlag( ComposedTriangle ) )
        {
            // two parallel lines with length adj.
            _x = - nvecX * (2 * m_l + delta);
            _y = - nvecY * (2 * m_l + delta);
            _x += x1;
            _y += y1;

            ppainter->drawLine( (int)(x1 * scale), (int)(y1*scale), (int)(_x * scale), (int)(_y * scale));

            _x = - nvecX * (2 * m_l - delta);
            _y = - nvecY * (2 * m_l - delta);
            _x += x2;
            _y += y2;

            ppainter->drawLine( (int)(x2 * scale), (int)(y2*scale), (int)(_x * scale), (int)(_y * scale));

            x -= nvecX * 2 * m_l;
            y -= nvecY * 2 * m_l;
        }
        else
        {
            // Crow feets
            _x = - nvecX * 2 *m_l;
            _y = - nvecY * 2 *m_l;
            _x += x;
            _y += y;
            ppainter->drawLine( (int)(x1 * scale), (int)(y1*scale), (int)(_x * scale), (int)(_y * scale));
            ppainter->drawLine( (int)(x2 * scale), (int)(y2*scale), (int)(_x * scale), (int)(_y * scale));
            x -= nvecX * 2.2 * m_l;
            y -= nvecY * 2.2 * m_l;
        }
    }
    else
    {
        x -= nvecX * 2 * m_l;
        y -= nvecY * 2 * m_l;
    }

    // Identified
    // - draws triangle
    if ( Flag.testFlag( ComposedTriangle ) )
    {
        QVector<QLineF> vectorTriangleLines;
        vectorTriangleLines.append( QLineF( 0, 1, 0, -1 ) );
        vectorTriangleLines.append( QLineF( 0, 1, 1.5, 0 ) );
        vectorTriangleLines.append( QLineF( 0, -1, 1.5, 0 ) );
        vecCount = 3;
        pointCount = 0;
        // struct vector * vectors = triangle_vectors;
        QPolygonF polygonArrow(vecCount * 2);
        // rotate triangle
        for (int i = 0;i<vecCount;i++)
        {
            doRotate( vectorTriangleLines[i].p1().x() * m_l, vectorTriangleLines[i].p1().y() * m_l, &_x, &_y, angle);
            _x += x;
            _y += y;
            polygonArrow[pointCount++] = QPointF( (_x*scale), (_y*scale) );
            doRotate( vectorTriangleLines[i].p2().x() * m_l, vectorTriangleLines[i].p2().y() * m_l, &_x, &_y, angle);
            _x += x;
            _y += y;
            polygonArrow[pointCount++] = QPointF( (_x*scale), (_y*scale) );
        }

        if( withLines )
        {
            QPointF center = (polygonArrow.at(0)+polygonArrow.at(1))/2;
            QPointF top = polygonArrow.at(3);

            ppainter->drawLine(top, QPointF( ((savex+vecX)*scale), ((savey+vecY)*scale)));
            ppainter->drawLine( QPointF( (savex*scale), (savey*scale)), center);
        }


        ppainter->drawPolygon(polygonArrow);

        if ( Flag.testFlag( ComposedPerpendicular ) )
        {
            x -= nvecX * (1.5 * m_l + m_d / 2);
            y -= nvecY * (1.5 * m_l + m_d / 2);
        }
        else
        {
            x -= nvecX * 1.8 * m_l;
            y -= nvecY * 1.8 * m_l;
        }
    } 
    else if ( withLines )
    {
            ppainter->drawLine((int)savex, (int)savey, (int)(savex+vecX), (int)(savey+vecY));
    }

    // Ordinality
    // Mandatory/Dependent - striked line 
    // Optional - circle
    if ( Flag.testFlag( ComposedPerpendicular ) )
    {
        QLineF lineStriked( QPointF( 0, 0.7 ), QPointF( 0, -0.7 ) );

        pointCount = 0;
        QPolygonF polygonArrow(2);

        doRotate( lineStriked.p1().x() * m_l, lineStriked.p1().y() * m_l, &_x, &_y, angle);
        _x += x;
        _y += y;
        polygonArrow[pointCount++] = QPointF( (_x*scale), (_y*scale) );
        doRotate( lineStriked.p2().x() * m_l, lineStriked.p2().y() * m_l, &_x, &_y, angle);
        _x += x;
        _y += y;
        polygonArrow[pointCount++] = QPointF( (_x*scale), (_y*scale) );
        
        ppainter->drawPolygon( polygonArrow );
    }
    else
    {
        _x = x - m_d / 2;
        _y = y - m_d / 2;
        ppainter->drawEllipse((int)(_x*scale), (int)(_y*scale), (int)(m_d*scale), (int)(m_d*scale));
    }

}

void LMRelationship::doRotate(float x, float y, float * _x, float * _y, float angle )
{
    // convert to radians
    float rad = (3.14*angle)/180.;
//    float rad = (M_PI*angle)/180.;
    * _x = cos(rad)*x - sin(rad)*y;
    * _y = sin(rad)*x + cos(rad)*y;
}


