#include "LibInfo.h"
#include "DATAWQueryConnection.h"

#include <DATASystem.h>
#include <DATAEnvironment.h>
#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATAProfileWidget.h>
#include <DATAProfile.h>

#include "DATAWQueryCatalog.h"
#include "DATAWQueryExtendedInfo.h"

DATAWQueryConnection::DATAWQueryConnection( ADObject *pParent, DATAEnvironment *pEnvironment )
    : DATAWQuery( pParent )
{
    setObjectName( "DATAWQueryConnection" );

    this->pEnvironment      = pEnvironment;
    bAllCatalogs            = false;
    pConnection             = new DATAConnection( pEnvironment );

    if ( !pConnection->isAlloc( false ) ) pConnection->doAlloc();

    SQLRETURN nReturn;

    // be as flexible as possible with where we get our cursor from
#ifndef Q_OS_WIN
    nReturn = pConnection->setAttrOdbcCursors( DATAConnection::CurUseIfNeeded );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        printf( "[PAH][%s][%s][%d] Failed to set DATAConnection::CurUseIfNeeded.\n", __FILE__, __FUNCTION__, __LINE__ );
    }
#endif

    // this is the default but ensure that it is in effect
    nReturn = pConnection->setAttrAutocommit( DATAConnection::AutocommitOn );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        printf( "[PAH][%s][%s][%d] Failed to set DATAConnection::AutocommitOn.\n", __FILE__, __FUNCTION__, __LINE__ );
    }

    connect( pConnection, SIGNAL(signalConnected()), this, SLOT(slotConnected()) );
    connect( pConnection, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()) );

    mapMeta.insert( "DATAWQueryCatalog", ADMeta( QIcon(), "DATAWQueryCatalog", "Catalog" ) );
    mapMeta.insert( "DATAWQueryExtendedInfo", ADMeta( QIcon(), "DATAWQuerySchema", "ExtendedInfo" ) );
}

DATAWQueryConnection::~DATAWQueryConnection()
{
printf( "[%s][%s][%d] hopefully got here?\n", __FILE__, __FUNCTION__, __LINE__ );
    // delete all children
    doClear();
    // try it the nice way
    if ( isConnected() ) slotDisconnect();
    // free it up
    if ( pConnection ) delete pConnection;
    pConnection = nullptr;
}

QPixmap DATAWQueryConnection::getIcon()
{
    if ( pConnection->isConnected() ) return QPixmap( ":DATAW/Connected" );
    return QPixmap( ":DATAW/Disconnected" );
}

ADObject *DATAWQueryConnection::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryCatalog" )        
        p = new DATAWQueryCatalog( this, pConnection );           
    else if ( stringClass == "DATAWQueryExtendedInfo" )        
        p = new DATAWQueryExtendedInfo( this, pConnection );           

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

QMenu *DATAWQueryConnection::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );
                                                                                                               
    if ( isConnected() )                                                                                       
    {                                                                                                          
        pMenu->addAction( QPixmap( ":DATAW/Disconnected" ), "&Disconnect", this, SLOT(slotDisconnect()) );          
        pMenu->addSeparator();                                                                                   
        pMenu->addAction( "Generate Profile In Paste Buffer", this, SLOT(slotGenerateProfile()) );               
        // pMenu->addAction( QPixmap( ":DATAW/Properties" ), "Properties...", this, SLOT(slotProperties()) );          
    }                                                                                                          
    else                                                                                                       
    {                                                                                                          
        pMenu->addAction( QPixmap( ":DATAW/Connected" ), "&Connect...", this, SLOT(slotConnect()) );                
                                                                                                               
        QAction *pActionAllCatalogs = new QAction( QPixmap( ":DATAW/ClosedCatalog" ), tr("All Catalogs"), pMenu );
        pActionAllCatalogs->setCheckable( true );                                                              
        pActionAllCatalogs->setChecked( bAllCatalogs );                                                        
        pMenu->addAction( pActionAllCatalogs );                                                                  
        connect( pActionAllCatalogs, SIGNAL(toggled(bool)), SLOT(slotAllCatalogs(bool)) );                     
    }                                                                                                          
    pMenu->addAction( QPixmap( ":DATAW/Refresh" ), tr("Refresh"), this, SLOT(slotRefresh()) );
    pMenu->addAction( QPixmap( ":DATAW/Delete" ), "Delete", this, SLOT(slotDelete()) );                             
                                                                                                               
    return pMenu; // caller invokes and then deletes
}

AWPropWidget *DATAWQueryConnection::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DATAWQuery::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Data Source Profile"), new DATAProfileWidget( pConnection->getProfile(), pPropWidget ) );

    return pPropWidget;
}

bool DATAWQueryConnection::isConnected()
{
    if ( !pConnection ) return false;
    return pConnection->isConnected();
}

QStringList DATAWQueryConnection::getAutoCompletions()
{
    QStringList l;
    if ( !isConnected() ) return l;

/*
    if ( !childCount() ) doLoadChildren();                                                                                                
    QList<DATAWQuery*> listItems_1 = getChildren( "DATAWQueryCatalog" );                                                                    
    DATAWQuery *pItem_1;                                                                                                                   
    foreach( pItem_1, listItems_1 )                                                                                                       
    {                                                                                                                                     
        QString stringCatalog = pItem_1->text( 0 );                                                                                       
        if ( !pItem_1->childCount() ) pItem_1->doLoadChildren();                                                                          
        QList<DATAWQuery*> listItems_2 = pItem_1->getChildren( "DATAWQuerySchema" );                                                        
        DATAWQuery *pItem_2;                                                                                                               
        foreach( pItem_2, listItems_2 )                                                                                                   
        {                                                                                                                                 
            QString stringSchema = pItem_2->text( 0 );                                                                                    
            if ( !pItem_2->childCount() ) pItem_2->doLoadChildren();                                                                      
            QList<DATAWQuery*> listItems_3 = pItem_2->getChildren( "DATAWQueryTableType" );                                                 
            DATAWQuery *pItem_3;                                                                                                           
            foreach( pItem_3, listItems_3 )                                                                                               
            {                                                                                                                             
                if ( !pItem_3->childCount() ) pItem_3->doLoadChildren();                                                                  
                QList<DATAWQuery*> listItems_4 = pItem_3->getChildren( "DATAWQueryTable" );                                                 
                DATAWQuery *pItem_4;                                                                                                       
                foreach( pItem_4, listItems_4 )                                                                                           
                {                                                                                                                         
                    QString stringTable = pItem_4->text( 0 );                                                                             
                    if ( !pItem_4->childCount() ) pItem_4->doLoadChildren();                                                              
                    QList<DATAWQuery*> listItems_5 = pItem_4->getChildren( "DATAWQueryColumn" );                                            
                    DATAWQuery *pItem_5;                                                                                                   
                    foreach( pItem_5, listItems_5 )                                                                                       
                    {                                                                                                                     
                        QString stringColumn = pItem_5->text( 0 );                                                                        
                        if ( !stringCatalog.isEmpty() )                                                                                   
                            l << QString( "%1.%2.%3.%4" ).arg( stringCatalog ).arg( stringSchema ).arg( stringTable ).arg( stringColumn );
                        else if ( !stringSchema.isEmpty() )                                                                               
                            l << QString( "%1.%2.%3" ).arg( stringSchema ).arg( stringTable ).arg( stringColumn );                        
                        else                                                                                                              
                            l << QString( "%1.%2" ).arg( stringTable ).arg( stringColumn );                                               
                    }                                                                                                                     
                }                                                                                                                         
            }                                                                                                                             
        }                                                                                                                                 
    }                                                                                                                                     
*/

    return l;
}

QStringList DATAWQueryConnection::getAutoCompletionsFlat()
{
    QStringList l;
    if ( !isConnected() ) return l;

/*
    int nProgress = 0;                                                                                                                
    QProgressDialog progress( tr( "Gathering AutoComplete Info..." ), tr( "Abort" ), 0, 100, treeWidget() );                          
    progress.setWindowModality( Qt::WindowModal );                                                                                    
    progress.show();                                                                                                                  
                                                                                                                                      
    if ( !childCount() ) doLoadChildren();                                                                                            
    QList<DATAWQuery*> listItems_1 = getChildren( "DATAWQueryCatalog" );                                                                
    DATAWQuery *pItem_1;                                                                                                               
    foreach( pItem_1, listItems_1 )                                                                                                   
    {                                                                                                                                 
        QString stringCatalog = pItem_1->text( 0 );                                                                                   
                                                                                                                                      
        progress.setLabelText( QString( "Gathering AutoComplete Info for: %1 ..." ).arg( stringCatalog ) );                           
                                                                                                                                      
        if ( !stringCatalog.isEmpty() ) l << QString( "%1?%2" ).arg( stringCatalog ).arg( DATAConnection::AutoCatalog );              
        if ( !pItem_1->childCount() ) pItem_1->doLoadChildren();                                                                      
        QList<DATAWQuery*> listItems_2 = pItem_1->getChildren( "DATAWQuerySchema" );                                                    
        DATAWQuery *pItem_2;                                                                                                           
        foreach( pItem_2, listItems_2 )                                                                                               
        {                                                                                                                             
            QString stringSchema = pItem_2->text( 0 );                                                                                
                                                                                                                                      
            progress.setLabelText( QString( "Gathering AutoComplete Info for: %1.%2 ..." ).arg( stringCatalog ).arg( stringSchema ) );
                                                                                                                                      
            if ( !stringSchema.isEmpty() ) l << QString( "%1?%2" ).arg( stringSchema ).arg( DATAConnection::AutoSchema );             
            if ( !pItem_2->childCount() ) pItem_2->doLoadChildren();                                                                  
            QList<DATAWQuery*> listItems_3 = pItem_2->getChildren( "DATAWQueryTableType" );                                             
            DATAWQuery *pItem_3;                                                                                                       
            foreach( pItem_3, listItems_3 )                                                                                           
            {                                                                                                                         
                QString stringTableType = pItem_3->text( 0 );                                                                         
                if ( !pItem_3->childCount() ) pItem_3->doLoadChildren();                                                              
                QList<DATAWQuery*> listItems_4 = pItem_3->getChildren( "DATAWQueryTable" );                                             
                progress.setMaximum( listItems_4.count() );                                                                           
                nProgress = 0;                                                                                                        
                DATAWQuery *pItem_4;                                                                                                   
                foreach( pItem_4, listItems_4 )                                                                                       
                {                                                                                                                     
                    progress.setValue( nProgress++ );                                                                                 
                    QCoreApplication::processEvents();                                                                                
                                                                                                                                      
                    QString stringTable = pItem_4->text( 0 );                                                                         
                    if ( !stringTable.isEmpty() )                                                                                     
                    {                                                                                                                 
                        if ( stringTableType == "VIEW" )                                                                              
                            l << QString( "%1?%2" ).arg( stringTable ).arg( DATAConnection::AutoView );                               
                        else                                                                                                          
                            l << QString( "%1?%2" ).arg( stringTable ).arg( DATAConnection::AutoTable );                              
                    }                                                                                                                 
                                                                                                                                      
                    if ( !pItem_4->childCount() ) pItem_4->doLoadChildren();                                                          
                    QList<DATAWQuery*> listItems_5 = pItem_4->getChildren( "DATAWQueryColumn" );                                        
                    DATAWQuery *pItem_5;                                                                                               
                    foreach( pItem_5, listItems_5 )                                                                                   
                    {                                                                                                                 
                        QString stringColumn = pItem_5->text( 0 );                                                                    
                        if ( !stringColumn.isEmpty() ) l << QString( "%1?%2" ).arg( stringColumn ).arg( DATAConnection::AutoColumn ); 
                                                                                                                                      
                        if ( progress.wasCanceled() ) return l;                                                                       
                    }                                                                                                                 
                }                                                                                                                     
            }                                                                                                                         
        }                                                                                                                             
    }                                                                                                                                 
*/

    return l;
}

/*!
 * \brief Query 
 *  
 * Query/Refresh list of catalogs.
 * 
 * Most drivers are going to use catalogs to represent a database. 
 * We can load all catalogs or just the current one.  
 *  
 * \sa bAllCatalogs 
 *  
 * \author pharvey (1/2/21)
 */
void DATAWQueryConnection::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !isConnected() ) return;
    bLoadingChildren = true;

    // catalogs
    if ( bAllCatalogs )                                                                                        
    {                                                                                                          
        QStringList l;                                                                                         

        DATAStatement *pStatement = pConnection->getCatalogs();                                                
        if ( pStatement )                                                                                      
        {                                                                                                      
            SQLRETURN n = pStatement->doFetch();                                                               
            while ( SQL_SUCCEEDED( n ) )                                                                       
            {                                                                                                  
                QString stringCatalog = pStatement->getData( 1 ).toString();                                   
                // avoid duplicates (some drivers report duplicates)                                                                            
                if ( !l.contains( stringCatalog ) )                                                            
                {                                                                                              
                    ADObject *p = getObject( "DATAWQueryCatalog" );
                    p->setName( stringCatalog );
                    p->setModified( false );
                    l << stringCatalog;                                                                        
                }                                                                                              

                n = pStatement->doFetch();                                                                     
            }                                                                                                  
            delete pStatement;                                                                                 
        }                                                                                                      
        // ensure we have at least one                                                                         
        if ( !l.count() ) getObject( "Catalog" );
    }                                                                                                          
    else                                                                                                       
    {
        ADObject *p = getObject( "DATAWQueryCatalog" );
        p->setName( pConnection->getAttrCurrentCatalog() );
        p->setModified( false );
    }                                                                                                          

    // features supported                                                                                      
    getObject( "DATAWQueryExtendedInfo" );

    bQueried = true;
    bLoadingChildren = false;
}

void DATAWQueryConnection::slotConnect()
{
    doConnect();
}

void DATAWQueryConnection::slotDisconnect()
{
    doDisconnect();
}

void DATAWQueryConnection::slotAllCatalogs( bool b )
{
    bAllCatalogs = b;
}

// this can probably go away as the entire profiles database can be shared
void DATAWQueryConnection::slotGenerateProfile()
{
    if ( !isConnected() ) return;
    Q_ASSERT( !pConnection->getProfile() );

    QString s;

    s += "\nODBCBrowser\n";
    s += "Version=" + QString( SDK_VER ) + "\n";
    s += "Description=Driver Profile\n";
    s += "Generated=" + QDateTime::currentDateTime().toString() + "\n";

    s += "\nBuild Platform\n";
#ifdef Q_OS_LINUX
    s += "Kernel=linux\n";
    s += "KernelVersion=4.12.14-lp151.28.36-default\n";
    s += "DM=unixODBC\n";
    s += "DMVersion=2.3.6\n";
#endif
    s += "Environment=SQL_OV_ODBC3\n";
 
    s += "\nRunning Platform\n";
    s += "Kernel=" + QSysInfo::kernelType() + "\n";
    s += "KernelVersion=" + QSysInfo::kernelVersion() + "\n";
#ifdef Q_OS_LINUX
    s += "DM=unknown\n";
    s += "DMVersion=unknown\n";
#endif

    // CLI
    {
        s += "\nCLI\n";
        DATAFunction *pFunction;
        foreach( pFunction, pConnection->getProfile()->mapFunctions )
        {
            s += pFunction->stringFunction + "=" + ( pFunction->bSupported ? "Y" : "N" ) + "\n";
        }
    }

    // info
    {
        s += "\nInfo\n";                                                                 
        DATAInfo *pInfoParent;
        foreach( pInfoParent, pConnection->getProfile()->mapInfo )
        {
            s += pInfoParent->stringInfo + "=";
            if ( pInfoParent->mapValues.count() )
            {
                QString s2;
                DATAInfo *pInfoChild;
                foreach( pInfoChild, pInfoParent->mapValues )
                {
                    if ( pInfoChild->vValue.toString() == "Y" )
                    { 
                        if ( s2.isEmpty() ) s2 = pInfoChild->stringInfo;
                        else s2 += "," + pInfoChild->stringInfo;
                    }
                }
                s += s2 + "\n";
            }
            else s += pInfoParent->vValue.toString() + "\n";
        }
    }

    // data types
    {
        s += "\nDataTypes\n";                                                                 
        DATADataTypeSpec *pDataTypeSpec;                                                                    
        foreach( pDataTypeSpec, pConnection->getProfile()->mapDataTypes )                                                                        
        {                                                                                      
            s += "'" + pDataTypeSpec->TYPE_NAME + "',";                 
            s += pDataTypeSpec->DATA_TYPE + ",";                 
            s += pDataTypeSpec->COLUMN_SIZE + ",";               
            s += pDataTypeSpec->LITERAL_PREFIX + ",";            
            s += pDataTypeSpec->LITERAL_SUFFIX + ",";            
            s += "'" + pDataTypeSpec->CREATE_PARAMS + "',";             
            s += pDataTypeSpec->NULLABLE + ",";                  
            s += pDataTypeSpec->CASE_SENSITIVE + ",";            
            s += pDataTypeSpec->SEARCHABLE + ",";                
            s += pDataTypeSpec->UNSIGNED_ATTRIBUTE + ",";        
            s += pDataTypeSpec->FIXED_PREC_SCALE + ",";          
            s += pDataTypeSpec->AUTO_UNIQUE_VALUE + ",";         
            s += pDataTypeSpec->LOCAL_TYPE_NAME + ",";           
            s += pDataTypeSpec->MINIMUM_SCALE + ",";             
            s += pDataTypeSpec->MAXIMUM_SCALE + ",";             
            s += "'" + pDataTypeSpec->SQL_DATA_TYPE + "',";             
            s += pDataTypeSpec->SQL_DATETIME_SUB + ",";          
            s += pDataTypeSpec->NUM_PREC_RADIX + ",";            
            s += pDataTypeSpec->INTERVAL_PRECISION + ",";        
            s += pDataTypeSpec->stringDescription + ",";         
            s += "\n";                                           
        }                                                                                      
    }

    //
    QClipboard *pClipboard = QGuiApplication::clipboard();
    pClipboard->setText( s );

    // invoke web browser on codebydesign page for submitting a profile
    QDesktopServices::openUrl( QUrl( "https://www.codebydesign.com/ODBC/SubmitProfile" ) );
}

void DATAWQueryConnection::slotConnected()
{
    doQuery();
}

void DATAWQueryConnection::slotDisconnected()
{
    doClear();
}


void DATAWQueryConnection::doDisconnect()
{
    if ( !isConnected() ) return;

    // clear data messages
    // emit signalClear();                             
                                                    
    SQLRETURN nReturn = pConnection->doDisconnect();
    if ( !SQL_SUCCEEDED(nReturn) ) return;          
}


