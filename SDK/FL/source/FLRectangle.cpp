#include "LibInfo.h"
#include "FLRectangle.h"
    
#include <AWDynamicDataWidget.h>

#include <DGraphicsProxyItem.h>

#include "FLDiagram.h"
#include "FLNameWidget.h"
#include "FLFlow.h"

#define FLSinkTop 0
#define FLSinkBottom 1

#define FLMenuStep 0
#define FLMenuCondition 1
#define FLMenuLoop 2

FLRectangle::FLRectangle( FLDiagram *pDiagram, const QString &stringName )
    : DBitmap( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    pixmap = QPixmap();
    bDynamicSinks   = false;
    bFixedSize      = true;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLRectangle::~FLRectangle()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void FLRectangle::setSelectType()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            DRectangleBase::setSelectType( SelectMenu );
            break;
        case SelectTransformRotate:
            break;
        case SelectTransformShear:
            break;
        case SelectMenu:
            DRectangleBase::setSelectType( SelectResize );
            break;
        case SelectOnly:
            break;
    }
}

void FLRectangle::setMacro( const QString &s )
{
    if ( stringMacro == s ) return;
    stringMacro = s;
    setModified();
}

FLFlow *FLRectangle::getFlow( CBD::DirectionTypes nDir )
{
    if ( nDir == CBD::DirectionN ) return getFlow( "Top" );
    if ( nDir == CBD::DirectionS ) return getFlow( "Bottom" );
    if ( nDir == CBD::DirectionE ) return getFlow( "Right" );
    if ( nDir == CBD::DirectionW ) return getFlow( "Left" );
    return nullptr;
}

FLFlow *FLRectangle::getFlow( const QString &stringSink )
{
    DSink *pSink = getSink( stringSink );
    if ( !pSink ) return nullptr;

    QList<DSource*> listSources = pSink->getSources();
    if ( listSources.count() < 1 ) return nullptr;

    return (FLFlow*)(listSources.first()->getLine());
}

FLRectangle *FLRectangle::getNext( CBD::DirectionTypes nDir )
{
    Q_UNUSED(nDir);
    return (FLRectangle*)getFlow( "Bottom" )->getRectangle( CBD::EOLEnd );
}

FLRectangle *FLRectangle::getPrevious( CBD::DirectionTypes nDir )
{
    Q_UNUSED(nDir);
    return (FLRectangle*)getFlow( "Top" )->getRectangle( CBD::EOLBegin );
}

/*!
 * \brief To get previous which inherits class. 
 *  
 * Used to get previous FLConditionOpen and FLLoopOpen when we are somewhere in the middle of their content. 
 *  
 * FLConditionOpen 
 *  
 * Will return nullptr if previous has no "Top" sink. So when it hits FLConditionClose or FLBegin. 
 * Will only return a FLConditionOpen if we are in it. 
 *  
 * \tip Compare getPos() of this with that of the condition to determine if this is positioned on Left or Right side. 
 *  
 * FLLoopOpen 
 *  
 * Will return nullptr if previous has no "Top" sink. So when it hits FLConditionClose or FLBegin. 
 * Will pass though a FLLoopBottom (because it has a "Top"). Meaning it can return a FLLoopOpen for 
 * a loop we are not in. 
 *  
 * \sa getNext 
 *  
 * \author pharvey (3/27/20)
 * 
 * \param stringInherits 
 * 
 * \return FLRectangle* 
 */
FLRectangle *FLRectangle::getPrevious( const QString &stringInherits )
{
    FLRectangle *p = this;
    do
    {
        FLFlow *pFlow = getFlow( "Top" );
        if ( !pFlow ) return nullptr;
        p = (FLRectangle*)pFlow->getRectangle( CBD::EOLBegin );
        if ( p->inherits( stringInherits.toLatin1() ) ) return p;

    } while ( 1 );

    return nullptr;
}

QPoint FLRectangle::getCell()
{
    return ((FLDiagram*)getParent( "FLDiagram" ))->mapSceneToCell( getPos() );
}

AWDataWidget *FLRectangle::getObjectWidget( QWidget *pWidgetParent )    
{                                                                       
    AWDataWidget *p = DRectangle::getObjectWidget( pWidgetParent );
                                
    p->removeByClass( "AWObjectNameWidget" );

    p->insertTab( 0, new FLNameWidget( this, p ), tr("Name") );               
                                                                        
    p->setCurrentIndex( 0 );

    return p;                                                           
}                                                                       

AWPropWidget *FLRectangle::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr("Name") );                                                                                 
    pPropWidget->removeWidget( tr("Geometry") );                                                                                 
    pPropWidget->addWidget( tr("Name"), new FLNameWidget( this, pPropWidget ) );

    return pPropWidget;

}

QDomElement FLRectangle::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement pdomElem = DRectangle::doSave( pdomDoc, pdomElemParent );

    pdomElem.setAttribute( "Macro", stringMacro );

    return pdomElem;
}

bool FLRectangle::doLoad( QDomElement *pdomElem )
{
    DRectangle::doLoad( pdomElem );

    stringMacro = pdomElem->attribute( "Macro", "" );

    return true;
}

void FLRectangle::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )   
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);                                                                                          
}                                                                                          
                                                                                           
void FLRectangle::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )    
{                                                                                          
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);                                                                                          
}                                                                                          
                                                                                           
void FLRectangle::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene ) 
{                                                                                          
    Q_UNUSED(pointScene);

    if ( pHandle->getFunction() != DHandle::FunctionMenu ) return;

    doMenu( pHandle );
}                                                                                          
                                                                                          
void FLRectangle::slotDelete()
{
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
}

void FLRectangle::doSinksCreate()
{
    vectorSinks.resize( 2 );
    vectorSinks[FLSinkTop] = new DSink( this, "Top" );
    vectorSinks[FLSinkBottom] = new DSink( this, "Bottom" );
    doSinksSync();
}

void FLRectangle::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkTop]->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
    vectorSinks[FLSinkBottom]->setPos( getProxy()->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
}

void FLRectangle::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[FLSinkTop]->setPosRaw( getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() + 1 ) ) );
    vectorSinks[FLSinkBottom]->setPosRaw( getProxy()->mapToScene( QPointF( rect.center().x(), rect.bottom() - 1 ) ) );
}

void FLRectangle::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    Q_UNUSED(pointScenePos);

    if ( pHandle == vectorHandles.at( FLMenuStep ) )                                            
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLStep", this, getSink( "Bottom" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuCondition ) )                                  
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLCondition", this, getSink( "Bottom" ) );
    }                                                                                           
    else if ( pHandle == vectorHandles.at( FLMenuLoop ) )                                    
    {                                                                                           
        FLDiagram *pModel = (FLDiagram*)getParent( "FLDiagram" );                               
        pModel->doInsert( "FLLoop", this, getSink( "Bottom" ) );
    }                                                                                           
}

void FLRectangle::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 3 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QIcon( ":FL/Step" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuStep] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Condition" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuCondition] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 32 ) ) );
    pHandle->setPixmap( QIcon( ":FL/Loop" ).pixmap( 16, 16 ) );
    vectorHandles[FLMenuLoop] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}


void FLRectangle::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void FLRectangle::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[FLMenuStep];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[FLMenuCondition];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );

    pHandle = vectorHandles[FLMenuLoop];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 32 ) ) );
}

void FLRectangle::doInitResize()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 4 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionSelectTopLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topLeft() ) );
    vectorHandles[CBD::RectangleTopLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectTopRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    vectorHandles[CBD::RectangleTopRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomRight() ) );
    vectorHandles[CBD::RectangleBottomRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomLeft() ) );
    vectorHandles[CBD::RectangleBottomLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}


