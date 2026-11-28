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
}

QPixmap DRText::getIcon()
{
    return QPixmap( ":DR/Text" );
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

    QString stringText;
    if ( bDisplayCode ) stringText = getCode();
    if ( bDisplayName ) stringText = ( stringText.isEmpty() ? getName() : "\n" + getName() );
    if ( bDisplayComment ) stringText = ( stringText.isEmpty() ? getComment() : "\n" + getComment() );

    // apply context
    pPainter->setPen( text.color );
    pPainter->setBrush( text.brush );
    pPainter->setFont( text.font );

    // fill background
    QRectF rectBackground = pPainter->boundingRect( r, text.nHAlign | text.nVAlign | Qt::TextWordWrap, stringText );
    QRectF rectIntersected = r.intersected( rectBackground );
    pPainter->fillRect( rectIntersected, text.brush );

    // draw shape
    pPainter->drawText( rectIntersected, text.nHAlign | text.nVAlign | Qt::TextWordWrap, stringText );

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
}

void DRText::doDisconnectSettings()
{
    // skip DRRectangleBase
    DRObject::doDisconnectSettings();
}

DRSText::DRSText()
{
    pText        = (SText*)g_SSettings->getSetting( "SText" ); Q_ASSERT( pText );
    pTextContent = (STextContent*)g_SSettings->getSetting( "STextContent" ); Q_ASSERT( pText );
}

QWidget *DRSText::getButton( QWidget *pParent )
{
    return pText->getButton( pParent );
}

QWidget *DRSText::getToolBar( QWidget *pParent )
{
    return new DRSTextToolBar( pParent );
}

QWidget *DRSText::getPanel( QWidget *pParent )
{
    return pTextContent->getPanel( pParent );
}

QWidget *DRSText::getPanelMax( QWidget *pParent )
{
    QToolBox *p = new QToolBox( pParent );
    p->addItem( pTextContent->getPanel( p ), QIcon(), "Content" );
    p->addItem( pText->getPanel( p ), QIcon(), "Format" );
    return p;
}

void DRSText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED( pdomDoc );
    Q_UNUSED( pdomElem );
}

void DRSText::doLoad( QDomElement *pdomElem )
{
    Q_UNUSED( pdomElem );
}

DRSTextToolBar::DRSTextToolBar( QWidget *pParent )
    : STextContentToolBar( pParent )
{
    setObjectName( "DRSTextToolBar" );
}

void DRSTextToolBar::slotMore()
{
    DRSText *pSetting = (DRSText*)g_SSettings->getSetting( "DRSText" ); Q_ASSERT( pSetting );
    pSetting->doDialog( this );
}


