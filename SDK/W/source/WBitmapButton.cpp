/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WBitmapButton.h"

WBitmapButton::WBitmapButton( const QPixmap &pm, QWidget *pParent )
    : QWidget( pParent )
{
    pixmap = pm;
}

WBitmapButton::~WBitmapButton()
{
}

void WBitmapButton::setPixmap( const QPixmap &pm  )
{
    pixmap = pm;
    update();
}

void WBitmapButton::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );

    if ( pixmap.isNull() ) painter.drawText( QPoint( width() / 2, height() / 2), tr("Double Click\nTo Select Image") );
    else painter.drawPixmap( QPoint( 0, 0 ), pixmap.scaled( width(), height() ) );
}

void WBitmapButton::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QFileDialog fileDialog( nullptr );
    fileDialog.setAcceptMode( QFileDialog::AcceptOpen );
    fileDialog.setNameFilter("Bitmap files (*.png *.xpm *.jpg)");
    fileDialog.setWindowTitle(tr("Select Bitmap..."));

    if ( fileDialog.exec() != QDialog::Accepted ) return;

    QString stringFileName = fileDialog.selectedFiles().constFirst();
    QPixmap pixmap;
    bool b = pixmap.load( stringFileName );
    if ( !b ) return;

    setPixmap( pixmap );

    emit signalChanged( pixmap );
}

