/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASelectRectangle.h"

#include "PAPaint.h"

PASelectRectangle::PASelectRectangle( PAPaint *pLayer, bool bAll )
    : PARectangle( pLayer )
{
    bBind = false; 
    if ( bAll ) doSelectAll();
}

/*!
 * \brief Return a copy of the SELECTED AREA.
 * 
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage PASelectRectangle::getCopy()
{
    if ( nXRadius || nYRadius )
    {
        // get a copy of r
        QImage imageCopy = pLayer->getImageRef()->copy( r );
        // get a mask (solid black ellipse of r) and use it to trim imageCopy
        return getTrimmed( r, imageCopy, getMask() );
    }

    return pLayer->getImageRef()->copy( r );
}

// this removes the auto commit
void PASelectRectangle::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        doManipulateState();
        break;
    case StateManipulate:
        pHandle = nullptr;
        break;
    }
}

void PASelectRectangle::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );
    // no commit for a select shape - we just go straight to idle
    doIdleState();
}

void PASelectRectangle::doCut()
{
    QImage *pImage = pLayer->getImageRef();

    emit signalPreCommit();

    // copy
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage( pImage->copy( r ) );
    // delete (drawRect() with a transparent brush is a NoOp) so scan pixels
    QPainter painter( pImage );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    QColor colorBackground( Qt::transparent );      // this does not set alpha for some reason
    colorBackground.setAlpha( 0 );                  // so we set here
    r = r.normalized();
    qreal nLeft       = r.left() + 1;
    qreal nTop        = r.top() + 1;
    qreal nRight      = r.right() - 1;
    qreal nBottom     = r.bottom() - 1;

    for ( qreal nX = nLeft; nX <= nRight; nX++ )
    {
        for ( qreal nY = nTop; nY <= nBottom; nY++ )
        {
            pImage->setPixelColor( nX, nY, colorBackground );
        }
    }

    emit signalCommitted();
}

void PASelectRectangle::doDelete()
{
    QImage *pImage = pLayer->getImageRef();

    emit signalPreCommit();

    // delete (drawRect() with a transparent brush is a NoOp) so scan pixels
    QPainter painter( pImage );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    QColor colorBackground( Qt::transparent );      // this does not set alpha for some reason
    colorBackground.setAlpha( 0 );                  // so we set here
    r = r.normalized();
    qreal nLeft       = r.left() + 1;
    qreal nTop        = r.top() + 1;
    qreal nRight      = r.right() - 1;
    qreal nBottom     = r.bottom() - 1;

    for ( qreal nX = nLeft; nX <= nRight; nX++ )
    {
        for ( qreal nY = nTop; nY <= nBottom; nY++ )
        {
            pImage->setPixelColor( nX, nY, colorBackground );
        }
    }

    emit signalCommitted();
}

void PASelectRectangle::doSelectAll()
{
    if ( nState != StateIdle ) return;
    // fake doDraw to cover all
    r = QRect( QPoint( 0, 0 ), pLayer->getImageRef()->size() );
    nState = StateDraw;
    // go straight into manipulate
    doManipulateState();
}

QImage PASelectRectangle::getMask()
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
        painter.drawRoundedRect( QRect( 0, 0, r.width(), r.height() ), nXRadius, nYRadius, nSizeMode );
    }
    return imageMask;
}

void PASelectRectangle::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // Black dash line on white background to ensure visible. 
    QPen penWhite( Qt::white );
    QPen penDashLine( Qt::DashLine );

    // Set pen width based upon zoom so that the pen width looks the same despite zoom.
    int nZoom = pLayerView->transform().m11() * 100; // m11=x-scale
    penWhite.setWidthF( 1 / nZoom );
    penDashLine.setWidthF( 1 / nZoom );

    if ( nXRadius || nYRadius )
    {
        pPainter->setPen( penWhite );
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );

        pPainter->setPen( penDashLine );
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );
    }
    else
    {
        pPainter->setPen( penWhite );
        pPainter->drawRect( r.normalized() );                         

        pPainter->setPen( penDashLine );
        pPainter->drawRect( r.normalized() );                         
    }
}

