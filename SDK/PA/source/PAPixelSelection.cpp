/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAPixelSelection.h"

#include "PAPaint.h"

#define PAPixelSelectionMove 0

PAPixelSelection::PAPixelSelection( PAPaint *pLayer )
    : PATool( pLayer )
{
}

PAPixelSelection::~PAPixelSelection()
{
    doCancel();
}

void PAPixelSelection::doDoubleClick( PMouseEvent *pEvent )
{ 
    Q_UNUSED( pEvent );
}

void PAPixelSelection::doPress( PMouseEvent *pEvent )
{
    Q_ASSERT( !acceptHoverEvents() );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        doDrawState( pEvent->pos() );
        doManipulateState();
        pHandle = getHandle( pEvent->pos() );
        break;
    case StateDraw:
        break;
    case StateManipulate:
        pHandle = getHandle( pEvent->pos() );
        break;
    }
}

void PAPixelSelection::doMove( PMouseEvent *pEvent ) 
{
    Q_ASSERT( !acceptHoverEvents() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PAPixelSelection::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        pHandle = nullptr;
        break;
    }
}

void PAPixelSelection::doCommit()
{
}

void PAPixelSelection::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    //
    pPainter->setPen( Qt::white );
    pPainter->drawLine( r.topLeft(), r.topRight() );
    pPainter->drawLine( r.topLeft(), r.bottomLeft() );

    //
    QPen pen( Qt::DashLine );
    pen.setColor( Qt::black );
    pPainter->setPen( pen );
    pPainter->drawLine( r.topLeft(), r.topRight() );
    pPainter->drawLine( r.topLeft(), r.bottomLeft() );
}

void PAPixelSelection::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );
    r = QRect( point, QSize( 50, 50 ) );
    nState = StateDraw;
    update();
    emit signalChanged();
}

void PAPixelSelection::doManipulateState()
{
    Q_ASSERT( nState == StateDraw );
    doCreateHandles();
    nState = StateManipulate;
    emit signalChanged();
}

void PAPixelSelection::doIdleState()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    if ( nState == StateDraw )
    {
        nState = StateIdle;
    }
    else if ( nState == StateManipulate )
    {
        doDeleteHandles();
        nState = StateIdle;
    }
    update();
    emit signalMoved( QPoint() );         // to get pixel view to repaint itself (to blank)
    emit signalChanged();
}

void PAPixelSelection::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // their parent will be the viewport so...
    QRect rectView = pLayerView->mapFromScene( r ).boundingRect();

    // a single handle - just for moving rect around.
    // we use topleft instead of center so the view is less obscured.
    LHandle *pHandle = new LHandle( this, pLayerView, LHandle::TypeDrag, rectView.topLeft() );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void PAPixelSelection::doSyncHandles()
{
    QRect rectView = pLayerView->mapFromScene( r.normalized() ).boundingRect();

    vectorHandles[PAPixelSelectionMove]->setCenter( rectView.topLeft() );
}

void PAPixelSelection::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    // adjust our geometry
    if ( pHandle == vectorHandles[PAPixelSelectionMove] )
    {
        r.moveTopLeft( pointPos );
        pHandle->setCenter( pLayerView->mapFromScene( pointPos ) );
        emit signalMoved( pointPos );
    }
    update();
}




