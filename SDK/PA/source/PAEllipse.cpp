/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAEllipse.h"

#include "PAPaint.h"

#include <SSettings.h>

PAEllipse::PAEllipse( PAPaint *pLayer )
    : PARectangle( pLayer )
{
    // we want to ignore rectangle settings - particularly bind - and use ellipse version
    pSettingEllipse   = g_SSettings->getSetting( "LSEllipse" ); Q_ASSERT( pSettingEllipse );

    slotRefreshEllipse();

    disconnect( pSettingRectangle, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
    connect( pSettingEllipse, SIGNAL(signalChanged()), SLOT(slotRefreshEllipse()) );
}

void PAEllipse::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    // render has already been set in pGraphics
    pPainter->setPen( pen );
    // paint
    pPainter->drawEllipse( r.normalized() );                         
}

void PAEllipse::slotRefreshEllipse()
{
    bBind = pSettingEllipse->property( "bBind" ).toInt();
}

