#include "LibInfo.h"
#include "DGraphicsView.h"

#include "DGraphicsSceneProxy.h"

DGraphicsView::DGraphicsView( DGraphicsSceneProxy *pScene, QWidget *pParent )
    : QGraphicsView( pScene, pParent )
{
    Q_ASSERT(pScene);
    // \todo only way to catch this?
    // - when document size changes and not triggering other events because all is still in viewport
    connect( pScene, SIGNAL(sceneRectChanged(const QRectF &)), SLOT(slotSceneRectChanged(const QRectF &)) );

    setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
}

/*!
 * \brief The X offset between viewport.topleft and scene.topleft. 
 *  
 * Created to notify the ruler. 
 *  
 * \author pharvey (2/21/20)
 * 
 * \return int 
 */
qreal DGraphicsView::getOffsetH()
{
    // default to case where scene is larger than viewport - we rely on scroll bar values
    qreal nOffsetH = -horizontalScrollBar()->value();

    // handle case where entire width is in viewport - we assume scene is centered in viewport
    qreal nSceneWidth = sceneRect().width() * getScaleH();
    if ( nSceneWidth < viewport()->width() ) nOffsetH = (viewport()->width() - nSceneWidth) / 2;

    return nOffsetH;
}

/*!
 * \brief Horizontal scale in play. 
 *  
 * m11 = horizontal 
 * m22 = vertical 
 * 
 * \author pharvey (2/25/20)
 * 
 * \return qreal 
 */
qreal DGraphicsView::getScaleH()
{
    return transform().m11();
}

qreal DGraphicsView::getLengthH()
{
    return scene()->width();
}

/*!
 * \brief The Y offset between viewport.topleft and scene.topleft. 
 *  
 * Created to notify the ruler. 
 *  
 * \author pharvey (2/21/20)
 * 
 * \return qreal 
 */
qreal DGraphicsView::getOffsetV()
{
    // default to case where scene is larger than viewport - we rely on scroll bar values
    qreal nOffsetV = -verticalScrollBar()->value();

    // handle case where entire height is in viewport - we assume scene is centered in viewport
    qreal nSceneHeight = sceneRect().height() * getScaleV();
    if ( nSceneHeight < viewport()->height() ) nOffsetV = (viewport()->height() - nSceneHeight) / 2;

    return nOffsetV;
}

/*!
 * \brief Vertical scale in play. 
 *  
 * m11 = horizontal 
 * m22 = vertical 
 * 
 * \author pharvey (2/25/20)
 * 
 * \return qreal 
 */
qreal DGraphicsView::getScaleV()
{
    return transform().m22();
}

qreal DGraphicsView::getLengthV()
{
    return scene()->height();
}

void DGraphicsView::slotZoomChanged( WZoomWidget::FitTypes nFit, int nZoom )
{
    // get scale
    qreal nScale = 1.0;
// qInfo() << scene()->sceneRect();
// qInfo() << viewport()->width() << viewport()->height();
    switch ( nFit )
    {
    case WZoomWidget::FitWidth:
        // if ( scene()->sceneRect().width() <= viewport()->width() ) return;
        nScale = viewport()->width() / scene()->sceneRect().width();
        break;
    case WZoomWidget::FitHeight:
        // if ( scene()->sceneRect().height() <= viewport()->height() ) return;
        nScale = viewport()->height() / scene()->sceneRect().height();
        break;
    case WZoomWidget::FitAll:
        {
            qreal nScaleWidth   = 0;
            qreal nScaleHeight  = 0;
            // if ( scene()->sceneRect().width() > viewport()->width() ) nScaleWidth = viewport()->width() / scene()->sceneRect().width();
            // if ( scene()->sceneRect().height() > viewport()->height() ) nScaleHeight = viewport()->height() / scene()->sceneRect().height();
            nScaleWidth = viewport()->width() / scene()->sceneRect().width();
            nScaleHeight = viewport()->height() / scene()->sceneRect().height();
            nScale = nScaleWidth > nScaleHeight ? nScaleHeight : nScaleWidth;
        }
        break;
    case WZoomWidget::FitIgnore:
        nScale = qreal(nZoom) / 100;
        break;
    }

    if ( nScale == 0 ) return;

    // apply scale
    setScale( nScale );
//    pFile->slotZoomChanged( nZoom );

//    emit signalZoomChanged( nFit, nZoom );
}

void DGraphicsView::slotSceneRectChanged( const QRectF &r )
{
    Q_UNUSED( r );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
}

void DGraphicsView::setScale( qreal n )
{

/*
    QMatrix matrix;      
    matrix.scale( n, n );
    setMatrix( matrix ); 
*/

    QTransform transform;
    transform.scale( n, n );
    setTransform( transform );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
}

void DGraphicsView::resizeEvent( QResizeEvent *pEvent )
{
    QGraphicsView::resizeEvent( pEvent );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
}

void DGraphicsView::scrollContentsBy( int dx, int dy )
{
    QGraphicsView::scrollContentsBy( dx, dy );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
}



