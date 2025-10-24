#include "LibInfo.h"
#include "DATAWQueryIndexs.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryIndex.h"

DATAWQueryIndexs::DATAWQueryIndexs( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent, tr( "Indexs" ) )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryIndex", ADMeta( QIcon(), "DATAWQueryIndex", "Index" ) );
}

DATAWQueryIndexs::~DATAWQueryIndexs()
{
}

QPixmap DATAWQueryIndexs::getIcon()
{
    return QPixmap( ":DATAW/Index" );
}

ADObject *DATAWQueryIndexs::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryIndex" )        
        p = new DATAWQueryIndex( this, pConnection, stringCatalog, stringSchema, stringTable );           

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

void DATAWQueryIndexs::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAStatement *pStatement = pConnection->getIndexs( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        bLoadingChildren = false;
        return;
    }

    QString stringIndexNamePrev;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringUnique        = pStatement->getData( 4 ).toString();
        QString stringIndexName     = pStatement->getData( 6 ).toString();

        QString stringDescription;
        if ( !stringUnique.isEmpty() && stringUnique.toUInt() ) stringDescription = "UNIQUE"; 

        if ( stringIndexName.isEmpty() ) stringIndexName = tr( "Unknown" );

        if ( stringIndexName != stringIndexNamePrev )
        {
            stringIndexNamePrev = stringIndexName;
            ADObject *p = getObject( "DATAWQueryIndex" );
            p->setName( stringIndexName );
            p->setComment( stringDescription );
            p->setModified( false );
        }

        // carry on
        n = pStatement->doFetch();
    }

    delete pStatement;

    bQueried = true;
    bLoadingChildren = false;
}




