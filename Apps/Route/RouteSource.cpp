#include "RouteSource.h"

#include "RouteScene.h"

RouteSource::RouteSource( RouteScene *pScene, QObject *pObjectParent )
    : RSource( pObjectParent )
{
    this->pScene = pScene;
}

QPointF RouteSource::getBegin() 
{ 
    return mapCellToScene( pScene->getBegin() ); 
}

QPointF RouteSource::getEnd() 
{ 
    return mapCellToScene( pScene->getEnd() ); 
}

QSize RouteSource::getGridSize()
{
    return pScene->getGridSize();
}

QSize RouteSource::getCellSize()
{
    return pScene->getCellSize();
}

RSource::CellContentTypes RouteSource::getCellContent( const QPoint &pointCell )
{
    RSourceCell rc = pScene->getCell( pointCell );

    if ( rc.nContent == RSourceCell::CellContentLine ) return RSource::CellContentLine;
    if ( rc.nContent == RSourceCell::CellContentShape ) return RSource::CellContentShape;

    return RSource::CellContentClear;
}

QWidget *RouteSource::getControls( QWidget *pParent )
{
    if ( !pScene ) return nullptr;

    return pScene->getControls( pParent );
}


