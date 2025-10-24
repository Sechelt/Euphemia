/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PARectangle.h"

#include <SSettings.h>

#include "PAPaint.h"

#define PARectangleBegin 0
#define PARectangleMove 1
#define PARectangleEnd 2

PARectangle::PARectangle( PAPaint *pLayer )
    : PATool( pLayer )
{
    pSettingRectangle   = g_SSettings->getSetting( "LSRectangle" );      Q_ASSERT( pSettingRectangle );

    slotRefreshRectangle();

    connect( pSettingRectangle, SIGNAL(signalChanged()), SLOT(slotRefreshRectangle()) );
}

PARectangle::~PARectangle()
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
QImage PARectangle::getCopy()
{
    QImage image( pLayer->getImageRef()->size(), QImage::Format_ARGB32 );
    image.fill( Qt::transparent );
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    return image.copy( r );
}

void PARectangle::doDoubleClick( PMouseEvent *pEvent )
{ 
    Q_UNUSED( pEvent );
}

void PARectangle::doPress( PMouseEvent *pEvent )
{
    Q_ASSERT( !acceptHoverEvents() );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        doDrawState( pEvent->pos() );
        break;
    case StateDraw:
        break;
    case StateManipulate:
        pHandle = getHandle( pEvent->pos() );
        if ( !pHandle ) doCommit();
        break;
    }
}

void PARectangle::doMove( PMouseEvent *pEvent ) 
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
        update();
        break;
    case StateManipulate:
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PARectangle::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    Q_ASSERT( !acceptHoverEvents() );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        if ( bAutoCommit ) return doCommit();
        doManipulateState();
        break;
    case StateManipulate:
        pHandle = nullptr;
        break;
    }
}

void PARectangle::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    emit signalPreCommit();

    QPainter painter( pLayer->getImageRef() );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    emit signalCommitted();

    doIdleState();
}

void PARectangle::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );

    // paint
    if ( nXRadius || nYRadius )
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );
    else
        pPainter->drawRect( r );
}

void PARectangle::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );
    r = QRect( point, QSize( 1, 1 ) );
    nState = StateDraw;
    update();
//    emit signalChanged();
}

void PARectangle::doManipulateState()
{
    Q_ASSERT( nState == StateDraw );
    doCreateHandles();
    nState = StateManipulate;
    emit signalChanged();
}

void PARectangle::doIdleState()
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
    emit signalChanged();
}

void PARectangle::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    r = r.normalized();

    // their parent will be the viewport so...
    QRect rectView = pLayerView->mapFromScene( r ).boundingRect();

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    // PARectangleBegin
    pHandle = new LHandle( this, pLayerView, LHandle::TypeSizeTopLeft, rectView.topLeft() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // PARectangleMove
    pHandle = new LHandle( this, pLayerView, LHandle::TypeDrag, rectView.center() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // PARectangleEnd
    pHandle = new LHandle( this, pLayerView, LHandle::TypeSizeBottomRight, rectView.bottomRight() );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void PARectangle::doSyncHandles()
{
    QRect rectView = pLayerView->mapFromScene( r.normalized() ).boundingRect();

    vectorHandles[PARectangleBegin]->setCenter( rectView.topLeft() );
    vectorHandles[PARectangleMove]->setCenter( rectView.center() );
    vectorHandles[PARectangleEnd]->setCenter( rectView.bottomRight() );
}

void PARectangle::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    // adjust our geometry
    if ( bBind )
    {
        if ( pHandle == vectorHandles[PARectangleBegin] )
        {
            QPoint pointDelta = pointPos - r.topLeft();
            int nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() + QPoint( nDelta, nDelta ) );
            r.setBottomRight( r.bottomRight() - QPoint( nDelta, nDelta ) );
        }
        else if ( pHandle == vectorHandles[PARectangleMove] )
        {
            QPoint pointDiff = pointPos - r.center();
            r.setTopLeft( r.topLeft() + pointDiff );
            r.setBottomRight( r.bottomRight() + pointDiff );
        }
        else if ( pHandle == vectorHandles[PARectangleEnd] )
        {
            QPoint pointDelta = pointPos - r.bottomRight();
            int nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() - QPoint( nDelta, nDelta ) );
            r.setBottomRight( r.bottomRight() + QPoint( nDelta, nDelta ) );
        }
    }
    else
    {
        if ( pHandle == vectorHandles[PARectangleBegin] )
        {
            r.setTopLeft( pointPos );
        }
        else if ( pHandle == vectorHandles[PARectangleMove] )
        {
            QPoint pointDiff = pointPos - r.center();
            r.setTopLeft( r.topLeft() + pointDiff );
            r.setBottomRight( r.bottomRight() + pointDiff );
        }
        else if ( pHandle == vectorHandles[PARectangleEnd] )
        {
            r.setBottomRight( pointPos );
        }
    }

    r = r.normalized();

    // adjust handles

    // their parent is the viewport so...
    QPoint  pointView   = pLayerView->mapFromScene( pointPos );

    if ( pHandle == vectorHandles[PARectangleBegin] )
    {
        pHandle->setCenter( pointView );
        doSyncHandleTypes();
        doSyncHandles();
    }
    else if ( pHandle == vectorHandles[PARectangleMove] )
    {
        doSyncHandles();
    }
    else if ( pHandle == vectorHandles[PARectangleEnd] )
    {
        pHandle->setCenter( pointView );
        doSyncHandleTypes();
        doSyncHandles();
    }
    update();
}

/*!
 * \brief Alter handle types if rectangle flipped (horiz/vert or both).
 * 
 * \author pharvey (2/28/23)
 */
void PARectangle::doSyncHandleTypes()
{
    QRect rectView = pLayerView->mapFromScene( r.normalized() ).boundingRect();

    if ( vectorHandles[PARectangleBegin]->geometry().contains( rectView.topLeft() ) ) vectorHandles[PARectangleBegin]->setType( LHandle::TypeSizeTopLeft );             
    else if ( vectorHandles[PARectangleBegin]->geometry().contains( rectView.topRight() ) ) vectorHandles[PARectangleBegin]->setType( LHandle::TypeSizeTopRight );      
    else if ( vectorHandles[PARectangleBegin]->geometry().contains( rectView.bottomLeft() ) ) vectorHandles[PARectangleBegin]->setType( LHandle::TypeSizeBottomLeft );  
    else if ( vectorHandles[PARectangleBegin]->geometry().contains( rectView.bottomRight() ) ) vectorHandles[PARectangleBegin]->setType( LHandle::TypeSizeBottomRight );
                                                                                                                                                                       
    if ( vectorHandles[PARectangleEnd]->geometry().contains( rectView.topLeft() ) ) vectorHandles[PARectangleEnd]->setType( LHandle::TypeSizeTopLeft );                 
    else if ( vectorHandles[PARectangleEnd]->geometry().contains( rectView.topRight() ) ) vectorHandles[PARectangleEnd]->setType( LHandle::TypeSizeTopRight );          
    else if ( vectorHandles[PARectangleEnd]->geometry().contains( rectView.bottomLeft() ) ) vectorHandles[PARectangleEnd]->setType( LHandle::TypeSizeBottomLeft );      
    else if ( vectorHandles[PARectangleEnd]->geometry().contains( rectView.bottomRight() ) ) vectorHandles[PARectangleEnd]->setType( LHandle::TypeSizeBottomRight );    
}

void PARectangle::slotRefreshRectangle()
{
    bBind       = pSettingRectangle->property( "bBind" ).toInt();
    nSizeMode   = (Qt::SizeMode)pSettingRectangle->property( "nSizeMode" ).toInt();
    nXRadius    = pSettingRectangle->property( "nXRadius" ).toReal();
    nYRadius    = pSettingRectangle->property( "nYRadius" ).toReal();
    update();
}

