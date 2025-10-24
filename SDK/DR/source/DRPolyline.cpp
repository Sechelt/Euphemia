/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRPolyline.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRPolyline::DRPolyline( DRDraw *pCanvas, const QPointF &pointPos )
    : DRObject( pCanvas, pointPos )
{
    if ( pointPos.isNull() )
    {
        // creating object for load
    }
    else
    {
        // initializing a fresh one
        polygon.append( pointPos );    // begin point
        pointMouse = pointPos;         // proposed next point
        bInit = true;
    }
}

DRPolyline::~DRPolyline()
{
}

QPainterPath DRPolyline::shape() const
{
    // draw our line into a path (perhaps make the path reusable to speed up code in the future)
    QPainterPath path;
    path.addPolygon( polygon ); // unlike QPainter - does not automatically close polygon
    // use a stroker to increase the width a bit to make the line easier to click on
    QPainterPathStroker stroker;
    stroker.setWidth( pen.width() + 10 );
    // return the, now slightly fatter, line
    return stroker.createStroke( path );
}

QPixmap DRPolyline::getIcon()
{
    return QPixmap( ":DR/Polyline" );
}

QRectF DRPolyline::getRect() const 
{ 
    if ( bInit ) 
    {
        QPolygonF t = polygon;
        t.append( pointMouse );
        return t.boundingRect(); 
    }

    return polygon.boundingRect(); 
}

QImage DRPolyline::getImage()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QImage();
}

// NOTE: Assumes that the handle is going to be moved.
LHandle *DRPolyline::getGrab( const QPointF &pointPos, bool bSizing )
{
    if ( bSizing ) 
    {
        // this should not happen because we have a diff way to initialize
        // see; doPress/doMove/doDoubleClick
        Q_ASSERT( 0 );
    }

    LHandle *pHandle = DRObject::getGrab( pointPos ); 
    if ( !pHandle ) return nullptr;

    // is it a point factory?
    if ( pHandle->getType() != LHandle::TypePointFactory ) return pHandle;

    // create a new point
    int nFirstMovePointHandle = polygon.count(); // would be count() - 1 except we also have the DragHandle at index 0
    QPointF point           = getView()->mapToScene( pHandle->getCenter().toPoint() );
    int nHandleNewPoint     = vectorHandles.indexOf( pHandle );
    int nPointNext          = nHandleNewPoint; // almost corresponds to polygon but less 1 to account for DragHandle
    int nHandlePointNext    = nFirstMovePointHandle + nPointNext;
    int nHandleNewPointNext = nHandleNewPoint + 1;

    // insert point
    polygon.insert( nPointNext, getView()->mapToScene( pHandle->getCenter().toPoint() ) );

    // insert another new point handle
    pHandle = new LHandle( this, getView(), LHandle::TypePointFactory, point );
    vectorHandles.insert( nHandleNewPointNext, pHandle );
    pHandle->show();
    nHandlePointNext++; // we just shoved everything over by one

    // insert another point handle for the point we just inserted
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, point );
    vectorHandles.insert( nHandlePointNext, pHandle );
    pHandle->show();

    doSyncHandles();

    return pHandle;
}

/*!
 * \brief Move current handle.
 *  
 * Only used during manipulation mode. 
 *  
 * The polygon will be moved if move handle otherwise we will move handle/point and 
 * adjust move handle so its center. 
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pointPos 
 */
void DRPolyline::doMove( LHandle *pHandle, const QPointF &pointPos )
{
    Q_ASSERT( pHandle );

    getGraphics()->doPrepareGeometryChange();

    // their parent will be the viewport so...
    QPolygon    polygonView     = getView()->mapFromScene( polygon );

    int nFirstMovePointHandle = polygon.count(); // would be count() - 1 except we also have the DragHandle at index 0

    // move handle?
    if ( pHandle->getType() == LHandle::TypeDrag )
    {
        QPointF pointDelta = pointPos - getView()->mapToScene( pHandle->getCenter().toPoint() );
        // move points
        for ( int n = 0; n < polygon.count(); n++ )
        {
            polygon.replace( n, polygon.at( n ) + pointDelta );
        }
    }
    else if ( pHandle->getType() == LHandle::TypeMovePoint )
    {
        // just a single point handle
        polygon.replace( vectorHandles.indexOf( pHandle ) - nFirstMovePointHandle, pointPos );
    }
    else if ( pHandle->getType() == LHandle::TypePointFactory )
    {
        // Should not happen.
        // see; getGrab()
    }

    doSyncHandles();
    // update();
}

void DRPolyline::doDrop( LHandle *pHandle, const QPointF &pointPos )
{
    if ( !pointPos.isNull() ) doMove( pHandle, pointPos );
    if ( shouldRemovePoint( pHandle ) ) doRemovePoint( pHandle );
}

void DRPolyline::doPress( const QPointF &point )
{
    Q_ASSERT( bInit );
    polygon.append( point );    // add current point
    pointMouse = point;         // proposed next point
    getGraphics()->update();
}

void DRPolyline::doMoveBy( const QPointF &pointOffset )
{
}

void DRPolyline::doMove( const QPointF &point )
{
    Q_ASSERT( bInit );
    getGraphics()->doPrepareGeometryChange();
    pointMouse = point;         // proposed next point
    // update();
}

void DRPolyline::doDoubleClick( const QPointF &point /* could be null */ )
{
    Q_UNUSED( point );  // no need for this as doPress would have been called already
    Q_ASSERT( bInit );

    getGraphics()->setAcceptHoverEvents( false );
    bInit = false;
    if ( nSelectionState != SelectionStateNone && nSelectionState != SelectionStateMinimal ) doCreateHandles();
}

QDomElement DRPolyline::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRObject::doSave( pdomDoc, pdomElemParent );

    int n = 1;
    foreach( QPointF point, polygon )
    {
        domElemThis.setAttribute( QString( "x%1" ).arg( n ), point.x() );
        domElemThis.setAttribute( QString( "y%1" ).arg( n ), point.y() );
        n++;
    }

    return domElemThis;
}

bool DRPolyline::doLoad( QDomElement *pdomElemThis )
{
    DRObject::doLoad( pdomElemThis );

    for ( int n = 1; ; n++ )
    {
        QVariant vX = pdomElemThis->attribute( QString( "x%1" ).arg( n ) );
        QVariant vY = pdomElemThis->attribute( QString( "y%1" ).arg( n ) );

        // vX.isNull() stopped working for some reason
        // if ( vX.isNull() ) break;
        if ( vX.toString().isEmpty() ) break;

        polygon.append( QPointF( vX.toDouble(), vY.toDouble() ) );
    }

    return true;
}

void DRPolyline::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw shape (could be a fat line)
    pPainter->drawPolyline( polygon );      
    
    // draw proposed line segment    
    if ( bInit ) pPainter->drawLine( polygon.last(), pointMouse );

    // draw 'outline' as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawPolyline( polygon );      

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawPolyline( polygon );      
    }
}

/*!
 * \brief Create handles.
 *  
 * Order matters when handles share a position. Last handle will be found first. 
 *  
 * It would be easiest to append the move handle at the end as this would mean there would 
 * be a direct correspondence between point indexs and handle indexs - but this would make 
 * the move handle the default when all handles happened to be stacked upon each other. 
 * This would make resizing a very small polygon difficult - so we have the move handle 
 * first (at bottom) followed by NewPoint handles and then MovePoint handles.
 *  
 * \author pharvey (2/1/23)
 */
void DRPolyline::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );     // must start with no handles
    Q_ASSERT( polygon.count() );                // need at least 1 point for this call to make sense

    if ( bInit ) return; // defer until after we are initialized see; doDoubleClick()

    // their parent will be the viewport so...
    QPolygonF polygonView = getView()->mapFromScene( polygon );

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    // move handle is always vectorHandles[0]
    pHandle = new LHandle( this, getView(), LHandle::TypeDrag, polygonView.boundingRect().center() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // add a handle between each point (moving it will create a new point)
    for ( int n = 1; n < polygonView.count(); n++ )
    {
        pHandle = new LHandle( this, getView(), LHandle::TypePointFactory, QRectF( polygonView.at( n - 1 ), polygonView.at( n ) ).center() );
        vectorHandles.append( pHandle );
        pHandle->show();
    }

    // add a handle for each point
    for ( QPointF point : polygonView )
    {
        pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, point );
        vectorHandles.append( pHandle );
        pHandle->show();
    }
}

void DRPolyline::doSyncHandles()
{
    QPolygon polygonView = getView()->mapFromScene( polygon );
    int nHandle = 0;

    // move handle (always at index 0)
    vectorHandles[nHandle]->setCenter( polygonView.boundingRect().center() );

    // new point handles
    for ( int n = 1; n < polygonView.count(); n++ )
    {
        vectorHandles[++nHandle]->setCenter( QRectF( polygonView.at( n - 1 ), polygonView.at( n ) ).center() );
    }

    // point handles
    for ( int n = 0; n < polygonView.count(); n++ )
    {
        vectorHandles[++nHandle]->setCenter( polygonView[n] );
    }
}


// call shouldRemovePoint() before calling here
void DRPolyline::doRemovePoint( LHandle *pHandle )
{
    // remove point
    int nFirstMovePointHandle = polygon.count() + 1;
    int nHandle = vectorHandles.indexOf( pHandle );
    int nPoint = nHandle - nFirstMovePointHandle + 1;
    polygon.remove( nPoint );

    // remove point handle
    delete vectorHandles.takeAt( nHandle );

    // remove left or right NewPoint handle?
    if ( nHandle > nFirstMovePointHandle ) nHandle = nPoint;  // left
    else nHandle = nPoint + 1;  // right

    delete vectorHandles.takeAt( nHandle );

    doSyncHandles();
    getGraphics()->update();
}

bool DRPolyline::shouldRemovePoint( LHandle *pHandle )
{
    // we must be working with an existing polygon point
    if ( !pHandle ) return false;
    if ( pHandle->getType() != LHandle::TypeMovePoint ) return false;
    // not enough points to consider removing one?
    if ( polygon.count() <= 2 ) return false;
    // on another point?
    LHandle *p = getHandleUnder( pHandle, LHandle::TypeMovePoint );
    if ( !p ) return false;
    // must be an adjacent point to eliminate a line segment - adjacent?
    if ( p != getHandlePrev( pHandle, LHandle::TypeMovePoint ) && p != getHandleNext( pHandle, LHandle::TypeMovePoint ) ) return false;

    return true;
}

void DRPolyline::doDump()
{
    qInfo() << polygon;
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
        LHandle *p = vectorHandles.at( n );
        qInfo() << n << ":" << p->getType() << p->getCenter();
    }
}

