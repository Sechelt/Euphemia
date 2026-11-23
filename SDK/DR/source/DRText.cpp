/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRText.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRText::DRText( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    // NOTE: This adds to DObject ie CBD::CBDText etc
    pSettingText = (LSText*)g_SSettings->getSetting( "LSText" ); Q_ASSERT(pSettingText);
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

    return domElemThis;
}

bool DRText::doLoad( QDomElement *pdomElemThis )
{
    DRRectangleBase::doLoad( pdomElemThis );

    stringText  = pdomElemThis->attribute( "Text" ); 

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
    pPainter->drawText( r, font.nHAlign | font.nVAlign, stringText );

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
    pSettingText->setSelected( stringText );
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
    if ( getGraphics() ) getGraphics()->update();
}


