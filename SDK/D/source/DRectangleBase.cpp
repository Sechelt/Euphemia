#include "LibInfo.h"
#include "DRectangleBase.h"

#include <WGeometryWidget.h>
#include <WLayoutWidget.h>

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

DRectangleBase::DRectangleBase( ADObject *pObjectParent, const QString &stringName )
    : DObject( pObjectParent, stringName )
{
    // sanity check - we need a diagram even if not immediate parent
    DDiagram *pDiagram = nullptr;
    if ( pObjectParent->inherits( "DDiagram" ) ) pDiagram = (DDiagram*)pObjectParent;
    else pDiagram = (DDiagram *)getParent( "DDiagram" );
    Q_ASSERT( pDiagram );

    // adopt cell width/height as our size
    // origin is top-left
    rect.setCoords( 0, 0, pDiagram->getCell().width(), pDiagram->getCell().height() );

    // \note an empty QSizeF will have values of ( -1, 0 ) and we do not want that so init explicitly
    sizeHint        = QSizeF( pDiagram->getCell().width(), pDiagram->getCell().height() );
    sizeMinimum     = sizeHint;
    sizeMaximum     = QSizeF( 0, 0 );
    sizeIncrement   = QSizeF( 1, 1 );
    sizeStretch     = QSize( 0, 0 );
}

DRectangleBase::~DRectangleBase()
{
}

/*!
 * \brief Set size. 
 *  
 * Calling \sa slotSize() is preferrable as it more strictly validates the call. 
 *  
 * This is used when loading and by any layout in use. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param size 
 */
void DRectangleBase::setSize( const QSizeF &size )
{
    getProxy()->doPrepareGeometryChange();
    rect.setSize( size );
    if ( isSelected() ) doHandlesSync();
    emit signalChangedSize( rect.size() );
}


/*!
 * \brief Set position. 
 *  
 * Calling \sa slotPos() is preferrable as it more strictly validates the call. 
 *  
 * This is used when loading and by any layout in use. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param pointScene 
 */
void DRectangleBase::setPos( const QPointF &pointScene )
{
   DObject::setPos( pointScene );
   if ( isSelected() ) doHandlesSync();
   emit signalChangedPos( pointScene );
}

void DRectangleBase::setPosRaw( const QPointF &pointScene )
{
    DObject::setPosRaw( pointScene );
    if ( isSelected() ) doHandlesSync();
}

void DRectangleBase::setStretch( const QSizeF &size )
{
    sizeStretch = size;
    emit signalChangedStretch( size );
}

void DRectangleBase::setSizeHint( const QSizeF &size )
{
    sizeHint = size;
    emit signalChangedSizeHint( size );
}

void DRectangleBase::setSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax )
{
    sizeMinimum = sizeMin;
    sizeMaximum = sizeMax;
    emit signalChangedSizeConstraints( sizeMin, sizeMax );
}

void DRectangleBase::setSizeIncrement( const QSizeF &size )
{
    sizeIncrement = size;
    emit signalChangedSizeIncrement( size );
}

void DRectangleBase::setMargins( const QMargins &m )
{
    margins = m;
    emit signalChangedMargins( m );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DRectangleBase::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DObject::getObjectWidget( pWidgetParent );

    return p;
}

AWPropWidget *DRectangleBase::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DObject::getPropWidget( pWidgetParent );

    // geometry
    {
        WGeometryWidget *p = new WGeometryWidget( getPos(), getSize(), pPropWidget );
        // pos
        connect( p, SIGNAL(signalChangedPos(const QPointF &)), this, SLOT(slotPos(const QPointF &)) );
        connect( this, SIGNAL(signalChangedPos(const QPointF &)), p, SLOT(slotPos(const QPointF &)) );
        // size
        connect( p, SIGNAL(signalChangedSize(const QSizeF &)), this, SLOT(slotSize(const QSizeF &)) );
        connect( this, SIGNAL(signalChangedSize(const QSizeF &)), p, SLOT(slotSize(const QSizeF &)) );

        pPropWidget->addWidget( tr("Geometry"), p );
    }
    // layout
    {
        WLayoutWidget *p = new WLayoutWidget( pPropWidget );
        // size hint
        p->setSizeHint( getSizeHint() );
        connect( p, SIGNAL(signalChangedSizeHint(const QSizeF &)), this, SLOT(slotSizeHint(const QSizeF &)) );
        connect( this, SIGNAL(signalChangedSizeHint(const QSizeF &)), p, SLOT(slotSizeHint(const QSizeF &)) );
        // margins
        p->setMargins( margins );
        connect( p, SIGNAL(signalChangedMargins(const QMargins &)), this, SLOT(slotMargins(const QMargins &)) );
        connect( this, SIGNAL(signalChangedMargins(const QMargins &)), p, SLOT(slotMargins(const QMargins &)) );
        // constraints
        p->setSizeConstraints( getSizeMinimum(), getSizeMaximum() );
        connect( p, SIGNAL(signalChangedSizeConstraints(const QSizeF &,const QSizeF &)), this, SLOT(slotSizeConstraints(const QSizeF &,const QSizeF &)) );
        connect( this, SIGNAL(signalChangedSizeConstraints(const QSizeF &,const QSizeF &)), p, SLOT(slotSizeConstraints(const QSizeF &,const QSizeF &)) );
        // stretch
        p->setStretch( getStretch() );
        connect( p, SIGNAL(signalChangedStretch(const QSizeF &)), this, SLOT(slotStretch(const QSizeF &)) );
        connect( this, SIGNAL(signalChangedStretch(const QSizeF &)), p, SLOT(slotStretch(const QSizeF &)) );

        pPropWidget->addWidget( tr("Layout"), p );
    }

    return pPropWidget;
}

QRectF DRectangleBase::boundingRect() const
{
    return rect;
}

QPainterPath DRectangleBase::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}

void DRectangleBase::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget )
{
    Q_UNUSED( pOption );
    Q_UNUSED( pWidget );

    paintAt( pPainter, rect.topLeft() );
}

void DRectangleBase::doGroupSync()
{
    if ( isSelected() ) doHandlesSync();
}

/*!
 * \brief Scale pos and size.
 *  
 * \sa DGroup::doScale 
 *  
 * \author pharvey (10/31/20)
 * 
 * \param sizeFactor 
 */
void DRectangleBase::doGroupScale( const QSizeF &sizeFactor )
{
    // adjust pos
    {
        QPointF point = getPos();
        QPointF pointNew( point.x() * sizeFactor.width(), point.y() * sizeFactor.height() );
        setPos( pointNew );
    }

    // adjust size
    {
        QSizeF size = getSize();
        QSizeF sizeNew( size.width() * sizeFactor.width(), size.height() * sizeFactor.height() );
        setSize( sizeNew );
    }
}

void DRectangleBase::doGroupIn( const QPointF &pointDelta )
{
    setPos( getPos() - pointDelta );
}

void DRectangleBase::doGroupOut( const QPointF &pointDelta )
{
    setPos( getPos() + pointDelta );
}

QDomElement DRectangleBase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DObject::doSave( pdomDoc, pdomElemParent );

    // size
    domElemThis.setAttribute( "Width", rect.width() );
    domElemThis.setAttribute( "Height", rect.height() );
    // pos
    domElemThis.setAttribute( "x", getProxy()->pos().x() );
    domElemThis.setAttribute( "y", getProxy()->pos().y() );
    // size stretch
    domElemThis.setAttribute( "Stretch.x", sizeStretch.width() );
    domElemThis.setAttribute( "Stretch.y", sizeStretch.height() );
    // size constraints
    domElemThis.setAttribute( "Min.x", sizeMinimum.width() );
    domElemThis.setAttribute( "Min.y", sizeMinimum.height() );
    domElemThis.setAttribute( "Max.x", sizeMaximum.width() );
    domElemThis.setAttribute( "Max.y", sizeMaximum.height() );
    // size increment
    domElemThis.setAttribute( "Inc.x", sizeIncrement.width() );
    domElemThis.setAttribute( "Inc.y", sizeIncrement.height() );
    // size hint
    domElemThis.setAttribute( "Hint.x", sizeHint.width() );
    domElemThis.setAttribute( "Hint.y", sizeHint.height() );
    // margins
    domElemThis.setAttribute( "Margin.N", margins.top() );
    domElemThis.setAttribute( "Margin.S", margins.bottom() );
    domElemThis.setAttribute( "Margin.W", margins.left() );
    domElemThis.setAttribute( "Margin.E", margins.right() );

    // do this in case child has changed
    domElemThis.setAttribute( "NextOID", nNextOID );

    return domElemThis;
}

bool DRectangleBase::doLoad( QDomElement *pdomElemObject )
{
    DObject::doLoad( pdomElemObject );

    getProxy()->doPrepareGeometryChange();

    QString stringAttribute;

    // pos
    {
        QPointF point( pdomElemObject->attribute( "x", "0" ).toDouble(), 
                       pdomElemObject->attribute( "y", "0" ).toDouble() );

        // bypass our setPos
        getProxy()->setPos( point );
        pointPos = point;
    }
    // size
    rect.setWidth( pdomElemObject->attribute( "Width", "10" ).toDouble() );
    rect.setHeight( pdomElemObject->attribute( "Height", "10" ).toDouble() );
    // size stretch
    sizeStretch.setWidth( pdomElemObject->attribute( "Stretch.x", "0" ).toDouble() );
    sizeStretch.setHeight( pdomElemObject->attribute( "Stretch.y", "0" ).toDouble() );
    // size constraints
    sizeMinimum.setWidth( pdomElemObject->attribute( "Min.x", "0" ).toDouble() );
    sizeMinimum.setHeight( pdomElemObject->attribute( "Min.y", "0" ).toDouble() );
    sizeMaximum.setWidth( pdomElemObject->attribute( "Max.x", "0" ).toDouble() );
    sizeMaximum.setHeight( pdomElemObject->attribute( "Max.y", "0" ).toDouble() );
    // size increment
    sizeIncrement.setWidth( pdomElemObject->attribute( "Inc.x", "0" ).toDouble() );
    sizeIncrement.setHeight( pdomElemObject->attribute( "Inc.y", "0" ).toDouble() );
    // size hint
    sizeHint.setWidth( pdomElemObject->attribute( "Hint.x", "0" ).toDouble() );
    sizeHint.setHeight( pdomElemObject->attribute( "Hint.y", "0" ).toDouble() );
    // margins
    margins.setTop( pdomElemObject->attribute( "Margin.N", "0" ).toInt() );
    margins.setBottom( pdomElemObject->attribute( "Margin.S", "0" ).toInt() );
    margins.setLeft( pdomElemObject->attribute( "Margin.W", "0" ).toInt() );
    margins.setRight( pdomElemObject->attribute( "Margin.E", "0" ).toInt() );

    return true;
}

void DRectangleBase::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);
}

void DRectangleBase::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    doHandleMove( pHandle, pointScene );
}

void DRectangleBase::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    doHandleMove( pHandle, pointScene );
}

/*!
 * \brief Request to set position. 
 *  
 * This should be used, instead of \sa setPos(), as much as possible 
 * because it more strictly validates the call. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param pointPos 
 */
void DRectangleBase::slotPos( const QPointF pointPos )
{
    // nothing to do - short circuit to end any round-trip request
    if ( pointPos == getPos() ) return;

    // can we change pos?
    if ( inLayout() )
    {
        // DLayout will use setPos() so this must be from a control
        // continue the round-trip request by sending the control the unchanged value (request denied)
        emit signalChangedPos( getPos() );
        return;
    }

    // do it
    setPos( pointPos );
    setModified();
}

/*!
 * \brief Request to set size. 
 *  
 * This should be used, instead of \sa setSize() or \sa setRect(), as much as possible 
 * because it more strictly validates the call. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param size 
 */
void DRectangleBase::slotSize( const QSizeF &size )
{
    // nothing to do - short circuit to end any round-trip request
    if ( size == rect.size() ) return;

    // can we change size?
    if ( inLayout() )
    {
        emit signalChangedSize( rect.size() );
        return;
    }

    // do it
    setSize( size );
    setModified();
}

void DRectangleBase::slotStretch( const QSizeF &size )
{
    if ( size == sizeStretch ) return;
    setStretch( size );
    setModified();
    emit signalChangedLayout();
}

void DRectangleBase::slotSizeHint( const QSizeF &size )
{
    if ( size == sizeHint ) return;
    setSizeHint( size );
    setModified();
    emit signalChangedLayout();
}

void DRectangleBase::slotSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax )
{
    if ( sizeMin == sizeMinimum && sizeMax == sizeMaximum ) return;
    setSizeConstraints( sizeMin, sizeMax );
    setModified();
    emit signalChangedLayout();
}

void DRectangleBase::slotSizeIncrement( const QSizeF &size )
{
    if ( size == sizeIncrement ) return;
    setSizeIncrement( size );
    setModified();
    emit signalChangedLayout();
}

void DRectangleBase::slotMargins( const QMargins &m )
{
    if ( m == margins ) return;
    setMargins( m );
    setModified();
    emit signalChangedLayout();
}

/*!
 * \brief Handle resize request - presumably a sizer handle has been moved. 
 *  
 * This will; 
 *  
 * - prevent inverting the rectangle 
 * - honour any size constraints 
 *  
 * \author pharvey (9/21/20)
 * 
 * \param pHandle 
 * \param pointScenePos 
 */
void DRectangleBase::doResize( DHandle *pHandle, const QPointF &pointScenePos )
{
    int nIndex = vectorHandles.indexOf( pHandle );
    if ( nIndex < 0 ) return;

    QPointF pointSceneProposed  = pointScenePos;                        // because we may adjust request
    QRectF  rectScene           = getProxy()->mapRectToScene( rect );   // lets work in scene coords

    if ( nIndex == CBD::RectangleTopLeft )
    {
        // avoid inverting rectangle
        if ( pointSceneProposed.x() > rectScene.right() ) pointSceneProposed.setX( rectScene.right() );
        if ( pointSceneProposed.y() > rectScene.bottom() ) pointSceneProposed.setY( rectScene.bottom() );

        QSizeF size( rect.width() + rectScene.topLeft().x() - pointSceneProposed.x(), rect.height() + rectScene.topLeft().y() - pointSceneProposed.y() );

        // honour min size
        if ( sizeMinimum.width() && size.width() < sizeMinimum.width() )
        {
            pointSceneProposed.setX( pointSceneProposed.x() - (sizeMinimum.width() - size.width()) ); 
            size.setWidth( sizeMinimum.width() );
        }
        if ( sizeMinimum.height() && size.height() < sizeMinimum.height() )
        {
            pointSceneProposed.setY( pointSceneProposed.y() - (sizeMinimum.height() - size.height()) ); 
            size.setHeight( sizeMinimum.height() );
        }

        // honour max size
        if ( sizeMaximum.width() && size.width() > sizeMaximum.width() )
        {
            pointSceneProposed.setX( pointSceneProposed.x() + (size.width() - sizeMaximum.width()) ); 
            size.setWidth( sizeMaximum.width() );
        }
        if ( sizeMaximum.height() && size.height() > sizeMaximum.height() )
        {
            pointSceneProposed.setY( pointSceneProposed.y() + (size.height() - sizeMaximum.height()) ); 
            size.setHeight( sizeMaximum.height() );
        }

        setSize( size );
        setPos( pointSceneProposed );
    }
    else if ( nIndex == CBD::RectangleTopRight )
    {
        // avoid inverting rectangle
        if ( pointSceneProposed.x() < rectScene.left() ) pointSceneProposed.setX( rectScene.left() );
        if ( pointSceneProposed.y() > rectScene.bottom() ) pointSceneProposed.setY( rectScene.bottom() );

        QSizeF size( rect.width() + (pointSceneProposed.x() - rectScene.right()), rect.height() + (rectScene.top() - pointSceneProposed.y()) );

        // honour min size                                                                             
        if ( sizeMinimum.width() && size.width() < sizeMinimum.width() )                               
        {                                                                                              
            pointSceneProposed.setX( pointSceneProposed.x() + (sizeMinimum.width() - size.width()) );  
            size.setWidth( sizeMinimum.width() );                                                      
        }                                                                                              
        if ( sizeMinimum.height() && size.height() < sizeMinimum.height() )                            
        {                                                                                              
            pointSceneProposed.setY( pointSceneProposed.y() - (sizeMinimum.height() - size.height()) );
            size.setHeight( sizeMinimum.height() );                                                    
        }                                                                                              
                                                                                                       
        // honour max size                                                                             
        if ( sizeMaximum.width() && size.width() > sizeMaximum.width() )                               
        {                                                                                              
            pointSceneProposed.setX( pointSceneProposed.x() - (size.width() - sizeMaximum.width()) );  
            size.setWidth( sizeMaximum.width() );                                                      
        }                                                                                              
        if ( sizeMaximum.height() && size.height() > sizeMaximum.height() )                            
        {                                                                                              
            pointSceneProposed.setY( pointSceneProposed.y() + (size.height() - sizeMaximum.height()) );
            size.setHeight( sizeMaximum.height() );                                                    
        }                                                                                              

        setSize( size );
        setPos( QPointF( pointSceneProposed.x() - size.width(), pointSceneProposed.y() ) );
    }
    else if ( nIndex == CBD::RectangleBottomRight )
    {
        // avoid inverting rectangle
        if ( pointSceneProposed.x() < rectScene.left() ) pointSceneProposed.setX( rectScene.left() );
        if ( pointSceneProposed.y() < rectScene.top() ) pointSceneProposed.setY( rectScene.top() );

        QSizeF size( rect.width() + pointSceneProposed.x() - rectScene.bottomRight().x(), rect.height() + pointSceneProposed.y() - rectScene.bottomRight().y() );

        // honour min size
        if ( sizeMinimum.width() && size.width() < sizeMinimum.width() )
        {
            pointSceneProposed.setX( pointSceneProposed.x() + (sizeMinimum.width() - size.width()) ); 
            size.setWidth( sizeMinimum.width() );
        }
        if ( sizeMinimum.height() && size.height() < sizeMinimum.height() )
        {
            pointSceneProposed.setY( pointSceneProposed.y() + (sizeMinimum.height() - size.height()) ); 
            size.setHeight( sizeMinimum.height() );
        }

        // honour max size
        if ( sizeMaximum.width() && size.width() > sizeMaximum.width() )
        {
            pointSceneProposed.setX( pointSceneProposed.x() - (size.width() - sizeMaximum.width()) ); 
            size.setWidth( sizeMaximum.width() );
        }
        if ( sizeMaximum.height() && size.height() > sizeMaximum.height() )
        {
            pointSceneProposed.setY( pointSceneProposed.y() - (size.height() - sizeMaximum.height()) ); 
            size.setHeight( sizeMaximum.height() );
        }

        // update rect
        setSize( size );
        // pos (top-right) never changes in this case
    }
    else if ( nIndex == CBD::RectangleBottomLeft )
    {
        // avoid inverting rectangle
        if ( pointSceneProposed.x() > rectScene.right() ) pointSceneProposed.setX( rectScene.right() );
        if ( pointSceneProposed.y() < rectScene.top() ) pointSceneProposed.setY( rectScene.top() );

        QSizeF size( rect.width() + (rectScene.left() - pointSceneProposed.x()), rect.height() + (pointSceneProposed.y() - rectScene.bottom()) );

        // honour min size                                                                             
        if ( sizeMinimum.width() && size.width() < sizeMinimum.width() )                               
        {                                                                                              
            pointSceneProposed.setX( pointSceneProposed.x() - (sizeMinimum.width() - size.width()) );  
            size.setWidth( sizeMinimum.width() );                                                      
        }                                                                                              
        if ( sizeMinimum.height() && size.height() < sizeMinimum.height() )                            
        {                                                                                              
            pointSceneProposed.setY( pointSceneProposed.y() + (sizeMinimum.height() - size.height()) );
            size.setHeight( sizeMinimum.height() );                                                    
        }                                                                                              
                                                                                                       
        // honour max size                                                                             
        if ( sizeMaximum.width() && size.width() > sizeMaximum.width() )                               
        {                                                                                              
            pointSceneProposed.setX( pointSceneProposed.x() + (size.width() - sizeMaximum.width()) );  
            size.setWidth( sizeMaximum.width() );                                                      
        }                                                                                              
        if ( sizeMaximum.height() && size.height() > sizeMaximum.height() )                            
        {                                                                                              
            pointSceneProposed.setY( pointSceneProposed.y() - (size.height() - sizeMaximum.height()) );
            size.setHeight( sizeMaximum.height() );                                                    
        }                                                                                              

        setSize( size );
        setPos( QPointF( pointSceneProposed.x(), pointSceneProposed.y() - size.height() ) );
    }
    else
        return;

    // sync handles
    doHandlesSync();

    emit signalChangedSize( rect.size() );
}

void DRectangleBase::doInitResize()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 4 );

    DHandle *pHandle;

    if ( inLayout() )
    { 
        // We can be selected in a layout because we can be dragged out of (or deleted from the) layout.
        // However; we do not want resize handles - just simple select handles.
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
    else
    {
        // The default is that being selected means having resize handles.
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
}

void DRectangleBase::doFiniResize()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void DRectangleBase::doSyncResize()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[CBD::RectangleTopLeft];
    pHandle->setPos( pProxy->mapToScene( rect.topLeft() ) );

    pHandle = vectorHandles[CBD::RectangleTopRight];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[CBD::RectangleBottomRight];
    pHandle->setPos( pProxy->mapToScene( rect.bottomRight() ) );

    pHandle = vectorHandles[CBD::RectangleBottomLeft];
    pHandle->setPos( pProxy->mapToScene( rect.bottomLeft() ) );
}


