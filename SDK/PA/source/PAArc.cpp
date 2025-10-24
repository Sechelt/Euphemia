/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAArc.h"

#include <CBDMath.h>

#include "PAPaint.h"

#define PARectangleBegin 0
#define PARectangleMove 1
#define PARectangleEnd 2
#define PAArcStartAngle 3
#define PAArcSpanAngle 4

PAArc::PAArc( PAPaint *pLayer )
    : PARectangle( pLayer )
{
}

void PAArc::doMove( PMouseEvent *pEvent ) 
{
    Q_ASSERT( !acceptHoverEvents() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        if ( bBind )
        {
            QPoint pointDelta = pEvent->pos() - r.bottomRight();
            int nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() - QPoint( nDelta, nDelta ) );
            r.setBottomRight( r.bottomRight() + QPoint( nDelta, nDelta ) );
        }
        else
        {
            r.setBottomRight( pEvent->pos() );
        }
        // ensure these points are consistent with nStartAngle=0-E and nEndAngle=180=W
        // the handles will take over during manipulate
        pointStart.setX( r.topRight().x() );
        pointStart.setY( r.center().y() );
        pointStop.setX( r.x() );
        pointStop.setY( r.center().y() );
        update();
        break;
    case StateManipulate:
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PAArc::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    Q_ASSERT( !acceptHoverEvents() );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        doManipulateState();
        break;
    case StateManipulate:
        pHandle = nullptr;
        break;
    }
}

void PAArc::doPaint( QPainter *pPainter, bool bCommit )
{
    if ( nState == StateIdle ) return;

    // apply context
    // render has already been set in pGraphics
    pPainter->setPen( pen );
    // paint
    // - factors when calculating the span;
    //      - arc will be drawn from nStartAngle, counter-clockwise, span degrees
    //      - span degrees should be calculated from nStartAngle to nEndAngle in the counter-clockwise direction
    //      - span could cross 0 degrees
    //      - 0 degrees is East (3 o'clock) and increases counter-clockwise
    int nSpan;
    if ( nEndAngle > nStartAngle )
        nSpan = nEndAngle - nStartAngle;
    else
        nSpan = 360 - ( nStartAngle - nEndAngle );

    pPainter->drawArc( r, nStartAngle * 16, nSpan * 16 );                         

    if ( !bCommit )
    {
        QPen pen( Qt::white );
        pPainter->setPen( pen );
        pPainter->drawLine( r.center(), pointStart );
        pPainter->drawLine( r.center(), pointStop );

        pen.setColor( Qt::darkGray );
        pen.setStyle( Qt::DashLine );
        pPainter->setPen( pen );
        pPainter->drawLine( r.center(), pointStart );
        pPainter->drawLine( r.center(), pointStop );
    }
}

void PAArc::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );
    r = QRect( point, QSize( 1, 1 ) );
    pointStart = r.center();
    pointStop = r.center();
    // angles: we use positive values only - so span is always clockwise
    nStartAngle = 0;               // 0 (east)
    nEndAngle   = 180;             // 180 degrees (west)

    nState = StateDraw;
    update();
    emit signalChanged();
}

void PAArc::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    PARectangle::doCreateHandles();

    // angle handles must be after rect handles

    // PAArcStartAngle
    pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, pLayerView->mapFromScene( pointStart ) );
    pHandle->setToolTip( tr("start angle") );
    vectorHandles.append( pHandle );
    pHandle->show();

    // PAArcSpanAngle
    pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, pLayerView->mapFromScene( pointStop ) );
    pHandle->setToolTip( tr("stop angle") );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void PAArc::doSyncHandles()
{
    PARectangle::doSyncHandles();

    vectorHandles[PAArcStartAngle]->setCenter( pLayerView->mapFromScene( pointStart ) );
    vectorHandles[PAArcSpanAngle]->setCenter( pLayerView->mapFromScene( pointStop ) );
}

void PAArc::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    QPoint  pointView   = pLayerView->mapFromScene( pointPos );

    // adjusting angles?
    if ( pHandle == vectorHandles[PAArcStartAngle] )
    {
        pHandle->setCenter( pointView );
        pointStart = pointPos;
        // change origin to rect center and get polar
        QPointF pointFromCenter = CBDMath::getTopLeftToOrigin( pLayer->boundingRect(), r.center(), pointStart );
        CBDMath::Polar polar = CBDMath::getCartesianToPolar( pointFromCenter.x(), pointFromCenter.y() );
        // update our start angle
        nStartAngle = int(CBDMath::getRadiansToDegrees( polar.angle ));
        update();
        return;
    }

    if ( pHandle == vectorHandles[PAArcSpanAngle] )
    {
        pHandle->setCenter( pointView );
        pointStop = pointPos;
        QPointF pointFromCenter = CBDMath::getTopLeftToOrigin( pLayer->boundingRect(), r.center(), pointStop );

        // change origin to rect center and get polar
        CBDMath::Polar polar = CBDMath::getCartesianToPolar( pointFromCenter.x(), pointFromCenter.y() );
        // update our span
        nEndAngle = int(CBDMath::getRadiansToDegrees( polar.angle ));
        update();
        return;
    }

    //
    // adjust rect
    //

    // adjust our geometry
    if ( pHandle == vectorHandles[PARectangleBegin] )
    {
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    }
    else if ( pHandle == vectorHandles[PARectangleMove] )
    {
        QPoint pointDiff = pointPos - r.center();
        r.setTopLeft( r.topLeft() + pointDiff );
        r.setBottomRight( r.bottomRight() + pointDiff );

        pointStart += pointDiff;
        pointStop += pointDiff;
    }
    else if ( pHandle == vectorHandles[PARectangleEnd] )
    {
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    }

    //
    PARectangle::doMoveHandle( pointPos );
}


