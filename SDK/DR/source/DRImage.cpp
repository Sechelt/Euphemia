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

