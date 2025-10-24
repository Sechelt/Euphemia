#include "LibInfo.h"
#include "FLLoopClose.h"
    
#include "FLDiagram.h"
#include "FLLoopOpen.h"

FLLoopClose::FLLoopClose( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLLoopClose::~FLLoopClose()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void FLLoopClose::setOpen( FLLoopOpen *p )
{
    // create a reference (not for presentation purposes)
    doConnect( p, "Open" );
}

QPixmap FLLoopClose::getIcon()
{
    return QPixmap( ":FL/LoopClose" );
}

FLLoopOpen *FLLoopClose::getOpen()
{
    return (FLLoopOpen*)getConnected( "Open" );
}

void FLLoopClose::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
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
     
    polygon << QPointF( rect.left(), rect.top() + nH ); 
    polygon << QPointF( rect.right(), rect.top() + nH );
    polygon << QPointF( rect.right(), rect.bottom() - nH - nCorner );
    polygon << QPointF( rect.right() - nCorner, rect.bottom() - nH );
    polygon << QPointF( rect.left() + nCorner, rect.bottom() - nH );
    polygon << QPointF( rect.left(), rect.bottom() - nH - nCorner );

    pPainter->drawPolygon( polygon );

    pPainter->setPen( QPen( font.color ) );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( rect, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( rect, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( rect, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            break;
    }
}

