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
    pixmap = QPixmap( ":DR/Image" ); // default so something shows up when drawn
    nPlacement = W::WImagePlacementScaled;
}

void DRSImage::setPixmap( const QPixmap &t )
{
    if ( t.toImage() == pixmap.toImage() ) return;
    pixmap = t;
    emit signalChanged();
}

void DRSImage::setPlacement( W::WImagePlacements n )
{
    if ( n == nPlacement ) return;
    nPlacement = n;
    emit signalChanged();
}

void DRSImage::setSelected( const QPixmap &t, W::WImagePlacements n )
{
    if ( t.toImage() == pixmap.toImage() && n == nPlacement ) return;
    pixmap = t;
    nPlacement = n;
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

QToolBox *DRSImage::getToolBox( QWidget *pParent )
{
    QToolBox *pToolBox = SSetting::getToolBox( pParent );
    pToolBox->insertItem( -1, getPanel( pToolBox ), QIcon(), "Image" );
    return pToolBox;
}

void DRSImage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElem = pdomDoc->createElement( "DRSImage" );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmap.toImage(), &nBytesCompressed );
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
            pixmap = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
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

    setIcon( QPixmap( ":DR/Image" ) );
    setToolTip( tr("image") );
    setText( ":" );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void DRSImageButton::slotRefresh()
{
    // leave icon as default
}

void DRSImageButton::slotClicked()
{
    QFileDialog fileDialog( nullptr );
    fileDialog.setAcceptMode( QFileDialog::AcceptOpen );
    fileDialog.setNameFilter("Bitmap files (*.png *.xpm *.jpg)");
    fileDialog.setWindowTitle(tr("Select Bitmap..."));

    if ( fileDialog.exec() != QDialog::Accepted ) return;

    QString stringFileName = fileDialog.selectedFiles().constFirst();
    QPixmap pixmap;
    bool b = pixmap.load( stringFileName );
    if ( !b ) 
    {
        QMessageBox::warning( this, tr("Image Load..."), tr("Failed to load the image file.") );
        return;
    }

    pSetting->setSelected( pixmap, pSetting->getPlacement() );
}

/****************************************************************
 * DRSImageToolBar
 ****************************************************************/
DRSImageToolBar::DRSImageToolBar( QWidget *pParent, bool bCompress )
    : QWidget( pParent )
{
    Q_UNUSED( bCompress );

    setObjectName( "DRSImageToolBar" );
    setWindowTitle( tr("Image") );

    pSetting = (DRSImage*)g_SSettings->getSetting( "DRSImage" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [DRSImage].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pImageButton = new DRSImageButton( this ); // will catch its own click event
    pLayout->addWidget( pImageButton );

    pPlacement = new QComboBox( this );
    pPlacement->addItem( "Normal", (int)W::WImagePlacementNormal );
    pPlacement->addItem( "Tiled", (int)W::WImagePlacementTiled );
    pPlacement->addItem( "Scaled", (int)W::WImagePlacementScaled );
    WSetComboBox( pPlacement, pSetting->getPlacement() );
    pLayout->addWidget( pPlacement );

    pLayout->addStretch( 10 );

    connect( pPlacement, SIGNAL(currentIndexChanged(int)), SLOT(slotChangedPlacement(int)) );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void DRSImageToolBar::slotRefresh()
{
    WSetComboBox( pPlacement, pSetting->getPlacement() );
}

void DRSImageToolBar::slotChangedPlacement( int nIndex )
{
    pSetting->setSelected( pSetting->getPixmap(), W::WImagePlacements(pPlacement->itemData( nIndex ).toInt()) );
}


/****************************************************************
 * DRSImagePanel
 ****************************************************************/
DRSImagePanel::DRSImagePanel( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "DRSImagePanel" );
    setWindowTitle( tr("Image") );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pLayoutTop->addWidget( new DRSImageToolBar( this ) );
    pLayoutTop->addWidget( new DRSImageSwatch( this ), 10 );
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
    pixmap = pSetting->getPixmap();
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void DRSImageSwatch::slotRefresh()
{
    pixmap = pSetting->getPixmap();
    update();
}

void DRSImageSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter(this);
    painter.drawPixmap( 0, 0, pixmap.scaled( size() ) );
}

QSize DRSImageSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}



