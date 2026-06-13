#include "AppInfo.h"
#include "SAFile.h"

#include <AWFactoryWidget.h>
#include <AWDocumentNameWidget.h>

#include <DGraphicsSceneProxy.h>

#include <DATAWQuerySystem.h>
#include <DATAWSql.h>
#include <DDiagram.h>
#include <DStencils.h>
#include <ORGDiagram.h>
#include <FRForm.h>
#include <FLDiagram.h>
#include <LMModel.h>
#include <SQL92Model.h>
#include <ODBCMinModel.h>
#include <ODBCCorModel.h>
#include <ODBCExtModel.h>
#include <ODBCDrvModel.h>
#include <MSAModel.h>
#include <SLModel.h>

#include "SAConnection.h"
#include "SANewDialog.h"

SAFile::SAFile( const QString &stringName )
    : DFile( stringName )
{
    mapMeta.clear(); 

    mapMeta.insert( "DStencils", ADMeta( QIcon(), "DStencils", "Stencils" ) );

    mapMeta.insert( "SAConnection", ADMeta( QIcon(), "SAConnection", "Data Connection" ) );
    mapMeta.insert( "DATAWQuerySystem", ADMeta( QIcon(), "DATAWQuerySystem", "ODBC" ) );
    mapMeta.insert( "DATAWSql", ADMeta( QIcon(), "DATAWSql", "SQL" ) );
    mapMeta.insert( "DDiagram", ADMeta( QIcon(), "DDiagram", "Diagram" ) );
    mapMeta.insert( "FLDiagram", ADMeta( QIcon(), "FLDiagram", "Logic" ) );
    mapMeta.insert( "FRForm", ADMeta( QIcon(), "FRForm", "Form" ) );
    mapMeta.insert( "ORGDiagram", ADMeta( QIcon(), "ORGDiagram", "Organization Chart" ) );
    mapMeta.insert( "LMModel", ADMeta( QIcon(), "LMModel", "ER: Logical Model" ) );
    mapMeta.insert( "SQL92Model", ADMeta( QIcon(), "SQL92Model", "ER: SQL92" ) );
    mapMeta.insert( "ODBCMinModel", ADMeta( QIcon(), "ODBCMinModel", "ER: ODBC Minimum" ) );
    mapMeta.insert( "ODBCCorModel", ADMeta( QIcon(), "ODBCCorModel", "ER: ODBC Core" ) );
    mapMeta.insert( "ODBCExtModel", ADMeta( QIcon(), "ODBCExtModel", "ER: ODBC Extended" ) );
    mapMeta.insert( "ODBCDrvModel", ADMeta( QIcon(), "ODBCDrvModel", "ER: ODBC Driver" ) );
    mapMeta.insert( "MSAModel", ADMeta( QIcon(), "MSAModel", "ER: MS Access" ) );
    mapMeta.insert( "SLModel", ADMeta( QIcon(), "SLModel", "ER: SQLite" ) );
}

SAFile::~SAFile()
{
}

QPixmap SAFile::getIcon()
{
    return QPixmap( ":DA/Workspace" );
}

AWDataWidget *SAFile::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    AWFactoryWidget *pFactoryWidget;

    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Workspace") );

    p->addTab( new AWDocumentNameWidget( this, p ), tr("Name") );
    p->addTab( pFactoryWidget = new AWFactoryWidget( this, p ), tr("Factory") );

    pFactoryWidget->setAllowCreate( true );
    pFactoryWidget->setAllowDelete( true );
    pFactoryWidget->setAllowEdit( true );

    p->setCurrentIndex( 0 );

    return p;
}

ADObject *SAFile::getObject( const QString &s, ADObject * )
{
    ADObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DStencils" )
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DStencils" ).count() )
        {
            doMessageBox( "INFO", tr("New Stencils Folder"), tr("A stencils folder already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DStencils( this );
    }
    else if ( stringClass == "SAConnection" )
    {
        SAConnection *pData;
        p = pData = new SAConnection( this );
    }
    else if ( stringClass == "DATAWQuerySystem" )
    {
        DATAWQuerySystem *pData;
        if ( getObjects( QString(), QString(), "DATAWQuerySystem" ).count() )
        {
            doMessageBox( "INFO", tr("New Query System"), tr("Already exists and the maximum is one.") );
            return nullptr; 
        }
        p = pData = new DATAWQuerySystem( this );
    }
    else if ( stringClass == "DATAWSql" )
    {
        DATAWSql *pSQL;
        p = pSQL = new DATAWSql( this );
    }
    else if ( stringClass == "DDiagram" )
    {
        DDiagram *pDiagram;
        p = pDiagram = new DDiagram( this );
        pDiagram->getValidationConfiguration();
        // we must always have a scene to calc things even when not displaying   
        // and this must be done right after diagram created and not within it   
        pDiagram->setScene( new DGraphicsSceneProxy( pDiagram, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "FLDiagram" )
    {
        FLDiagram *pDiagram;
        p = pDiagram = new FLDiagram( this );
        pDiagram->getValidationConfiguration();
        pDiagram->setScene( new DGraphicsSceneProxy( pDiagram, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "ORGDiagram" )
    {
        ORGDiagram *pDiagram;
        p = pDiagram = new ORGDiagram( this );
        pDiagram->getValidationConfiguration();
        pDiagram->setScene( new DGraphicsSceneProxy( pDiagram, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "FRForm" )
    {
        FRForm *pDiagram;
        p = pDiagram = new FRForm( this );
        pDiagram->getValidationConfiguration();
        pDiagram->setScene( new DGraphicsSceneProxy( pDiagram, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "LMModel" )
    {
        LMModel *pModel;
        p = pModel = new LMModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
        connect( pModel, SIGNAL(signalCreatePhysicalModel(LMModel *)), SLOT(slotCreatePhysicalModel(LMModel *)) );
    }

    else if ( stringClass == "SQL92Model" )
    {
        SQL92Model *pModel;
        p = pModel = new SQL92Model( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "ODBCMinModel" )
    {
        ODBCMinModel *pModel;
        p = pModel = new ODBCMinModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "ODBCCorModel" )
    {
        ODBCCorModel *pModel;
        p = pModel = new ODBCCorModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "ODBCExtModel" )
    {
        ODBCExtModel *pModel;
        p = pModel = new ODBCExtModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "ODBCDrvModel" )
    {
        ODBCDrvModel *pModel;
        p = pModel = new ODBCDrvModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "MSAModel" )
    {
        MSAModel *pModel;
        p = pModel = new MSAModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
    else if ( stringClass == "SLModel" )
    {
        SLModel *pModel;
        p = pModel = new SLModel( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
    }
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

QMenu *SAFile::getContextMenu( QWidget *pParent )
{
    // build from scratch 
    // alt would be remove menu items added by AWObject and DFile
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":DA/ObjectNew" ), tr( "New Object..." ), this, SLOT( slotNewDocument() ) );
    pMenu->addAction( QPixmap( ":DR/Stencils" ), tr( "New Stencils Folder" ), this, SLOT( slotNewStencils() ) );
    pMenu->addAction( QPixmap( ":DATAW/NewConnection" ), tr( "New Data Connection" ), this, SLOT( slotNewDataConnection() ) );
    if ( getObjects( QString(), QString(), "DATAWQuerySystem" ).isEmpty() ) 
        pMenu->addAction(QPixmap(":DATAW/ODBC"), tr("Add Data Source Browser"), this, SLOT(slotNewDataSourceBrowser()));
    pMenu->addSeparator();
    if ( isModified() )
    { 
        pMenu->addAction( QPixmap( ":W/DocumentFileSave" ), tr( "Save" ), this, SLOT( slotSave() ) );
    }
    pMenu->addAction( QPixmap( ":W/saveas" ), tr( "Save As..." ), this, SLOT( slotSaveAs() ) );
    pMenu->addAction( QPixmap( ":W/Validate" ), tr( "Validate" ), this, SLOT( slotValidate() ) );
    pMenu->addAction( QPixmap( ":W/DocumentFileClose" ), tr( "Close" ), this, SLOT( slotClose() ) );
    pMenu->addSeparator();
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Class..."), this, SLOT(slotClassDialog()) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );

    return pMenu; // caller invokes and then deletes
}

// just so we can unprotect method
void SAFile::doClear()
{
    DFile::doClear();
}

bool SAFile::slotNewDocument()
{
    SANewDialog dialog( getEditorWidgetParent() );
    if ( dialog.exec() != QDialog::Accepted ) return false;
    QString stringClass = dialog.getSelected();

    // SANITY CHECKS
    if ( stringClass.isEmpty() ) return false;
    ADObject *pObject = getObject( stringClass );
    Q_ASSERT( pObject );
    Q_ASSERT( pObject->inherits( "AWObject" ) );

    AWObject *pDocument = (AWObject*)pObject;
    if ( pDocument->inherits( "ODBCDrvModel" ) ) 
    { 
        ODBCDrvModel *p = (ODBCDrvModel*)pDocument;
        if ( !p->slotQueryForFeatures() )
        {
            p->slotDelete();
            return false;
        }
    }
    pDocument->slotOpenEditor();
    pDocument->slotObjectDialog( getEditorWidgetParent() );

    return true;
}

void SAFile::slotNewDataConnection()
{
    DATAWConnection *p = (DATAWConnection*)getObject( "SAConnection" );
    if ( !p ) return;

    p->slotObjectDialog( getEditorWidgetParent() );
    p->slotConnectWizard();
}

void SAFile::slotNewDataSourceBrowser()
{
    if ( !getObjects( QString(), QString(), "DATAWQuerySystem" ).isEmpty() ) return;
    getObject( "DATAWQuerySystem" );
}

void SAFile::slotCreatePhysicalModel( LMModel *pLogicalModel )
{
    SANewDialog dialog( getEditorWidgetParent(), true );
    if ( dialog.exec() != QDialog::Accepted ) return;
    QString stringClass = dialog.getSelected();
    if ( stringClass.isEmpty() ) return;
    PMModel *pPhysicalModel = (PMModel*)getObject( stringClass );
    if ( pPhysicalModel->inherits( "ODBCDrvModel" ) ) 
    { 
        ODBCDrvModel *p = (ODBCDrvModel*)pPhysicalModel;
        if ( !p->slotQueryForFeatures() )
        {
            p->slotDelete();
            return;
        }
    }
    pPhysicalModel->slotOpenEditor();
    pPhysicalModel->doLoad( pLogicalModel );
}


