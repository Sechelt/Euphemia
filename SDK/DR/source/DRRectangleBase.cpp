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

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

#define DRRectangleBaseBegin 0
#define DRRectangleBaseMove 1
#define DRRectangleBaseEnd 2

DRRectangleBase::DRRectangleBase( DRDraw *pCanvas, const QPointF &pointPos )
    : DRObject( pCanvas, pointPos )
{
    r = QRectF( pointPos.isNull() ? QPointF( 0, 0 ) : pointPos, QSize( 1, 1 ) );

    pSettingRectangle   = g_SSettings->getSetting( "LSRectangle" ); Q_ASSERT( pSettingRectangle );
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
        return vectorHandles.at( DRRectangleBaseEnd );
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
    if ( bBind )
    {
        if ( pHandle == vectorHandles[DRRectangleBaseBegin] )
        {
            QPointF pointDelta = pointPos - r.topLeft();
            qreal nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() + QPointF( nDelta, nDelta ) );
            r.setBottomRight( r.bottomRight() - QPointF( nDelta, nDelta ) );
        }
        else if ( pHandle == vectorHandles[DRRectangleBaseMove] )
        {
            QPointF pointDiff = pointPos - r.center();
            r.setTopLeft( r.topLeft() + pointDiff );
            r.setBottomRight( r.bottomRight() + pointDiff );
        }
        else if ( pHandle == vectorHandles[DRRectangleBaseEnd] )
        {
            QPointF pointDelta = pointPos - r.bottomRight();
            qreal nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() - QPointF( nDelta, nDelta ) );
            r.setBottomRight( r.bottomRight() + QPointF( nDelta, nDelta ) );
        }
    }
    else
    {
        if ( pHandle == vectorHandles[DRRectangleBaseBegin] )
        {
            r.setTopLeft( pointPos );
        }
        else if ( pHandle == vectorHandles[DRRectangleBaseMove] )
        {
            QPointF pointDiff = pointPos - r.center();
            r.setTopLeft( r.topLeft() + pointDiff );
            r.setBottomRight( r.bottomRight() + pointDiff );
        }
        else if ( pHandle == vectorHandles[DRRectangleBaseEnd] )
        {
            r.setBottomRight( pointPos );
        }
    }

    r = r.normalized();

    // adjust handles

    // their parent is the viewport so...
    QPointF pointView = getView()->mapFromScene( pointPos );

    if ( pHandle == vectorHandles[DRRectangleBaseBegin] )
    {
        pHandle->setCenter( pointView );
        doSyncHandleTypes();
        doSyncHandles();
    }
    else if ( pHandle == vectorHandles[DRRectangleBaseMove] )
    {
        doSyncHandles();
    }
    else if ( pHandle == vectorHandles[DRRectangleBaseEnd] )
    {
        pHandle->setCenter( pointView );
        doSyncHandleTypes();
        doSyncHandles();
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

void DRRectangleBase::doConnectSettings()
{
    DRObject::doConnectSettings();
    connect( pSettingRectangle, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangleBase::doDisconnectSettings()
{
    DRObject::doDisconnectSettings();
    disconnect( pSettingRectangle, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangleBase::slotRefreshRectangle()
{
    bBind = pSettingRectangle->property( "bBind" ).toInt();
}

