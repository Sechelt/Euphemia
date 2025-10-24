/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAFloodFill.h"

#include "PAPaint.h"

PAFloodFill::PAFloodFill( PAPaint *pLayer )
    : PAToolFree( pLayer )
{
}

void PAFloodFill::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    emit signalPreCommit();

    pImage = pLayer->getImageRef();
    doFillFlood( pEvent->pos() );
    pImage = nullptr;

    emit signalCommitted();
}

void PAFloodFill::doMove( PMouseEvent *pEvent )
{
}

void PAFloodFill::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

}

void PAFloodFill::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;
    if ( nState != StateIdle ) return;
     
    // \ToDo paint a cursor
}

void PAFloodFill::doFillFlood( const QPoint &pointSeed )
{
    if ( brush.style() == Qt::SolidPattern ||
         !brush.textureImage().isNull() ||
         brush.style() != Qt::NoBrush )
    {
        // set things up for an undo
        // stackRedo.clear();
        // stackUndo.push( image );
        // if ( stackUndo.count() >= nMaxUndo ) stackUndo.removeFirst();

        //
        if ( brush.style() == Qt::SolidPattern )
            doFillFloodColor( pointSeed );
        else if ( !brush.textureImage().isNull() )
            doFillFloodTiled( pointSeed );
        else if ( brush.style() != Qt::NoBrush )
            doFillFloodTiled( pointSeed );

        // if ( isModified() )
        //     emit signalChanged(); 
        // else
        //    setModified();

        // update();
    }
    else
        QMessageBox::warning( qApp->activeWindow(), tr("Flood Fill"), tr("Please select a brush.") );
}

/*!
 * \brief Flood fill with current brush color.
 *  
 * This is not super efficient (can check pixel 4x) but it is a super simple 
 * bit of code. 
 *  
 * Lets Qt worry about color format. 
 *  
 * \author pharvey (1/27/23)
 * 
 * \param pointSeed 
 */
void PAFloodFill::doFillFloodColor( const QPoint &pointSeed )
{
    int nWidth          = pImage->size().width();
    int nHeight         = pImage->size().height();
    QColor colorNew     = brush.color();            // fill color
    QColor colorSeed    = pImage->pixelColor( pointSeed );            // color at seed point - color we are replacing
    QStack<QPoint> stackPointsToProcess;

    // prime stack with seed point
    stackPointsToProcess.push( pointSeed );
    // process until stack is empty
    while ( !stackPointsToProcess.isEmpty() )
    {
        QPoint point = stackPointsToProcess.pop();
        int nX = point.x();
        int nY = point.y();
        if ( pImage->pixelColor( point ) == colorNew ) continue;      // we already changed this pixel or we have hit a boundary (which happens to be fill color) so do nothing
        if ( pImage->pixelColor( point ) != colorSeed ) continue;     // not the color we want to fill over so do nothing
        pImage->setPixelColor( point, colorNew );                     // fill
        // check N
        if ( nY - 1 >= 0 )
        {
            stackPointsToProcess.push( QPoint( nX, nY - 1 ) );
        }
        // check S
        if ( nY + 1 < nHeight )
        {
            stackPointsToProcess.push( QPoint( nX, nY + 1 ) );
        }
        // check E
        if ( nX + 1 < nWidth )
        {
            stackPointsToProcess.push( QPoint( nX + 1, nY ) );
        }
        // check W
        if ( nX - 1 >= 0 )
        {
            stackPointsToProcess.push( QPoint( nX - 1, nY ) );
        }
    }
}

/*!
 * \brief Flood fill with current brush pattern or texture.
 *  
 * This is based upon doFillFloodColor() but addresses the fact that 
 * a pattern/texture fill does not always result in a target pixel being 
 * changed. 
 *  
 * So just checking the color of the pixel does not tell us if 
 * that pixel has already been processed. 
 *  
 * We add a 2d array indicating whether or not a pixel has been 
 * processed. 
 *  
 * This also; 
 *  - reduces use of the stack
 *  - reduces full checks for a given point to 0-1
 *  
 * \author pharvey (1/27/23)
 * 
 * \param point 
 */
void PAFloodFill::doFillFloodTiled( const QPoint &pointSeed )
{
    int nWidth = pImage->size().width();
    int nHeight = pImage->size().height();

    QVector<QVector<bool>> tableProcessed( nWidth, QVector<bool>( nHeight, false ) );

    QColor  colorSeed        = pImage->pixelColor( pointSeed );            // we will use to determine the boundary
    QImage  imageSource      = QImage( pImage->size(), pImage->format() );   // we will use to get our new pixel colors

    // Initialize the source image.
    {
        // We will use a pattern with a transparent background so fill with the seed color.
        imageSource.fill( colorSeed );
        // Fill with pattern.
        QPainter painter( &imageSource );
        painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
        painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
        painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
        painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
        painter.fillRect( 0, 0, nWidth, nHeight, brush );
    }

    // prime stack with seed point
    QStack<QPoint> stackPointsToProcess;
    stackPointsToProcess.push( pointSeed );
    // process until stack is empty
    while ( !stackPointsToProcess.isEmpty() )
    {
        QPoint point = stackPointsToProcess.pop();
        int nX = point.x();
        int nY = point.y();
        tableProcessed[nX][nY] = true;                                      // first and last time we will process this point 
        if ( pImage->pixelColor( point ) != colorSeed ) continue;             // not the color we want to fill over so do nothing
        pImage->setPixelColor( point, imageSource.pixelColor( point ) );      // fill
        // look N
        if ( nY - 1 >= 0 && tableProcessed[nX][nY-1] == false )
        {
            stackPointsToProcess.push( QPoint( nX, nY - 1 ) );
        }
        // look S
        if ( nY + 1 < nHeight && tableProcessed[nX][nY+1] == false )
        {
            stackPointsToProcess.push( QPoint( nX, nY + 1 ) );
        }
        // look E
        if ( nX + 1 < nWidth && tableProcessed[nX+1][nY] == false )
        {
            stackPointsToProcess.push( QPoint( nX + 1, nY ) );
        }
        // look W
        if ( nX - 1 >= 0 && tableProcessed[nX-1][nY] == false )
        {
            stackPointsToProcess.push( QPoint( nX - 1, nY ) );
        }
    }
}

//
// !!! NOT USED !!!
//
void PAFloodFill::doFillGradient( const QPoint &pointSeed )
{
    int nWidth              = pImage->size().width();
    int nHeight             = pImage->size().height();
    QVector<QVector<bool>> tableProcessed( nWidth, QVector<bool>( nHeight, false ) );
    QColor colorSeed        = pImage->pixelColor( pointSeed );            // we will use to determine the boundary
    QImage imageSource      = QImage( pImage->size(), pImage->format() );   // we will use to get our new pixel colors

    // Initialize the source image.
    {
        // We will use a pattern with a transparent background so fill with the seed color.
        imageSource.fill( colorSeed );
        // Fill with pattern.
        QPainter painter( &imageSource );
        painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
        painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
        painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
        painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
        // QBrush brush = brush;
        // QGradient gradient = g_SSetting->getGradient();
        // QGradient gradient;

        // example 1
        QRadialGradient radialGrad( pointSeed, 100 );
        radialGrad.setColorAt( 0, Qt::red);
        radialGrad.setColorAt( 0.5, Qt::blue);
        radialGrad.setColorAt( 1, Qt::green);

        // example 2 (preset)
        QGradient gradient( QGradient::WarmFlame );
        gradient.setColorAt( 0, Qt::red);
        gradient.setColorAt( 0.5, Qt::blue);
        gradient.setColorAt( 1, Qt::green);

        painter.fillRect( 0, 0, nWidth, nHeight, QBrush( gradient ) );
return;
    }

    // prime stack with seed point
    QStack<QPoint> stackPointsToProcess;
    stackPointsToProcess.push( pointSeed );
    // process until stack is empty
    while ( !stackPointsToProcess.isEmpty() )
    {
        QPoint point = stackPointsToProcess.pop();
        int nX = point.x();
        int nY = point.y();
        tableProcessed[nX][nY] = true;                                      // first and last time we will process this point 
        if ( pImage->pixelColor( point ) != colorSeed ) continue;             // not the color we want to fill over so do nothing
        pImage->setPixelColor( point, imageSource.pixelColor( point ) );      // fill
        // look N
        if ( nY - 1 >= 0 && tableProcessed[nX][nY-1] == false )
        {
            stackPointsToProcess.push( QPoint( nX, nY - 1 ) );
        }
        // look S
        if ( nY + 1 < nHeight && tableProcessed[nX][nY+1] == false )
        {
            stackPointsToProcess.push( QPoint( nX, nY + 1 ) );
        }
        // look E
        if ( nX + 1 < nWidth && tableProcessed[nX+1][nY] == false )
        {
            stackPointsToProcess.push( QPoint( nX + 1, nY ) );
        }
        // look W
        if ( nX - 1 >= 0 && tableProcessed[nX-1][nY] == false )
        {
            stackPointsToProcess.push( QPoint( nX - 1, nY ) );
        }
    }
}


