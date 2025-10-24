#include "LibInfo.h"
#include "DATAWQueryEIDBMS.h"

#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIDBMS::DATAWQueryEIDBMS( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "DBMS" ) )
{
    this->pConnection        = pConnection;
    stringComment = tr( "Information about the DBMS product, such as the DBMS name and version." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIDBMS::~DATAWQueryEIDBMS()
{
}

ADObject *DATAWQueryEIDBMS::getObject( const QString &s, ADObject * )
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

void DATAWQueryEIDBMS::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    ADObject *  pChild;
    QVariant    v;
    QString     stringComment;

    pChild = getObject( "DATAWQueryExtendedInfoItem" );
    pChild->setName( "SQL_DATABASE_NAME" );
    pChild->setComment( tr("current database in use") );                                         
    v = pConnection->getInfo( SQL_DATABASE_NAME );                                                  
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               

    pChild = getObject( "DATAWQueryExtendedInfoItem" );
    pChild->setName( "SQL_DBMS_NAME" );
    pChild->setComment( tr("name of database management system") );                                         
    v = pConnection->getInfo( SQL_DBMS_NAME );                                                  
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               

    pChild = getObject( "DATAWQueryExtendedInfoItem" );
    pChild->setName( "SQL_DBMS_VER" );
    pChild->setComment( tr("version of database management system") );  
    v = pConnection->getInfo( SQL_DBMS_VER );                                                  
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               

    setModified( false ); // self and all of our descendents

    bQueried = true;
    bLoadingChildren = false;
}



