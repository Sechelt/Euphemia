/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PARectangleFilled.h"

#include "PAPaint.h"

PARectangleFilled::PARectangleFilled( PAPaint *pLayer )
    : PARectangle( pLayer )
{
}

void PARectangleFilled::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // paint
    if ( nXRadius || nYRadius )
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );
    else
        pPainter->drawRect( r );
}


