#include "LibInfo.h"
#include "FLBegin.h"
    
#include <DGraphicsProxyItem.h>

#include "FLDiagram.h"
#include "FLStep.h"
#include "FLConditionOpen.h"
#include "FLConditionClose.h"
#include "FLLoopOpen.h"
#include "FLFlow.h"

#define FLSinkBottom 0

FLBegin::FLBegin( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    brush.setStyle( Qt::SolidPattern );
    brush.setColor( Qt::darkGreen );

    pDiagram->setBegin( this );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLBegin::~FLBegin()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    ((FLDiagram*)getParent( "FLDiagram" ))->setBegin( nullptr );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap FLBegin::getIcon()
{
    return QPixmap( ":FL/Begin" );
}

void FLBegin::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
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

void FLBegin::doSinksCreate()
{
    vectorSinks.resize( 1 );
    vectorSinks[FLSinkBottom] = new DSink( this, "Bottom" );
    doSinksSync();
}

void FLBegin::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkBottom]->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
}

void FLBegin::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkBottom]->setPosRaw( getProxy()->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
}


