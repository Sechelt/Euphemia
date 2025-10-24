#include "LibInfo.h"
#include "DATAWQueryForeignKey.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryColumn.h"

DATAWQueryForeignKey::DATAWQueryForeignKey( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent, tr( "ForeignKey" ) )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryColumn", ADMeta( QIcon(), "DATAWQueryColumn", "Column" ) );
}

DATAWQueryForeignKey::~DATAWQueryForeignKey()
{
}

QPixmap DATAWQueryForeignKey::getIcon()
{
    return QPixmap( ":DATAW/ForeignKey" );
}

ADObject *DATAWQueryForeignKey::getObject( const QString &s, ADObject * )
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

void DATAWQueryForeignKey::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAStatement *pStatement = pConnection->getForeignKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        bLoadingChildren = false;
        return;
    }

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringPKTable   = pStatement->getData( 3 ).toString();
        QString stringPKColumn  = pStatement->getData( 4 ).toString();
        QString stringFKColumn  = pStatement->getData( 8 ).toString();
        QString stringSeq       = pStatement->getData( 9 ).toString();
        QString stringFKName    = pStatement->getData( 12 ).toString();
        if ( stringFKName == getName() )
        {
            ADObject *p = getObject( "DATAWQueryColumn" );
            p->setName( stringFKColumn );
            p->setComment( stringPKTable + "." + stringPKColumn + " (" + stringSeq + ")" );
            p->setModified( false );
        }

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;

    bLoadingChildren = false;
    bQueried = true;
}



