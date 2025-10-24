/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAChord.h"

#include "PAPaint.h"

PAChord::PAChord( PAPaint *pLayer )
    : PAArc( pLayer )
{
}

void PAChord::doPaint( QPainter *pPainter, bool bCommit )
{
    if ( nState == StateIdle ) return;

    // apply context
    // render has already been set in pGraphics
    pPainter->setPen( pen );
    pPainter->setBrush( brush ); // should this be a ChordFilled ???

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

    pPainter->drawChord( r, nStartAngle * 16, nSpan * 16 );                         

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


