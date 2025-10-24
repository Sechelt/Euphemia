/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAPolyline.h"

#include "PAPaint.h"

PAPolyline::PAPolyline( PAPaint *pLayer )
    : PATool( pLayer )
{
}

PAPolyline::~PAPolyline()
{
    doCancel();
}

/*!
 * \brief Return a copy of the CURRENT SHAPE.
 *  
 * This is done by getting doPaint to draw on an empty, temp, canvas and then 
 * copying from that canvas. 
 *  
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage PAPolyline::getCopy()
{
    QImage image( pLayer->getImageRef()->size(), QImage::Format_ARGB32 );
    image.fill( Qt::transparent );
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    return image.copy( polygon.boundingRect() );
}

/*!
 * \brief Switch from drawing mode to manipulating mode.
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pEvent 
 * 
 * \return QRect 
 */
void PAPolyline::doDoubleClick( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( nState != StateDraw ) return;

    doManipulateState();

    if ( bAutoCommit ) doCommit();
}

/*!
 * \brief Create a new point (drawing mode) or start to move a point (manipulating mode).
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pEvent 
 * 
 * \return bool 
 */
void PAPolyline::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        doDrawState( pEvent->pos() );
        polygon.boundingRect();
        break;
    case StateDraw:
        polygon.append( pEvent->pos() );
        polygon.boundingRect();
        pointMouse = pEvent->pos();
        update();
        break;
    case StateManipulate:
        Q_ASSERT( !acceptHoverEvents() );
        pHandle = getHandle( pEvent->pos() );
        if ( !pHandle ) doCommit();
        break;
    }
}

/*!
 * \brief Move the last point (drawing mode) or the handle/point (manipulating mode).
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pEvent 
 * 
 * \return bool 
 */
void PAPolyline::doMove( PMouseEvent *pEvent ) 
{
    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        pointMouse = pEvent->pos();
        update();
        break;
    case StateManipulate:
        Q_ASSERT( !acceptHoverEvents() );
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PAPolyline::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    // if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        Q_ASSERT( !acceptHoverEvents() );
        if ( pHandle )
        {
            if ( shouldRemovePoint() ) doRemovePoint();
            pHandle = nullptr;
        }
        break;
    }
}

/*!
 * \brief Commit the polygon to canvas. 
 *  
 * 
 * \author pharvey (2/1/23)
 */
void PAPolyline::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    emit signalPreCommit();

    QPainter painter( pLayer->getImageRef());
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    emit signalCommitted();

    doIdleState();

    Q_ASSERT( !acceptHoverEvents() );
}

void PAPolyline::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );

    // paint
    pPainter->drawPolyline( polygon );      
    
    if ( nState == StateDraw ) pPainter->drawLine( polygon.last(), pointMouse );
}

void PAPolyline::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );

    polygon.append( point );    // begin
    pointMouse = point;

    nState = StateDraw;
    update();
    emit signalChanged();

    setAcceptHoverEvents( true ); // mouse tracking - mouse move event even when no button down
}

void PAPolyline::doManipulateState()
{
    Q_ASSERT( nState == StateDraw );
    doCreateHandles();
    nState = StateManipulate;
    emit signalChanged();

    setAcceptHoverEvents( false );
}

void PAPolyline::doIdleState()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    if ( nState == StateDraw )
    {
        nState = StateIdle;
        setAcceptHoverEvents( false );
    }
    else if ( nState == StateManipulate )
    {
        doDeleteHandles();
        polygon.clear();
        nState = StateIdle;
    }

    Q_ASSERT( !acceptHoverEvents() );

    update();
    emit signalChanged();
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
void PAPolyline::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // their parent will be the viewport so...
    QPolygon polygonView = pLayerView->mapFromScene( polygon );

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    // move handle is always vectorHandles[0]
    pHandle = new LHandle( this, pLayerView, LHandle::TypeDrag, polygonView.boundingRect().center() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // add a handle between each point (moving it will create a new point)
    for ( int n = 1; n < polygonView.count(); n++ )
    {
        pHandle = new LHandle( this, pLayerView, LHandle::TypePointFactory, QRect( polygonView.at( n - 1 ), polygonView.at( n ) ).center() );
        vectorHandles.append( pHandle );
        pHandle->show();
    }

    // add a handle for each point
    for ( QPoint point : polygonView )
    {
        pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, point );
        vectorHandles.append( pHandle );
        pHandle->show();
    }

}

void PAPolyline::doSyncHandles()
{
    QPolygon polygonView = pLayerView->mapFromScene( polygon );
    int nHandle = 0;

    // move handle
    vectorHandles[nHandle]->setCenter( polygonView.boundingRect().center() );

    // new point handles
    for ( int n = 1; n < polygonView.count(); n++ )
    {
        vectorHandles[++nHandle]->setCenter( QRect( polygonView.at( n - 1 ), polygonView.at( n ) ).center() );
    }

    // point handles
    for ( int n = 0; n < polygonView.count(); n++ )
    {
        vectorHandles[++nHandle]->setCenter( polygonView[n] );
    }
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
void PAPolyline::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    // their parent will be the viewport so...
    QPolygon    polygonView     = pLayerView->mapFromScene( polygon );

    int nFirstMovePointHandle = polygon.count(); // would be count() - 1 except we also have the DragHandle at index 0

    // move handle?
    if ( pHandle->getType() == LHandle::TypeDrag )
    {
        QPoint pointDelta = pointPos - pLayerView->mapToScene( pHandle->getCenter().toPoint() ).toPoint();
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
        QPoint point            = pLayerView->mapToScene( pHandle->getCenter().toPoint() ).toPoint();
        int nHandleNewPoint     = vectorHandles.indexOf( pHandle );
        int nPointNext          = nHandleNewPoint; // almost corresponds to polygon but less 1 to account for DragHandle
        int nHandlePointNext    = nFirstMovePointHandle + nPointNext;
        int nHandleNewPointNext = nHandleNewPoint + 1;

        // insert point
        polygon.insert( nPointNext, pLayerView->mapToScene( pHandle->getCenter().toPoint() ).toPoint() );

        // insert another new point handle
        pHandle = new LHandle( this, pLayerView, LHandle::TypePointFactory, point );
        vectorHandles.insert( nHandleNewPointNext, pHandle );
        pHandle->show();
        nHandlePointNext++; // we just shoved everything over by one

        // insert another point handle for the point we just inserted
        pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, point );
        vectorHandles.insert( nHandlePointNext, pHandle );
        pHandle->show();
    }

    doSyncHandles();
    update();
}

// call shouldRemovePoint() before calling here
void PAPolyline::doRemovePoint()
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
    update();
}

bool PAPolyline::shouldRemovePoint()
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

void PAPolyline::doDump()
{
    qInfo() << polygon;
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
        LHandle *p = vectorHandles.at( n );
        qInfo() << n << ":" << p->getType() << p->getCenter();
    }
}

