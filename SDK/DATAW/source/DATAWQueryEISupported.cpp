#include "LibInfo.h"
#include "DATAWQueryEISupported.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEISupported::DATAWQueryEISupported( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Supported SQL" ) )
{
    this->pConnection        = pConnection;
    stringComment = tr( "Information about the SQL statements supported by the data source." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEISupported::~DATAWQueryEISupported()
{
}

ADObject *DATAWQueryEISupported::getObject( const QString &s, ADObject *p )
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

void DATAWQueryEISupported::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    doQueryDetails( "SQL_PROCEDURES" );
    doQueryDetails( "SQL_SUBQUERIES" );

    setModified( false );  // self and all of our descendents
    bQueried = true;
    bLoadingChildren = false;
}


void DATAWQueryEISupported::doQueryDetails( const QString &s )
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

