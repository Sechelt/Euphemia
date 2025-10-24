#include "LibInfo.h"
#include "DATAWQueryDrivers.h"

#include <DATAEnvironment.h>

#include "DATAWQueryDriver.h"

DATAWQueryDrivers::DATAWQueryDrivers( ADObject *pParent, DATAEnvironment *pEnvironment )
    : DATAWQuery( pParent, tr( "Drivers" ) )
{
    this->pEnvironment = pEnvironment;

    mapMeta.insert( "DATAWQueryDriver", ADMeta( QIcon(), "DATAWQueryDriver", "Driver" ) );
}

DATAWQueryDrivers::~DATAWQueryDrivers()
{
}

QPixmap DATAWQueryDrivers::getIcon()
{
    return QPixmap( ":DATAW/Drivers" );
}

ADObject *DATAWQueryDrivers::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryDriver" )        
        p = new DATAWQueryDriver( this, pEnvironment );           

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

/*!
 * \brief Query 
 *  
 * Query/Refresh list of drivers. 
 * Preserve any that have connection(s). 
 * 
 * \author pharvey (1/2/21)
 */
void DATAWQueryDrivers::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    bLoadingChildren = true;

    //
    QStringList listDriverNames = pEnvironment->getDrivers();
    QString stringDriverName;
    foreach( stringDriverName, listDriverNames )
    {
        // drivers with active connections are not deleted in doClear so avoid adding those and creating duplicates
        if ( getObjects( stringDriverName, QString(), QString() ).count() ) continue;
        ADObject *p = getObject( "DATAWQueryDriver" );
        p->setName( stringDriverName );
        p->setModified( false );
    }

    bQueried = true;
    bLoadingChildren = false;
}

/*!
 * \brief Clear 
 *  
 * Delete all data source names - except any that have connection(s). 
 * The remainder will not have to be recreated during a refresh. 
 * The remainder will be deleted in destructor if we are being deleted. 
 * 
 * \author pharvey (1/2/21)
 */
void DATAWQueryDrivers::doClear()
{
    // we assume that the only children we have are DATAWQueryDriver
    QList<ADObject*> l = getObjects();
    ADObject *p;
    foreach( p, l )
    {
        Q_ASSERT( p->inherits( "DATAWQueryDriver" ) );
        DATAWQueryDriver *p2 = (DATAWQueryDriver*)p;
        if ( p2->hasConnection() ) continue;
        slotDelete( p2 );
    }
    setModified( false );
    bQueried = false;
}


