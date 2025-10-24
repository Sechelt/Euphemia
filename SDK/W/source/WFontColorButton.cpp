/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WFontColorButton.h"

#include "WFontColorDialog.h"

WFontColorButton::WFontColorButton( const QColor &color, const QBrush &brush, QWidget *pParent )
    : QToolButton( pParent )
{
    colorForeground = color;
    brushBackground = brush;

    setIcon( getIcon() );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void WFontColorButton::setForeground( const QColor &color )
{
    if ( colorForeground == color ) return;
    colorForeground = color;
    setIcon( getIcon() );
    emit signalChanged( colorForeground, brushBackground );
}

void WFontColorButton::setBackground( const QBrush &brush )
{
    if ( brushBackground == brush ) return;
    brushBackground = brush;
    setIcon( getIcon() );
    emit signalChanged( colorForeground, brushBackground );
}

QPixmap WFontColorButton::getIcon( int nSize )
{
    QFont font;
    font.setPixelSize( nSize );
    QPixmap pixmap( nSize, nSize );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 0, nSize, nSize ), brushBackground );
    p.setFont( font );
    p.setPen( colorForeground );
    p.drawText( 2, nSize - 2, "A" );
    return pixmap;
}

void WFontColorButton::slotClicked()
{
    bool bOk = false;

    CBD::CBDText font;
    font.color = colorForeground;
    font.brush = brushBackground;

    font = WFontColorDialog::getValue( &bOk, font, this );
    if ( bOk )
    {
        setForeground( font.color );
        setBackground( font.brush );
    }
}


