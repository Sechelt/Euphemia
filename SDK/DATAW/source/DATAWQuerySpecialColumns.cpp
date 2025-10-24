#include "LibInfo.h"
#include "DATAWQuerySpecialColumns.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryColumn.h"

DATAWQuerySpecialColumns::DATAWQuerySpecialColumns( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent, tr( "SpecialColumns" ) )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryColumn", ADMeta( QIcon(), "DATAWQueryColumn", "Column" ) );
}

DATAWQuerySpecialColumns::~DATAWQuerySpecialColumns()
{
}

QPixmap DATAWQuerySpecialColumns::getIcon()
{
    return QPixmap( ":DATAW/ID" );
}

ADObject *DATAWQuerySpecialColumns::getObject( const QString &s, ADObject * )
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

void DATAWQuerySpecialColumns::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAStatement *pStatement = pConnection->getSpecialColumns( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        bLoadingChildren = false;
        return;
    }

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringColumn = pStatement->getData( 2 ).toString();

        if ( stringColumn.isEmpty() ) stringColumn = tr( "Unknown" );

        ADObject *p = getObject( "DATAWQueryColumn" );
        p->setName( stringColumn );
        p->setModified( false );
        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;

    bLoadingChildren = false;
    bQueried = true;
}


