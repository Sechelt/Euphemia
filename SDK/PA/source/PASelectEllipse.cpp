/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PASelectEllipse.h"

#include "PAPaint.h"

PASelectEllipse::PASelectEllipse( PAPaint *pLayer )
    : PASelectRectangle( pLayer )
{
}

/*!
 * \brief Return a copy of the SELECTED AREA.
 * 
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage PASelectEllipse::getCopy()
{
    // get a copy of r
    QImage imageCopy = pLayer->getImageRef()->copy( r );
    // get a mask (solid black ellipse of r) and use it to trim imageCopy
    return getTrimmed( r, imageCopy, getMask() );
}

void PASelectEllipse::doCut()
{
    emit signalPreCommit();

    QClipboard *pClipboard = QGuiApplication::clipboard();
    QImage imageCopy = pLayer->getImageRef()->copy( r );
    pClipboard->setImage( getTrimmed( r, imageCopy, getMask(), true ) );

    emit signalCommitted();
}

void PASelectEllipse::doCopy()
{
    // put it into the system clipboard 
     QClipboard *pClipboard = QGuiApplication::clipboard();
     pClipboard->setImage( getCopy() );
}

void PASelectEllipse::doDelete()
{
    emit signalPreCommit();

    QImage imageCopy = pLayer->getImageRef()->copy( r );
    getTrimmed( r, imageCopy, getMask(), true );

    emit signalCommitted();
}

QImage PASelectEllipse::getMask()
{
    // create a mask where the area we want is black and the rest is transparent
    QImage imageMask( r.size(), QImage::Format_ARGB32 );
    {
        imageMask.fill( Qt::transparent );
        QPainter painter( &imageMask );

// DO NOT turn on anti aliasing here - we want a hard edge
//        painter.setRenderHint( QPainter::Antialiasing );
//        painter.setRenderHint( QPainter::SmoothPixmapTransform );

        QBrush brush( Qt::black );
        brush.setStyle( Qt::SolidPattern );
        painter.setBrush( brush );

        painter.setPen( QPen( Qt::NoPen ) );
        painter.drawEllipse( QRect( 0, 0, r.width(), r.height() ) );
    }
    return imageMask;
}

void PASelectEllipse::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // Black dash line on white background to ensure visible. 
    QPen penWhite( Qt::white );
    QPen penDashLine( Qt::DashLine );

    // Set pen width based upon zoom so that the pen width looks the same despite zoom.
    int nZoom = pLayerView->transform().m11() * 100; // m11=x-scale
    penWhite.setWidthF( 1 / nZoom );
    penDashLine.setWidthF( 1 / nZoom );

    // temp outline of area
    pPainter->setPen( penWhite );
    pPainter->drawEllipse( r );                         

    pPainter->setPen( penDashLine );
    pPainter->drawEllipse( r );                         
}



