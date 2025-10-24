#include "LibInfo.h"
#include "DATAWQueryEIDriver.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIDriver::DATAWQueryEIDriver( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Driver" ) )
{
    this->pConnection        = pConnection;

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIDriver::~DATAWQueryEIDriver()
{
}

ADObject *DATAWQueryEIDriver::getObject( const QString &s, ADObject *p )
{
    ADObject *pParent   = ( p ? p : this ); 
    AWObject *pChild    = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryExtendedInfoItem" )        
        pChild = new DATAWQueryExtendedInfoItem( pParent );

    if ( !pChild )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit pParent->signalCreated( pChild );    
    emit pParent->signalCreated( pParent, pChild );

    return pChild;
}

void DATAWQueryEIDriver::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    doQueryDetails( "SQL_ACTIVE_ENVIRONMENTS" );       
    doQueryDetails( "SQL_ASYNC_MODE" );                
    doQueryDetails( "SQL_DATABASE_NAME" );             
    doQueryDetails( "SQL_BATCH_ROW_COUNT" );           
    doQueryDetails( "SQL_BATCH_SUPPORT" );             
    doQueryDetails( "SQL_DATA_SOURCE_NAME" );          
    doQueryDetails( "SQL_DRIVER_HDBC" );               
    doQueryDetails( "SQL_DRIVER_HENV" );               
    doQueryDetails( "SQL_DRIVER_HLIB" );               
    doQueryDetails( "SQL_DRIVER_NAME" );               
    doQueryDetails( "SQL_DRIVER_ODBC_VER" );           
    doQueryDetails( "SQL_DRIVER_VER" );                
    doQueryDetails( "SQL_DYNAMIC_CURSOR_ATTRIBUTES1" );
    doQueryDetails( "SQL_DYNAMIC_CURSOR_ATTRIBUTES2" );
    doQueryDetails( "SQL_INFO_SCHEMA_VIEWS" );         
    doQueryDetails( "SQL_ODBC_INTERFACE_CONFORMANCE" );
    doQueryDetails( "SQL_ODBC_VER" );                  
    doQueryDetails( "SQL_SERVER_NAME" );               

    setModified( false );  // self and all of our descendents
    bQueried = true;
    bLoadingChildren = false;
}

void DATAWQueryEIDriver::doQueryDetails( const QString &s )
{
    ADObject *pParent;                  
    ADObject *pChild;                 
    DATAInfo *pInfoParent;                                         
    DATAInfo *pInfoChild;                                        
                                                        
    pInfoParent = pConnection->getProfile()->mapInfo.value( s );          
    pParent = getObject( "DATAWQueryExtendedInfoItem" );      
    pParent->setName( pInfoParent->stringInfo );                  
    pParent->setCode( pInfoParent->vValue.toString() );           
    pParent->setComment( pInfoParent->stringDescription );           
                                                        
    foreach( pInfoChild, pInfoParent->mapValues )                          
    {                                                   
        pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );      
        pChild->setName( pInfoChild->stringInfo );                  
        pChild->setCode( pInfoChild->vValue.toString() );           
        pChild->setComment( pInfoChild->stringDescription );           
    }                                                   
}


