/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WBitmapWidget.h"

#include "WBitmapButton.h"

WBitmapWidget::WBitmapWidget( bool bUse, const QPixmap &pixmap, W::WImagePlacements nPlacement, QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayout = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout01 = new QHBoxLayout();
        pUseBitMap = new QCheckBox( tr("Use Bitmap"), this );
        pLayout01->addWidget( pUseBitMap );
        pPlacement = new QComboBox( this );
        pPlacement->addItem( "Normal", (int)W::WImagePlacementNormal );
        pPlacement->addItem( "Tiled", (int)W::WImagePlacementTiled );
        pPlacement->addItem( "Scaled", (int)W::WImagePlacementScaled );
        WSetComboBox( pPlacement, nPlacement );
        pLayout01->addWidget( pPlacement );
        pLayout01->addStretch( 10 );
        pLayout->addLayout( pLayout01 );
    }
    {
        pBitmapButton = new WBitmapButton( pixmap, this );
        pLayout->addWidget( pBitmapButton );
    }

    if ( bUse )
    {
        pUseBitMap->setChecked( true );
    }
    else
    {
        pUseBitMap->setChecked( false );
        pPlacement->setEnabled( false );
        pBitmapButton->setEnabled( false );
    }

    connect( pUseBitMap, SIGNAL(clicked(bool)), SIGNAL(signalUse(bool)) );
    connect( pPlacement, SIGNAL(currentIndexChanged(int)), SLOT(slotChangedPlacement(int)) );
    connect( pBitmapButton, SIGNAL(signalChanged(const QPixmap &)), SIGNAL(signalBitmap(const QPixmap &)) );
}

void WBitmapWidget::slotUse( bool b )
{
    if ( b )
    {
        pPlacement->setEnabled( true );
        pBitmapButton->setEnabled( true );
    }
    else
    {
        pPlacement->setEnabled( false );
        pBitmapButton->setEnabled( false );
    }

    pUseBitMap->setChecked( b );
}

void WBitmapWidget::slotPlacement( W::WImagePlacements nPlacement )
{
    WSetComboBox( pPlacement, nPlacement );
}

void WBitmapWidget::slotBitmap( const QPixmap &pixmap )
{
    pBitmapButton->setPixmap( pixmap );
}

void WBitmapWidget::slotChangedPlacement( int nIndex )
{
    emit signalPlacement( W::WImagePlacements(pPlacement->itemData( nIndex ).toInt()) );
}






















