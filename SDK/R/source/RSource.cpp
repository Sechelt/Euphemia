#include "RSource.h"

RSource::RSource( QObject *pObjectParent )
    : QObject( pObjectParent )
{
}

QPoint RSource::mapSceneToCell( const QPointF &pointScene )
{
    QSize sizeCell = getCellSize();
    // - always round UP if there is a remainder
    // - cells are zero based
    div_t divX = div( pointScene.x(), sizeCell.width() );
    div_t divY = div( pointScene.y(), sizeCell.height() );

    int nCellX = ( divX.rem > 0 ? divX.quot : divX.quot - 1 );
    int nCellY = ( divY.rem > 0 ? divY.quot : divY.quot - 1 );

    return QPoint( nCellX, nCellY );
}

/*!
 * \brief Map *center* of cell to scene.
 *  
 * This is different then RouteScene::mapCellToScene. 
 *  
 * \author pharvey (5/14/20)
 * 
 * \param pointCell 
 * 
 * \return QPointF 
 */
QPointF RSource::mapCellToScene( const QPoint &pointCell )
{
    QSize sizeCell = getCellSize();
    // - return CENTER of cell in scene coords
    qreal nX = pointCell.x() * sizeCell.width() + sizeCell.width() / 2;
    qreal nY = pointCell.y() * sizeCell.height() + sizeCell.height() / 2;

    return QPointF( nX, nY );
}

QRect RSource::mapSceneToCell( const QRectF &rectScene )
{
    // topleft as 0-based cell coord
    QPoint pointTopLeft = mapSceneToCell( rectScene.topLeft() );
    // we get the width/height as 0-based cell coord...
    QPoint pointSize = mapSceneToCell( QPointF( rectScene.width(), rectScene.height() ) );
    // ...and adjust to 1-based here
    return QRect( pointTopLeft.x(), pointTopLeft.y(), pointSize.x() + 1, pointSize.y() + 1 );
}

QRectF RSource::getCellRect( const QPoint &pointCell )
{
    QSize sizeCell = getCellSize();
    return QRectF( pointCell.x() * sizeCell.width(), pointCell.y() * sizeCell.height(), sizeCell.width(), sizeCell.height() );
}





