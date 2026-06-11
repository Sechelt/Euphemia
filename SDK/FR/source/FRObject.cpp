#include "LibInfo.h"
#include "FRObject.h"

#include <DGraphicsSceneProxy.h>

#include "FRGraphicsProxyObject.h"
#include "FRForm.h"

FRObject::FRObject( FRForm *pForm, const QString &stringName )
    : DRectangle( pForm, stringName )
{
}

FRObject::~FRObject()
{
}

void FRObject::setSize( const QSizeF &size )
{
    DRectangle::setSize( size );
    getProxy()->resize( size );                                                                                                             
    return;

    if ( isFixedSize() ) return;
    if ( size == rect.size() ) return;

    FRGraphicsProxyObject *pProxy = (FRGraphicsProxyObject*)getProxy();

    pProxy->doPrepareGeometryChange();

    pProxy->resize( size );                                                                                                             
    rect.setSize( pProxy->size() );                                                                                                        

    // sync handles
    doHandlesSync();
    doSinksSync();
    setModified();

    // the following can change the document coordinates in a bad way so do not do it... 
    // getDiagram()->doEnsurePages();

    emit signalChangedShapeSize( getShapeSize() );
    emit signalChangedSize( rect.size() );
}

/*!
 * \brief Do nothing.
 * 
 * This comes from the widget and is always read-only.
 * 
 * \author pharvey (2026-02-28)
 * 
 * \param size   
 */
void FRObject::setSizeHint( const QSizeF &size )
{
    Q_UNUSED(size);
}

/*!
 * \brief Do nothing.
 * 
 * This comes from the widget and is always read-only.
 * 
 * \author pharvey (2026-02-28)
 * 
 * \param sizeMin 
 * \param sizeMax 
 */
void FRObject::setSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax )
{
    Q_UNUSED(sizeMin);
    Q_UNUSED(sizeMax);
}

AWPropWidget *FRObject::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr("Font") );                                                                                 
    pPropWidget->removeWidget( tr("Pen") );                                                                                 
    pPropWidget->removeWidget( tr("Brush") );                                                                                 
    pPropWidget->removeWidget( tr("Text") );                                                                                 

    return pPropWidget;

}

const QMetaObject *FRObject::getMetaObject() 
{ 
    return getWidget()->metaObject(); 
}

QWidget *FRObject::getWidget()
{
    Q_ASSERT( getProxy() );
    Q_ASSERT( getProxy()->getItem() );
    Q_ASSERT( getProxy()->getItem()->inherits( "FRGraphicsObject" ) );
    Q_ASSERT( ((FRGraphicsProxyObject*)getProxy())->widget() );
    return ((FRGraphicsProxyObject*)getProxy())->widget();
}

QSizeF FRObject::getSizeHint()
{
    return getWidget()->sizeHint();
}

QSizeF FRObject::getSizeMinimum()
{
    return getWidget()->minimumSize();
}

QSizeF FRObject::getSizeMaximum()
{
    return getWidget()->maximumSize();
}

/*
void FRObject::slotSize( const QSizeF &size )                                           
{                                                                                       
    if ( isFixedSize() ) return;                                                        
    if ( size == rect.size() ) return;                                                  
                                                                                        
    FRGraphicsProxyObject *pProxy = (FRGraphicsProxyObject*)getProxy();                 
                                                                                        
    pProxy->doPrepareGeometryChange();                                                  
                                                                                        
    pProxy->resize( size );                                                             
    rect.setSize( pProxy->size() );                                                     
                                                                                        
    // sync handles                                                                     
    doHandlesSync();                                                                    
    doSinksSync();                                                                      
    setModified();                                                                      
                                                                                        
    // the following can change the document coordinates in a bad way so do not do it...
    // getDiagram()->doEnsurePages();                                                   
                                                                                        
    emit signalChangedShapeSize( getShapeSize() );                                      
    emit signalChangedSize( rect.size() );                                              
}                                                                                       
*/

/*!
 * \brief Do nothing. 
 * 
 * This comes from the widget and is always read-only.
 * 
 * \author pharvey (2026-02-28)
 * 
 * \param size   
 */
void FRObject::slotSizeHint( const QSizeF &size )
{
    Q_UNUSED(size);
}

/*!
 * \brief Do nothing.
 * 
 * This comes from the widget and is always read-only.
 * 
 * \author pharvey (2026-02-28)
 * 
 * \param sizeMin 
 * \param sizeMax 
 */
void FRObject::slotSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax )
{
    Q_UNUSED(sizeMin);
    Q_UNUSED(sizeMax);
}

/*!
 * \brief Resize, and possibly reposition, based upon a sizer handle moving.
 *  
 * This is a bit glitchy but gets worse when trying to use; 
 *  
 * - getProxy()->geometry() 
 * - getProxy()->mapRectToScene() 
 *  
 * Possibly because updates are not happening straight away. 
 *  
 * \author pharvey (7/30/20)
 * 
 * \param pHandle 
 * \param pointScenePos 
 */
void FRObject::doResize( DHandle *pHandle, const QPointF &pointScenePos )                                                                   
{   
    int n = vectorHandles.indexOf( pHandle );                                                                                                 
    if ( n < 0 ) return;                                                                                                                      

    getProxy()->doPrepareGeometryChange();                                                                                                    
                                                                                                                                              
    int nShapeSize = getShapeSize();                                                                                                          
                                                                                                                                                  
    QRectF r( getPos(), rect.size() );
    if ( n == CBD::RectangleTopLeft ) r.setTopLeft( pointScenePos );                                                                          
    else if ( n == CBD::RectangleTopRight ) r.setTopRight( pointScenePos );                                                                   
    else if ( n == CBD::RectangleBottomRight ) r.setBottomRight( pointScenePos );                                                             
    else if ( n == CBD::RectangleBottomLeft ) r.setBottomLeft( pointScenePos );                                                               

    // set it...
    // getProxy()->setGeometry( r );                                                                                                             
    getProxy()->setPos( r.topLeft() );                                                                                                             
    getProxy()->resize( r.size() );                                                                                                             
    // now see what took...
    rect.setSize( getProxy()->size() );                                                                                                        
    if ( n != CBD::RectangleBottomRight ) setPos( r.topLeft() );
    doHandlesSync();
    doSinksSync();

    if ( nShapeSize != getShapeSize() )                                                                                                       
    {                                                                                                                                         
        emit signalChangedShapeSize( getShapeSize() );                                                                                        
    }                                                                                                                                         
    emit signalChangedSize( rect.size() );
}                                                                                                                                             
                                                                                                                                              
void FRObject::doInitResize()                                                                                                               
{            
    if ( vectorHandles.count() > 0 ) return;                                                                                                  
    if ( !getProxy() ) return;                                                                                                                
                                                                                                                                              
    vectorHandles.resize( 4 );                                                                                                                
                                                                                                                                              
    DHandle *pHandle;                                                                                                                         
                                                                                                                                              
    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topLeft() ) );    
    vectorHandles[CBD::RectangleTopLeft] = pHandle;                                                                                           
    getProxy()->scene()->addItem( pHandle );                                                                                                  
                                                                                                                                              
    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );   
    vectorHandles[CBD::RectangleTopRight] = pHandle;                                                                                          
    getProxy()->scene()->addItem( pHandle );                                                                                                  
                                                                                                                                              
    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomRight() ) );
    vectorHandles[CBD::RectangleBottomRight] = pHandle;                                                                                       
    getProxy()->scene()->addItem( pHandle );                                                                                                  
                                                                                                                                              
    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomLeft() ) ); 
    vectorHandles[CBD::RectangleBottomLeft] = pHandle;                                                                                        
    getProxy()->scene()->addItem( pHandle );                                                                                                  
}                                                                                                                                             
                                                                                                                                              
void FRObject::doSyncResize()                                                                                                               
{
    if ( vectorHandles.count() < 1 ) return;                                                                                                  
    if ( !getProxy() ) return;                                                                                                                
                                                                                                                                              
    DHandle *pHandle;                                                                                                                         
                                                                                                                                              
    pHandle = vectorHandles[CBD::RectangleTopLeft];                                                                                           
    pHandle->setPos( getProxy()->mapToScene( rect.topLeft() ) );                                                                              
                                                                                                                                              
    pHandle = vectorHandles[CBD::RectangleTopRight];                                                                                          
    pHandle->setPos( getProxy()->mapToScene( rect.topRight() ) );                                                                             
                                                                                                                                              
    pHandle = vectorHandles[CBD::RectangleBottomRight];                                                                                       
    pHandle->setPos( getProxy()->mapToScene( rect.bottomRight() ) );                                                                          
                                                                                                                                              
    pHandle = vectorHandles[CBD::RectangleBottomLeft];                                                                                        
    pHandle->setPos( getProxy()->mapToScene( rect.bottomLeft() ) );                                                                           
}                                                                                                                                             

//
//
//
FRObjectWidget::FRObjectWidget( FRObject *p, QWidget *pParent )
    : QWidget( pParent )
{
    Q_ASSERT( p );
    pObject = p;

}

FRObjectWidget::~FRObjectWidget()
{
}


