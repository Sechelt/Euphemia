#include "LibInfo.h"
#include "DATAWQueryIndex.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryColumn.h"

DATAWQueryIndex::DATAWQueryIndex( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryColumn", ADMeta( QIcon(), "DATAWQueryColumn", "Column" ) );
}

DATAWQueryIndex::~DATAWQueryIndex()
{
}

QPixmap DATAWQueryIndex::getIcon()
{
    return QPixmap( ":DATAW/Index" );
}

ADObject *DATAWQueryIndex::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryColumn" )        
        p = new DATAWQueryColumn( this );

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

void DATAWQueryIndex::doQuery()
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

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringIndexName = pStatement->getData( 6 ).toString();
        QString stringColumnName = pStatement->getData( 9 ).toString();

        if ( stringIndexName == getName() ) 
        {
            ADObject *p = getObject( "DATAWQueryColumn" );
            p->setName( stringColumnName );
            p->setModified( false );
        }

        // carry on
        n = pStatement->doFetch();
    }

    delete pStatement;

    bQueried = true;
    bLoadingChildren = false;
}



