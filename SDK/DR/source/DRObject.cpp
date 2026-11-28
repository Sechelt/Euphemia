/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsView.h>

#include "DRObject.h"
#include "DRDraw.h"

DRObject::DRObject( DRDraw *pLayer, const QPointF &pointPos, const QString &stringName )
    : LObject( pLayer, stringName )
{
    bSelectable = true;

    // pointPos will probably have to get used but for now we accept 0,0 as the standard
    Q_UNUSED( pointPos );
    // setPos( pointPos );
    this->pLayer = pLayer;

    // settings...
    pSettingObject = (DRSObject*)g_SSettings->getSetting( "DRSObject" ); Q_ASSERT( pSettingObject );
    slotRefreshPen();
    slotRefreshBrush();
    slotRefreshTextFormat();
    slotRefreshTextContent();
}

DRObject::~DRObject()
{
    if ( vectorHandles.count() ) doDeleteHandles();
}

/*!
 * \brief Scene wants this to paint itself. 
 *  
 * Ignore if this is StateIdle otherwise strip down the call and pass to doDraw. 
 * 
 * \author pharvey (2/10/23)
 * 
 * \param pPainter 
 * \param nOption  
 * \param pWidget  
 */
void DRObject::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    doDraw( pPainter );
}

QRectF DRObject::boundingRect() const
{
    // add padding for line width and any handles
    int nPadding = qMax( 16, pen.width() + 2 );

    /*
    // do not use vectorHandles here - we may have count but may not set yet
    if ( vectorHandles.count() )
        nPadding = vectorHandles.at( 0 )->width() + 2;
    else
        nPadding = pen.width() + 2; // better a little extra than too little
    */

    QRectF r = getRect();
    r.moveTo( r.topLeft() - QPointF( nPadding / 2 , nPadding / 2 ) );
    r.setSize( r.size() + QSizeF( nPadding, nPadding ) );

    return r;
}

/*!
 * \brief Select/unselect object.
 * 
 * Will call DRObject::setSelected(SelectionStates) to harmonize.
 * 
 * WARNING: DO NOT call this directly when using an ADObjectSelectionManager.
 * 
 * \author pharvey (2026-10-31)
 * 
 * \param b      
 */
void DRObject::setSelected( bool b )
{
// qInfo() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Current: " << isSelected() << " Requested: " << b;
    AWObject::setSelected( b ); 
    if ( isSelected() ) setSelected( SelectionStateStandard );
    else setSelected( SelectionStateNone );
}

/*!
 * \brief Change state.
 * 
 * WARNING: Changing to/from SelectionStateNone must be done via setSelected(bool).
 *          Exception when isSelector
 * 
 * \author pharvey (2026-10-31)
 * 
 * \param n      
 */
void DRObject::setSelected( SelectionStates n )
{
// qInfo() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Current: " << getSelectionStateString( nSelectionState ) << " Requested: " << getSelectionStateString( n );
    if ( n == nSelectionState )
    {
        qInfo() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] SelectionState already is " << getSelectionStateString( nSelectionState );
        return;
    }

    // sanity check
    if ( !isSelector() )
    {
        // invalid request to switch TO SelectionStateNone?
        if ( n == SelectionStateNone ) Q_ASSERT( !isSelected() );
        // invalid request to switch FROM SelectionStateNone?
        else Q_ASSERT( isSelected() );
    }

    // undo current state
    if ( nSelectionState != SelectionStateNone ) doDisconnectSettings();
    if ( vectorHandles.count() ) doDeleteHandles();

    // set new state
    nSelectionState = n;
    if ( nSelectionState != SelectionStateNone ) doConnectSettings();
    if ( nSelectionState != SelectionStateNone && nSelectionState != SelectionStateMinimal ) doCreateHandles();

    if ( pGraphics ) pGraphics->update();
    emit signalChanged();
}

void DRObject::setTransparentHandles( bool b, LHandle *pHandleGrabbed )
{
    // some objects may only process some handles depending upon the grabbed handle
    // default is all handles are processed
    Q_UNUSED( pHandleGrabbed ); 

    // sanity checks
    Q_ASSERT( vectorHandles.count() );

    for ( LHandle *p : vectorHandles )
    {
        p->setTransparent( b );
    }
}

void DRObject::setPen( const QPen &t )
{
    pen = t; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

void DRObject::setBrush( const QBrush &t )
{
    brush = t; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

void DRObject::setTextFormat( const CBD::CBDText &t )
{
    text = t; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

void DRObject::setTextDisplayCode( bool b )
{
    bDisplayCode = b; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

void DRObject::setTextDisplayName( bool b )
{
    bDisplayName = b; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

void DRObject::setTextDisplayComment( bool b )
{
    bDisplayComment = b; 
    if ( pGraphics ) pGraphics->update();
    setModified();
}

// this may create a handle (temp or otherwise) in the future
LHandle *DRObject::getGrab( const QPointF &pointPos, bool bSizing )
{
    // derived class should override to handle bSizing
    Q_UNUSED( bSizing ); 
    // search in REVERSE order to reflect z-order
    for ( int n = vectorHandles.count() - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        QRectF r( getView()->mapToScene( p->geometry().topLeft() ).toPoint(), p->size() );
        if ( r.contains( pointPos ) ) return p;
    }
    return nullptr;
}

// this will never create a handle
LHandle *DRObject::getHandle( const QPointF &pointPos )
{
    // search in REVERSE order to reflect z-order
    for ( int n = vectorHandles.count() - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        QRectF r( getView()->mapToScene( p->geometry().topLeft() ).toPoint(), p->size() );
        if ( r.contains( pointPos ) ) return p;
    }
    return nullptr;
}

QString DRObject::getSelectionStateString( SelectionStates n )
{
    switch ( n )
    {
        case SelectionStateNone:
            return "SelectionStateNone";
        case SelectionStateMinimal:
            return "SelectionStateMinimal";
        case SelectionStateStandard:
            return "SelectionStateStandard";
        case SelectionStateScale:
            return "SelectionStateScale";
        case SelectionStateShear:
            return "SelectionStateShear";
        case SelectionStateRotate:
            return "SelectionStateRotate";
        case SelectionStateGradient:
            return "SelectionStateGradient";
    }
    return "unknown";
}

void DRObject::doMoveBy( const QPointF & )
{
    getGraphics()->doPrepareGeometryChange();
}

void DRObject::doMove( LHandle *, const QPointF & )
{
    getGraphics()->doPrepareGeometryChange();
}

void DRObject::doSelectionStateCycle()
{
    switch ( nSelectionState )
    {
        case SelectionStateNone:
            setSelected( SelectionStateStandard );
            break;
        case SelectionStateStandard:
            setSelected( SelectionStateScale );
            break;
        case SelectionStateScale:
            setSelected( SelectionStateShear );
            break;
        case SelectionStateShear:
            setSelected( SelectionStateRotate );
            break;
        case SelectionStateRotate:
            setSelected( SelectionStateStandard );
            break;
        case SelectionStateMinimal:
        case SelectionStateGradient:
            break;
    }
}

void DRObject::doCopy()
{
//     QClipboard *clipboard = QGuiApplication::clipboard();
//     clipboard->setImage( getCopy() );
#ifdef CBD_TODO
    qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] ToDo";
#endif
}

void DRObject::doZoom()
{
    if ( !vectorHandles.count() ) return;
    doSyncHandles();
}

void DRObject::doScrolled()
{
    if ( !vectorHandles.count() ) return;
    doSyncHandles();
}

QDomElement DRObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = LObject::doSave( pdomDoc, pdomElemParent );

    if ( bDisplayCode ) domElemThis.setAttribute( "DisplayCode", QString::number( 1 ) );
    if ( bDisplayName ) domElemThis.setAttribute( "DisplayName", QString::number( 1 ) );
    if ( bDisplayComment ) domElemThis.setAttribute( "DisplayComment", QString::number( 1 ) );

    QTransform t = getGraphics()->transform();
    if ( t.m11() != 1 ) domElemThis.setAttribute( "m11", t.m11() );
    if ( t.m12() ) domElemThis.setAttribute( "m12", t.m12() );
    if ( t.m13() ) domElemThis.setAttribute( "m13", t.m13() );
    if ( t.m21() ) domElemThis.setAttribute( "m21", t.m21() );
    if ( t.m22() != 1 ) domElemThis.setAttribute( "m22", t.m22() );
    if ( t.m23() ) domElemThis.setAttribute( "m23", t.m23() );
    if ( t.m31() ) domElemThis.setAttribute( "m31", t.m31() );
    if ( t.m32() ) domElemThis.setAttribute( "m32", t.m32() );
    if ( t.m33() != 1 ) domElemThis.setAttribute( "m33", t.m33() );
    if ( t.dx() ) domElemThis.setAttribute( "dx", t.dx() );
    if ( t.dy() ) domElemThis.setAttribute( "dy", t.dy() );

    domElemThis.appendChild( CBDPersistNative::doSaveText( text, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveBrush( brush, pdomDoc ) );

    return domElemThis;
}

bool DRObject::doLoad( QDomElement *pdomElemThis )
{
    LObject::doLoad( pdomElemThis );
    if ( getGraphics() ) getGraphics()->doPrepareGeometryChange();

    bDisplayCode = pdomElemThis->attribute( "DisplayCode", "0" ).toInt();
    bDisplayName = pdomElemThis->attribute( "DisplayName", "0" ).toInt();
    bDisplayComment = pdomElemThis->attribute( "DisplayComment", "0" ).toInt();

    // transformation
    {
        QTransform t;                                                         
        t.setMatrix(    pdomElemThis->attribute( "m11", "1" ).toDouble(),   
                        pdomElemThis->attribute( "m12", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m13", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m21", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m22", "1" ).toDouble(),   
                        pdomElemThis->attribute( "m23", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m31", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m32", "0" ).toDouble(),   
                        pdomElemThis->attribute( "m33", "1" ).toDouble()  );
        t.translate(    pdomElemThis->attribute( "dx", "0" ).toDouble(),    
                        pdomElemThis->attribute( "dy", "0" ).toDouble()  ); 
        if ( getGraphics() ) getGraphics()->setTransform( t );                                        
    }

    // load additional nodes...
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Pen" )
            pen = CBDPersistNative::doLoadPen( &domElem );
        else if ( domElem.tagName() == "Brush" )
            brush = CBDPersistNative::doLoadBrush( &domElem );
        else if ( domElem.tagName() == "Font" )
            text = CBDPersistNative::doLoadText( &domElem );

        domNode = domNode.nextSibling();
    }

    return true;
}

/*!
 * \brief Return a handle of given type that is below the given handle.
 * 
 * \author pharvey (2/20/23)
 * 
 * \param pHandle 
 * \param nType   
 * 
 * \return LHandle* 
 */
LHandle *DRObject::getHandleUnder( LHandle *pHandle, LHandle::Type nType )
{
    QPoint point = pHandle->geometry().center();

    // search in REVERSE order to reflect z-order
    for ( int n = vectorHandles.count() - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() != nType ) continue;          // looking for a handle this is the same type
        if ( p == pHandle ) continue;                   // and not given handle
        if ( p->geometry().contains( point ) ) return p;    // and at same pos (roughly)
    }
    return nullptr;
}

LHandle *DRObject::getHandleNext( LHandle *pHandle, LHandle::Type nType )
{
    for ( int n = vectorHandles.indexOf( pHandle ) + 1; n < vectorHandles.count(); n++ )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() == nType ) return p;
    }
    return nullptr;
}

LHandle *DRObject::getHandlePrev( LHandle *pHandle, LHandle::Type nType )
{
    for ( int n = vectorHandles.indexOf( pHandle ) - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() == nType ) return p;
    }
    return nullptr;
}

/*!
 * \brief Create handles for a rectangle. This may not be good enough for all shapes in SelectionStateStandard but probably rest is ok with this default. 
 * 
 * \author pharvey (10/20/25)
 */
void DRObject::doCreateHandles()
{
    // sanity checks
    Q_ASSERT( vectorHandles.count() == 0 );

    switch ( nSelectionState )
    {
        case SelectionStateStandard:
            // alloc handles for objects that add handles via index and leave alone for those just append (polyline/polygon)
            if ( nHandlesAlloc > 0 ) vectorHandles.resize( nHandlesAlloc );
            doCreateHandlesStandard();
            break;
        case SelectionStateScale:
        case SelectionStateShear:
        case SelectionStateRotate:
            doCreateHandlesTransform();
            break;
        case SelectionStateGradient:
            printf( "[PAH][%s][%s][%d] SelectionStateGradient not implemented.\n", __FILE__, __FUNCTION__, __LINE__ );
            break;
        case SelectionStateNone:
        case SelectionStateMinimal:
            qFatal( "[PAH][%s][%s][%d] Invalid call.\n", __FILE__, __FUNCTION__, __LINE__ );
            break;
    }
}

void DRObject::doDeleteHandles()
{
    //
    pHandle = nullptr;
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
        delete vectorHandles.at( n );
    }
    vectorHandles.clear();
}

void DRObject::doSyncHandles()
{
    switch ( nSelectionState )
    {
        case SelectionStateStandard:
            doSyncHandlesStandard();
            break;
        case SelectionStateScale:
        case SelectionStateShear:
        case SelectionStateRotate:
            doSyncHandlesTransform();
            break;
        case SelectionStateNone:
        case SelectionStateMinimal:
        case SelectionStateGradient:
            qFatal( "[PAH][%s][%s][%d] Invalid call.\n", __FILE__, __FUNCTION__, __LINE__ );
            break;
    }
}

/*!
 * \brief Alter handle types if rectangle flipped (horiz/vert or both).
 * 
 * \author pharvey (2/28/23)
 */
void DRObject::doSyncHandleTypes()
{
#ifdef CBD_TODO
printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    QRect rectView = getView()->mapFromScene( r ).boundingRect();

    if ( vectorHandles[DRRectangleBaseBegin]->geometry().contains( rectView.topLeft() ) ) vectorHandles[DRRectangleBaseBegin]->setType( LHandle::TypeSizeTopLeft );             
    else if ( vectorHandles[DRRectangleBaseBegin]->geometry().contains( rectView.topRight() ) ) vectorHandles[DRRectangleBaseBegin]->setType( LHandle::TypeSizeTopRight );      
    else if ( vectorHandles[DRRectangleBaseBegin]->geometry().contains( rectView.bottomLeft() ) ) vectorHandles[DRRectangleBaseBegin]->setType( LHandle::TypeSizeBottomLeft );  
    else if ( vectorHandles[DRRectangleBaseBegin]->geometry().contains( rectView.bottomRight() ) ) vectorHandles[DRRectangleBaseBegin]->setType( LHandle::TypeSizeBottomRight );
                                                                                                                                                                       
    if ( vectorHandles[DRRectangleBaseEnd]->geometry().contains( rectView.topLeft() ) ) vectorHandles[DRRectangleBaseEnd]->setType( LHandle::TypeSizeTopLeft );                 
    else if ( vectorHandles[DRRectangleBaseEnd]->geometry().contains( rectView.topRight() ) ) vectorHandles[DRRectangleBaseEnd]->setType( LHandle::TypeSizeTopRight );          
    else if ( vectorHandles[DRRectangleBaseEnd]->geometry().contains( rectView.bottomLeft() ) ) vectorHandles[DRRectangleBaseEnd]->setType( LHandle::TypeSizeBottomLeft );      
    else if ( vectorHandles[DRRectangleBaseEnd]->geometry().contains( rectView.bottomRight() ) ) vectorHandles[DRRectangleBaseEnd]->setType( LHandle::TypeSizeBottomRight );    
*/
}

void DRObject::doConnectSettings()
{
    pSettingObject->pPen->setSelected( pen );
    pSettingObject->pBrush->setSelected( brush );
    pSettingObject->pText->setSelected( text );
    pSettingObject->pTextContent->setSelectedCode( getCode() );
    pSettingObject->pTextContent->setSelectedName( getName() );
    pSettingObject->pTextContent->setSelectedComment( getComment() );
    connect( pSettingObject->pPen, SIGNAL(signalChanged()), this, SLOT(slotRefreshPen()) );
    connect( pSettingObject->pBrush, SIGNAL(signalChanged()), this, SLOT(slotRefreshBrush()) );
    connect( pSettingObject->pText, SIGNAL(signalChanged()), this, SLOT(slotRefreshTextFormat()) );
    connect( pSettingObject->pTextContent, SIGNAL(signalChanged()), this, SLOT(slotRefreshTextContent()) );
}

void DRObject::doDisconnectSettings()
{
    disconnect( pSettingObject->pPen, SIGNAL(signalChanged()), this, SLOT(slotRefreshPen()) );
    disconnect( pSettingObject->pBrush, SIGNAL(signalChanged()), this, SLOT(slotRefreshBrush()) );
    disconnect( pSettingObject->pText, SIGNAL(signalChanged()), this, SLOT(slotRefreshTextFormat()) );
    disconnect( pSettingObject->pTextContent, SIGNAL(signalChanged()), this, SLOT(slotRefreshTextContent()) );
}

/*!
 * \brief Create handles when selected for transform.
 * 
 * This should be good for all derived classes.
 * 
 * \author pharvey (2026-11-17)
 */
void DRObject::doCreateHandlesTransform()
{
    switch ( nSelectionState )
    {
        case SelectionStateScale:
            {
                Q_ASSERT( vectorHandles.count() == 0 );
                LGraphicsView *pView = getView();
                Q_ASSERT( getView() );

                QRectF r = boundingRect();

                // their parent will be the viewport so...
                QRect rectView = pView->mapFromScene( r ).boundingRect();

                // Order matters when handles share a position. Last handle will be found first.
                LHandle *pHandle;

                pHandle = new LHandle( this, pView, LHandle::TypeSizeLeft, QPoint( rectView.left(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();

                pHandle = new LHandle( this, pView, LHandle::TypeSizeRight, QPoint( rectView.right(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();

                pHandle = new LHandle( this, pView, LHandle::TypeSizeTop, QPoint( rectView.center().x(), rectView.top() ) );
                vectorHandles.append( pHandle );
                pHandle->show();

                pHandle = new LHandle( this, pView, LHandle::TypeSizeBottom, QPoint( rectView.center().x(), rectView.bottom() ) );
                vectorHandles.append( pHandle );
                pHandle->show();
            }
            break;
        case SelectionStateShear:
            {
                Q_ASSERT( vectorHandles.count() == 0 );
                LGraphicsView *pView = getView();
                Q_ASSERT( getView() );

                QRectF r = boundingRect();

                // their parent will be the viewport so...
                QRect rectView = pView->mapFromScene( r ).boundingRect();

                // Order matters when handles share a position. Last handle will be found first.
                LHandle *pHandle;

                pHandle = new LHandle( this, pView, LHandle::FunctionShearHIndicator, QPoint( rectView.left(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();

                pHandle = new LHandle( this, pView, LHandle::FunctionShearVIndicator, QPoint( rectView.right(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();
            }
            break;
        case SelectionStateRotate:
            {
                Q_ASSERT( vectorHandles.count() == 0 );
                LGraphicsView *pView = getView();
                Q_ASSERT( getView() );

                QRectF r = boundingRect();

                // their parent will be the viewport so...
                QRect rectView = pView->mapFromScene( r ).boundingRect();

                // Order matters when handles share a position. Last handle will be found first.
                LHandle *pHandle;

                pHandle = new LHandle( this, pView, LHandle::FunctionBaseTransform, QPoint( rectView.left(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();

                pHandle = new LHandle( this, pView, LHandle::FunctionRotateIndicator, QPoint( rectView.right(), rectView.center().y() ) );
                vectorHandles.append( pHandle );
                pHandle->show();
            }
            break;
        case SelectionStateNone:
        case SelectionStateMinimal:
        case SelectionStateStandard:
        case SelectionStateGradient:
            qFatal( "[PAH][%s][%s][%d] Invalid call.\n", __FILE__, __FUNCTION__, __LINE__ );
            break;
    }
}

void DRObject::doSyncHandlesTransform()
{
    switch ( nSelectionState )
    {
        case SelectionStateScale:
            {
                QRect rectView = getView()->mapFromScene( boundingRect() ).boundingRect();

                vectorHandles[0]->setCenter( QPoint( rectView.left(), rectView.center().y() ) );
                vectorHandles[1]->setCenter( QPoint( rectView.right(), rectView.center().y() ) );
                vectorHandles[2]->setCenter( QPoint( rectView.center().x(), rectView.top() ) );
                vectorHandles[3]->setCenter( QPoint( rectView.center().x(), rectView.bottom() ) );
            }
            break;
        case SelectionStateShear:
            break;
        case SelectionStateRotate:
        {
                QRect rectView = getView()->mapFromScene( boundingRect() ).boundingRect();
                QPointF pointBase       = rectView.center();
                QPointF pointIndicator  = pointBase - pointRotateDelta;

                vectorHandles[0]->setCenter( pointBase );
                vectorHandles[1]->setCenter( pointIndicator );
        }
            break;
        case SelectionStateNone:
        case SelectionStateMinimal:
        case SelectionStateStandard:
        case SelectionStateGradient:
            qFatal( "[PAH][%s][%s][%d] Invalid call.\n", __FILE__, __FUNCTION__, __LINE__ );
            break;
    }
}

void DRObject::slotRefreshPen()
{
    pen = pSettingObject->pPen->property( "pen" ).value<QPen>();
    if ( pGraphics ) pGraphics->update();
}

void DRObject::slotRefreshBrush()
{
    brush = pSettingObject->pBrush->property( "brush" ).value<QBrush>();
    if ( pGraphics ) pGraphics->update();
}

void DRObject::slotRefreshTextFormat()
{
    text = pSettingObject->pText->getValue();
    if ( pGraphics ) pGraphics->update();
}

void DRObject::slotRefreshTextContent()
{
    setCode( pSettingObject->pTextContent->getCode() );
    setName( pSettingObject->pTextContent->getName() );
    setComment( pSettingObject->pTextContent->getComment() );
    setTextDisplayCode( pSettingObject->pTextContent->getDisplayCode() );
    setTextDisplayName( pSettingObject->pTextContent->getDisplayName() );
    setTextDisplayComment( pSettingObject->pTextContent->getDisplayComment() );
    if ( pGraphics ) pGraphics->update();
}

/*
void DRObject::doTransform()
{
    QTransform t;
    t.rotate( -nAngle );
    t.shear( -(nShearH / 75), nShearV / 75 );

    getGraphics()->setTransform( t, false );
}

void DRObject::doInitTransform()
{
    QRectF rect = getGraphics()->boundingRect();

    pointRotateDelta = rect.center() - QPointF( rect.center().x(), rect.top() - 20 );
    pointShearHDelta = rect.center() - QPointF( rect.center().x(), rect.top() - 20 );
    pointShearVDelta = rect.center() - QPointF( rect.right() + 20, rect.center().y() );
    nAngle  = 0;
    nShearH = 0;
    nShearV = 0;
    bInitTransform = true;
}

void DRObject::doFiniTransform()
{
    pointRotateDelta = QPointF();
    pointShearHDelta = QPointF();
    pointShearVDelta = QPointF();
    nAngle  = 0;
    nShearH = 0;
    nShearV = 0;
    bInitTransform = false;
}
*/


DRSObject::DRSObject()
{
    pPen         = (SPen*)g_SSettings->getSetting( "SPen" ); Q_ASSERT( pPen );
    pBrush       = (SBrush*)g_SSettings->getSetting( "SBrush" ); Q_ASSERT( pBrush );
    pText        = (SText*)g_SSettings->getSetting( "SText" ); Q_ASSERT( pText );
    pTextContent = (STextContent*)g_SSettings->getSetting( "STextContent" ); Q_ASSERT( pText );
}

QWidget *DRSObject::getToolBar( QWidget *pParent )
{
    return pText->getToolBar( pParent );
}

QWidget *DRSObject::getPanel( QWidget *pParent )
{
    return pText->getPanel( pParent );
}

QWidget *DRSObject::getPanelMax( QWidget *pParent )
{
    QToolBox *p = new QToolBox( pParent );
    p->addItem( pBrush->getPanel( p ), QIcon(), "Brush" );
    p->addItem( pPen->getPanel( p ), QIcon(), "Pen" );
    p->addItem( pText->getPanel( p ), QIcon(), "Text Format" );
    p->addItem( pTextContent->getPanel( p ), QIcon(), "Text Content" );
    return p;
}

void DRSObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED( pdomDoc );
    Q_UNUSED( pdomElem );
}

void DRSObject::doLoad( QDomElement *pdomElem )
{
    Q_UNUSED( pdomElem );
}


