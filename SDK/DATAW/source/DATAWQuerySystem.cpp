#include "LibInfo.h"
#include "DATAWQuerySystem.h"

#include "DATAWQueryDrivers.h"
#include "DATAWQueryDataSources.h"

DATAWQuerySystem::DATAWQuerySystem( ADObject *pParent )
    : DATAWQuery( pParent, "ODBC" )
{
    stringComment = tr( "SubSystem" );

    pSystem         = new DATASystem();
    pEnvironment    = new DATAEnvironment( pSystem );
    pEnvironment->doAlloc();
    pEnvironment->setAttrODBCVersion( DATAEnvironment::OVOdbc3 );

    // the following introduces the ODBC messages from DATASystem
    // echo out odbc messages
    // we do not have to do this for other DATA objects as they echo up to DATASystem
    connect( pSystem, SIGNAL(signalMessage(DATAMessage*)), SIGNAL(signalMessage(DATAMessage*)) );
    connect( pSystem, SIGNAL(signalDiagnostic(DATADiagnostic*)), SIGNAL(signalDiagnostic(DATADiagnostic*)) );

    mapMeta.insert( "DATAWQueryDrivers", ADMeta( QIcon(), "DATAWQueryDrivers", "Drivers" ) );
    mapMeta.insert( "UserDataSources", ADMeta( QIcon(), "UserDataSources", "UserDataSources" ) );
    mapMeta.insert( "SystemDataSources", ADMeta( QIcon(), "SystemDataSources", "SystemDataSources" ) );
}

DATAWQuerySystem::~DATAWQuerySystem()
{
    // delete system and all descendents go with it
    if ( pSystem )
    pSystem = 0;
}

QPixmap DATAWQuerySystem::getIcon()
{
    return QPixmap( ":DATAW/ODBC" );
}

ADObject *DATAWQuerySystem::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryDrivers" )        
        p = new DATAWQueryDrivers( this, pEnvironment );           
    else if ( stringClass == "UserDataSources" ) 
        p = new DATAWQueryDataSources( this, pEnvironment, DATASystem::User );           
    else if ( stringClass == "SystemDataSources" )        
        p = new DATAWQueryDataSources( this, pEnvironment, DATASystem::System );           

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

QMenu *DATAWQuerySystem::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Manage Data Sources..."), this, SLOT(slotProperties()) );
    if ( parent() ) pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Remove"), this, SLOT(slotDelete()) );
    return pMenu;
}

/*!
 * \brief Query 
 *  
 * Create our child objects. These objects are always the same so we are not really 
 * querying anything at this point. 
 * 
 * \author pharvey (1/2/21)
 */
void DATAWQuerySystem::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    bLoadingChildren = true;

    getObject( "DATAWQueryDrivers" );
    getObject( "UserDataSources" );
    getObject( "SystemDataSources" );

    bQueried = true;
    bLoadingChildren = false;
}

/*!
 * \brief Clear 
 *  
 * This works differently then most others. Our children are always the same so, once loaded, we 
 * do not bother to delete them - we just call doClear() on them. In this way we can avoid deleting 
 * any active connections.
 *  
 * The child objects will be deleted in the destructor when we are deleted. 
 *  
 * \author pharvey (1/2/21)
 */
/*
void DATAWQuerySystem::doClear()
{
    QList<ADObject*> l = getObjects( QString(), QString(), "DATAWQuery" );
    ADObject *p;
    foreach( p, l )
    {
        ((DATAWQuery*)p)->doClear();
    }
}
*/

/*!
 * \brief Properties 
 *  
 * Invokes the properties dialog. 
 * In this case we invoke the 'ODBC Administrator'.
 * 
 * \author pharvey (1/2/21)
 */
void DATAWQuerySystem::slotProperties()
{
    pSystem->doManageDataSources( getEditorWidgetParent() );
}





