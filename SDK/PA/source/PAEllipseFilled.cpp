/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAEllipseFilled.h"

#include "PAPaint.h"

PAEllipseFilled::PAEllipseFilled( PAPaint *pLayer )
    : PAEllipse( pLayer )
{
}

void PAEllipseFilled::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    // render has already been set in pGraphics
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // paint
    pPainter->drawEllipse( r.normalized() );                         
}


