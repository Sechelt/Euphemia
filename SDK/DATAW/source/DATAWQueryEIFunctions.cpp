#include "LibInfo.h"
#include "DATAWQueryEIFunctions.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIFunctions::DATAWQueryEIFunctions( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Scalar Functions" ) )
{
    this->pConnection = pConnection;
    stringComment = tr( "Information about the scalar functions supported by the data source and the driver." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIFunctions::~DATAWQueryEIFunctions()
{
}

ADObject *DATAWQueryEIFunctions::getObject( const QString &s, ADObject *p )
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

void DATAWQueryEIFunctions::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    doQueryDetails( "SQL_CONVERT_FUNCTIONS" );
    doQueryDetails( "SQL_NUMERIC_FUNCTIONS" );

    setModified( false );  // self and all of our descendents

    bQueried = true;
    bLoadingChildren = false;
}

void DATAWQueryEIFunctions::doQueryDetails( const QString &s )
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


