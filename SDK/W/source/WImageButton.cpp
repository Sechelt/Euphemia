/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WImageButton.h"

WImageButton::WImageButton( QWidget *pParent, const QImage &image )
    : QWidget( pParent )
{
    this->image = image;
    setMinimumWidth( 16 );
    setMinimumHeight( 16 );
}

void WImageButton::setImage( const QImage &image  )
{
    this->image = image;
    update();
}

void WImageButton::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setPen( Qt::black );
    if ( !image.isNull() )
    { 
        QImage imageScaled = image.scaled( width(), height(), Qt::KeepAspectRatio );
        QRect rectImage( 0, 0, imageScaled.width(), imageScaled.height() );
        rectImage.moveCenter( rect().center() );

        painter.drawImage( rectImage, imageScaled );
    }
    painter.drawRect( rect().x(), rect().y(), rect().width() -1, rect().height() -1 );
}

void WImageButton::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    Q_UNUSED(pEvent);
    emit signalClick();
}

