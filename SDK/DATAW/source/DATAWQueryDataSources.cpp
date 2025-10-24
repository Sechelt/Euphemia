#include "LibInfo.h"
#include "DATAWQueryDataSources.h"

#include <DATAEnvironment.h>

#include "DATAWQueryDataSource.h"

DATAWQueryDataSources::DATAWQueryDataSources( ADObject *pParent, DATAEnvironment *pEnvironment, DATASystem::ConfigModes nDataSourceType )
    : DATAWQuery( pParent )
{
    this->pEnvironment      = pEnvironment;
    this->nDataSourceType   = nDataSourceType;

    switch ( nDataSourceType )
    {
    case DATASystem::System: 
        stringName = tr( "System Data Sources" );
        break;
    case DATASystem::User:
        stringName = tr( "User Data Sources" );
        break;
    case DATASystem::Both: 
        stringName = tr( "User And System Data Sources" );
        break;
    }

    mapMeta.insert( "DATAWQueryDataSource", ADMeta( QIcon(), "DATAWQueryDataSource", "DataSource" ) );
}

DATAWQueryDataSources::~DATAWQueryDataSources()
{
}

QPixmap DATAWQueryDataSources::getIcon()
{
    switch ( nDataSourceType )
    {
    case DATASystem::System: 
        return QPixmap( ":DATAW/DataSourceSystem" );
        break;
    case DATASystem::User:
        return QPixmap( ":DATAW/DataSourceUser" );
        break;
    case DATASystem::Both: 
        return QPixmap( ":DATAW/DataSourceSystem" );
        break;
    }

    return QPixmap();
}

ADObject *DATAWQueryDataSources::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryDataSource" )        
        p = new DATAWQueryDataSource( this, pEnvironment, nDataSourceType );           

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
 * Query/Refresh list of data source names. 
 * Preserve any that have connection(s). 
 * 
 * \author pharvey (1/2/21)
 */
void DATAWQueryDataSources::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    bLoadingChildren = true;

    //
    bool bSystem    = nDataSourceType == DATASystem::System || nDataSourceType == DATASystem::Both;
    bool bUser      = nDataSourceType == DATASystem::User || nDataSourceType == DATASystem::Both;

    QStringList listDataSourceNames = pEnvironment->getDataSources( bUser, bSystem );
    QString stringDataSourceName;
    foreach( stringDataSourceName, listDataSourceNames )
    {
        // data sources with active connections are not deleted in doClear so avoid adding those and creating duplicates
        if ( getObjects( stringDataSourceName, QString(), QString() ).count() ) continue;
        ADObject *p = getObject( "DATAWQueryDataSource" );
        p->setName( stringDataSourceName );
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
void DATAWQueryDataSources::doClear()
{
    // we assume that the only children we have are DATAWQueryDataSource
    QList<ADObject*> l = getObjects();
    ADObject *p;
    foreach( p, l )
    {
        Q_ASSERT( p->inherits( "DATAWQueryDataSource" ) );
        DATAWQueryDataSource *p2 = (DATAWQueryDataSource*)p;
        if ( p2->hasConnection() ) continue;
        slotDelete( p2 );
    }
    setModified( false );
    bQueried = false;
}


