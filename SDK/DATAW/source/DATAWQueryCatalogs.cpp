#include "LibInfo.h"
#include "DATAWQueryCatalogs.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryCatalog.h"

DATAWQueryCatalogs::DATAWQueryCatalogs( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQuery( pParent )
{
    this->pConnection       = pConnection;
    stringName = tr( "CATALOGS" );
    stringComment = tr( "All catalogs (databases)." );

    mapMeta.insert( "DATAWQueryCatalog", ADMeta( QIcon(), "DATAWQueryCatalog", "Catalog" ) );
}

DATAWQueryCatalogs::~DATAWQueryCatalogs()
{
}

QPixmap DATAWQueryCatalogs::getIcon( AWObject::enumIconRoles nRole )
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

QPixmap DATAWQueryCatalogs::getIcon()
{
    return QPixmap( ":DATAW/FolderClosed" );
}

QMenu *DATAWQueryCatalogs::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = DATAWQuery::getContextMenu( pParent );

    if ( parent() ) pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Remove Catalogs Browser" ), this, SLOT(slotDelete()) );
    return pMenu;
}

ADObject *DATAWQueryCatalogs::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryCatalog" )        
        p = new DATAWQueryCatalog( this, pConnection );           

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

void DATAWQueryCatalogs::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    QStringList l;

    // query
    DATAStatement *pStatement = pConnection->getCatalogs();
    if ( pStatement )
    {
        // process results
        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            QString stringCatalog = pStatement->getData( 1 ).toString();
            // prevent duplicate names
            if ( !l.contains( stringCatalog ) )
            {
                // create catalog object
                ADObject *p = getObject( "Catalog" );
                p->setName( (stringCatalog.isEmpty()?"CATALOG":stringCatalog) );
                p->setModified( false );
                l << stringCatalog;
            }
            n = pStatement->doFetch();
        }
        delete pStatement;
    }
    // ensure we have at least one
    if ( l.count() < 1 )
    {
        ADObject *p = getObject( "Catalog" );
        p->setName( "CATALOG" );
        p->setModified( false );
    }

    bQueried = true;
    bLoadingChildren = false;
}



