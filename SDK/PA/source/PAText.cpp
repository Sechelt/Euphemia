/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAText.h"

#include <SSettings.h>

#include "PAPaint.h"

PAText::PAText( PAPaint *pLayer )
    : PARectangle( pLayer )
{
    pSettingText = g_SSettings->getSetting( "LSText" ); Q_ASSERT(pSettingText);
    slotRefreshText();
    connect( pSettingText, SIGNAL(signalChanged()), SLOT(slotRefreshText()) );
}

// this removes the auto commit
void PAText::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        doManipulateState();
        break;
    case StateManipulate:
        pHandle = nullptr;
        break;
    }
}

// paint - but without the rect
void PAText::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    emit signalPreCommit();

    QPainter painter( pLayer->getImageRef());
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    

    // apply context
    painter.setPen( pen );
    painter.setFont( font );

    // paint
    painter.drawText( r, nHAlign | nVAlign, stringText );

    emit signalCommitted();

    //
    doIdleState();
}

void PAText::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // temp outline of rect area
    {
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( r.normalized() );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( r.normalized() );                         
    }

    // apply context
    pPainter->setPen( pen );
    pPainter->setFont( font );

    // paint
    pPainter->drawText( r, nHAlign | nVAlign, stringText );
}

void PAText::slotRefreshText()
{
    stringText  = pSettingText->property( "stringText" ).toString();
    nHAlign     = (Qt::AlignmentFlag)pSettingText->property( "nHAlign" ).toInt();
    nVAlign     = (Qt::AlignmentFlag)pSettingText->property( "nVAlign" ).toInt();
}

