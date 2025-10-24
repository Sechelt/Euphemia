#include "LibInfo.h"
#include "DATAWQueryDataSource.h"

#include <DATAEnvironment.h>

#include "DATAWQueryDataSourceConnection.h"

DATAWQueryDataSource::DATAWQueryDataSource( ADObject *pParent, DATAEnvironment *pEnvironment, SQLUSMALLINT nDataSourceType )
    : AWObject( pParent )
{
    this->pEnvironment      = pEnvironment;
    this->nDataSourceType   = nDataSourceType;

    mapMeta.insert( "DATAWQueryDataSourceConnection", ADMeta( QIcon(), "DATAWQueryDataSourceConnection", "Connection" ) );
}

DATAWQueryDataSource::~DATAWQueryDataSource()
{
}

QPixmap DATAWQueryDataSource::getIcon()
{
    return QPixmap( ":DATAW/DataSource" );
}

ADObject *DATAWQueryDataSource::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryDataSourceConnection" )        
        p = new DATAWQueryDataSourceConnection( this, pEnvironment, nDataSourceType, getName() );           

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

QMenu *DATAWQueryDataSource::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":DATAW/Connected" ), "New Connection", this, SLOT(slotNewConnection()) );

    return pMenu;
}

bool DATAWQueryDataSource::hasConnection()
{
    return getObjects( QString(), QString(), "DATAWQueryConnection" ).count();
}

bool DATAWQueryDataSource::isConnected()
{
    QList<ADObject*> l = getObjects( QString(), QString(), "DATAWQueryConnection" );
    ADObject *p;
    foreach( p, l )
    {
        if ( ((DATAWQueryConnection*)p)->isConnected() ) return true;
    }

    return false;
}

void DATAWQueryDataSource::slotNewConnection()
{
    getObject( "Connection" );
}


