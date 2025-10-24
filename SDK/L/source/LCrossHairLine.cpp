#include "LibInfo.h"
#include "LCrossHairLine.h"

#include <SSettings.h>

LCrossHairLine::LCrossHairLine( Qt::Orientation n )
    : QGraphicsObject()
{
    // over LObjects but below LHandles
    setZValue( ZORDER_UTILITY_LINE );

    nOrientation = n;

    pSettingPointer = (LSPointer*)g_SSettings->getSetting( "LSPointer" ); Q_ASSERT( pSettingPointer );
    slotRefreshPointer();
    connect( pSettingPointer, SIGNAL(signalChanged()), SLOT(slotRefreshPointer()) );
}

LCrossHairLine::~LCrossHairLine()
{
}

void LCrossHairLine::setLine( const QPointF &point, const QLineF &line )
{
   prepareGeometryChange();
   this->point  = point;
   this->line   = line;
   update();
}

QRectF LCrossHairLine::boundingRect() const
{
    QRectF r;

    if ( line.isNull() ) return r;

    // rect around line
    r.setTopLeft( line.p1() );
    r.setBottomRight( line.p2() );
    r = r.normalized();

    // expand to account for line/pen width
    qreal nPenWidth = pen.width() * 2;
    r.moveTopLeft( QPointF( r.left() - nPenWidth, r.top() - nPenWidth ) );
    r.moveBottomRight( QPointF( r.right() + nPenWidth, r.bottom() + nPenWidth ) );

    return r;
}

void LCrossHairLine::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( line.isNull() ) return;

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

    qreal nGap = 0;
    if ( nOrientation == Qt::Horizontal ) nGap = sizeCursor.width();
    else nGap = sizeCursor.height();

    if ( nGap )
    {
        if ( nOrientation == Qt::Horizontal )
        {
            pPainter->drawLine( line.x1() * m11, line.y1() * m22, point.x() - nGap / 2 * m11, line.y2() * m22 );
            pPainter->drawLine( point.x() + nGap / 2 * m11, line.y1() * m22, line.x2() * m11, line.y2() * m22 );
        }
        else
        {
            pPainter->drawLine( line.x1() * m11, line.y1() * m22, line.x2() * m11, point.y() - nGap / 2 * m22 );
            pPainter->drawLine( line.x1() * m11, point.y() + nGap / 2 * m22, line.x2() * m11, line.y2() * m22 );
        }
    }
    else
        pPainter->drawLine( line.x1() * m11, line.y1() * m22, line.x2() * m11, line.y2() * m22 );

    // Turn any scale back on...
    pPainter->restore(); 
}

void LCrossHairLine::slotRefreshPointer()
{
    sizeCursor = pSettingPointer->getSizeCursor();
    pen = pSettingPointer->getPen();
    update();
}


