#include "LibInfo.h"
#include "PMModel.h"

#include <DATAProfiles.h>
#include <DATAProfile.h>
#include <DATAProfileWidget.h>

#include <DATAWConnection.h>

#include <DDiagramEditorWidget.h>
#include <DGraphicsView.h>
#include <DGraphicsSceneProxy.h>
#include <DCallOut.h>
#include <DTitle.h>
#include <DTitleDoc.h>

#include <LMModel.h>
#include <LMDomain.h>
#include <LMEntityBase.h>
#include <LMEntityNormal.h>
#include <LMEntityAssociative.h>
#include <LMRelationship.h>
#include <LMDataItem.h>
#include <LMAttribute.h>
#include <LMIdentifier.h>

#include "PMTableNormal.h"
#include "PMView.h"
#include "PMViewColumn.h"
#include "PMReference.h"
#include "PMReferenceColumn.h"
#include "PMTableDerived.h"
#include "PMInherit.h"
#include "PMProcedure.h"
#include "PMDomain.h"
#include "PMSequence.h"
#include "PMRule.h"
#include "PMColumn.h"
#include "PMKey.h"
#include "PMIndex.h"
#include "PMIndexColumn.h"
#include "PMDDLWidget.h"
#include "PMNotesWidget.h"
#include "PMModelDisplayWidget.h"

PMModel::PMModel( ADObject *pParent, const QString &stringName )
    : DDiagram( pParent, stringName ), PMObject( this ), PMDDLManager( this )
{
    mapMeta.clear();

    // mark-up stuff
    mapMeta.insert( "DTitle", ADMeta( QIcon(), "DTitle", "Title" ) );
    mapMeta.insert( "DTitleDoc", ADMeta( QIcon(), "DTitleDoc", "Title Doc" ) );
    mapMeta.insert( "DCallOut", ADMeta( QIcon(), "DCallOut", "Call Out" ) );

    nDisplayFlags = 0;
    nDisplayFlags |= DisplayDataType;
    nDisplayFlags |= (int)DisplayDomain;
    nDisplayFlags |= (int)DisplayParams;
    nDisplayFlags |= (int)DisplayDefault;
    nDisplayFlags |= (int)DisplayNull;

    slotRuler( false );
    setModified( false );

    pProfile        = nullptr;
    pSystem         = nullptr;       
    pEnvironment    = nullptr;  
    pConnection     = nullptr;   
}

PMModel::~PMModel()
{
    if ( pConnection )
    {
        if ( pConnection->isConnected() ) pConnection->doDisconnect();
        delete pConnection; pConnection = nullptr;
    }
    if ( pEnvironment )
    {
        delete pEnvironment; pEnvironment = nullptr;
    }
    if ( pSystem )
    {
        delete pSystem; pSystem = nullptr;
    }
    
    g_Profiles->doSave();
}

void PMModel::setDisplayFlag( int nFlag )
{ 
    this->nDisplayFlags |= (int)nFlag; 
}

QPixmap PMModel::getIcon()
{
    return QPixmap( ":PM/PhysicalModel" );
}

QString PMModel::getCreateAuto()
{
    QString stringCreate;
    QList<ADObject*> l;
    ADObject *pObject;

    // SEQUENCES
    l = getObjects( QString(), QString(), "PMSequence" );
    foreach( pObject, l )
    {
        PMSequence *p = (PMSequence*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    // DOMAINS
    l = getObjects( QString(), QString(), "PMDomain" );
    foreach( pObject, l )
    {
        PMDomain *p = (PMDomain*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    // TABLES
    l = getObjects( QString(), QString(), "PMTableNormal" );
    foreach( pObject, l )
    {
        PMTableNormal *p = (PMTableNormal*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    // REFERENCES
    l = getObjects( QString(), QString(), "PMReference" );
    foreach( pObject, l )
    {
        PMReference *p = (PMReference*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    // VIEWS
    l = getObjects( QString(), QString(), "PMView" );
    foreach( pObject, l )
    {
        PMView *p = (PMView*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    // PROCEDURES
    l = getObjects( QString(), QString(), "PMProcedure" );
    foreach( pObject, l )
    {
        PMProcedure *p = (PMProcedure*)pObject;
        if ( p->isIncludeCreate() )
        {
            if ( p->isCreateManual() ) stringCreate += p->getCreateManual();
            else stringCreate += p->getCreateAuto();
        }
    }

    return stringCreate;
}

QString PMModel::getDropAuto()
{
    QString stringDrop;
    QList<ADObject*> l;
    ADObject *pObject;

    // PROCEDURES
    l = getObjects( QString(), QString(), "PMProcedure" );
    foreach( pObject, l )
    {
        PMProcedure *p = (PMProcedure*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    // VIEWS
    l = getObjects( QString(), QString(), "PMView" );
    foreach( pObject, l )
    {
        PMView *p = (PMView*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    // REFERENCES
    l = getObjects( QString(), QString(), "PMReference" );
    foreach( pObject, l )
    {
        PMReference *p = (PMReference*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    // TABLES
    l = getObjects( QString(), QString(), "PMTableNormal" );
    foreach( pObject, l )
    {
        PMTableNormal *p = (PMTableNormal*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    // DOMAINS
    l = getObjects( QString(), QString(), "PMDomain" );
    foreach( pObject, l )
    {
        PMDomain *p = (PMDomain*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    // SEQUENCES
    l = getObjects( QString(), QString(), "PMSequence" );
    foreach( pObject, l )
    {
        PMSequence *p = (PMSequence*)pObject;
        if ( p->isIncludeDrop() )
        {
            if ( p->isDropManual() ) stringDrop += p->getDropManual();
            else stringDrop += p->getDropAuto();
        }
    }

    return stringDrop;
}

/*!
 * \brief Create objects which are likley to be common to all derived classes.
 *  
 * This is just mark-up classes. 
 *  
 * Derived classes can call here in their getObject() when they do not know the class. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param s 
 * 
 * \return ADObject* 
 */
ADObject *PMModel::getObject( const QString &s, ADObject * )
{
    // we do not support grouping or layout here
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DCallOut" )
    {
        p = new DCallOut( this );
        p->setFolder( tr( "MarkUp" ) );
    }
    else if ( stringClass == "DTitle" )
    {
        p = new DTitle( this );
        p->setFolder( tr( "MarkUp" ) );
    }
    else if ( stringClass == "DTitleDoc" )
    {
        p = new DTitleDoc( this );
        p->setFolder( tr( "MarkUp" ) );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    if ( p->inherits( "DObject" ) )
    {
        DObject *pObject = (DObject*)p; 
        DGraphicsProxyItem *pProxy = new DGraphicsProxyObject( pObject );
        pProxy->doCreateGraphicsObject();
        pProxy->setScene( pScene );
        pObject->setProxy( pProxy );
        pObject->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) ); // to pos when created by AWFactoryWidget
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

   return p;
}

/*!
 * \brief Load validation configuration defaults. 
 *  
 * This is one of two ways to load the validation configuration. The 
 * other way is from XML when loading the model. The latter may be 
 * a User configured version. 
 * 
 * \author pharvey (11/29/19)
 */
void PMModel::getValidationConfiguration()
{
    mapValidationConfiguration.clear();
    getValidationConfiguration( mapValidationConfiguration );
}

/*!
 * \brief Load our validation data. 
 *  
 * Each class has a static method that is called to load its validation data. 
 * This does not do the validation just build the data representing all of the 
 * validations we can do. 
 *  
 * The validation level can then be configured. 
 *  
 * Each class will then access this data during a validation to determine what 
 * kind of a message (if any) to emit for a failed validation. 
 *  
 * \note This is maintained in the diagram as the diagram is the top level for 
 * running a validation. This may be a 'model' in the future - which will run 
 * the validation for all its diagrams. 
 * 
 * \author pharvey (11/28/19)
 * 
 * \param mapClasses 
 */
ADValidationDatabase PMModel::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMModel", mapStrictness );

    // Top Level                                                                     
   PMTableNormal::getValidationConfiguration( mapClasses );             
   PMView::getValidationConfiguration( mapClasses );           
   PMReference::getValidationConfiguration( mapClasses );         
   PMTableDerived::getValidationConfiguration( mapClasses );     
   PMInherit::getValidationConfiguration( mapClasses );         
   PMProcedure::getValidationConfiguration( mapClasses );     
   PMDomain::getValidationConfiguration( mapClasses );         
   PMSequence::getValidationConfiguration( mapClasses );         
//   PMModel::getValidationConfiguration( mapClasses );     
   return mapClasses;
}

AWClassWidget *PMModel::getClassWidget( QWidget *pWidgetParent )
{
    AWClassWidget *p = DDiagram::getClassWidget( pWidgetParent );

    p->addTab( new DATAProfileWidget( pProfile, p ), tr("Data Source Profile") );

    return p;
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * Here, we introduce; some display options and a preview for create/drop SQL. 
 *  
 * \sa PMModelDisplayWidget 
 * \sa PMPreviewWidget 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *PMModel::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DDiagram::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new PMModelDisplayWidget( this, p ), tr("Display") );
    p->insertTab( 1, new DATAProfileWidget( pProfile, p ), tr("Data Source Profile") );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );

    p->setCurrentIndex( 0 );

    return p;
}

QList<QString> PMModel::getTypeNames() 
{ 
    QList<QString> listDataTypes;

    foreach( DATADataTypeSpec *p, pProfile->vectorDataTypes )
    {
        listDataTypes << p->TYPE_NAME;
    }

    return listDataTypes;
}

/*!
 * \brief Get DATADataTypeSpec for the given TYPE_NAME.
 * 
 * Here we use TYPE_NAME instead of DATA_TYPE.
 * Its ok *in this case* as we do not need to worry about inconsistent values between SQLGetTypeInfo and SQLColumns - we are not using SQLColumns.
 * 
 * \author pharvey (2025-11-12)
 * 
 * \param stringTYPE_NAME 
 * 
 * \return DATADataTypeSpec* 
 */
DATADataTypeSpec *PMModel::getDataTypeSpec( const QString &stringTYPE_NAME ) 
{ 
    Q_ASSERT( pProfile );

    foreach( DATADataTypeSpec *p, pProfile->vectorDataTypes )
    {
        if ( p->TYPE_NAME == stringTYPE_NAME ) return p;
    }

    return nullptr;
}

/*!
    Return list of Domain names.
    
    Good for validation and pick lists.
*/
QStringList PMModel::getDomains()
{
    QStringList         listNames;
    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "PMDomain" );
    ADObject *          pObject;

    foreach( pObject, listDomains )
    {
        listNames += pObject->getName();
    }

    return listNames;
}

/*!
    Return the Domain object for the given name.
*/
PMDomain * PMModel::getDomain( const QString str, const bool cs )
{
    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "PMDomain" );
    ADObject *          pObject;

    foreach( pObject, listDomains )
    {
        PMDomain *pDomain = (PMDomain*)pObject;
        if ( cs )
        {
            if ( str == pDomain->getName() ) return pDomain;
        }
        else
        {
            if ( str.toLower() == pDomain->getName().toLower() ) return pDomain;
        }
    }

    return nullptr;
}

/*!
    getIsStandardDataType
    
    Returns true if the given data type is a standard data type for a PMModel.
*/
bool PMModel::getIsStandardDataType( const QString &stringTYPE_NAME )
{
    foreach( DATADataTypeSpec *p, pProfile->vectorDataTypes )
    {
        if ( p->TYPE_NAME == stringTYPE_NAME ) return true;
    }
    return false;
}

/*!
    getIsDomainDataType
    
    Returns true if the given Domain name exists.
*/    
bool PMModel::getIsDomainDataType( const QString &str )
{
    if ( getIsStandardDataType( str ) ) return false;

    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "PMDomain" );
    ADObject *          pObject;
    QString             stringName = str.toLower();

    foreach( pObject, listDomains )
    {
        if ( stringName == pObject->getName().toLower() )
            return true;
    }

    return false;
}

bool PMModel::isDataConnected()
{
    if ( !pConnection ) return false;
    return pConnection->isConnected();
}


/*!
 * \brief Creates a Logical Model from a Physical Model.
 *  
 * Uses 'spoke-and-hub' paradigm. 
 * The LM is the 'hub' and the PM's are the 'spokes' 
 * The LM does not know about PM's but each PM must know how to save/load a LM. 
 *  
 * \author pharvey (5/26/20)
 * 
 * \param pModel 
 */
void PMModel::doSave( LMModel *pModel )
{
    doSaveDomains( pModel );
    doSaveTables( pModel );
    doSaveReferences( pModel );
    pModel->doEnsurePages();
}


/*!
 * \brief Create a Physical Model from a Logical Model.
 *  
 * Uses 'spoke-and-hub' paradigm. 
 * The LM is the 'hub' and the PM's are the 'spokes' 
 * The LM does not know about PM's but each PM must know how to save/load a LM. 
 *  
 * \author pharvey (5/26/20)
 * 
 * \param pModel 
 * 
 * \return bool 
 */
void PMModel::doLoad( LMModel *pModel )
{
    doLoadDomains( pModel );
    doLoadTables( pModel );
    doLoadAssociativeTables( pModel );
    doLoadReferences( pModel );
    doEnsurePages();
}

QDomElement PMModel::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DDiagram::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "DisplayFlags", getDisplayFlags() );

    return domElemThis;
}

bool PMModel::doLoad( QDomElement *pdomElemThis )
{
    DDiagram::doLoad( pdomElemThis );
    PMObject::doLoadPMObject( pdomElemThis );
    PMDDLManager::doLoadPMDDLManager( pdomElemThis );

    nDisplayFlags = pdomElemThis->attribute( "DisplayFlags", "0" ).toInt();                      

    return true;
}

void PMModel::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "PMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "PMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "PMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Annotation...                                                                                                                 
    stringValidation = tr("Annotation missing");                                                                         
    nSeverity = getValidationSeverity( "PMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAnnotation().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "LModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "LModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getVer().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           

    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      

        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

void PMModel::doClearDisplayFlag( int nFlag )
{ 
    this->nDisplayFlags &= ~nFlag; 
}

QMenu *PMModel::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu = DDiagram::getContextMenu( pParent );
    QAction *   pAction;

    QMenu *pMenuCreate = new QMenu( tr( "Create" ) );
    pMenu->insertMenu( pMenu->actions().first(), pMenuCreate );

    pAction = new QAction( QPixmap( ":PM/DatabaseReverse" ), tr("Physical Model From A Database..."), this );
    connect( pAction, SIGNAL(triggered()), this, SLOT(slotReverseEngineer()) );
    pMenuCreate->addAction( pAction );

    pAction = new QAction( QPixmap( ":PM/DatabaseCreate" ), tr("Database From This..."), this );
    connect( pAction, SIGNAL(triggered()), this, SLOT(slotForwardEngineer()) );
    pMenuCreate->addAction( pAction );

    pAction = new QAction( QPixmap( ":PM/LogicalModel" ), tr("Logical Model From This..."), this );
    connect( pAction, SIGNAL(triggered()), this, SLOT(slotCreateLogicalModel()) );
    pMenuCreate->addAction( pAction );

    return pMenu; // caller invokes and then deletes
}

/*!
    Set focus to ourself and invoke our property view if we are the target.
*/
void PMModel::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    slotOpenEditor();

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return; 
    }
    // 
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

bool PMModel::doDataConnect()
{
    // DATA CONNECTION
    // - messages echo up so just catch them all at the top (DATASystem)
    pSystem         = new DATASystem();
    connect( pSystem, SIGNAL(signalMessage(DATAMessage *)), SLOT(slotMessage(DATAMessage *)) );
    connect( pSystem, SIGNAL(signalDiagnostic(DATADiagnostic *)), SLOT(slotDiagnostic(DATADiagnostic *)) );
    pSystem->doAlloc();

    pEnvironment    = new DATAEnvironment( pSystem );
    pEnvironment->doAlloc();
    pEnvironment->setAttrODBCVersion( DATAEnvironment::OVOdbc3 );

    pConnection     = new DATAConnection( pEnvironment );
    pConnection->doAlloc();

    if ( !pConnection->isConnected() )
    {
        pConnection->slotConnect( getEditorWidgetParent() ); 
        if ( !pConnection->isConnected() )
        {
            // we are useless without a connection
            doMessageBox( "WARNING", tr( "ODBC Driver Model" ) , tr( "Failed to connect." ) );
            return false; 
        }
        // DO NOT setModified here because it triggers a save before a load.
        // setModified( true ); // update icon
    }
    return true;
}

void PMModel::doDataDisconnect()
{
    // leave pProfile alone
    if ( !pConnection ) return;
    if ( pConnection->isConnected() ) pConnection->doDisconnect();
    delete pConnection; pConnection = nullptr;
    delete pEnvironment; pEnvironment = nullptr;
    delete pSystem; pSystem = nullptr;
}

void PMModel::slotReverseEngineer()
{
    DATAConnection *p = nullptr;
    nRF = 0;

    // use DATAWConnection->DATAConnection up hierarchy (if any)
    // failing that we create a this->DATAConnection just for this task

    // if we have a DATAWConnection to work with - use it or fail
    DATAWConnection *pConnectionParent = (DATAWConnection*)getParent( "DATAWConnection" );
    if ( pConnectionParent )
    {
        if ( !pConnectionParent->isConnected() )
        {
            pConnectionParent->slotConnect();
            if ( !pConnectionParent->isConnected() ) return;
        }
        p = pConnectionParent->getConnection(); 
    }
    // no DATAWConnection so create/use a DATAConnection just for this purpose
    else
    {
        if ( !isDataConnected() )
        {
            if ( !doDataConnect() ) return;
        }
        p = pConnection;
    }

    // prompt for objects 
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    PMSelectObjectsDialog dialog( p, nullptr );
    if ( dialog.exec() != QDialog::Accepted ) return;
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

// putting this into a transaction suppresses calls to setModified which prevents tree view from updating itself
// so avoid until this can be sorted 

    Q_ASSERT( !g_Transaction->pProgress );
    g_Transaction->pProgress = new QProgressDialog( nullptr,  Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    g_Transaction->pProgress->setWindowModality( Qt::ApplicationModal );
    g_Transaction->pProgress->setCancelButton( nullptr );
    g_Transaction->pProgress->setMinimum( 0 );
    g_Transaction->pProgress->setMaximum( 1 );
    g_Transaction->pProgress->setValue( 0 );
    g_Transaction->pProgress->show();

    QPoint pointCell( 0, 0 );
//    g_Transaction->initLoad( this );
    nPK = 0;
    nRF = 0;
    doLoadTables( p, dialog.getTables(), pointCell );
    doLoadViews( p, dialog.getViews(), pointCell );
    doEnsurePages();
    QApplication::processEvents();

    doLoadProcedures( p, dialog.getProcedures() );
    doLoadForeignKeys( p );
//    g_Transaction->fini();

    delete g_Transaction->pProgress;
    g_Transaction->pProgress = nullptr;

//    setModified();

    if ( !pConnectionParent ) doDataDisconnect();

    // doEnsurePages();

    // ensure something is visible
    if ( isEditor() )
    {
        // center on first table else topleft
        DDiagramEditorWidget *pDiagramEditorWidget = (DDiagramEditorWidget*)getEditorWidget();
        QList<PMTableNormal*> l = findChildren<PMTableNormal*>( QString(), Qt::FindDirectChildrenOnly );
        if ( l.first() )
            pDiagramEditorWidget->getView()->centerOn( l.first()->getProxy()->getItem() );
        else
            pDiagramEditorWidget->getView()->centerOn( 0, 0 );
    }
}

void PMModel::slotForwardEngineer()
{
    // create a connection
    // save to the connection 
}

/*!
 * \brief Create a new Logical Model and save ourself into it.
 * 
 * \author pharvey (5/26/20)
 */
void PMModel::slotCreateLogicalModel()
{
    LMModel *pModel = (LMModel*)getObjectAnyParent( "LMModel" );
    if ( !pModel ) 
    { 
        doMessage( "ERROR", tr( "Failed to create a Logical Model." ) );
        return;
    }

    doSave( pModel );

    pModel->slotOpenEditor();
}

bool PMModel::doLoadTables( DATAConnection *pConnection, const QStringList &listTables, QPoint &pointCell )
{
    DATAStatement *pStatement = pConnection->getTables( QString(), QString() );
    if ( !pStatement ) return false;

    QSize sizeCells = getCells();
    g_Transaction->pProgress->setMaximum( listTables.count() );
    g_Transaction->pProgress->setValue( 0 );

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringTable = pStatement->getData( 3 ).toString();

        if ( !listTables.contains( stringTable ) )
        {
            n = pStatement->doFetch();
            continue;
        }

        g_Transaction->pProgress->setLabelText( "Table: " + stringTable );
        g_Transaction->pProgress->setValue( g_Transaction->pProgress->value() + 1 );
        QApplication::processEvents();

        if ( !stringSchema.isEmpty() )                          
        {                                                       
            stringTable = stringSchema + "." + stringTable;     
            if ( !stringCatalog.isEmpty() ) stringTable = stringCatalog + "." + stringTable;
        }                                                       

        // create table object
        PMTableNormal *pTable = (PMTableNormal*)getObject( "Table" );
        if ( !pTable ) break;

        pTable->setName( stringTable );
        // avoid rect edge being right on scene edge
        pTable->setPos( mapCellToScene( pointCell ) += QPointF( 10, 10 ) );

        pointCell.setX( pointCell.x() + 2 );
        if ( pointCell.x() > sizeCells.width() )
        {
            pointCell.setX( 0 );
            pointCell.setY( pointCell.y() + 2 );
        }

        doLoadTable( pConnection, pTable );

        // pTable->slotOptimizeSize();

        // carry on
        n = pStatement->doFetch();
    }

    delete pStatement;

    return true;
}

bool PMModel::doLoadViews( DATAConnection *pConnection, const QStringList &listViews, QPoint &pointCell )
{
    DATAStatement *pStatement = pConnection->getViews( QString(), QString() );
    if ( !pStatement ) return false;

    QSize sizeCells = getCells();
    g_Transaction->pProgress->setMaximum( listViews.count() );
    g_Transaction->pProgress->setValue( 0 );

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringView = pStatement->getData( 3 ).toString();

        if ( !listViews.contains( stringView ) )
        {
            n = pStatement->doFetch();
            continue;
        }

        g_Transaction->pProgress->setLabelText( "View: " + stringView );
        g_Transaction->pProgress->setValue( g_Transaction->pProgress->value() + 1 );
        QApplication::processEvents();

        if ( !stringSchema.isEmpty() )                          
        {                                                       
            stringView = stringSchema + "." + stringView;     
            if ( !stringCatalog.isEmpty() ) stringView = stringCatalog + "." + stringView;
        }                                                       

        // create procedure object
        PMView *pView = (PMView*)getObject( "View" );
        if ( !pView ) break;

        pView->setName( stringView );
        pView->setPos( mapCellToScene( pointCell ) += QPointF( 10, 10 ) );

        pointCell.setX( pointCell.x() + 2 );
        if ( pointCell.x() > sizeCells.width() )
        {
            pointCell.setX( 0 );
            pointCell.setY( pointCell.y() + 2 );
        }

        doLoadViewColumns( pConnection, pView );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadTable( DATAConnection *pConnection, PMTableNormal *pTable )
{
    bool b1 = doLoadTableColumns( pConnection, pTable );      // PMColumn's
    bool b2 = doLoadTablePrimaryKey( pConnection, pTable );   // PMKey's
    bool b3 = doLoadTableIndexs( pConnection, pTable );       // PMIndex's
    return ( b1 || b2 || b3 );
}

bool PMModel::doLoadTableColumns( DATAConnection *pConnection, PMTableNormal *pTable )
{
    DATAStatement *pStatement = pConnection->getColumns( QString(), QString(), pTable->getName() );
    if ( !pStatement ) return false;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringColumn = pStatement->getData( 4 ).toString();
        QString stringTypeName = pStatement->getData( 6 ).toString();
        QString stringColumnSize = pStatement->getData( 7 ).toString();
        QString stringDecimalDigits = pStatement->getData( 9 ).toString();
        int nNullable = pStatement->getData( 11 ).toInt();

        // create column object
        PMColumn *pColumn = (PMColumn*)pTable->getObject( "Column" );
        if ( !pColumn ) break;

        // \todo This is ok for most cases but needs work to handle all.
        pColumn->setName( stringColumn );
        pColumn->setDataType( stringTypeName.toUpper() );
        pColumn->setLength( stringColumnSize );
        pColumn->setPrecision1( stringColumnSize );
        pColumn->setScale( stringDecimalDigits );
        pColumn->setMandatory( nNullable == SQL_NO_NULLS ? true : false );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadTablePrimaryKey( DATAConnection *pConnection, PMTableNormal *pTable )
{
    DATAStatement *pStatement = pConnection->getPrimaryKeys( QString(), QString(), pTable->getName() );
    if ( !pStatement ) return false;

    PMKey *pKey = nullptr;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        if ( !pKey )
        {
            pKey = (PMKey*)pTable->getObject( "Key" );
            if ( !pKey ) break;
            QString stringPK_NAME = pStatement->getData( 6 ).toString();
            if ( stringPK_NAME.isEmpty() )
            {
                stringPK_NAME = "PK~" + QString().number( nPK );
                nPK++;
            }
            pKey->setName( stringPK_NAME );
            pKey->setPrimary();
        }

        QString stringColumn = pStatement->getData( 4 ).toString();
        PMColumn *pColumn = pTable->getColumn( stringColumn );
        Q_ASSERT( pColumn );

        pKey->doAddColumn( pColumn );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadTableIndexs( DATAConnection *pConnection, PMTableNormal *pTable )
{
    DATAStatement *pStatement = pConnection->getIndexs( QString(), QString(), pTable->getName() );
    if ( !pStatement ) return false;

    PMIndex *pIndex = nullptr;
    QString stringIndexName;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString s = pStatement->getData( 6 ).toString();                // index name
        QString stringColumn = pStatement->getData( 9 ).toString();     // column name
        QString stringOrder = pStatement->getData( 10 ).toString();     // order (for column)

        if ( s.isEmpty() && stringColumn.isEmpty() )                    // spec says this index 'is a table' when this happens - we ignore
        {
            n = pStatement->doFetch();
            continue;
        }

        if (!pIndex || stringIndexName != s)                            // we are on to a new index?
        {
            stringIndexName = s;
            pIndex = (PMIndex*)pTable->getObject( "Index" );
            if ( !pIndex ) break;
            pIndex->setName( stringIndexName );
            pIndex->setUnique( pStatement->getData( 4 ).toInt() == SQL_FALSE );
        }

        PMIndexColumn *pColumn = (PMIndexColumn*)pIndex->getObject( "Column" );
        if ( !pColumn )
        {
            n = pStatement->doFetch();
            continue;
        }

        pColumn->setName( stringColumn );
        if ( stringOrder == "A" ) pColumn->setOrder( "ASC" );
        else if ( stringOrder == "D" ) pColumn->setOrder( "DESC" );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadProcedures( DATAConnection *pConnection, const QStringList &listProcedures )
{
    DATAStatement *pStatement = pConnection->getProcedures( QString(), QString() );
    if ( !pStatement ) return false;

    g_Transaction->pProgress->setMaximum( listProcedures.count() );
    g_Transaction->pProgress->setValue( 0 );

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringProcedure = pStatement->getData( 3 ).toString();

        if ( !listProcedures.contains( stringProcedure ) )
        {
            n = pStatement->doFetch();
            continue;
        }

        g_Transaction->pProgress->setLabelText( "Procedure: " + stringProcedure );
        g_Transaction->pProgress->setValue( g_Transaction->pProgress->value() + 1 );
        QApplication::processEvents();

        // MS Access will provide path+file for catalog and for schema and we want to ignore this.
        // \todo Try to be smart about this.
/*
        if ( !stringSchema.isEmpty() )                          
        {                                                       
            stringTable = stringSchema + "." + stringTable;     
            if ( !stringCatalog.isEmpty() )                     
                stringTable = stringCatalog + "." + stringTable;
        }                                                       
*/

        // create procedure object
        PMProcedure *pProcedure = (PMProcedure*)getObject( "Procedure" );
        if ( !pProcedure ) break;

        pProcedure->setName( stringProcedure );

        // doLoadProcedure( pConnection, pProcedure );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadForeignKeys( DATAConnection *pConnection )
{
    QList<ADObject*> l = getObjects( QString(), QString(), "PMTableNormal" );
    ADObject *pObject;

    g_Transaction->pProgress->setMaximum( l.count() );
    g_Transaction->pProgress->setValue( 0 );

    foreach( pObject, l )
    {
        g_Transaction->pProgress->setLabelText( "Foreign Keys: " + pObject->getName() );
        g_Transaction->pProgress->setValue( g_Transaction->pProgress->value() + 1 );
        QApplication::processEvents();

        if ( !doLoadTableForeignKeys( pConnection, (PMTableNormal*)pObject ) ) return false;
    }
    return true;
}

/*!
 * \brief Create all references for the given table.
 * 
 * parent table: is where the key is a PK
 * child table: is where the key is a FK
 * 
 * \note We can NOT assume that all tables are in the model.
 * 
 * \author pharvey (2/11/25)
 * 
 * \param pConnection 
 * \param pTableChild 
 * 
 * \return bool 
 */
bool PMModel::doLoadTableForeignKeys( DATAConnection *pConnection, PMTableNormal *pFKTable )
{
    DATAStatement *pStatement = pConnection->getForeignKeys( QString(), QString(), pFKTable->getName() );
    if ( !pStatement ) return false;

    PMReference *   pReference  = nullptr;
    PMTableNormal * pPKTable    = nullptr;

    QString stringPrimaryKeyTableName;
    QString stringForeignKeyTableName;
    QString stringForeignKeyName;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringPKTABLE_NAME  = pStatement->getData( 3 ).toString();
        QString stringPKCOLUMN_NAME = pStatement->getData( 4 ).toString();
        QString stringFKTABLE_NAME  = pStatement->getData( 7 ).toString();
        QString stringFKCOLUMN_NAME = pStatement->getData( 8 ).toString();
        QString stringFK_NAME       = pStatement->getData( 12 ).toString();
        QString stringPK_NAME       = pStatement->getData( 13 ).toString();

        // we need the PK table to be in the model to create a reference 
        QList<ADObject*> l = getObjects( stringPKTABLE_NAME, QString(), "PMTableNormal" );
        if ( !l.count() ) continue;

        if ( !pReference || 
             stringFKTABLE_NAME != stringForeignKeyTableName || 
             stringPKTABLE_NAME != stringPrimaryKeyTableName || 
             stringFK_NAME != stringForeignKeyName )
        {
            stringPrimaryKeyTableName   = stringPKTABLE_NAME;
            stringForeignKeyTableName   = stringFKTABLE_NAME;
            stringForeignKeyName        = stringFK_NAME;
            pPKTable                    = nullptr;
            pReference = (PMReference*)getObject( "Reference" );
            if ( !pReference ) break;
            pReference->setTextDisplay( DObject::TextDisplayComment );

            // connect the reference/line to the tables
            QList<ADObject*> l = getObjects( stringPKTABLE_NAME, QString(), "PMTableNormal" );
            if ( l.count() )
            {
                pPKTable = (PMTableNormal*)l.first(); // should only be one
                pReference->setName( "REF~" + QString::number( nRF ) );
                pReference->setPos( pPKTable->getPos() );
                pReference->setBegin( pFKTable->getPosCenter() );
                pReference->setEnd( pPKTable->getPosCenter() );
                pReference->doConnect( pFKTable, "Begin" );
                pReference->doConnect( pPKTable, "End" );
            }
            nRF++;
        }

        // add the two columns
        PMReferenceColumn *pColumn = (PMReferenceColumn*)pReference->getObject( "PMReferenceColumn" );
        Q_ASSERT( pColumn );

        {
            // get column from child and parent tables
            PMColumn *pChild    = pFKTable->getColumn( stringFKCOLUMN_NAME );
            PMColumn *pParent   = pPKTable->getColumn( stringPKCOLUMN_NAME );
            // make our column reference child and parent column
            if ( pChild )
                pColumn->setChildColumn( pChild );
            else
                qWarning( "[PAH][%s][%s][%d] Could not find FK column %s\n", __FILE__, __FUNCTION__, __LINE__, stringFKCOLUMN_NAME.toUtf8().constData() );
            if ( pParent )
                pColumn->setParentColumn( pParent );
            else
                qWarning( "[PAH][%s][%s][%d] Could not find PK column %s\n", __FILE__, __FUNCTION__, __LINE__, stringPKCOLUMN_NAME.toUtf8().constData() );
        }

        pReference->setComment( pReference->getComment() + stringPKCOLUMN_NAME + " = " + stringFKCOLUMN_NAME + "\n" );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

bool PMModel::doLoadViewColumns( DATAConnection *pConnection, PMView *pView )
{
    DATAStatement *pStatement = pConnection->getColumns( QString(), QString(), pView->getName() );
    if ( !pStatement ) return false;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringColumn = pStatement->getData( 4 ).toString();

        // create column object
        PMViewColumn *pColumn = (PMViewColumn*)pView->getObject( "Column" );
        if ( !pColumn ) break;

        pColumn->setName( stringColumn );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

void PMModel::doSaveDomains( LMModel *pModel )
{
    QList<ADObject*> l = getObjects( QString(), QString(), "PMDomain" );
    ADObject *p;
    foreach( p, l )
    {
        PMDomain *pDomain1 = (PMDomain*)p;
        LMDomain *pDomain2 = (LMDomain*)pModel->getObject( "Domain" );
        pDomain2->setName( pDomain1->getName() );
        pDomain2->setCode( pDomain1->getCode() );
        pDomain2->setComment( pDomain1->getComment() );
        pDomain2->setDataTypeObject( mapDataTypeToLogical( pDomain1->getDataTypeObject() ) );
    }
}

void PMModel::doSaveTables( LMModel *pModel )
{
    QList<ADObject*> l = getObjects( QString(), QString(), "PMTableNormal" );
    ADObject *p;
    foreach( p, l )
    {
        PMTableNormal *pTable = (PMTableNormal*)p;
        LMEntityNormal *pEntity = (LMEntityNormal*)pModel->getObject( "LMEntityNormal" );
        pEntity->setSize( pTable->boundingRect().size() );
        pEntity->setPos( pTable->getPos() );
        pEntity->setName( pTable->getName() );
        doSaveTableColumns( pTable, pEntity );
        doSaveTableKeys( pTable, pEntity );
    }
}

void PMModel::doSaveReferences( LMModel *pModel )
{
    QList<ADObject*> l = getObjects( QString(), QString(), "PMReference" );
    ADObject *p;
    foreach( p, l )
    {
        PMReference *pReference = (PMReference*)p;
        LMRelationship *pRelationship = (LMRelationship*)pModel->getObject( "LMRelationship" );
        pRelationship->setPos( pReference->getPos() );
        pRelationship->setBegin( pReference->getProxy()->mapToScene( pReference->getBegin() ) );
        pRelationship->setEnd( pReference->getProxy()->mapToScene( pReference->getEnd() ) );
        pRelationship->setName( pReference->getName() );

        // connect
        // - needs tables to have unique names to work best
        DRectangle *pTable;
        // begin
        pTable = pReference->getRectangle( CBD::EOLBegin );
        if ( pTable && pTable->inherits( "PMTableNormal" ) )
        {
            QList<ADObject*> l = pModel->getObjects( pTable->getName(), QString(), "LMEntityNormal" );
            if ( l.count() == 1 )
            {
                LMEntityNormal *pEntity = (LMEntityNormal*)l.first(); 
                pRelationship->doConnect( pEntity, "Begin" );

                //
                // \todo - set cardinality restricted delete etc
                //
            }
        }
        // end
        pTable = pReference->getRectangle( CBD::EOLEnd );
        if ( pTable && pTable->inherits( "PMTableNormal" ) )
        {
            QList<ADObject*> l = pModel->getObjects( pTable->getName(), QString(), "LMEntityNormal" );
            if ( l.count() == 1 )
            {
                LMEntityNormal *pEntity = (LMEntityNormal*)l.first(); 
                pRelationship->doConnect( pEntity, "End" );

                //
                // \todo - set cardinality restricted delete etc
                //
            }
        }
    }
}

void PMModel::doSaveTableColumns( PMTableNormal *pTable, LMEntityNormal *pEntity )
{
    QList<ADObject*> l = pTable->getObjects( QString(), QString(), "PMColumn" );
    ADObject *p;
    foreach( p, l )
    {
        PMColumn *pColumn = (PMColumn*)p;
        doSaveTableColumn( pColumn, pEntity );
    }
}

/*!
 * \brief Save a PMColumn into a LMEntity as an LMAttribute. 
 *  
 * An LMAttribute has a few properties of its own but most are via a reference to a LMDataItem. 
 * Normally; LMDataItem's have unique names. If we strictly obey this rule we could lose a lot 
 * of data-type info in the PMColumn's. So, for the purposes of translating, we consider an 
 * LMDataItem to be unique based upon its name + data-type. 
 *  
 * An LMDataItem can, itself, be based upon a LMDomain via a ref. This is the case when we create 
 * a LMDataItem for a PMColumn that is using a PMDomain. 
 *  
 * \author pharvey (5/27/20)
 * 
 * \param pEntity 
 * \param pColumn 
 */
void PMModel::doSaveTableColumn( PMColumn *pColumn, LMEntityNormal *pEntity )
{
    // for now - just create a new LMDataItem for every column
    LMModel *pModel = (LMModel*)pEntity->getParent( "LMModel" );
    LMDataItem *pDataItem = (LMDataItem*)pModel->getObject( "LMDataItem" );
    doSaveTableColumn( pColumn, pDataItem );

    LMAttribute *pAttribute = (LMAttribute*)pEntity->getObject( "LMAttribute" );
    doSaveTableColumn( pColumn, pAttribute );
    pAttribute->doConnect( pDataItem );
}

void PMModel::doSaveTableColumn( PMColumn *pColumn, LMDataItem *pDataItem )
{
    pDataItem->setName( pColumn->getName() );
    pDataItem->setCode( pColumn->getCode() );
    pDataItem->setComment( pColumn->getComment() );
    pDataItem->setDataTypeObject( mapDataTypeToLogical( pColumn->getDataTypeObject() ) );
    if ( pColumn->getDomain() )
    {
        LMModel *pModel = (LMModel*)pDataItem->getParent( "LMModel" );
        pDataItem->setDomain( pModel->getDomain( pColumn->getDomain()->getName() ) ); 
    }
}

void PMModel::doSaveTableColumn( PMColumn *pColumn, LMAttribute *pAttribute )
{
    pAttribute->setIdentifier( pColumn->isPrimaryKey() );
    pAttribute->setMandatory( pColumn->isMandatory() );
    pAttribute->setHidden( pColumn->isHidden() );
}

void PMModel::doSaveTableKeys( PMTableNormal *pTable, LMEntityNormal *pEntity )
{
    QList<ADObject*> l = pTable->getObjects( QString(), QString(), "PMKey" );
    ADObject *p;
    foreach( p, l )
    {
        PMKey *pKey = (PMKey*)p;
        LMIdentifier *pIdentifier = (LMIdentifier*)pEntity->getObject( "LMIdentifier" );
        doSaveTableKey( pKey, pIdentifier );
    }
}

void PMModel::doSaveTableKey( PMKey *pKey, LMIdentifier *pIdentifier )
{
    LMEntityNormal *pEntity = (LMEntityNormal*)pIdentifier->getParent( "LMEntityNormal" );

    pIdentifier->setName( pKey->getName() );
    pIdentifier->setCode( pKey->getCode() );
    pIdentifier->setComment( pKey->getComment() );
    pIdentifier->setPrimary( pKey->isPrimary() );

    QList<ADObjectReferenceKey> l = pKey->getUsesKeys( nullptr, QString(), QString(), "PMColumn" );
    ADObjectReferenceKey Key;
    foreach( Key, l )
    {
        PMColumn *pColumn = (PMColumn*)Key.pObject;
        QList<ADObject*> listColumns = pEntity->getObjects( pColumn->getName(), QString(), "LMAttribute" );
        if ( !listColumns.count() ) continue;
        LMAttribute *pAttribute = (LMAttribute*)listColumns.first();
        pIdentifier->doConnect( pAttribute );
    }
}

void PMModel::doLoadDomains( LMModel *pModel )
{
    QList<ADObject*> l = pModel->getObjects( QString(), QString(), "LMDomain" );
    ADObject *p;
    foreach( p, l )
    {
        LMDomain *pDomain1 = (LMDomain*)p;
        PMDomain *pDomain2 = (PMDomain*)getObject( "Domain" );
        pDomain2->setName( pDomain1->getName() );
        pDomain2->setCode( pDomain1->getCode() );
        pDomain2->setComment( pDomain1->getComment() );
        pDomain2->setDataTypeObject( mapDataTypeToPhysical( pDomain1->getDataTypeObject() ) );
    }
}

void PMModel::doLoadTables( LMModel *pModel )
{
    QList<ADObject*> l = pModel->getObjects( QString(), QString(), "LMEntityNormal" );
    ADObject *p;
    foreach( p, l )
    {
        LMEntityNormal *pEntity = (LMEntityNormal*)p;
        PMTableNormal *pTable = (PMTableNormal*)getObject( "Table" );
        pTable->setSize( pEntity->boundingRect().size() );
        pTable->setPos( pEntity->getPos() );
        pTable->setName( pEntity->getName() );
        doLoadTableColumns( pEntity, pTable );
        doLoadTableKeys( pEntity, pTable );
    }
}

void PMModel::doLoadAssociativeTables( LMModel *pModel )
{
    QList<ADObject*> l = pModel->getObjects( QString(), QString(), "LMEntityAssociative" );
    ADObject *p;
    foreach( p, l )
    {
        LMEntityAssociative *pEntity = (LMEntityAssociative*)p;
        PMTableNormal *pTable = (PMTableNormal*)getObject( "Table" );
        pTable->setSize( pEntity->boundingRect().size() );
        pTable->setPos( pEntity->getPos() );
        pTable->setName( pEntity->getName() );
        doLoadTableColumns( pEntity, pTable );
    }
}

void PMModel::doLoadReferences( LMModel *pModel )
{
    QList<ADObject*> l = pModel->getObjects( QString(), QString(), "LMRelationship" );
    ADObject *p;
    foreach( p, l )
    {
        LMRelationship *pRelationship = (LMRelationship*)p;
        PMReference *pReference = (PMReference*)getObject( "Reference" );
        pReference->setPos( pRelationship->getPos() );
        pReference->setBegin( pRelationship->getProxy()->mapToScene( pRelationship->getBegin() ) );
        pReference->setEnd( pRelationship->getProxy()->mapToScene( pRelationship->getEnd() ) );
        pReference->setName( pRelationship->getName() );

        // connect
        // - needs tables to have unique names to work best
        DRectangle *pEntity;
        // begin
        pEntity = pRelationship->getRectangle( CBD::EOLBegin );
        if ( pEntity && pEntity->inherits( "LMEntityBase" ) )
        {
            QList<ADObject*> l = getObjects( pEntity->getName(), QString(), "PMTableBase" );
            if ( l.count() == 1 )
            {
                PMTableBase *pTable = (PMTableBase*)l.first(); 
                pReference->doConnect( pTable, "Begin" );
            }
        }
        // end
        pEntity = pRelationship->getRectangle( CBD::EOLEnd );
        if ( pEntity && pEntity->inherits( "LMEntityBase" ) )
        {
            QList<ADObject*> l = getObjects( pEntity->getName(), QString(), "PMTableBase" );
            if ( l.count() == 1 )
            {
                PMTableBase *pTable = (PMTableBase*)l.first(); 
                pReference->doConnect( pTable, "End" );
            }
        }
    }
}

void PMModel::doLoadTableColumns( LMEntityBase *pEntity, PMTableNormal *pTable )
{
    QList<ADObject*> l = pEntity->getObjects( QString(), QString(), "LMAttribute" );
    ADObject *p;
    foreach( p, l )
    {
        LMAttribute *pAttribute = (LMAttribute*)p;
        doLoadTableColumn( pAttribute, pTable );
    }
}

void PMModel::doLoadTableColumn( LMAttribute *pAttribute, PMTableNormal *pTable )
{
    PMColumn *pColumn = (PMColumn*)pTable->getObject( "Column" );

    pColumn->setName( pAttribute->getName() );
    pColumn->setCode( pAttribute->getCode() );
    pColumn->setComment( pAttribute->getComment() );
    pColumn->setDataTypeObject( mapDataTypeToPhysical( pAttribute->getDataTypeObject() ) );
    if ( pAttribute->getDomain() )
    {
        pColumn->setDomain( getDomain( pAttribute->getDomain()->getName() ) ); 
    }
}

void PMModel::doLoadTableKeys( LMEntityNormal *pEntity, PMTableNormal *pTable )
{
    QList<ADObject*> l = pEntity->getObjects( QString(), QString(), "LMIdentifier" );
    ADObject *pObject;
    foreach( pObject, l )
    {
        LMIdentifier *pIdentifier = (LMIdentifier*)pObject;
        PMKey *pKey = (PMKey*)pTable->getObject( "Key" );
        doLoadTableKey( pIdentifier, pKey );
    }
}

void PMModel::doLoadTableKey( LMIdentifier *pIdentifier, PMKey *pKey )
{
    PMTableNormal *pTable = (PMTableNormal*)pKey->getParent( "PMTableNormal" );

    pKey->setName( pIdentifier->getName() );
    pKey->setCode( pIdentifier->getCode() );
    pKey->setComment( pIdentifier->getComment() );
    pKey->setPrimary( pIdentifier->isPrimary() );

    QList<ADObjectReferenceKey> l = pIdentifier->getUsesKeys( nullptr, QString(), QString(), "LMAttribute" );
    ADObjectReferenceKey Key;
    foreach( Key, l )
    {
        LMAttribute *pAttribute = (LMAttribute*)Key.pObject;
        QList<ADObject*> listColumns = pTable->getObjects( pAttribute->getName(), QString(), "PMColumn" );
        if ( !listColumns.count() ) continue;
        PMColumn *pColumn = (PMColumn*)listColumns.first();
        pKey->doConnect( pColumn );
    }
}

/*!
 * \brief Map a data-type so its suitable for the destination. 
 *  
 * The default is to simply return the same data-type. 
 * The \sa LMModel uses data-types defined for ODBC Extended/SQL92. 
 * 
 * \author pharvey (5/30/20)
 * 
 * \param DataType 
 * 
 * \return DATADataType 
 */
DATADataType PMModel::mapDataTypeToLogical( const DATADataType DataType )
{
    return DataType;
}

/*!
 * \brief Map a data-type so its suitable for the destination. 
 *  
 * The default is to simply return the same data-type. 
 * The \sa LMModel uses data-types defined for ODBC Extended/SQL92. 
 * 
 * \author pharvey (5/30/20)
 * 
 * \param DataType 
 * 
 * \return DATADataType 
 */
DATADataType PMModel::mapDataTypeToPhysical( const DATADataType DataType )
{
    return DataType;
}

/*!
    getModel
    
    This exists because we do not want to create duplicate objects during a paste.
*/
/*
ADObject *PMModel::getObject( QDomElement *pdomElem )                               
{                                                                                   
    int     nOID            = pdomElem->attribute( "OID", "0" ).toInt();            
    QString stringName      = pdomElem->attribute( "Name" );                        
    QString stringClass     = pdomElem->tagName();                                  
                                                                                    
    QList<ADObject*> listChildren = getObjects();                                   
                                                                                    
    if ( listChildren.count() > 0 )                                                 
    {                                                                               
        QObject *           pobject;                                                
        ADObject *          pmodelFound;                                            
                                                                                    
        // Search for match on OID first because it is a more accurate result. This 
        // handles the case where we are pasting to same PMModel.                   
        foreach( pobject, listChildren )                                            
        {                                                                           
            if ( !(pobject->metaObject()->className() == stringClass) )             
                continue;                                                           
            pmodelFound = (ADObject*)pobject;                                       
            if ( pmodelFound->getOID() != nOID )                                    
                continue;                                                           
            if ( pmodelFound->getName() != stringName )                             
                continue;                                                           
                                                                                    
            //                                                                      
            // FOUND  IT!                                                           
            //                                                                      
            return pmodelFound;                                                     
                                                                                    
        } // while                                                                  
                                                                                    
        // No OID match so lets try to be smart and match on remaining criteria     
        // this handles the condition where a paste is being made to another PMModel
        // where some of the dependent objects exist but with diff OID. This is not 
        // perfect but close enough for now.                                        
        foreach( pobject, listChildren )                                            
        {                                                                           
            if ( !(pobject->metaObject()->className() == stringClass) )             
                continue;                                                           
            pmodelFound = (ADObject*)pobject;                                       
            if ( pmodelFound->getName() != stringName )                             
                continue;                                                           
                                                                                    
            //                                                                      
            // FOUND  IT!                                                           
            //                                                                      
            return pmodelFound;                                                     
                                                                                    
        } // while                                                                  
    }                                                                               
                                                                                    
    return 0;                                                                       
}                                                                                   
*/


bool PMModel::doDragDrop( const QStringList &stringList, const QPointF &pointScene )
{
    QString stringClass = stringList.first();

    if ( !canDrop( stringClass, pointScene ) ) return false;

    //
    slotSelectNone();
    ADObject *p = getObject( stringClass );
    if ( !p ) return false;
    if ( !p->inherits( "DObject" ) )
    {
        Q_ASSERT( p->inherits( "AWObject" ) );
        // just created PMDomain or similar
        ((AWObject*)p)->slotObjectDialog();
        return true;
    }

    DObject *pObject = (DObject*)p;

    // common
    if ( isSnapGrid() && pObject->inherits( "DRectangle" ) ) pObject->setPos( getGridCenter( pointScene ) );
    else pObject->setPos( pointScene );

    getSelectionManager()->setSelected( pObject );
    doEnsurePages();

   return true; 
}

void PMModel::slotMessage( DATAMessage *p )
{
    // package message and send it out via ADObject
    ADDataEventGeneral Event( p );
    emit signalOutput( &Event );
}

void PMModel::slotDiagnostic( DATADiagnostic *p )
{
    // package message and send it out via ADObject
    ADDataEventDiagnostic Event( p );
    emit signalOutput( &Event );
}

//
// PMSelectObjectsWidget
//
PMSelectObjectsWidget::PMSelectObjectsWidget( DATAConnection *p, QWidget *pParent )
    : QWidget( pParent )
{
    pConnection = p;
    QGridLayout *pLayout = new QGridLayout( this );

    pLayout->addWidget( new QLabel( tr("Tables") ), 0, 0 );
    pLayout->addWidget( new QLabel( tr("Views") ), 0, 1 );
    pLayout->addWidget( new QLabel( tr("Procedures") ), 0, 2 );

    pLayout->addWidget( pTables = new QListWidget( this ), 1, 0 );
    pLayout->addWidget( pViews = new QListWidget( this ), 1, 1 );
    pLayout->addWidget( pProcedures = new QListWidget( this ), 1, 2 );

    pTables->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTables->setSelectionMode( QAbstractItemView::ExtendedSelection );
    pViews->setSelectionBehavior( QAbstractItemView::SelectRows );
    pViews->setSelectionMode( QAbstractItemView::ExtendedSelection );
    pProcedures->setSelectionBehavior( QAbstractItemView::SelectRows );
    pProcedures->setSelectionMode( QAbstractItemView::ExtendedSelection );

    doLoadTables();
    doLoadViews();
    doLoadProcedures();
    doLoadForeignKeys();
}

QStringList PMSelectObjectsWidget::getTables()
{
    QStringList listSelected;

    QList<QListWidgetItem *> l =  pTables->selectedItems();
    QListWidgetItem *p;
    foreach( p, l )
    {
        listSelected << p->text();
    }

    return listSelected;
}

QStringList PMSelectObjectsWidget::getViews()
{
    QStringList listSelected;

    QList<QListWidgetItem *> l =  pViews->selectedItems();
    QListWidgetItem *p;
    foreach( p, l )
    {
        listSelected << p->text();
    }

    return listSelected;
}

QStringList PMSelectObjectsWidget::getProcedures()
{
    QStringList listSelected;

    QList<QListWidgetItem *> l =  pProcedures->selectedItems();
    QListWidgetItem *p;
    foreach( p, l )
    {
        listSelected << p->text();
    }

    return listSelected;
}

void PMSelectObjectsWidget::doLoadTables()
{
    DATAStatement *pStatement = pConnection->getTables( QString(), QString() );
    if ( !pStatement ) return;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringTable = pStatement->getData( 3 ).toString();

//        if ( !stringSchema.isEmpty() )                          
//        {                                                       
//            stringTable = stringSchema + "." + stringTable;     
//            if ( !stringCatalog.isEmpty() ) stringTable = stringCatalog + "." + stringTable;
//        }                                                       

        pTables->addItem( new QListWidgetItem( stringTable ) );

        // carry on
        n = pStatement->doFetch();
    }

    delete pStatement;
}

void PMSelectObjectsWidget::doLoadViews()
{
    DATAStatement *pStatement = pConnection->getViews( QString(), QString() );
    if ( !pStatement ) return;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringView = pStatement->getData( 3 ).toString();

 //       if ( !stringSchema.isEmpty() )                          
 //       {                                                       
 //           stringView = stringSchema + "." + stringView;     
 //           if ( !stringCatalog.isEmpty() ) stringView = stringCatalog + "." + stringView;
 //       }                                                       

        pViews->addItem( new QListWidgetItem( stringView ) );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
}

void PMSelectObjectsWidget::doLoadProcedures()
{
    DATAStatement *pStatement = pConnection->getProcedures( QString(), QString() );
    if ( !pStatement ) return;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringProcedure = pStatement->getData( 3 ).toString();

        pProcedures->addItem( new QListWidgetItem( stringProcedure ) );

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
}

void PMSelectObjectsWidget::doLoadForeignKeys()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

//
// PMSelectObjectsDialog
//
PMSelectObjectsDialog::PMSelectObjectsDialog( DATAConnection *p, QWidget *pParent )
    : QDialog( pParent )
{
    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( metaObject()->className() );
    // setWindowIcon( pObject->getIcon() );
    setWindowTitle( tr("Select Objects") );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    playoutTop->addWidget( pWidget = new PMSelectObjectsWidget( p, this ) );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    playoutTop->addWidget( pButtonBox );

    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    setLayout( playoutTop );

    doLoadState();
}

void PMSelectObjectsDialog::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;
    restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void PMSelectObjectsDialog::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );
}

void PMSelectObjectsDialog::slotAccepted()
{
    doSaveState();
    done( QDialog::Accepted );
}

void PMSelectObjectsDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}


