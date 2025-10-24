#include "LibInfo.h"
#include "FLEnd.h"
    
#include <DGraphicsProxyItem.h>

#include "FLDiagram.h"

#define FLSinkTop 0

FLEnd::FLEnd( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    brush.setStyle( Qt::SolidPattern );
    brush.setColor( Qt::darkRed );

    pDiagram->setEnd( this );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLEnd::~FLEnd()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    ((FLDiagram*)getParent( "FLDiagram" ))->setEnd( nullptr );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap FLEnd::getIcon()
{
    return QPixmap( ":FL/End" );
}

void FLEnd::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( !pixmap.isNull() )
        return DBitmap::paint( pPainter, nOption, pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QRectF r( rect.x(), rect.y() + rect.height() / 4, rect.width(), rect.height() / 2 );
    pPainter->drawRoundedRect( r, 10, 10 );

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

void FLEnd::doSinksCreate()
{
    vectorSinks.resize( 1 );
    vectorSinks[FLSinkTop] = new DSink( this, "Top" );
    doSinksSync();
}

void FLEnd::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkTop]->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
}

void FLEnd::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkTop]->setPosRaw( getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
}


