#include "LibInfo.h"
#include "DATAWQueryTableType.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryTable.h"

DATAWQueryTableType::DATAWQueryTableType( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema )
    : DATAWQuery( pParent )
{
    this->pConnection       = pConnection;
    this->stringCatalog     = stringCatalog;
    this->stringSchema      = stringSchema;

    stringComment = tr( "TABLE TYPE" );

    mapMeta.insert( "DATAWQueryTable", ADMeta( QIcon(), "DATAWQueryTable", "Table" ) );
}

DATAWQueryTableType::~DATAWQueryTableType()
{
}

QPixmap DATAWQueryTableType::getIcon( AWObject::enumIconRoles nRole )
{
    switch ( nRole )
    {
    case nIconRoleExpanded:
        return QPixmap( ":DATAW/FolderOpened" );
    case nIconRoleCollapsed:
        return QPixmap( ":DATAW/FolderClosed" );
    }
    return getIcon();
}

QPixmap DATAWQueryTableType::getIcon()
{
    return QPixmap( ":DATAW/FolderClosed" );
}


ADObject *DATAWQueryTableType::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryTable" )        
        p = new DATAWQueryTable( this, pConnection, stringCatalog, stringSchema, getName() );           

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

void DATAWQueryTableType::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATAStatement *pStatement;
    pStatement = pConnection->getTables( stringCatalog, stringSchema.isEmpty() ? QString() : stringSchema, getName() );
    if ( !pStatement )
    {
        bLoadingChildren = false;
        return;
    }

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringTable     = pStatement->getData( 3 ).toString();
        QString stringRemark    = pStatement->getData( 5 ).toString();

        DATAWQueryTable *p = (DATAWQueryTable*)getObject( "Table" );
        p->setName( stringTable );
        p->setComment( stringRemark );
        p->setModified( false );
        // carry on
        n = pStatement->doFetch();
    }

    delete pStatement;
    bQueried = true;
    bLoadingChildren = false;
}

