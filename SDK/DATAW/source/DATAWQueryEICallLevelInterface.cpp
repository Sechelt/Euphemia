#include "LibInfo.h"
#include "DATAWQueryEICallLevelInterface.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEICallLevelInterface::DATAWQueryEICallLevelInterface( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Call Level Interface (CLI)" ) )
{
    this->pConnection = pConnection;
    stringComment = tr( "List of ODBC functions supported." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEICallLevelInterface::~DATAWQueryEICallLevelInterface()
{
}

ADObject *DATAWQueryEICallLevelInterface::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryExtendedInfoItem" )        
        p = new DATAWQueryExtendedInfoItem( this );

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

void DATAWQueryEICallLevelInterface::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAFunction *pFunction;                                                               
    foreach( pFunction, pConnection->getProfile()->mapFunctions )                                                               
    {
        ADObject *p = getObject( "DATAWQueryExtendedInfoItem" );
        QString stringCompliance;                               
        switch ( pFunction->nCompliance )                                                  
        {                                                                         
            case CBD::StandardISO92:                                              
                stringCompliance = "ISO92";                                     
                break;                                                            
            case CBD::StandardOpenGroup:                                          
                stringCompliance = "OpenGroup";                                 
                break;                                                            
            case CBD::StandardODBC:                                               
                stringCompliance = "ODBC";                                      
                break;                                                            
        }                                                                         

        p->setName( pFunction->stringFunction + " (" + stringCompliance + ")"  );
        p->setCode( pFunction->bSupported ? "Y" : "N" );     
        p->setModified( false );                                         
    }     
                                                                            
    bQueried = true;
    bLoadingChildren = false;
}


