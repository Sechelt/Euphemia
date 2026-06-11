/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAErase.h"

#include <SSettings.h>
#include <LGraphicsLayer.h>

#include "PAPaint.h"

PAErase::PAErase( PAPaint *pLayer )
    : PAToolFree( pLayer )
{
    pSettingErase   = g_SSettings->getSetting( "PASErase" ); Q_ASSERT( pSettingErase );

    setAcceptHoverEvents( true );

    // set some sensible defaults
    colorTransparent.setRgba( qRgba( 0, 0, 0, 0 ) );

    // fake a refresh to tune the defaults
    slotRefreshPen();
    slotRefreshErase();

    connect( pSettingErase, SIGNAL(signalChanged()), SLOT(slotRefreshErase()) );
}

void PAErase::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        {
            doDrawState( pEvent->pos() );
            button = Qt::LeftButton;
            QPainter painter( pLayer->getImageRef() );
            painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
            painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
            painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
            painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
            doErase( &painter, r );
        }
        break;
    case StateDraw:
    case StateManipulate:
        break;
    }
    update();
}

void PAErase::doMove( PMouseEvent *pEvent )
{
    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        if ( button == Qt::LeftButton ) 
        {
            QPainter painter( pLayer->getImageRef() );
            painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
            painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
            painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
            painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
            doErase( &painter, r );
        }
        break;
    case StateManipulate:
        break;
    }
    update();
}

void PAErase::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    r.moveCenter( pEvent->pos() );

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        button = Qt::NoButton;
        doIdleState();
        break;
    case StateManipulate:
        break;
    }
}

QImage PAErase::getEraserImage()
{
    QSize size = getEraserSize();
    QImage i( size, QImage::Format_ARGB32 );
    i.fill( Qt::transparent );

    QPainter painter( &i );                                         
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    

    QPen pen( Qt::white );
    painter.setPen( pen );

    if ( pLayer->getBackground().alpha() > 5 ) pen.setColor( pLayer->getBackground() );

    switch ( nShape )
    {
        case PASErase::ShapePen:
            pen.setWidth( size.width() );
            painter.setPen( pen );
            painter.drawPoint( QPoint( size.width()/2, size.height()/2 ) );
            break;
        case PASErase::ShapeEllipse:
            painter.drawEllipse( QRect( QPoint( 0, 0 ), size ) );
            break;
        case PASErase::ShapeRectangle:
            painter.drawRect( QRect( QPoint( 0, 0 ), size ) );
            break;
        case PASErase::ShapeCross:
            pen.setWidth( size.width() );
            painter.setPen( pen );
            painter.drawLine( 0, size.height()/2, size.width(), size.height()/2 );
            painter.drawLine( size.width()/2, 0, size.width()/2, size.height() );
            break;
        case PASErase::ShapeImageScaled:
            painter.drawImage( QPoint( 0, 0 ), image );
            break;
        case PASErase::ShapeImage:
            painter.drawImage( QPoint( 0, 0 ), image );
            break;
    }

    return i;
}

QSize PAErase::getEraserSize()
{
    QSize sizeReturn;

    switch ( nShape )
    {
        case PASErase::ShapePen:
            sizeReturn.setWidth( pen.width() );
            sizeReturn.setHeight( pen.width() );
            break;
        case PASErase::ShapeEllipse:
            sizeReturn = size;
            break;
        case PASErase::ShapeRectangle:
            sizeReturn = size;
            break;
        case PASErase::ShapeCross:
            sizeReturn = size;
            break;
        case PASErase::ShapeImageScaled:
            sizeReturn = image.size();
            break;
        case PASErase::ShapeImage:
            sizeReturn = image.size();
            break;
    }

    return sizeReturn;
}

void PAErase::doPaint( QPainter *pPainter, bool )
{
    // render has already been set in pGraphics
    pPainter->drawImage( r, imageEraser );
}

/*!
 * \brief Erase using the current 'shape'. 
 *  
 * This will erase to PRaster::getBackground() or transparent. 
 *  
 * The least efficient is to erase to transparent because drawing 
 * with QPen/QBrush with Qt::Transparent (or Alpha=0) is a NoOp. 
 * To get around this we often resort to creating and applying a mask. 
 * 
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doErase( QPainter *pPainter, const QRect &r )
{
    switch ( nShape )
    {
        case PASErase::ShapePen:
            doErasePen( pPainter, r );
            break;
        case PASErase::ShapeEllipse:
            doEraseEllipse( pPainter, r );
            break;
        case PASErase::ShapeRectangle:
            doEraseRectangle( pPainter, r );
            break;
        case PASErase::ShapeCross:
            doEraseCross( pPainter, r );
            break;
        case PASErase::ShapeImageScaled:
            doEraseImage( pPainter, r );
            break;
        case PASErase::ShapeImage:
            doEraseImage( pPainter, r );
            break;
    }
    pLayerGraphics->update( pLayer->boundingRect() );
}

/*!
 * \brief Erase using the current pen. 
 *  
 * This will use the current pen width and style but not the color nor 
 * any brush that may be in the pen. 
 *  
 * The main difference from an doEraseEllipse is that; 
 *  
 * - this will be painted by drawing a line between last and current point (not stamp) 
 * - pen is, mostly, a circle 
 * - this will use pen style (although SolidLine is probably most useful)
 *  
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doErasePen( QPainter *pPainter, const QRect &r )
{
    // draw line
    if ( pLayer->getBackground().alpha() <= 5 )
    {
        // create a mask
        QImage imageMask( r.size(), QImage::Format_ARGB32 );
        imageMask.fill( Qt::white );            // white will be ignored
        // paint mask
        QPainter painter( &imageMask );
        painter.setRenderHint( QPainter::Antialiasing );
        painter.setRenderHint( QPainter::SmoothPixmapTransform );
        QPen pen( Qt::black );              // black will indicate pixel to erase
        pen.setWidth( size.width() );
        painter.setPen( pen );
        painter.drawPoint( QPoint( r.width()/2, r.height()/2 ) );
        // apply mask
        doErase( r, imageMask, pLayer->getImageRef(), QColor( Qt::black ) );
    }
    else
    {
        QPen pen( pLayer->getBackground() );
        pen.setWidth( size.width() );
        pPainter->setPen( pen );
        pPainter->drawPoint( r.center() );
    }
}

/*!
 * \brief Erase using an ellipse.
 * 
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doEraseEllipse( QPainter *pPainter, const QRect &r )
{
    if ( pLayer->getBackground().alpha() <= 5 )
    {
        // create a mask
        QColor colorMask( Qt::black );
        QImage imageMask( r.size(), QImage::Format_ARGB32 );
        {
            imageMask.fill( Qt::transparent );
            QPainter painter( &imageMask );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.setRenderHint( QPainter::SmoothPixmapTransform );

            QBrush brush( colorMask );
            brush.setStyle( Qt::SolidPattern );
            painter.setBrush( brush );

            painter.setPen( QPen( Qt::NoPen ) );

            painter.drawEllipse( QRect( 0, 0, r.width(), r.height() ) );
        }
        doErase( r, imageMask, pLayer->getImageRef(), colorMask );
    }
    else
    {
        QBrush brush( pLayer->getBackground() );
        brush.setStyle( Qt::SolidPattern );
        pPainter->setBrush( brush );
        pPainter->setPen( Qt::NoPen );
        pPainter->drawEllipse( r );
    }
}

/*!
 * \brief Erase using a rectangle.
 * 
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doEraseRectangle( QPainter *pPainter, const QRect &r )
{
    if ( pLayer->getBackground().alpha() <= 5 )
    {
        QImage *pImageCanvas = pLayer->getImageRef();               
        QColor colorBackground = pLayer->getBackground();
        int nLeft     = r.left();
        int nRight    = r.right();
        int nTop      = r.top();
        int nBottom   = r.bottom();

        // do not need a mask for rectangle
        for ( int nX = nLeft; nX <= nRight; nX++ )
        {
            for ( int nY = nTop; nY <= nBottom; nY++ )
            {
                pImageCanvas->setPixelColor( nX, nY, colorBackground );
            }
        }
    }
    else
    {
        QBrush brush( pLayer->getBackground() );
        brush.setStyle( Qt::SolidPattern );
        pPainter->setBrush( brush );
        pPainter->setPen( Qt::NoPen );
        pPainter->drawRect( r );
    }
}

/*!
 * \brief Erase using a cross. 
 *  
 * A cross can be useful when creating granular/pixelated images. 
 *  
 * \todo Factor in line width to prevent wonky looking cross with think line. 
 *  
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doEraseCross( QPainter *pPainter, const QRect &r )
{
    if ( pLayer->getBackground().alpha() <= 5 )
    {
        // mask image
        QImage imageMask( r.size(), QImage::Format_ARGB32 );
        imageMask.fill( Qt::white );            // white will be ignored
        // paint mask
        QPainter painter( &imageMask );
        painter.setRenderHint( QPainter::Antialiasing );
        painter.setRenderHint( QPainter::SmoothPixmapTransform );
        QPen pen( Qt::black );                  // black will indicate pixel to erase
        pen.setWidth( size.width() );
        pPainter->setPen( pen );
        pPainter->drawLine( 0, r.height()/2, r.width(), r.height()/2 );
        pPainter->drawLine( r.width()/2, 0, r.width()/2, r.height() );

        // apply mask
        doErase( r, imageMask, pLayer->getImageRef(), QColor(Qt::black) );
    }
    else
    {
        // paint a cross in the background color
        QPen pen( pLayer->getBackground() );
        pen.setWidth( size.width() );
        pPainter->setPen( pen );
        pPainter->drawLine( r.left(), r.center().y(), r.right(), r.center().y() );
        pPainter->drawLine( r.center().x(), r.top(), r.center().x(), r.bottom() );
    }
}

/*!
 * \brief Uses a given image as a mask. 
 *  
 * All colors (other than transparent) are converted to black to create a mask. 
 * This mask is then passed to doErase to apply to canvas image. 
 *  
 * The mask can be scaled (to erase size) or not. 
 *  
 * \note The t.image is preprocessed in doPress for efficiency. 
 *  
 * \author pharvey (2/16/23)
 * 
 * \param point  
 */
void PAErase::doEraseImage( QPainter *pPainter, const QRect &r )
{
    // image should be preprocessed for us in doPress

    if ( image.isNull() )
    {
        qWarning( "Null image." );
        return;
    }

    // apply mask
    doErase( r, image, pLayer->getImageRef(), QColor( Qt::black ) );
}

void PAErase::doDrawState( const QPoint &point )
{
    r.setSize( getEraserSize() );
    r.moveCenter( point );

    nState = StateDraw;
    emit signalPreCommit();
}

void PAErase::doIdleState()
{
    nState = StateIdle;
    emit signalCommitted();
}

void PAErase::slotRefreshPen()
{
    pen = pSettingPen->property( "pen" ).value<QPen>();

    QPoint point = r.center();
    r.setSize( getEraserSize() );
    r.moveCenter( point );

    imageEraser = getEraserImage();
}

void PAErase::slotRefreshErase()
{
    nShape = (PASErase::Shapes)pSettingErase->property( "nShape" ).toInt();
    size   = pSettingErase->property( "size" ).toSize();
    image  = pSettingErase->property( "image" ).value<QImage>();

    switch ( nShape )
    {
        case PASErase::ShapePen:
            break;
        case PASErase::ShapeEllipse:
            break;
        case PASErase::ShapeRectangle:
            break;
        case PASErase::ShapeCross:
            break;
        case PASErase::ShapeImageScaled:
            if ( image.isNull() )
                nShape = PASErase::ShapePen;
            else
            {
                image = image.scaled( size );
                getMask( &image );
            }
            break;
        case PASErase::ShapeImage:
            if ( image.isNull() )
                nShape = PASErase::ShapePen;
            else
            {
                size = image.size();
                getMask( &image );
            }
            break;
    }

    QPoint point = r.center();
    r.setSize( getEraserSize() );
    r.moveCenter( point );

    imageEraser = getEraserImage();
}

/*!
 * \brief Modifies the given image to make it a mask. 
 *  
 * All color pixels (pixels that are not transparent) are set to black. 
 * 
 * \author pharvey (2/16/23)
 * 
 * \param pImage 
 * 
 * \return QImage* 
 */
QImage *PAErase::getMask( QImage *pImage )
{
    if ( pImage->isNull() ) return pImage;

    size.setWidth( pImage->size().width() );
    size.setHeight( pImage->size().height() );

    for ( int nX = 0; nX < size.width(); nX++ )
    {
        for ( int nY = 0; nY < size.height(); nY++ )
        {
            QColor color = pImage->pixelColor( nX, nY );
            if ( color != Qt::transparent && color.alpha() != 0 ) 
            {
                pImage->setPixelColor( nX, nY, Qt::black );
            }
        }
    }

    return pImage;
}


/*!
 * \brief Applies a mask to the canvas image. 
 *  
 * Scans the mask. Wherever it finds a colorMask pixel it sets the corresponding pixel in the 
 * canvas image to the background color. 
 *  
 * \note    Setting pen or brush color to Qt::transparent is a NoOp even if Alpha is
 *          explicitly set to 0. So we must set image pixels to transparent directly. 
 *  
 * \author pharvey (2/16/23)
 * 
 * \param rect         
 * \param imageMask    
 * \param pImageCanvas 
 * \param colorMask    
 */
void PAErase::doErase( const QRect &rect, const QImage &imageMask, QImage *pImageCanvas, const QColor &colorMask )
{
    QColor colorBackground = pLayer->getBackground();
    // fill (apply mask)
    int nLeft     = rect.left();
    int nTop      = rect.top();

    size.setWidth( rect.width() );
    size.setHeight( rect.height() );

    // process the mask
    for ( int nX = 0; nX < size.width(); nX++ )
    {
        for ( int nY = 0; nY < size.height(); nY++ )
        {
            if ( imageMask.pixelColor( nX, nY ) == colorMask ) 
            {
                pImageCanvas->setPixelColor( nX + nLeft, nY + nTop, colorBackground );
            }
        }
    }
}

