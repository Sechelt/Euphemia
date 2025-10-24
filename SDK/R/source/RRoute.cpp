#include "LibInfo.h"
#include "RRoute.h"

#include "RSource.h"

RRoute::RRoute( RSource *pSource, QObject *pObjectParent )
    : QObject( pObjectParent )
{
    this->pSource = pSource;
}

void RRoute::slotRoute() 
{
    if ( !pSource ) return;
    getRoute( pSource->getBegin(), pSource->getEnd(), false ); 
}

void RRoute::doDelay( int nSeconds )
{
    QEventLoop loop;
 
    QTimer::singleShot( nSeconds * 1000, &loop, &QEventLoop::quit );
 
    loop.exec();
}


