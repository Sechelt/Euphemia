/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRChord.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRChord::DRChord( DRDraw *pCanvas, const QPointF &pointPos )
    : DRArc( pCanvas, pointPos )
{
}

QPixmap DRChord::getIcon()
{
    return QPixmap( ":DR/Chord" );
}

void DRChord::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

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

    // draw shape
    pPainter->drawChord( r, nStartAngle * 16, nSpan * 16 );                         

    // draw manipulation indicators
    if ( nSelectionState == SelectionStateStandard )
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
    // draw outline as selected
    else if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawChord( r, nStartAngle * 16, nSpan * 16 );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawChord( r, nStartAngle * 16, nSpan * 16 );                         
    }
}


