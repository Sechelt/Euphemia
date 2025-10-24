#include "LibInfo.h"
#include "LGraphicsLine.h"

LGraphicsLine::LGraphicsLine( const QLineF &line )
    : QGraphicsObject()
{
    // over LObjects but below DHandles
    setZValue( ZORDER_UTILITY_LINE );

    this->line = line;
    pen.setColor( Qt::black );
    pen.setStyle( Qt::DotLine );
}

LGraphicsLine::~LGraphicsLine()
{
}

void LGraphicsLine::setLine( const QLineF &line )
{
    prepareGeometryChange();
    this->line = line;
    update();
}

void LGraphicsLine::setBegin( const QPointF &point )
{
    prepareGeometryChange();
    line.setP1( point );
    update();
}

void LGraphicsLine::setEnd( const QPointF &point )
{
    prepareGeometryChange();
    line.setP2( point );
    update();
}

void LGraphicsLine::setPen( const QPen &pen )
{
    this->pen = pen;
    update();
}

QRectF LGraphicsLine::boundingRect() const
{
    QRectF r = shape().boundingRect();
    qreal nPenWidth = pen.width() * 2;
    r.moveTopLeft( QPointF( r.left() - nPenWidth, r.top() - nPenWidth ) );
    r.moveBottomRight( QPointF( r.right() + nPenWidth, r.bottom() + nPenWidth ) );
    return r;
}

void LGraphicsLine::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    // Turn any scale off (to avoid fat lines)...
    // get current transform so we can muck with the scale
    QTransform t = pPainter->transform();
    // X/Y scale
    qreal m11 = t.m11(), m22 = t.m22();
    // save painter state so we can restore later
    pPainter->save();
    // undo scale for our draw to prevent grid lines from getting fat
    pPainter->setTransform( QTransform( 1, t.m12(), t.m13(), t.m21(), 1, t.m23(), t.m31(), t.m32(), t.m33() ) );

    // draw
    pPainter->setPen( pen );
    pPainter->drawLine( line.x1() * m11, line.y1() * m22, line.x2() * m11, line.y2() * m22 );

    // Turn any scale back on...
    pPainter->restore(); 
}

QPainterPath LGraphicsLine::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    polygon << line.p1() << line.p2();
    path.addPolygon( polygon );

    return path;
}


