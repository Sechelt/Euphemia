/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRRectangleBase.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRRectangleBase::DRRectangleBase( DRDraw *pCanvas, const QPointF &pointPos )
    : DRObject( pCanvas, pointPos )
{
    r = QRectF( pointPos.isNull() ? QPointF( 0, 0 ) : pointPos, QSize( 1, 1 ) );
    nHandlesAlloc = 3;

    pSettingRectangleBase = (DRSRectangleBase*)g_SSettings->getSetting( "DRSRectangleBase" ); Q_ASSERT( pSettingRectangleBase );
    slotRefreshRectangle();
}

DRRectangleBase::~DRRectangleBase()
{
}

QRectF DRRectangleBase::boundingRect() const
{
    return r;
}

QPainterPath DRRectangleBase::shape() const
{
    QPainterPath path;
    path.addRect( r );
    return path;
}

void DRRectangleBase::setRect( const QRectF &r )
{
    if ( this->r == r ) return;
    getGraphics()->doPrepareGeometryChange();
    this->r = r;
    if ( vectorHandles.count() ) doSyncHandles();
}

/*!
 * \brief Return a copy of the CURRENT SHAPE.
 *  
 * This is done by getting doDraw to draw on an empty, temp, canvas and then 
 * copying from that canvas. 
 *  
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage DRRectangleBase::getImage()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QImage();
}

LHandle *DRRectangleBase::getGrab( const QPointF &pointPos, bool bSizing )
{
    if ( bSizing ) 
    {
        if ( nSelectionState != SelectionStateStandard ) return nullptr;
        return vectorHandles.at( nIndexHandleBottomRight );
    }

    return DRObject::getGrab( pointPos ); 
}

void DRRectangleBase::doMoveBy( const QPointF &pointOffset )
{
    getGraphics()->doPrepareGeometryChange();
    r.moveCenter( r.center() + pointOffset );
    if ( vectorHandles.count() ) doSyncHandles();
}

void DRRectangleBase::doMove( LHandle *pHandle, const QPointF &pointPos )
{
    Q_ASSERT( pHandle );

    getGraphics()->doPrepareGeometryChange();

    // adjust our geometry
    if ( nSelectionState == SelectionStateStandard )
    {
        if ( bBind )
        {
            if ( pHandle->getType() == LHandle::FunctionMove )
            {
                QPointF pointDiff = pointPos - r.center();
                r.setTopLeft( r.topLeft() + pointDiff );
                r.setBottomRight( r.bottomRight() + pointDiff );
            }
            else if ( pHandle->getType() == LHandle::TypeSizeTopLeft )
            {
                QPointF pointDelta = pointPos - r.topLeft();
                qreal nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
                r.setTopLeft( r.topLeft() + QPointF( nDelta, nDelta ) );
                r.setBottomRight( r.bottomRight() - QPointF( nDelta, nDelta ) );
            }
            else if ( pHandle->getType() == LHandle::TypeSizeBottomRight )
            {
                QPointF pointDelta = pointPos - r.bottomRight();
                qreal nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
                r.setTopLeft( r.topLeft() - QPointF( nDelta, nDelta ) );
                r.setBottomRight( r.bottomRight() + QPointF( nDelta, nDelta ) );
            }
        }
        else
        {
            if ( pHandle->getType() == LHandle::FunctionMove )
            {
                QPointF pointDiff = pointPos - r.center();
                r.setTopLeft( r.topLeft() + pointDiff );
                r.setBottomRight( r.bottomRight() + pointDiff );
            }
            else if ( pHandle->getType() == LHandle::TypeSizeTopLeft )
            {
                r.setTopLeft( pointPos );
            }
            else if ( pHandle->getType() == LHandle::TypeSizeBottomRight )
            {
                r.setBottomRight( pointPos );
            }
        }

        r = r.normalized();

        // adjust handles

        // their parent is the viewport so...
        QPointF pointView = getView()->mapFromScene( pointPos );

        if ( pHandle->getType() == LHandle::FunctionMove )
        {
            doSyncHandles();
        }
        else if ( pHandle->getType() == LHandle::TypeSizeTopLeft )
        {
            pHandle->setCenter( pointView );
            doSyncHandleTypes();
            doSyncHandles();
        }
        else if ( pHandle->getType() == LHandle::TypeSizeBottomRight )
        {
            pHandle->setCenter( pointView );
            doSyncHandleTypes();
            doSyncHandles();
        }
    }
}

void DRRectangleBase::doDrop( LHandle *pHandle, const QPointF &point )
{
    if ( point.isNull() ) return;
    doMove( pHandle, point );
}

QDomElement DRRectangleBase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRObject::doSave( pdomDoc, pdomElemParent );

    // size
    domElemThis.setAttribute( "Width", r.width() );
    domElemThis.setAttribute( "Height", r.height() );
    // pos
    domElemThis.setAttribute( "x", r.x() );
    domElemThis.setAttribute( "y", r.y() );
    // other
    domElemThis.setAttribute( "Bind", bBind );

    return domElemThis;
}

bool DRRectangleBase::doLoad( QDomElement *pdomElemThis )
{
    DRObject::doLoad( pdomElemThis );

    // pos
    r.setTopLeft( QPointF( pdomElemThis->attribute( "x", "0" ).toDouble(), 
                           pdomElemThis->attribute( "y", "0" ).toDouble() ) );
    // size
    r.setWidth( pdomElemThis->attribute( "Width", "10" ).toDouble() );
    r.setHeight( pdomElemThis->attribute( "Height", "10" ).toDouble() );
    // other
    bBind       = pdomElemThis->attribute( "Bind", "0" ).toInt();

    return true;
}

/*!
 * \brief Create handles for a rectangle. This may not be good enough for all shapes in SelectionStateStandard but probably rest is ok with this default. 
 * 
 * \author pharvey (10/20/25)
 */
void DRRectangleBase::doCreateHandlesStandard()
{
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() >= 3 );
    LGraphicsView *pView = getView();
    Q_ASSERT( pView );

    // default is per rect
    QRectF r = boundingRect();

    // their parent will be the viewport so...
    QRect rectView = pView->mapFromScene( r ).boundingRect();

    // Order matters when handles share a position. Last handle will be found first.
    vectorHandles.resize( nHandlesAlloc );
    vectorHandles[nIndexHandleMove]        = new LHandle( this, pView, LHandle::FunctionMove, rectView.center() );
    vectorHandles[nIndexHandleTopLeft]     = new LHandle( this, pView, LHandle::TypeSizeTopLeft, rectView.topLeft() );
    vectorHandles[nIndexHandleBottomRight] = new LHandle( this, pView, LHandle::TypeSizeBottomRight, rectView.bottomRight() );

    vectorHandles[nIndexHandleMove]->setToolTip( tr("move") );
    vectorHandles[nIndexHandleTopLeft]->setToolTip( tr("top-left") );
    vectorHandles[nIndexHandleBottomRight]->setToolTip( tr("bottom-right") );

    vectorHandles[nIndexHandleMove]->show();
    vectorHandles[nIndexHandleTopLeft]->show();
    vectorHandles[nIndexHandleBottomRight]->show();
}

void DRRectangleBase::doSyncHandlesStandard()
{
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() > 0 );

    QRect rectView = getView()->mapFromScene( boundingRect() ).boundingRect();

    vectorHandles[nIndexHandleMove]->setCenter( rectView.center() );
    vectorHandles[nIndexHandleTopLeft]->setCenter( rectView.topLeft() );
    vectorHandles[nIndexHandleBottomRight]->setCenter( rectView.bottomRight() );
}

void DRRectangleBase::doConnectSettings()
{
    DRObject::doConnectSettings();
    pSettingRectangleBase->pRectangle->setSelected( bBind );
    connect( pSettingRectangleBase, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangleBase::doDisconnectSettings()
{
    DRObject::doDisconnectSettings();
    disconnect( pSettingRectangleBase, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangleBase::slotRefreshRectangle()
{
    // we only care about the bind property in this class
    bBind = pSettingRectangleBase->pRectangle->property( "bBind" ).toInt();
}

DRSRectangleBase::DRSRectangleBase()
{
    pRectangle = (LSRectangle*)g_SSettings->getSetting( "LSRectangle" ); Q_ASSERT( pRectangle );
    connect( pRectangle, SIGNAL(signalChanged()), SIGNAL(signalChanged()) );
}

QWidget *DRSRectangleBase::getButton( QWidget *pParent )
{
    return pRectangle->getButton( pParent );
}

QWidget *DRSRectangleBase::getToolBar( QWidget *pParent )
{
    return pRectangle->getToolBar( pParent );
}

QWidget *DRSRectangleBase::getPanel( QWidget *pParent )
{
    return pRectangle->getPanel( pParent );
}

QWidget *DRSRectangleBase::getSwatch( QWidget *pParent )
{
    return pRectangle->getSwatch( pParent );
}

QToolBox *DRSRectangleBase::getToolBox( QWidget *pParent )
{
    QToolBox *p = DRSObject::getToolBox( pParent );
    p->insertItem( 0, pRectangle->getPanel( p ), QIcon(), "Rectangle" );
    return p;
}

void DRSRectangleBase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED( pdomDoc );
    Q_UNUSED( pdomElem );
}

void DRSRectangleBase::doLoad( QDomElement *pdomElem )
{
    Q_UNUSED( pdomElem );
}


