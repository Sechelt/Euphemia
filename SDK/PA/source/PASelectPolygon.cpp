/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PASelectPolygon.h"

#include "PAPaint.h"

PASelectPolygon::PASelectPolygon( PAPaint *pLayer )
    : PAPolygon( pLayer )
{
}

/*!
 * \brief Return a copy of the SELECTED AREA.
 * 
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage PASelectPolygon::getCopy()
{
    QRect r = polygon.boundingRect();
    QImage imageCopy = pLayer->getImageRef()->copy( r );
    return getTrimmed( r, imageCopy, getMask() );
}

// remove the auto commit
void PASelectPolygon::doDoubleClick( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( nState != StateDraw ) return;

    doManipulateState();
}

void PASelectPolygon::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );
    // no commit for a select shape - we just go straight to idle
    doIdleState();
}

void PASelectPolygon::doCut()
{
    emit signalPreCommit();

    QRect r = polygon.boundingRect();
    QClipboard *pClipboard = QGuiApplication::clipboard();
    QImage imageCopy = pLayer->getImageRef()->copy( r );
    pClipboard->setImage( getTrimmed( r, imageCopy, getMask(), true ) );

    emit signalCommitted();
}

void PASelectPolygon::doDelete()
{
    emit signalPreCommit();

    QRect r = polygon.boundingRect();
    QImage imageCopy = pLayer->getImageRef()->copy( r );
    getTrimmed( r, imageCopy, getMask(), true );

    emit signalCommitted();
}

QImage PASelectPolygon::getMask()
{
    // create a mask where the area we want is black and the rest is transparent
    QRect r = polygon.boundingRect();
    QImage imageMask( r.size(), QImage::Format_ARGB32 );
    {
        QPolygonF polygonCopy = polygon;
        polygonCopy.translate( -r.left(), -r.top() );
        imageMask.fill( Qt::transparent );
        QPainter painter( &imageMask );

        // DO NOT turn on anti aliasing here - we want a hard edge
        //        painter.setRenderHint( QPainter::Antialiasing );
        //        painter.setRenderHint( QPainter::SmoothPixmapTransform );

        QBrush brush( Qt::black );
        brush.setStyle( Qt::SolidPattern );
        painter.setBrush( brush );

        painter.setPen( QPen( Qt::NoPen ) );
        painter.drawPolygon( polygonCopy );
    }
    return imageMask;
}

void PASelectPolygon::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // Black dash line on white background to ensure visible. 
    QPen penWhite( Qt::white );
    QPen penDashLine( Qt::DashLine );

    // Set pen width based upon zoom so that the pen width looks the same despite zoom.
    int nZoom = pLayerView->transform().m11() * 100; // m11=x-scale
    penWhite.setWidthF( 1 / nZoom );
    penDashLine.setWidthF( 1 / nZoom );

    // paint
    if ( nState == StateDraw )
    {
        QPolygon poly = polygon;
        poly.append( pointMouse );

        // temp outline of area
        pPainter->setPen( penWhite );
        pPainter->drawPolygon( poly );                         

        pPainter->setPen( penDashLine );
        pPainter->drawPolygon( poly );                         
    }
    else
    {
        // temp outline of area
        pPainter->setPen( penWhite );
        pPainter->drawPolygon( polygon );                         

        pPainter->setPen( penDashLine );
        pPainter->drawPolygon( polygon );                         
    }
}



