/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DREllipse.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DREllipse::DREllipse( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    // we want to ignore rectangle settings - particularly bind - and use ellipse version
    pSettingEllipse   = g_SSettings->getSetting( "LSEllipse" ); Q_ASSERT( pSettingEllipse );

    slotRefreshEllipse();
}

QPixmap DREllipse::getIcon()
{
    return QPixmap( ":DR/Ellipse" );
}

void DREllipse::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw shape
    pPainter->drawEllipse( r );                         

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawEllipse( r );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawEllipse( r );                         
    }
}

void DREllipse::doConnectSettings()
{
    // skip DRRectangleBase
    DRObject::doConnectSettings();
    connect( pSettingEllipse, SIGNAL(signalChanged()), this, SLOT(slotRefreshEllipse()) );
}

void DREllipse::doDisconnectSettings()
{
    // skip DRRectangleBase
    DRObject::doDisconnectSettings();
    disconnect( pSettingEllipse, SIGNAL(signalChanged()), this, SLOT(slotRefreshEllipse()) );
}

void DREllipse::slotRefreshEllipse()
{
    bBind = pSettingEllipse->property( "bBind" ).toInt();
}

