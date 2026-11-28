/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRImage.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRImage::DRImage( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    pSettingImage = (DRSImage*)g_SSettings->getSetting( "DRSImage" ); Q_ASSERT(pSettingImage);
    slotRefreshImage();
}

DRImage::~DRImage()
{
}

QPixmap DRImage::getIcon()
{
    return QPixmap( ":DR/Image" );
}

QImage DRImage::getImage()
{
    // ToDo need to paint the entire rect like doDraw

    // QImage image( r.size() );
    // QPainter painter( &image );
    // 
    // painter.drawPixmap( 0, 0, pixmap );

    return pixmap.toImage();
}

void DRImage::setPixmap( const QPixmap &i )
{
    pixmap = i;
    getGraphics()->update();
}

QDomElement DRImage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElem = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmap.toImage(), &nBytesCompressed );
    domElem.setAttribute( "ImageSize", QString::number( nBytesCompressed ) );
    domElem.setAttribute( "ImageData", stringEncodedImage );

    return domElem;
}

bool DRImage::doLoad( QDomElement *pdomElem )
{
    DRRectangleBase::doLoad( pdomElem );

    ulong nBytesCompressed = pdomElem->attribute( "ImageSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "ImageData" );
        if ( !stringAttribute.isEmpty() )
        {
            pixmap = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
            return true;
        }
    }

    return false;
}

void DRImage::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw shape
    switch ( nPlacement )
    {
        case W::WImagePlacementNormal:
            {
                int nWidth  = qMin( r.toRect().width(), pixmap.width() );
                int nHeight = qMin( r.toRect().height(), pixmap.height() );
                pPainter->drawPixmap( r.topLeft(), pixmap, QRectF( 0, 0, nWidth, nHeight ) );
            }
            break;
        case W::WImagePlacementTiled:
            pPainter->drawTiledPixmap( r, pixmap );
            break;
        case W::WImagePlacementScaled:
            {
                QPixmap pixmapScaled = pixmap.scaled( r.size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
                pPainter->drawPixmap( r.left(), r.top(), pixmapScaled );
            }
            break;
    }

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( r );

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( r );
    }
}

void DRImage::doConnectSettings()
{
    // skip DRRectangleBase
    DRObject::doConnectSettings();
    pSettingImage->setSelected( pixmap, nPlacement );
    connect( pSettingImage, SIGNAL(signalChanged()), this, SLOT(slotRefreshImage()) );
}

void DRImage::doDisconnectSettings()
{
    // skip DRRectangleBase
    DRObject::doDisconnectSettings();
    disconnect( pSettingImage, SIGNAL(signalChanged()), this, SLOT(slotRefreshImage()) );
}

void DRImage::slotRefreshImage()
{
    pixmap = pSettingImage->getPixmap();
    nPlacement = pSettingImage->getPlacement();
    if ( getGraphics() ) getGraphics()->update();
}

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

QWidget *DRSImage::getPanelMax( QWidget *pParent )
{
    // just rely on default?
    QToolBox *pToolBox = new QToolBox( pParent );
    pToolBox->addItem( getPanel( pToolBox ), QIcon(), "Image" );
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



