#include "LibInfo.h"
#include "FLConditionOpen.h"
    
#include <WCoordWidget.h>

#include <DGraphicsProxyItem.h>

#include "FLDiagram.h"
#include "FLStep.h"
#include "FLLoopOpen.h"
#include "FLLoopClose.h"
#include "FLEnd.h"
#include "FLFlow.h"
#include "FLConditionClose.h"

#define FLHSinkOffset 3

#define FLSinkTop 0
#define FLSinkLeft 1
#define FLSinkRight 2

#define FLMenuStepL 0
#define FLMenuConditionL 1
#define FLMenuLoopL 2

#define FLMenuStepR 3
#define FLMenuConditionR 4
#define FLMenuLoopR 5

FLConditionOpen::FLConditionOpen( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    nStretchLeft    = 0;
    nStretchRight   = 0;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLConditionOpen::~FLConditionOpen()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void FLConditionOpen::setClose( FLConditionClose *p )
{
    // create a reference (not for presentation purposes)
    doConnect( p, "Close" );
}

void FLConditionOpen::setStretch( CBD::DirectionTypes nDir, qreal n )
{
// qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "Stretch: " << n << "Dir:" << CBD::DirectionTypeStrings[nDir];
    if ( nDir == CBD::DirectionE ) nStretchRight = n;
    else if ( nDir == CBD::DirectionW ) nStretchLeft = n;

    // trigger FLFlow::doRoute() in a way that also causes paint
    getFlow( nDir )->doRoute();
}

QPixmap FLConditionOpen::getIcon()
{
    return QPixmap( ":FL/ConditionOpen" );
}

#ifdef DEBUG_PROPDOCK
AWPropWidget *FLConditionOpen::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = new AWPropWidget( this, pWidgetParent );

    // debug
    {
        QWidget *       pWidget         = new QWidget( pPropWidget );
        QGridLayout *   pLayoutGrid     = new QGridLayout( pWidget );

        WCoordWidget *  pLabelCell      = new WCoordWidget( pWidget );
        WCoordWidget *  pLabelPixel     = new WCoordWidget( pWidget );
        pLabelPixel->slotCoord( getPos() );
        pLabelCell->slotCoord( ((DDiagram*)getParent( "DDiagram" ))->mapSceneToCell( getPos() ) );

        pLayoutGrid->addWidget( new QLabel( tr("Cell:"), pWidget ), 0, 0 );
        pLayoutGrid->addWidget( pLabelCell, 0, 1 );
        pLayoutGrid->addWidget( new QLabel( tr("Pixel:"), pWidget ), 1, 0 );
        pLayoutGrid->addWidget( pLabelPixel, 1, 1 );
        pLayoutGrid->addWidget( new QLabel( tr("Stretch Left:"), pWidget ), 2, 0 );
        pLayoutGrid->addWidget( new QLabel( QString::number( nStretchLeft ), pWidget ), 2, 1 );
        pLayoutGrid->addWidget( new QLabel( tr("Stretch Right:"), pWidget ), 3, 0 );
        pLayoutGrid->addWidget( new QLabel( QString::number( nStretchRight ), pWidget ), 3, 1 );
        pPropWidget->addWidget( tr("Debug"), pWidget );
    }

    return pPropWidget;

}
#endif

FLConditionClose *FLConditionOpen::getClose()
{
    return (FLConditionClose*)getConnected( "Close" );
}

FLRectangle *FLConditionOpen::getNext( CBD::DirectionTypes nDir )
{
    if ( nDir == CBD::DirectionE ) return (FLRectangle*)getFlow( "Right" )->getRectangle( CBD::EOLEnd );
    else if ( nDir == CBD::DirectionW ) return (FLRectangle*)getFlow( "Left" )->getRectangle( CBD::EOLEnd );

    return nullptr;
}

/*!
 * \brief Get max in scene coordinates. 
 *  
 * Pursues outer reaches to Left/Right until it gets to its own Close. 
 * Returns the max found (center of cell). 
 *  
 * Outer : Outer extent Left or Right side. 
 *         Usefull to determine extent after inserting content.
 *  
 * Inner : Inner extent of Left or Right side. 
 *         Compare an Inner extent with the Outer extent of nested content to determine if there is overlap.
 *  
 * \author pharvey (3/26/20)
 * 
 * \return qreal 
 */
qreal FLConditionOpen::getFarthestX( CBD::DirectionTypes nDirStart, CBD::DirectionTypes nDir )
{
    FLRectangle *pRectangle = getNext( nDirStart );

   return getFarthestX( nDir, pRectangle->getPos().x(), pRectangle );
}

qreal FLConditionOpen::getStretch( CBD::DirectionTypes nDir )
{
    if ( nDir == CBD::DirectionW ) return nStretchLeft;
    if ( nDir == CBD::DirectionE ) return nStretchRight;
    return 0;
}

bool FLConditionOpen::isEmpty()
{
    return (isEmpty( CBD::DirectionW ) && isEmpty( CBD::DirectionE ));
}

bool FLConditionOpen::isEmpty( CBD::DirectionTypes nDir )
{
    FLRectangle *p = getNext( nDir );
    if ( p->inherits( "FLConditionClose" ) ) return true;
    return false;
}

void FLConditionOpen::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( !pixmap.isNull() )
        return DBitmap::paint( pPainter, nOption, pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QPointF pointCenter = rect.center();

    pPainter->drawLine( rect.left(), pointCenter.y(), pointCenter.x(), rect.top() ); // left -> top
    pPainter->drawLine( pointCenter.x(), rect.top(), rect.right(), pointCenter.y() ); // top -> right
    pPainter->drawLine( rect.right(), pointCenter.y(), rect.left(), pointCenter.y() ); // right -> left

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

void FLConditionOpen::doSinksCreate()
{
    vectorSinks.resize( 3 );
    vectorSinks[FLSinkTop]    = new DSink( this, "Top" );
    vectorSinks[FLSinkLeft]   = new DSink( this, "Left" );
    vectorSinks[FLSinkRight]  = new DSink( this, "Right" );
    doSinksSync();
}

void FLConditionOpen::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == FLSinkTop )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
        else if ( n == FLSinkLeft )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.left() + FLHSinkOffset, rect.center().y() ) ) );
        else if ( n == FLSinkRight )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.right() - FLHSinkOffset, rect.center().y() ) ) );
        else
            pSink->setPos();
        n++;
    }
}

void FLConditionOpen::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == FLSinkTop )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
        else if ( n == FLSinkLeft )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.left() + FLHSinkOffset, rect.center().y() ) ) );
        else if ( n == FLSinkRight )
            pSink->setPosRaw( pProxy->mapToScene( QPointF( rect.right() - FLHSinkOffset, rect.center().y() ) ) );
        else
            pSink->setPosRaw();
        n++;
    }
}

void FLConditionOpen::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    Q_UNUSED(pointScenePos);

    // left
    if ( pHandle == vectorHandles.at( FLMenuStepL ) )                                            
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLStep", this, getSink( "Left" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuConditionL ) )                                  
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLCondition", this, getSink( "Left" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuLoopL ) )                                    
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLLoop", this, getSink( "Left" ) );
    }                                                                                           

    // right
    if ( pHandle == vectorHandles.at( FLMenuStepR ) )                                            
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLStep", this, getSink( "Right" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuConditionR ) )                                  
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLCondition", this, getSink( "Right" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuLoopR ) )                                    
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLLoop", this, getSink( "Right" ) );
    }                                                                                           
}

void FLConditionOpen::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 6 );

    DHandle *pHandle;

    // false (left side)
    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() ) ) );
    pHandle->setPixmap( QIcon( ":FL/Step" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuStepL] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() + 18 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Condition" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuConditionL] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() + 32 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Loop" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuLoopL] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    // true (right side)
    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QIcon( ":FL/Step" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuStepR] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Condition" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuConditionR] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 32 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Loop" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuLoopR] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}


void FLConditionOpen::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void FLConditionOpen::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    // false
    pHandle = vectorHandles[FLMenuStepL];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() ) ) );

    pHandle = vectorHandles[FLMenuConditionL];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() + 18 ) ) );

    pHandle = vectorHandles[FLMenuLoopL];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topLeft().x() - 8, rect.topLeft().y() + 32 ) ) );

    // true
    pHandle = vectorHandles[FLMenuStepR];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[FLMenuConditionR];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );

    pHandle = vectorHandles[FLMenuLoopR];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 32 ) ) );
}

qreal FLConditionOpen::getFarthestX( CBD::DirectionTypes nDir, qreal nMax, FLRectangle *pNext )
{
    if ( !pNext ) return nMax;
    if ( pNext == getClose() ) return nMax;
    if ( nDir == CBD::DirectionE && pNext->getPos().x() > nMax ) nMax = pNext->getPos().x();
    if ( nDir == CBD::DirectionW && pNext->getPos().x() < nMax ) nMax = pNext->getPos().x();

    // handle case where there was no content but there is a stretch
    if ( pNext->inherits( "FLConditionClose" ) )
    {
// qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << ((FLDiagram*)getParent( "FLDiagram" ))->mapSceneToCell( pNext->getPos() );
        FLConditionClose *pClose = (FLConditionClose*)pNext;
        FLFlow *pFlow = pClose->getFlow( nDir );
        if ( pFlow->hasElbow() )
        {
            QPointF point = pFlow->getProxy()->mapToScene( pFlow->getElbow() );
// qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << point;
            if ( nDir == CBD::DirectionE && point.x() > nMax ) nMax = point.x();
            else if ( nDir == CBD::DirectionW && point.x() < nMax ) nMax = point.x();
        }
    }


    return getFarthestX( nDir, nMax, pNext->getNext( nDir ) );
}

