#include "DRouteSource.h"

#include "DGraphicsSceneProxy.h"
#include "DDiagram.h"

DRouteSource::DRouteSource( DDiagram *pDiagram, const QSize &sizeCell, QObject *pObjectParent )
    : RSource( pObjectParent )
{
    this->pDiagram = pDiagram;

    bCache = true;

    this->sizeCell = sizeCell;

    sizeGrid.setWidth( round( pDiagram->getScene()->width() / sizeCell.width() ) );
    sizeGrid.setHeight( round( pDiagram->getScene()->height() / sizeCell.height() ) );

    for ( int nRow = 0; nRow < sizeGrid.height(); nRow++ )
    {
        grid.append( RSourceRow( sizeGrid.width() ) );
    }
}

QSize DRouteSource::getGridSize()
{
    return sizeGrid;
}

QSize DRouteSource::getCellSize()
{
    return sizeCell;
}

RSource::CellContentTypes DRouteSource::getCellContent( const QPoint &pointCell )
{
    RSource::CellContentTypes n = RSource::CellContentClear;

    if ( bCache )
    { 
        if ( grid[pointCell.y()][pointCell.x()].nContent == RSourceCell::CellContentUnknown ) doCellLoad( pointCell );
    }
    else
        doCellLoad( pointCell );

    if ( grid[pointCell.y()][pointCell.x()].nContent == RSourceCell::CellContentLine ) n = RSource::CellContentLine;
    else if ( grid[pointCell.y()][pointCell.x()].nContent == RSourceCell::CellContentShape ) n = RSource::CellContentShape;

    return n;
}

/*!
 * \brief Load cell info from the diagram. 
 *  
 * The only things we need from the diagram are; 
 *  
 * 1 - cell size and grid dimensions (we loaded this in our constructor) 
 * 2 - whether a cell is blocked or not (we load this 'as needed'... here) 
 *  
 * \sa doCell 
 *  
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 */
void DRouteSource::doCellLoad( const QPoint &pointCell )
{
    Q_ASSERT( pointCell.y() >= 0 );
    Q_ASSERT( pointCell.y() < grid.count() );
    Q_ASSERT( pointCell.x() >= 0 );
    Q_ASSERT( pointCell.x() < grid[0].count() );

    RSourceCell::CellContentTypes n = RSourceCell::CellContentClear;
    QList<QGraphicsItem *> listItems = pDiagram->getScene()->items( QRectF( pointCell.x() * sizeCell.width(), pointCell.y() * sizeCell.height(), sizeCell.width(), sizeCell.height() )/*, Qt::IntersectsItemShape */);

    QGraphicsItem *p;
    foreach( p, listItems )
    {
        // all of our items are QGraphicsObject based so...
        QGraphicsObject *pGraphicsObject = (QGraphicsObject*)p;
        DGraphicsProxyItem *pProxy = nullptr;
        if ( pGraphicsObject->inherits( "DGraphicsObject" ) ) pProxy = ((DGraphicsObject*)p)->getProxy();
        // if ( pGraphicsObject->inherits( "FRGraphicsObject" ) ) pProxy = ((FRGraphicsObject*)p)->getProxy();
        if ( !pProxy ) continue;
        DObject *pObject = pProxy->getObject();
        if ( !pObject ) continue;
        if ( listIgnore.contains( pObject ) ) continue;
        if ( n < RSourceCell::CellContentLine && pObject->inherits( "DLine" ) ) n = RSourceCell::CellContentLine;  // line
        if ( pObject->inherits( "DRectangle" ) )
        {
           n = RSourceCell::CellContentShape;    // shape - can not get any more blocked so 'break'
           break;
        }
    }
    grid[pointCell.y()][pointCell.x()].nContent = n;
}




