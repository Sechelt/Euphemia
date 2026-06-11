/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRSImage.h"

/****************************************************************
 * DRSImage
 ****************************************************************/
DRSImage::DRSImage()
{
    image = QPixmap( ":DR/Image" ).toImage();
}

void DRSImage::setValue( const QImage &t )
{
    if ( t == image ) return;
    image = t;
    emit signalChanged();
}

void DRSImage::setSelected( const QImage &t )
{
    if ( t == image ) return;
    image = t;
    emit signalSync();
}

QWidget *DRSImage::getButton( QWidget *pParent )
{
    return new DRSImageButton( pParent );
}

QWidget *DRSImage::getToolBar( QWidget *pParent )
{
    return new DRSImageToolBar( pParent );
}

QWidget *DRSImage::getPanel( QWidget *pParent )
{
    return new DRSImagePanel( pParent );
}

QWidget *DRSImage::getSwatch( QWidget *pParent )
{
    return new DRSImageSwatch( pParent );
}

void DRSImage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElem = pdomDoc->createElement( "DRSImage" );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( image, &nBytesCompressed );
    domElem.setAttribute( "ImageSize", QString::number( nBytesCompressed ) );
    domElem.setAttribute( "ImageData", stringEncodedImage );

    pdomElemParent->appendChild( domElem );
}

void DRSImage::doLoad( QDomElement *pdomElem )
{
    ulong nBytesCompressed = pdomElem->attribute( "ImageSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "ImageData" );
        if ( !stringAttribute.isEmpty() )
            image = CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed );
    }
}

/****************************************************************
 * DRSImageButton
 ****************************************************************/
DRSImageButton::DRSImageButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "DRSImageButton" );

    pSetting = (DRSImage*)g_SSettings->getSetting( "DRSImage" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [DRSImage].";
        return;
    }

    setIcon( getIcon() );
    setToolTip( tr("image") );
    setText( ":" );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void DRSImageButton::slotRefresh()
{
    setIcon( getIcon() );
}

void DRSImageButton::slotClicked()
{
    SDialog dialog( this, "DRSImage", "Image" );
    dialog.exec();
}

QPixmap DRSImageButton::getIcon()
{
    QPixmap pixmap( width(), height() );
    QPainter p( &pixmap );
    p.drawImage( QRect( 0, 0, width(), height() ), pSetting->getValue() );
    return pixmap;
}

/****************************************************************
 * DRSImageToolBar
 ****************************************************************/
DRSImageToolBar::DRSImageToolBar( QWidget *pParent, bool bCompress )
    : QWidget( pParent )
{
    Q_UNUSED( bCompress );

    setObjectName( "DRSImageToolBar" );

    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pLayout->addWidget( new DRSImageButton( this ) );
    pLayout->addStretch( 10 );
}

/****************************************************************
 * DRSImagePanel
 ****************************************************************/
DRSImagePanel::DRSImagePanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Brush") );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // form
    {
        QFormLayout *pLayoutForm = new QFormLayout();
        pLayoutForm->addRow( tr("Image"), new DRSImageButton( this ) );
        pLayoutTop->addLayout( pLayoutForm );
    }
}

/****************************************************************
 * DRSImageSwatch
 ****************************************************************/
DRSImageSwatch::DRSImageSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pSetting = (DRSImage*)g_SSettings->getSetting( "DRSImage" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [DRSImage].";
        return;

    }
    image = pSetting->getValue();
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void DRSImageSwatch::slotRefresh()
{
    image = pSetting->getValue();
    update();
}

void DRSImageSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter(this);
    painter.drawImage( QRect( 0, 0, width(), height() ), image );
}

QSize DRSImageSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}



