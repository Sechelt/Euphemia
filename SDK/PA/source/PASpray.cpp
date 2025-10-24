/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PASpray.h"

#include <SSettings.h>
#include <LGraphicsLayer.h>

#include "PAPaint.h"

PASpray::PASpray( PAPaint *pLayer )
    : PAToolFree( pLayer )
{
    pSettingSpray   = g_SSettings->getSetting( "PASSpray" ); Q_ASSERT( pSettingSpray );

    setAcceptHoverEvents( true );

    slotRefreshSpray();

    connect( pSettingSpray, SIGNAL(signalChanged()), SLOT(slotRefreshSpray()) );
}

void PASpray::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        doDrawState( pEvent->pos() );
        button = Qt::LeftButton;
        doSpray( pEvent->pos() );
        break;
    case StateDraw:
    case StateManipulate:
        break;
    }
    update();
}

void PASpray::doMove( PMouseEvent *pEvent )
{
    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        if ( button == Qt::LeftButton ) doSpray( pEvent->pos() );
        break;
    case StateManipulate:
        break;
    }
    update();
}

void PASpray::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        button = Qt::NoButton;
        doIdleState();
        break;
    case StateManipulate:
        break;
    }
    update();
}

void PASpray::doPaint( QPainter *pPainter, bool )
{
    pPainter->setPen( QPen( Qt::white ) );
    pPainter->drawEllipse( r );

    QPen pen( Qt::black );
    pen.setStyle( Qt::DashLine ); 
    pPainter->drawEllipse( r );
}

void PASpray::doSpray( const QPoint &point )
{
    int nX = point.x();     
    int nY = point.y();     
    int nR = nRadius;
    int nN = nPoints;
    QPoint pointSpray;

    // draw points
    QPainter painter( pLayer->getImageRef() );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    painter.setPen( pen );

    for ( int n = 0; n < nN; n++ )
    {
        // Get Angle in radians
        double nTheta = 2 * M_PI * getRandom();
        // Get length from center
        double nLen = qSqrt( getRandom() ) * nR;
 
        pointSpray.setX( nX + nLen  * qCos( nTheta ) );
        pointSpray.setY( nY + nLen  * qSin( nTheta ) );

        painter.drawPoint( pointSpray );
    }

    pLayerGraphics->update();
}

void PASpray::doDrawState( const QPoint &point )
{
    r.setWidth( nRadius * 2 + pen.width() );
    r.setHeight(  nRadius * 2 + pen.width() );
    r.moveCenter( point );
    nState = StateDraw;
    emit signalPreCommit();
}

void PASpray::doIdleState()
{
    nState = StateIdle;
    emit signalCommitted();
}

void PASpray::slotRefreshSpray()
{
    nRadius = pSettingSpray->property( "nRadius" ).toInt();
    nPoints = pSettingSpray->property( "nPoints" ).toInt();

    QPoint point = r.center();
    r.setWidth( nRadius * 2 + pen.width() );
    r.setHeight(  nRadius * 2 + pen.width() );
    r.moveCenter( point );
}

double PASpray::getRandom()
{
    // something between 0 and 1
    return (double)rand() / RAND_MAX;
}


