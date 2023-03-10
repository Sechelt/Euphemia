/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 *
 * This file is part of Euphemia.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AppInfo.h"
#include "EPreferencesDialog.h"

#include <PPenToolBar.h>
#include <PBrushToolBar.h>
#include <PDrawFreeHand.h>
// #include <PDrawErase.h>
#include <PDrawSpray.h>
#include <PDrawText.h>
#include <PPasteRectangle.h>
// #include <PFillFlood.h>

EPreferencesDialog::EPreferencesDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Preferences") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    QWidget *p = new QWidget( this );

    {
        QVBoxLayout *pLayoutTop = new QVBoxLayout( p );

        // add tool preferences
        {
            QFormLayout *pLayout = new QFormLayout();

            pLayout->addRow( tr("General"), new PGeneralConfig( p ) );
            pLayout->addRow( tr("Pen"), new PPenToolBar( p ) );
            pLayout->addRow( tr("Brush"), new PBrushToolBar( p ) );
            pLayout->addRow( tr("Free Hand"), new PFreeHandToolBar( p ) );
            pLayout->addRow( tr("Spray"), new PSprayToolBar( p ) );
            pLayout->addRow( tr("Text"), new PTextToolBar( p, false ) );
            pLayout->addRow( tr("Paste"), new PPasteToolBar( p ) );

            pLayoutTop->addLayout( pLayout );
        }
        pLayoutTop->addStretch( 10 );
    }

    pLayout->addWidget( p ); 

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
}

void EPreferencesDialog::doPreferences( QWidget *pParent )
{
    EPreferencesDialog dialog( pParent );
    dialog.exec();
    g_Context->doSave();
}


//
//
//
PGeneralConfig::PGeneralConfig( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "PGeneralConfig" );

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    PContextGeneral general = g_Context->getGeneral();

    QBrush brush = general.brushTransparency;
    if ( brush.style() == Qt::NoBrush ) brush.setTextureImage( QImage(":W/Transparent") );

    pColor = new WColorButton( brush.color(), this, WColorButton::Fill );
    pColor->setToolTip( tr("brush color to represent transparency") );
    pLayout->addWidget( pColor );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );

    pStyle = new WBrushStyleComboBox( this, brush.style() );
    pStyle->setToolTip( tr("brush style to represent transparency") );
    pLayout->addWidget( pStyle );
    connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

    pImage = new WImageButton( brush.textureImage(), this );
    pImage->setToolTip( tr("brush texture to represent transparency") );
    pLayout->addWidget( pImage );
    connect( pImage, SIGNAL(signalClick()), this, SLOT(slotImage()) );

    pRestoreState = new QCheckBox( tr("Restore Window State"), this );
    pRestoreState->setChecked( general.bRestoreState );
    pLayout->addWidget( pRestoreState );
    connect( pRestoreState, SIGNAL(stateChanged(int)), SLOT(slotRestoreState(int)) );

    pAutoCommit = new QCheckBox( tr("Auto Commit Shapes"), this );
    pAutoCommit->setChecked( general.bAutoCommit );
    pLayout->addWidget( pAutoCommit );
    connect( pAutoCommit, SIGNAL(stateChanged(int)), SLOT(slotAutoCommit(int)) );

    pLayout->addStretch( 10 );

    connect( g_Context, SIGNAL(signalModified(const PContextGeneral &)), SLOT(slotRefresh(const PContextGeneral &)) );
}

void PGeneralConfig::slotRefresh( const PContextGeneral &t )
{
    QBrush brush = t.brushTransparency;
    if ( brush.style() == Qt::NoBrush ) brush.setTextureImage( QImage(":W/Transparent") );

    pColor->setValue( brush.color() );
    pStyle->setValue( brush.style() );
    pImage->setImage( brush.textureImage() );
    pRestoreState->setChecked( t.bRestoreState );
    pAutoCommit->setChecked( t.bAutoCommit );
}

void PGeneralConfig::slotColor( const QColor &color )
{
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setColor( color );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotStyle( Qt::BrushStyle n )
{
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setStyle( n );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setTextureImage( image );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotRestoreState( int n )
{
    PContextGeneral t = g_Context->getGeneral();
    t.bRestoreState = n;
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotAutoCommit( int n )
{
    PContextGeneral t = g_Context->getGeneral();
    t.bAutoCommit = n;
    g_Context->setGeneral( t );
}


