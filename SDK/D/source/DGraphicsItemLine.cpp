#include "LibInfo.h"
#include "DGraphicsItemLine.h"

DGraphicsItemLine::DGraphicsItemLine( const QLineF &line )
    : QGraphicsObject()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // over DIObjects but below DIHandles
    setZValue( ZORDER_UTILITY_LINE );

    this->line = line;
    pen.setColor( Qt::black );
    pen.setStyle( Qt::DotLine );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DGraphicsItemLine::~DGraphicsItemLine()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void DGraphicsItemLine::setLine( const QLineF &line )
{
    prepareGeometryChange();
    this->line = line;
}

void DGraphicsItemLine::setBegin( const QPointF &point )
{
    prepareGeometryChange();
    line.setP1( point );
}

void DGraphicsItemLine::setEnd( const QPointF &point )
{
    prepareGeometryChange();
    line.setP2( point );
}

void DGraphicsItemLine::setPen( const QPen &pen )
{
    this->pen = pen;
}

QRectF DGraphicsItemLine::boundingRect() const
{
    return shape().boundingRect();
}

void DGraphicsItemLine::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setRenderHints( QPainter::Antialiasing );

    pPainter->setPen( pen );
    pPainter->drawLine( line );
}

QPainterPath DGraphicsItemLine::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    polygon << line.p1() << line.p2();
    path.addPolygon( polygon );

    return path;
}


