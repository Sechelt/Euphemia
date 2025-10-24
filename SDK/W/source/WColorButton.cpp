/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WColorButton.h"
#include "W.h"

#include "WColorsPickerDialog.h"


WColorButton::WColorButton( const QColor &c, QWidget *pParent, Types n )
    : QToolButton( pParent )
{
    nType = n;
    color = c;

    setIcon( getIcon() );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void WColorButton::setValue( const QColor &color )
{
    if ( this->color == color ) return;
    this->color = color;
    setIcon( getIcon() );
    emit signalChanged( color );
}

QPixmap WColorButton::getIcon( int nSize )
{
//    Q_UNUSED(nSize);

    switch ( nType )
    {
        case Font:
            {
                // font color (foreground or background)
                QPixmap pixmap( ":W/Font16" );
                QPainter p( &pixmap );
                p.fillRect( QRect(0,12,16,5), color);
                return pixmap;
            }
            break;
        case Pen:
            {
                // pen color
                QPixmap pixmap( ":W/Pen16" );
                QPainter p( &pixmap );
                p.fillRect( QRect(0,13,16,5), color);
                return pixmap;
            }
            break;
        case Brush:
            {
                // brush color
                QPixmap pixmap( ":W/Brush" );
                QPainter p( &pixmap );
                p.fillRect( QRect(0,13,16,5), color );
                return pixmap;
            }
            break;
        case Fill:
            {
                // fill color
                QPixmap pixmap( ":W/Brush" );
                QPainter p( &pixmap );
                p.fillRect( QRect(0,13,16,5), color );
                return pixmap;
            }
            break;
        case Color:
        default:
            {
                QPixmap pixmap( nSize, nSize );
                pixmap.fill( color );
                return pixmap;
            }
            break;
    }

    return QPixmap();
}

void WColorButton::slotClicked()
{
    bool bOk = false;

    QColor colorNew = WColorsPickerDialog::getColor( &bOk, color, this, true );
    if ( bOk )
    {
        setValue( colorNew );
    }
}


