#include "LibInfo.h"
#include "DATAWQueryDriver.h"

#include <DATAEnvironment.h>

#include "DATAWQueryDriverConnection.h"

DATAWQueryDriver::DATAWQueryDriver( ADObject *pParent, DATAEnvironment *pEnvironment )
    : AWObject( pParent )
{
    this->pEnvironment = pEnvironment;

    mapMeta.insert( "DATAWQueryDriverConnection", ADMeta( QIcon(), "DATAWQueryDriverConnection", "Connection" ) );
}

DATAWQueryDriver::~DATAWQueryDriver()
{
}

QPixmap DATAWQueryDriver::getIcon()
{
    return QPixmap( ":DATAW/Driver" );
}

ADObject *DATAWQueryDriver::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryDriverConnection" )        
        p = new DATAWQueryDriverConnection( this, pEnvironment, getName() );           

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

QMenu *DATAWQueryDriver::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":DATAW/Connected" ), "New Connection", this, SLOT(slotNewConnection()) );

    return pMenu;
}

bool DATAWQueryDriver::hasConnection()
{
    return getObjects( QString(), QString(), "DATAWQueryConnection" ).count();
}

/*
bool DATAWQueryDriver::isConnected()
{
    QList<ADObject*> l = getObjects( QString(), QString(), "DATAWQueryConnection" );
    ADObject *p;
    foreach( p, l )
    {
        if ( ((DATAWQueryConnection*)p)->isConnected() ) return true;
    }

    return false;
}
*/

void DATAWQueryDriver::slotNewConnection()
{
    getObject( "Connection" );
}


