/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PALine.h"

#include <SSettings.h>

#include "PAPaint.h"

#define PALineBegin 0
#define PALineMove 1
#define PALineEnd 2

PALine::PALine( PAPaint *pLayer )
    : PATool( pLayer )
{
    pSettingLine = g_SSettings->getSetting( "LSLine" ); Q_ASSERT( pSettingLine );
    slotRefreshLine();
    connect( pSettingLine, SIGNAL(signalChanged()), SLOT(slotRefreshLine()) );
}

PALine::~PALine()
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
QImage PALine::getCopy()
{
    QRect r;
    r.setTopLeft( pointBegin );
    r.setBottomRight( pointEnd );

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

void PALine::doDoubleClick( PMouseEvent *pEvent )
{ 
    Q_UNUSED( pEvent );
}

void PALine::doPress( PMouseEvent *pEvent )
{
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


void PALine::doMove( PMouseEvent *pEvent ) 
{
    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        {
            pointEnd = pEvent->pos();
            update();
        }
        break;
    case StateManipulate:
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PALine::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

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

void PALine::doCommit()
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
}

void PALine::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font );
    // paint
    pPainter->drawLine( pointBegin, pointEnd );                         

    // draw line ends (if any)
    doDrawLineEnds( pPainter );
}

void PALine::doDrawLineEnds( QPainter *pPainter )
{
    doDrawLineEnd( pPainter, nStyleBegin, QLineF( pointBegin, pointEnd ) );
    // provide reversed line because doDrawLineEnd always draws at p1
    doDrawLineEnd( pPainter, nStyleEnd, QLineF( pointEnd, pointBegin ) );
}

void PALine::doDrawLineEnd( QPainter *pPainter, LSLine::LineEndStyles nStyle, const QLineF &line )
{
    switch ( nStyle )
    {
        case LSLine::LineEndArrow:
            doDrawLineEndArrow( pPainter, line );
            break;
        case LSLine::LineEndBall:
            doDrawLineEndBall( pPainter, line );
            break;
        case LSLine::LineEndNone:
            break;
    };
}

void PALine::doDrawLineEndArrow( QPainter *pPainter, const QLineF &line )
{
    // draw at p1
    qreal arrowSize = 6;
    const qreal Pi = 3.14;
    double angle = ::acos( line.dx() / line.length() );

    if ( line.dy() >= 0 )
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line.p1() + QPointF( sin( angle + Pi / 3 ) * arrowSize,
                                           cos( angle + Pi / 3 ) * arrowSize );
    QPointF arrowP2 = line.p1() + QPointF( sin( angle + Pi - Pi / 3 ) * arrowSize,
                                           cos( angle + Pi - Pi / 3 ) * arrowSize );

    QPolygonF polygonArrow;
    polygonArrow << line.p1() << arrowP1 << arrowP2;
    pPainter->drawPolygon( polygonArrow );
}

void PALine::doDrawLineEndBall( QPainter *pPainter, const QLineF &line )
{
    // draw at p1
    pPainter->drawEllipse( line.p1(), 4, 4 );
}

void PALine::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );
    pointBegin = pointEnd = point;
    nState = StateDraw;
    update();
    emit signalChanged();
}

void PALine::doManipulateState()
{
    Q_ASSERT( nState == StateDraw );
    doCreateHandles();
    nState = StateManipulate;
    emit signalChanged();
}

void PALine::doIdleState()
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

void PALine::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // their parent will be the viewport so...
    QPoint pointViewBegin   = pLayerView->mapFromScene( pointBegin );
    QPoint pointViewEnd     = pLayerView->mapFromScene( pointEnd );

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    // PALineBegin
    pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, pointViewBegin );
    vectorHandles.append( pHandle );
    pHandle->show();

    // PALineMove
    QRect r;
    r.setTopLeft( pointViewBegin );
    r.setBottomRight( pointViewEnd );
    r = r.normalized();

    pHandle = new LHandle( this, pLayerView, LHandle::TypeDrag, r.center() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // PALineEnd
    pHandle = new LHandle( this, pLayerView, LHandle::TypeMovePoint, pointViewEnd );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void PALine::doSyncHandles()
{
    QRect rectView = pLayerView->mapFromScene( QRect( pointBegin, pointEnd ).normalized() ).boundingRect();

    vectorHandles[PALineBegin]->setCenter( rectView.topLeft() );
    vectorHandles[PALineMove]->setCenter( rectView.center() );
    vectorHandles[PALineEnd]->setCenter( rectView.bottomRight() );
}

void PALine::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    // adjust our geometry
    if ( pHandle == vectorHandles[PALineBegin] )
    {
        // move the begin 
        pointBegin = pointPos;
    }
    else if ( pHandle == vectorHandles[PALineMove] )
    {
        // get diff
        QRect r( pointBegin, pointEnd );
        r = r.normalized();
        QPoint pointDiff = pointPos - r.center();
        // update points
        pointBegin += pointDiff;
        pointEnd += pointDiff;
    }
    else if ( pHandle == vectorHandles[PALineEnd] )
    {
        // move the end 
        pointEnd = pointPos;
    }

    // adjust handles
    QPoint  pointViewBegin  = pLayerView->mapFromScene( pointBegin );
    QPoint  pointViewPos    = pLayerView->mapFromScene( pointPos );
    QPoint  pointViewEnd    = pLayerView->mapFromScene( pointEnd );

    if ( pHandle == vectorHandles[PALineBegin] )
    {
        pHandle->setCenter( pointViewBegin );
        // get center
        QRect r;
        r.setTopLeft( pointViewBegin );
        r.setBottomRight( pointViewEnd );
        // adjust move handle
        vectorHandles[PALineMove]->setCenter( r.center() );
    }
    else if ( pHandle == vectorHandles[PALineMove] )
    {
        vectorHandles[PALineBegin]->setCenter( pointViewBegin );
        vectorHandles[PALineMove]->setCenter( pointViewPos );
        vectorHandles[PALineEnd]->setCenter( pointViewEnd );
    }
    else if ( pHandle == vectorHandles[PALineEnd] )
    {
        pHandle->setCenter( pointViewEnd );
        // get center
        QRect r;
        r.setTopLeft( pointViewBegin );
        r.setBottomRight( pointViewEnd );
        // adjust move handle
        vectorHandles[PALineMove]->setCenter( r.center() );
    }
    update();
}

void PALine::slotRefreshLine()
{
    nStyleBegin = LSLine::LineEndStyles(pSettingLine->property( "nBegin" ).toInt());
    nStyleEnd = LSLine::LineEndStyles(pSettingLine->property( "nEnd" ).toInt());
    update();
}

