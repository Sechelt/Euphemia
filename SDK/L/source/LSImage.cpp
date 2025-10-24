/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LSImage.h"

#include "SSettings.h"

/****************************************************************
 * LSImage
 ****************************************************************/
LSImage::LSImage()
{
}

void LSImage::setImageColorMode( Qt::ImageConversionFlag t )
{
    if ( t == nColorMode ) return;
    nColorMode = t;
    emit signalChanged();
}

void LSImage::setImageAlphaDither( Qt::ImageConversionFlag t )
{
    if ( t == nAlphaDither ) return;
    nAlphaDither = t;
    emit signalChanged();
}

void LSImage::setImageDither( Qt::ImageConversionFlag t )
{
    if ( t == nDither ) return;
    nDither = t;
    emit signalChanged();
}

void LSImage::setImageDitherMode( Qt::ImageConversionFlag t )
{
    if ( t == nDitherMode ) return;
    nDitherMode = t;
    emit signalChanged();
}

void LSImage::setSelected( Qt::ImageConversionFlag nColorMode, Qt::ImageConversionFlag nAlphaDither, Qt::ImageConversionFlag nDither, Qt::ImageConversionFlag nDitherMode )
{
    this->nColorMode    = nColorMode;
    this->nAlphaDither  = nAlphaDither;
    this->nDither       = nDither;
    this->nDitherMode   = nDitherMode;
    emit signalSync();
}

QWidget *LSImage::getToolBar( QWidget *pParent )
{
    return new LSImageWidget( pParent, Qt::Horizontal );
}

QWidget *LSImage::getPanel( QWidget *pParent )
{
    return new LSImageWidget( pParent, Qt::Vertical );
}

void LSImage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSImage" );

    domElem.setAttribute( "ColorMode", nColorMode );
    domElem.setAttribute( "AlphaDither", nAlphaDither );
    domElem.setAttribute( "Dither", nDither );
    domElem.setAttribute( "DitherMode", nDitherMode );

    pdomElem->appendChild( domElem );
}

void LSImage::doLoad( QDomElement *pdomElem )
{
    nColorMode      = Qt::ImageConversionFlag(pdomElem->attribute( "ColorMode", QString::number( int(nColorMode) ) ).toInt());
    nAlphaDither    = Qt::ImageConversionFlag(pdomElem->attribute( "AlphaDither", QString::number( int(nAlphaDither) ) ).toInt());
    nDither         = Qt::ImageConversionFlag(pdomElem->attribute( "Dither", QString::number( int(nDither) ) ).toInt());
    nDitherMode     = Qt::ImageConversionFlag(pdomElem->attribute( "DitherMode", QString::number( int(nDitherMode) ) ).toInt());
}

/****************************************************************
 * LSImageWidget
 ****************************************************************/
LSImageWidget::LSImageWidget( QWidget *p, Qt::Orientation n )
    : QWidget( p )
{
    pSetting = (LSImage*)g_SSettings->getSetting( "LSImage" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSImage].";
        return;
    }

    // nImageConversion
    pColorMode = new QComboBox( this );
    pColorMode->addItem( "AutoColor", Qt::AutoColor );
    pColorMode->addItem( "ColorOnly", Qt::ColorOnly );
    pColorMode->addItem( "MonoOnly", Qt::MonoOnly );
    pColorMode->setToolTip( tr("Color/Mono preference.") );
    pColorMode->setCurrentIndex( pColorMode->findData( pSetting->getImageColorMode() ) );
    connect( pColorMode, SIGNAL(currentIndexChanged(int)), SLOT(slotImageColorMode(int)) );

    pDither = new QComboBox( this );
    pDither->addItem( "DiffuseDither", Qt::DiffuseDither );
    pDither->addItem( "OrderedDither", Qt::OrderedDither );
    pDither->addItem( "ThresholdDither", Qt::ThresholdDither );
    pDither->setToolTip( tr("Dither mode preference.") );
    pDither->setCurrentIndex( pDither->findData( pSetting->getImageDither() ) );
    connect( pDither, SIGNAL(currentIndexChanged(int)), SLOT(slotImageDither(int)) );

    pAlphaDither = new QComboBox( this );
    pAlphaDither->addItem( "ThresholdAlphaDither", Qt::ThresholdAlphaDither );
    pAlphaDither->addItem( "OrderedAlphaDither", Qt::OrderedAlphaDither );
    pAlphaDither->addItem( "DiffuseAlphaDither", Qt::DiffuseAlphaDither );
    pAlphaDither->setToolTip( tr("Dither mode preference for 1-bit alpha masks.") );
    pAlphaDither->setCurrentIndex( pAlphaDither->findData( pSetting->getImageAlphaDither() ) );
    connect( pAlphaDither, SIGNAL(currentIndexChanged(int)), SLOT(slotImageAlphaDither(int)) );

    pDitherMode = new QComboBox( this );
    pDitherMode->addItem( "AutoDither", Qt::AutoDither );
    pDitherMode->addItem( "PreferDither", Qt::PreferDither );
    pDitherMode->addItem( "AvoidDither", Qt::AvoidDither );
    pDitherMode->addItem( "NoOpaqueDetection", Qt::DiffuseAlphaDither );
    pDitherMode->setToolTip( tr("Color matching versus dithering preference.") );
    pDitherMode->setCurrentIndex( pDitherMode->findData( pSetting->getImageDitherMode() ) );
    connect( pDitherMode, SIGNAL(currentIndexChanged(int)), SLOT(slotImageDitherMode(int)) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pColorMode );
        pLayout->addWidget( pDither );
        pLayout->addWidget( pAlphaDither );
        pLayout->addWidget( pDitherMode );
        pLayout->addStretch( 10 );
    }
    else
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        {
            QGroupBox *pGroupBox = new QGroupBox( tr("Image Conversion") );
            QFormLayout *pForm = new QFormLayout();
            pGroupBox->setLayout( pForm );
            pForm->addRow( tr("ColorMono"), pColorMode );
            pForm->addRow( tr("Dither"), pDither );
            pForm->addRow( tr("DitherMask"), pAlphaDither );
            pForm->addRow( tr("ColorMatching"), pDitherMode );
            pLayout->addWidget( pGroupBox );
        }
        pLayout->addStretch( 10 );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSImageWidget::slotRefresh()
{
    pColorMode->setCurrentIndex( pColorMode->findData( pSetting->getImageColorMode() ) );
    pDither->setCurrentIndex( pDither->findData( pSetting->getImageDither() ) );
    pAlphaDither->setCurrentIndex( pAlphaDither->findData( pSetting->getImageAlphaDither() ) );
    pDitherMode->setCurrentIndex( pDitherMode->findData( pSetting->getImageDitherMode() ) );
}

void LSImageWidget::slotImageColorMode( int )
{
    pSetting->setImageColorMode( Qt::ImageConversionFlag(pColorMode->currentData().toInt()) );
}

void LSImageWidget::slotImageAlphaDither( int )
{
    pSetting->setImageAlphaDither( Qt::ImageConversionFlag(pAlphaDither->currentData().toInt()) );
}

void LSImageWidget::slotImageDither( int )
{
    pSetting->setImageDither( Qt::ImageConversionFlag(pDither->currentData().toInt()) );
}

void LSImageWidget::slotImageDitherMode( int )
{
    pSetting->setImageDitherMode( Qt::ImageConversionFlag(pDitherMode->currentData().toInt()) );
}


