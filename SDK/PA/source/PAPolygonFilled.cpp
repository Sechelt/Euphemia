/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAPolygonFilled.h"

#include <SSettings.h>

#include "PAPaint.h"

PAPolygonFilled::PAPolygonFilled( PAPaint *pLayer )
    : PAPolygon( pLayer )
{
    pSettingPolygonFilled = g_SSettings->getSetting( "PASPolygonFilled" ); Q_ASSERT( pSettingPolygonFilled );
    slotRefreshPolygonFilled();
    connect( pSettingPolygonFilled, SIGNAL(signalChanged()), SLOT(slotRefreshPolygonFilled()) );
}

void PAPolygonFilled::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // paint
    QPainterPath path;

    if ( nState == StateDraw )
    {
        QPolygon poly = polygon;
        poly.append( pointMouse );
        path.addPolygon( poly );
        pPainter->drawPolygon( poly, nFillRule );
    }
    else
    {
        path.addPolygon( polygon );
        pPainter->drawPolygon( polygon, nFillRule );
    }

    pPainter->fillPath( path, brush );
}

void PAPolygonFilled::slotRefreshPolygonFilled()
{
    nFillRule = (Qt::FillRule)pSettingPolygonFilled->property( "nFillRule" ).toInt();
}

