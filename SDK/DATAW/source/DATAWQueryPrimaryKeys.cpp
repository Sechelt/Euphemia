#include "LibInfo.h"
#include "DATAWQueryPrimaryKeys.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryColumn.h"

DATAWQueryPrimaryKeys::DATAWQueryPrimaryKeys( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWQuery( pParent, tr( "PrimaryKey" ) )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;

    mapMeta.insert( "DATAWQueryColumn", ADMeta( QIcon(), "DATAWQueryColumn", "Column" ) );
}

DATAWQueryPrimaryKeys::~DATAWQueryPrimaryKeys()
{
}

QPixmap DATAWQueryPrimaryKeys::getIcon()
{
    return QPixmap( ":DATAW/PrimaryKey" );
}

ADObject *DATAWQueryPrimaryKeys::getObject( const QString &s, ADObject * )
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

void DATAWQueryPrimaryKeys::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    // is SQLPrimaryKeys supported?
    DATAStatement *pStatement = pConnection->getPrimaryKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        // NO!
        printf( "[PAH][%s][%s][%d] Failed to getPrimaryKeys. Looking for index named PrimaryKey.\n", __FILE__, __FUNCTION__, __LINE__ );

        // MS Access - may return an index named "PrimaryKey" - we can use that 
        DATAStatement *pStatement = pConnection->getIndexs( stringCatalog, stringSchema, stringTable );
        if ( !pStatement ) 
        {
            printf( "[PAH][%s][%s][%d] Failed to getIndexs.\n", __FILE__, __FUNCTION__, __LINE__ );
            bLoadingChildren = false;
            return;
        }

        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            QString stringIndexName = pStatement->getData( 6 ).toString();
            if ( stringIndexName == "PrimaryKey" )
            {
                ADObject *p = getObject( "DATAWQueryColumn" );
                p->setName( pStatement->getData( 9 ).toString() );
                p->setModified( false );
            }
            n = pStatement->doFetch();
        }
        delete pStatement;

        bLoadingChildren = false;
        bQueried = true;
        return;
    }

    // YES!
    // fetch primarykeys
    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringColumn = pStatement->getData( 4 ).toString();
        QString stringSeq = pStatement->getData( 5 ).toString();
        QString stringKeyName = pStatement->getData( 6 ).toString();

        QString stringDescription;
        if ( !stringKeyName.isEmpty() ) stringDescription = stringKeyName + ": ";
        if ( !stringSeq.isEmpty() ) stringDescription += "(" + stringSeq + ") ";
        if ( !stringColumn.isEmpty() ) stringDescription += stringColumn;

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



