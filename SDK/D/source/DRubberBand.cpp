#include "LibInfo.h"
#include "DRubberBand.h"

DRubberBand::DRubberBand()
    : QGraphicsObject()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // origin is in middle of rect and size is 0
    rect.setTopLeft( QPointF( 0, 0 ) );
    rect.setBottomRight( QPointF( 0, 0 ) );

    pHandle = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DRubberBand::~DRubberBand()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    delete pHandle;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void DRubberBand::setPos( const QPointF &pointScene )
{
    QGraphicsObject::setPos( pointScene );
    if ( !pHandle )
    { 
        pHandle = new DHandle( nullptr, DHandle::FunctionSelectBottomRight, DHandle::ConnectTypeNone, mapToScene( rect.topLeft() ) );
        scene()->addItem( pHandle );
    }
    pHandle->setPos( mapToScene( rect.bottomRight() ) );
}

QRectF DRubberBand::boundingRect() const
{
    return rect;
}

QPainterPath DRubberBand::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}

void DRubberBand::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( Qt::DashLine );
    pPainter->drawRect( rect );
}

void DRubberBand::mouseMoveHandle( QGraphicsSceneMouseEvent *pEvent )
{
    doHandleMove( pEvent->scenePos() );
}

void DRubberBand::mouseReleaseHandle( QGraphicsSceneMouseEvent *pEvent )
{
    doHandleMove( pEvent->scenePos() );
}

void DRubberBand::doHandleMove( const QPointF &pointScenePos )
{
    prepareGeometryChange();
    QRectF r = rect;

    // adopt the move with one exception - we do not allow the rect to be inverted
    QPointF point = mapFromScene( pointScenePos );
    if ( point.x() < r.left() ) point.setX( r.left() );
    if ( point.y() < r.top() ) point.setY( r.top() );
    r.setBottomRight( point );

    // need to maintain origin (center of rect) as 0,0
    QPointF pointPos = mapToScene( r.center() );
    qreal nX = r.width() / 2;
    qreal nY = r.height() / 2;

    rect.setTopLeft( QPointF( -nX, -nY ) );
    rect.setBottomRight( QPointF( nX, nY ) );

    setPos( pointPos );
}



