#include "LibInfo.h"
#include "DATAWQueryCatalog.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQuerySchema.h"

DATAWQueryCatalog::DATAWQueryCatalog( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQuery( pParent )
{
    this->pConnection       = pConnection;
    stringComment = tr( "CATALOG" );

    mapMeta.insert( "DATAWQuerySchema", ADMeta( QIcon(), "DATAWQuerySchema", "Schema" ) );
}

DATAWQueryCatalog::~DATAWQueryCatalog()
{
}

QPixmap DATAWQueryCatalog::getIcon( AWObject::enumIconRoles nRole )
{
    switch ( nRole )
    {
    case nIconRoleExpanded:
        return QPixmap( ":DATAW/Catalog" );
    case nIconRoleCollapsed:
        return QPixmap( ":DATAW/Catalog" );
    }
    return getIcon();
}

QPixmap DATAWQueryCatalog::getIcon()
{
    return QPixmap( ":DATAW/Catalog" );
}

QMenu *DATAWQueryCatalog::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = DATAWQuery::getContextMenu( pParent );

    if ( parent() ) pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Remove Catalog Browser" ), this, SLOT(slotDelete()) );
    return pMenu;
}

ADObject *DATAWQueryCatalog::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQuerySchema" )        
        p = new DATAWQuerySchema( this, pConnection, getName() );           

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

void DATAWQueryCatalog::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    QStringList l;

    // query
    DATAStatement *pStatement = pConnection->getSchemas( getName() );
    if ( pStatement )
    {
        // process results
        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            QString stringSchema = pStatement->getData( 2 ).toString();
            // prevent duplicate names
            if ( !l.contains( stringSchema ) )
            {
                // create schema object
                ADObject *p = getObject( "Schema" );
                // Allow empty schema name - its significant.
                // p->setName( (stringSchema.isEmpty()?"SCHEMA":stringSchema) );
                p->setName( stringSchema );
                p->setModified( false );
                l << stringSchema;
            }
            n = pStatement->doFetch();
        }
        delete pStatement;
    }
    // ensure we have at least one
    if ( l.count() < 1 )
    {
        ADObject *p = getObject( "Schema" );
        p->setName( "" );
        p->setModified( false );
    }

    bQueried = true;
    bLoadingChildren = false;
}



