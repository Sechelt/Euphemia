#include "AppInfo.h"
#include "SAConnection.h"

#include <DGraphicsSceneProxy.h>

#include <DATAWQueryCatalogs.h>
#include <DATAWQueryCatalog.h>
#include <DATAWSql.h>
#include <FRForm.h>
#include <LMModel.h>
#include <SQL92Model.h>
#include <ODBCMinModel.h>
#include <ODBCCorModel.h>
#include <ODBCExtModel.h>
#include <ODBCDrvModel.h>
#include <MSAModel.h>
#include <SLModel.h>

SAConnection::SAConnection( AWObject *pParent, const QString &stringName )
    : DATAWConnection( pParent, stringName )
{
    //
    mapMeta.insert( "FRForm", ADMeta( QIcon(), "FRForm", "Form" ) );
    // LM
    mapMeta.insert( "LMModel", ADMeta( QIcon(), "LMModel", "ER: Logical Model" ) );
    // all supported PM's
    mapMeta.insert( "SQL92Model", ADMeta( QIcon(), "SQL92Model", "ER: SQL92" ) );
    mapMeta.insert( "ODBCMinModel", ADMeta( QIcon(), "ODBCMinModel", "ER: ODBC Minimum" ) );
    mapMeta.insert( "ODBCCorModel", ADMeta( QIcon(), "ODBCCorModel", "ER: ODBC Core" ) );
    mapMeta.insert( "ODBCExtModel", ADMeta( QIcon(), "ODBCExtModel", "ER: ODBC Extended" ) );
    mapMeta.insert( "ODBCDrvModel", ADMeta( QIcon(), "ODBCDrvModel", "ER: ODBC Driver" ) );
    mapMeta.insert( "MSAModel", ADMeta( QIcon(), "MSAModel", "ER: MS Access" ) );
    mapMeta.insert( "SLModel", ADMeta( QIcon(), "SLModel", "ER: SQLite" ) );
    // other
    mapMeta.insert( "DATAWQueryCatalogs", ADMeta( QIcon(), "DATAWQueryCatalogs", "Catalogs" ) );
    mapMeta.insert( "DATAWQueryCatalog", ADMeta( QIcon(), "DATAWQueryCatalog", "Catalog" ) );
    mapMeta.insert( "DATAWSql", ADMeta( QIcon(), "DATAWSql", "SQL" ) );
}

SAConnection::~SAConnection()
{
}

ADObject *SAConnection::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryCatalogs" )    
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DATAWQueryCatalogs" ).count() )
        {
            doMessageBox( "INFO", tr("New Query Catalogs"), tr("Already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DATAWQueryCatalogs( this, pConnection );           
    }
    else if ( stringClass == "DATAWQueryCatalog" )    
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DATAWQueryCatalog" ).count() )
        {
            doMessageBox( "INFO", tr("New Query Catalog"), tr("Already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DATAWQueryCatalog( this, pConnection );           
    }
    else if ( stringClass == "FRForm" )
    {
        FRForm *pModel;
        p = pModel = new FRForm( this );
        pModel->getValidationConfiguration();
        pModel->setScene( new DGraphicsSceneProxy( pModel, QRectF( 0, 0, 1000, 1000 ) ) );
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
    else if ( stringClass == "DATAWSql" )        
        p = new DATAWSql( this );           

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );

    return p;
}

QMenu *SAConnection::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu   = DATAWConnection::getContextMenu( pParent );
    QAction *   pAction;

    // add our stuff at the top of the menu = reverse order
    pAction = pMenu->insertSeparator( pMenu->actions().first() );

    pAction = new QAction( QIcon( ":FR/Form" ), tr( "New Form" ), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotNewForm()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    if ( isConnected() )
    {
        // we want a connection so we can know what vendor/product we are dealing with
        // the User can change the connection later but we ignore that
        pAction = new QAction( QIcon( ":PM/DatabaseReverse" ), tr( "New ERD From Data Source" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotNewERDFromDataSource()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );

        pAction = new QAction( QIcon( ":PM/PhysicalModel" ), tr( "New ERD" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotNewERD()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    pAction = new QAction( QIcon(), tr( "Import..." ), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotImport()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    return pMenu;
}

void SAConnection::slotNewForm()
{
    FRForm *p = (FRForm *)getObject( "FRForm" );
    if ( !p ) return;
    p->slotOpenEditor();
    p->slotObjectDialog( getEditorWidgetParent() );
}

void SAConnection::slotNewERD()
{
    QString stringDBMS_NAME = pConnection->getInfoStr( SQL_DBMS_NAME );
    QString stringClass;

    if ( stringDBMS_NAME.isEmpty() )
    {
        doMessageBox( "ERROR", tr("New ERD"), tr("Unable to determine SQL_DBMS_NAME.") );
        return;
    }
    else if ( stringDBMS_NAME.contains( "sqlite", Qt::CaseInsensitive ) )
    {
        stringClass = "SLModel";
    }
    else if ( stringDBMS_NAME.contains( "MariaDB", Qt::CaseInsensitive ) )
    {
        doMessageBox( "INFO", tr("New ERD"), tr("SQL_DBMS_NAME: ") + stringDBMS_NAME + "\nUsing SQL92 standard for ERD." );
        stringClass = "SQL92Model";
    }
    else
    {
        doMessageBox( "ERROR", tr("New ERD"), tr("Unknown SQL_DBMS_NAME: ") + stringDBMS_NAME  );
        return;
    }

    PMModel *pPhysicalModel = (PMModel *)getObject( stringClass );
    if ( !pPhysicalModel ) return;
    pPhysicalModel->slotOpenEditor();
    pPhysicalModel->slotObjectDialog( getEditorWidgetParent() );
}

void SAConnection::slotNewERDFromDataSource()
{
    QString stringDBMS_NAME = pConnection->getInfoStr( SQL_DBMS_NAME );
    QString stringClass;

    if ( stringDBMS_NAME.isEmpty() )
    {
        doMessageBox( "ERROR", tr("New ERD"), tr("Unable to determine SQL_DBMS_NAME.") );
        return;
    }
    else if ( stringDBMS_NAME.contains( "SQLite", Qt::CaseInsensitive ) )
    {
        stringClass = "SLModel";
    }
    else if ( stringDBMS_NAME.contains( "ACCESS", Qt::CaseInsensitive ) )
    {
        QString s = tr( "References\n\n" ) +
                    "MS Access ODBC driver does not support SQLPrimaryKeys nor SQLForeignKeys but this information can be found by other means.\n\n" +
                    "* PrimaryKey is derived from a unique index called 'PrimaryKey'.\n" + 
                    "* ForeignKeys are sometimes found in the MSysRelationships system table.\n\n" + 
                    "Gaining access to MSysRelationships requires some manual steps within the MS Access application.\n" + 
                    "This should be done BEFORE generating an ERD from the data source.\n\n" + 
                    "1. Make system tables visible. This is done by changing 'Navigation Options'.\n" + 
                    "2. Make a copy of MSysRelationships called MSysRelationshipsCopy.\n\n" +
                    "Continue?"; 
        QMessageBox::StandardButton n = QMessageBox::question( nullptr, tr("Reverse Engineer..."), s );
        if ( n != QMessageBox::Yes ) return;
        stringClass = "MSAModel";
    }
    else if ( stringDBMS_NAME.contains( "MySQL", Qt::CaseInsensitive ) )
    {
        doMessageBox( "INFO", tr("New ERD"), tr("SQL_DBMS_NAME: ") + stringDBMS_NAME + "\nUsing SQL92 standard for ERD." );
        stringClass = "SQL92Model";
    }
    else if ( stringDBMS_NAME.contains( "MariaDB", Qt::CaseInsensitive ) )
    {
        doMessageBox( "INFO", tr("New ERD"), tr("SQL_DBMS_NAME: ") + stringDBMS_NAME + "\nUsing SQL92 standard for ERD." );
        stringClass = "SQL92Model";
    }
    else
    {
        doMessageBox( "ERROR", tr("New ERD"), tr("Unknown SQL_DBMS_NAME: ") + stringDBMS_NAME  );
        return;
    }

    PMModel *pPhysicalModel = (PMModel*)getObject( stringClass );
    if ( !pPhysicalModel ) return;
    pPhysicalModel->slotOpenEditor();
    pPhysicalModel->slotReverseEngineer();
}

void SAConnection::slotImport()
{
    QString stringFileName;
    QString stringSQL;

    // load file
    {
        // get file name                                                                                                                                                                                                                      
        stringFileName = QFileDialog::getOpenFileName( getEditorWidgetParent(), tr("Import..."), QString(), "SQL (*.sql)" );                                                                                                                                      
        if ( stringFileName.isEmpty() ) return;                                                                                                                                                                                                                     

        QFile file( stringFileName );
        if ( !file.open( QFile::ReadOnly | QFile::Text ) )
        {
            doMessageBox( "ERROR", APP_NAME, tr("Could not open file.\n\n") + file.fileName() );
            return;
        }

        QTextStream in( &file );
        stringSQL = in.readAll();

        file.close();
    }

    // create DATAWSql
    DATAWSql *p = (DATAWSql*)getObject( "DATAWSql" );
    Q_ASSERT( p );
    p->setName( stringFileName );
    p->setSQL( stringSQL );
    p->slotOpenEditor();
}


