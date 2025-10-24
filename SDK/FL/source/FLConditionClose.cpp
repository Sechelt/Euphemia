#include "LibInfo.h"
#include "FLConditionClose.h"
    
#include <DGraphicsProxyItem.h>

#include "FLDiagram.h"
#include "FLStep.h"
#include "FLConditionOpen.h"
#include "FLLoopOpen.h"
#include "FLLoopClose.h"
#include "FLEnd.h"
#include "FLFlow.h"

#define FLHSinkOffset 3

#define FLSinkBottom 0
#define FLSinkLeft 1
#define FLSinkRight 2

FLConditionClose::FLConditionClose( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    bIgnore = true;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLConditionClose::~FLConditionClose()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void FLConditionClose::setOpen( FLConditionOpen *p )
{
    // create a reference (not for presentation purposes)
    doConnect( p, "Open" );
}

QPixmap FLConditionClose::getIcon()
{
    return QPixmap( ":FL/ConditionClose" );
}

FLConditionOpen *FLConditionClose::getOpen()
{
    return (FLConditionOpen*)getConnected( "Open" );
}

FLRectangle *FLConditionClose::getPrevious( CBD::DirectionTypes nDir )
{
   if ( nDir == CBD::DirectionE ) 
   {
      return (FLRectangle*)getFlow( "Right" )->getRectangle( CBD::EOLBegin );
   }
    else if ( nDir == CBD::DirectionW ) 
   {
       return (FLRectangle*)getFlow( "Left" )->getRectangle( CBD::EOLBegin );
   }

    return nullptr;
}

void FLConditionClose::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( !pixmap.isNull() )
        return DBitmap::paint( pPainter, nOption, pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QPointF pointCenter = rect.center();

    pPainter->drawLine( rect.left(), pointCenter.y(), pointCenter.x(), rect.bottom() );
    pPainter->drawLine( pointCenter.x(), rect.bottom(), rect.right(), pointCenter.y() );
    pPainter->drawLine( rect.right(), pointCenter.y(), rect.left(), pointCenter.y() );

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

void FLConditionClose::doSinksCreate()
{
    vectorSinks.resize( 3 );
    vectorSinks[FLSinkBottom] = new DSink( this, "Bottom" );
    vectorSinks[FLSinkLeft]   = new DSink( this, "Left" );
    vectorSinks[FLSinkRight]  = new DSink( this, "Right" );
    doSinksSync();
}

void FLConditionClose::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == FLSinkBottom )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
        else if ( n == FLSinkLeft )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.left() + FLHSinkOffset, rect.center().y() ) ) );
        else if ( n == FLSinkRight )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.right() - FLHSinkOffset, rect.center().y() ) ) );
        else
            pSink->setPos();
        n++;
    }
}

void FLConditionClose::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == FLSinkBottom )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
        else if ( n == FLSinkLeft )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.left() + FLHSinkOffset, rect.center().y() ) ) );
        else if ( n == FLSinkRight )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.right() - FLHSinkOffset, rect.center().y() ) ) );
        else
            pSink->setPosRaw();
        n++;
    }
}


