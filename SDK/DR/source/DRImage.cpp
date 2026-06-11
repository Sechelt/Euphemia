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
    pSettingImage = g_SSettings->getSetting( "LSImage" ); Q_ASSERT(pSettingImage);
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
    return image.scaled( r.width(), r.height() );
}

void DRImage::setImage( const QImage &i )
{
    image = i;
    getGraphics()->update();
}

QDomElement DRImage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElem = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( image, &nBytesCompressed );
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
            image = CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed );
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
    pPainter->drawImage( r, image );

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
//    stringImage  = pSettingImage->property( "stringImage" ).toString();
//    nHAlign     = (Qt::AlignmentFlag)pSettingImage->property( "nHAlign" ).toInt();
//    nVAlign     = (Qt::AlignmentFlag)pSettingImage->property( "nVAlign" ).toInt();
    if ( getGraphics() ) getGraphics()->update();
}

