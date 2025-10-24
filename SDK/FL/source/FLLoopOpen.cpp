#include "LibInfo.h"
#include "FLLoopOpen.h"
    
#include "FLDiagram.h"
#include "FLLoopClose.h"

FLLoopOpen::FLLoopOpen( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLLoopOpen::~FLLoopOpen()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void FLLoopOpen::setClose( FLLoopClose *p )
{
    // create a reference (not for presentation purposes)
    doConnect( p, "Close" );
}

QPixmap FLLoopOpen::getIcon()
{
    return QPixmap( ":FL/LoopOpen" );
}

FLLoopClose *FLLoopOpen::getClose()
{
    return (FLLoopClose*)getConnected( "Close" );
}

void FLLoopOpen::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( !pixmap.isNull() )
        return DBitmap::paint( pPainter, nOption, pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QPolygonF polygon;
    qreal nCorner = rect.height() / 6;
    qreal nH = rect.height() / 4;

    polygon << QPointF( rect.left() + nCorner, rect.top() + nH ); 
    polygon << QPointF( rect.right() - nCorner, rect.top() + nH );
    polygon << QPointF( rect.right(), rect.top() + nH + nCorner );
    polygon << QPointF( rect.right(), rect.bottom() - nH );
    polygon << QPointF( rect.left(), rect.bottom() - nH );
    polygon << QPointF( rect.left(), rect.top() + nH + nCorner );

    pPainter->drawPolygon( polygon );

    pPainter->setPen( QPen( font.color ) );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( rect, nTextHAlign | nTextVAlign, getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( rect, nTextHAlign | nTextVAlign, getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( rect, nTextHAlign | nTextVAlign, getComment() );
            break;
        case TextDisplayNone:
            break;
    }
}

