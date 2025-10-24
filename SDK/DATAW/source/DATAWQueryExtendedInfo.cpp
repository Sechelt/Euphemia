#include "LibInfo.h"
#include "DATAWQueryExtendedInfo.h"

#include <DATAConnection.h>

#include "DATAWQueryFolder.h"
#include "DATAWQueryEIConversion.h"
#include "DATAWQueryEIDBMS.h"
#include "DATAWQueryEIDataSource.h"
#include "DATAWQueryEIDriver.h"
#include "DATAWQueryEIFunctions.h"
#include "DATAWQueryEILimits.h"
#include "DATAWQueryEISupported.h"
#include "DATAWQueryEIDataTypes.h"
#include "DATAWQueryEICallLevelInterface.h"

DATAWQueryExtendedInfo::DATAWQueryExtendedInfo( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Extended Info" ) )
{
    this->pConnection        = pConnection;

    mapMeta.insert( "DATAWQueryEIConversion", ADMeta( QIcon(), "DATAWQueryEIConversion", "Conversion" ) );
    mapMeta.insert( "DATAWQueryEIDBMS", ADMeta( QIcon(), "DATAWQueryEIDBMS", "DBMS" ) );
    mapMeta.insert( "DATAWQueryEIDataSource", ADMeta( QIcon(), "DATAWQueryEIDataSource", "DataSource" ) );
    mapMeta.insert( "DATAWQueryEIDriver", ADMeta( QIcon(), "DATAWQueryEIDriver", "Driver" ) );
    mapMeta.insert( "DATAWQueryEIFunctions", ADMeta( QIcon(), "DATAWQueryEIFunctions", "Functions" ) );
    mapMeta.insert( "DATAWQueryEILimits", ADMeta( QIcon(), "DATAWQueryEILimits", "Limits" ) );
    mapMeta.insert( "DATAWQueryEISupported", ADMeta( QIcon(), "DATAWQueryEISupported", "Supported" ) );
    mapMeta.insert( "DATAWQueryEIDataTypes", ADMeta( QIcon(), "DATAWQueryEIDataTypes", "DataTypes" ) );
    mapMeta.insert( "DATAWQueryEICallLevelInterface", ADMeta( QIcon(), "DATAWQueryEICallLevelInterface", "CallLevelInterface" ) );
}

DATAWQueryExtendedInfo::~DATAWQueryExtendedInfo()
{
}

ADObject *DATAWQueryExtendedInfo::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryEIConversion" )        
        p = new DATAWQueryEIConversion( this, pConnection );
    else if ( stringClass == "DATAWQueryEIDBMS" )        
        p = new DATAWQueryEIDBMS( this, pConnection );
    else if ( stringClass == "DATAWQueryEIDataSource" )        
        p = new DATAWQueryEIDataSource( this, pConnection );
    else if ( stringClass == "DATAWQueryEIDriver" )        
        p = new DATAWQueryEIDriver( this, pConnection );
    else if ( stringClass == "DATAWQueryEIFunctions" )        
        p = new DATAWQueryEIFunctions( this, pConnection );
    else if ( stringClass == "DATAWQueryEILimits" )        
        p = new DATAWQueryEILimits( this, pConnection );
    else if ( stringClass == "DATAWQueryEISupported" )        
        p = new DATAWQueryEISupported( this, pConnection );
    else if ( stringClass == "DATAWQueryEIDataTypes" )        
        p = new DATAWQueryEIDataTypes( this, pConnection );
    else if ( stringClass == "DATAWQueryEICallLevelInterface" )        
        p = new DATAWQueryEICallLevelInterface( this, pConnection );

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

void DATAWQueryExtendedInfo::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    getObject( "DATAWQueryEIConversion" );        
    getObject( "DATAWQueryEIDBMS" );              
    getObject( "DATAWQueryEIDataSource" );        
    getObject( "DATAWQueryEIDriver" );            
    getObject( "DATAWQueryEIFunctions" );         
    getObject( "DATAWQueryEILimits" );            
    getObject( "DATAWQueryEISupported" );         
    getObject( "DATAWQueryEIDataTypes" );         
    getObject( "DATAWQueryEICallLevelInterface" );

    bQueried = true;
    bLoadingChildren = false;
}



