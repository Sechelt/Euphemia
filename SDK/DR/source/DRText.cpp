/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRText.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRText::DRText( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    pSettingText = g_SSettings->getSetting( "LSText" ); Q_ASSERT(pSettingText);
    slotRefreshText();
}

QPixmap DRText::getIcon()
{
    return QPixmap( ":DR/Text" );
}

QDomElement DRText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "Text", stringText );
    domElemThis.setAttribute( "HAlign", nHAlign );
    domElemThis.setAttribute( "VAlign", nVAlign );

    return domElemThis;
}

bool DRText::doLoad( QDomElement *pdomElemThis )
{
    DRRectangleBase::doLoad( pdomElemThis );

    stringText  = pdomElemThis->attribute( "Text" ); 
    nHAlign     = (Qt::AlignmentFlag)pdomElemThis->attribute( "HAlign", QString::number( int(Qt::AlignHCenter) ) ).toInt(); 
    nVAlign     = (Qt::AlignmentFlag)pdomElemThis->attribute( "VAlign", QString::number( int(Qt::AlignVCenter) ) ).toInt(); 

    return true;
}

void DRText::doDraw( QPainter *pPainter )
{
    // draw rect for manipulation
    if ( nSelectionState == SelectionStateStandard )
    {
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( getRect() );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( getRect() );                         
    }

    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    // draw shape
    pPainter->drawText( r, nHAlign | nVAlign, stringText );

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( getRect() );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( getRect() );                         
    }
}

void DRText::doConnectSettings()
{
    // skip DRRectangleBase
    DRObject::doConnectSettings();
    connect( pSettingText, SIGNAL(signalChanged()), this, SLOT(slotRefreshText()) );
}

void DRText::doDisconnectSettings()
{
    // skip DRRectangleBase
    DRObject::doDisconnectSettings();
    disconnect( pSettingText, SIGNAL(signalChanged()), this, SLOT(slotRefreshText()) );
}

void DRText::slotRefreshText()
{
    stringText  = pSettingText->property( "stringText" ).toString();
    nHAlign     = (Qt::AlignmentFlag)pSettingText->property( "nHAlign" ).toInt();
    nVAlign     = (Qt::AlignmentFlag)pSettingText->property( "nVAlign" ).toInt();
    if ( getGraphics() ) getGraphics()->update();
}

