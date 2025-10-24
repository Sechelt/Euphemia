#include "LibInfo.h"
#include "DATAWQueryForeignKeys.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryForeignKey.h"

DATAWQueryForeignKeys::DATAWQueryForeignKeys( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent, tr( "ForeignKeys" ) )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryForeignKey", ADMeta( QIcon(), "DATAWQueryForeignKey", "ForeignKey" ) );
}

DATAWQueryForeignKeys::~DATAWQueryForeignKeys()
{
}

QPixmap DATAWQueryForeignKeys::getIcon()
{
    return QPixmap( ":DATAW/ForeignKey" );
}

ADObject *DATAWQueryForeignKeys::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryForeignKey" )        
        p = new DATAWQueryForeignKey( this, pConnection, stringCatalog, stringSchema, stringTable );           

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

void DATAWQueryForeignKeys::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAStatement *pStatement = pConnection->getForeignKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        // some data sources (ie MS Access) do not support foreign keys
        printf( "[PAH][%s][%s][%d] Failed to getForeignKeys.\n", __FILE__, __FUNCTION__, __LINE__ );
        bLoadingChildren = false;
        return;
    }

    QList<QString> listFKs;
    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringFKName = pStatement->getData( 12 ).toString();
        QString stringPKName = pStatement->getData( 13 ).toString();
        if ( !listFKs.contains( stringFKName ) )
        {
            listFKs << stringFKName;
            ADObject *p = getObject( "DATAWQueryForeignKey" );
            p->setName( stringFKName );
            p->setComment( stringPKName );
            p->setModified( false );
        }
        n = pStatement->doFetch();
    }
    delete pStatement;

    bLoadingChildren = false;
    bQueried = true;
}



