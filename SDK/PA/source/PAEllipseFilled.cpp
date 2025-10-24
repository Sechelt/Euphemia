/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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


