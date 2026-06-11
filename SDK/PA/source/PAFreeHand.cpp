/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAFreeHand.h"

#include <SSettings.h>
#include <LGraphicsLayer.h>

#include "PAPaint.h"

PAFreeHand::PAFreeHand( PAPaint *pLayer )
    : PAToolFree( pLayer )
{
    pSettingFreeHand    = g_SSettings->getSetting( "PASFreeHand" ); Q_ASSERT( pSettingFreeHand );

    setAcceptHoverEvents( true );

    slotRefreshPen();
    slotRefreshBrush();
    slotRefreshFreeHand();

    connect( pSettingFreeHand, SIGNAL(signalChanged()), SLOT(slotRefreshFreeHand()) );
}

void PAFreeHand::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    pointLast = pEvent->pos();
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
            doDraw( &painter, r );
        }
        break;
    case StateDraw:
    case StateManipulate:
        break;
    }
    update();
}

void PAFreeHand::doMove( PMouseEvent *pEvent )
{
    pointLast = r.center();
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
            doDraw( &painter, r );
        }
        break;
    case StateManipulate:
        break;
    }
    update();
}

void PAFreeHand::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    pointLast = r.center();
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

QImage PAFreeHand::getPenImage()
{
    QSize size = getPenSize();
    QImage i( size, QImage::Format_ARGB32 );
    i.fill( Qt::transparent );

    QPainter painter( &i );                                         
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    

    doDraw( &painter, QRect( QPoint( 0, 0 ), size ) );

    return i;
}

QSize PAFreeHand::getPenSize()
{
    QSize sizeReturn;

    switch ( nShape )
    {
        case PASFreeHand::ShapePen:
            sizeReturn.setWidth( pen.width() );
            sizeReturn.setHeight( pen.width() );
            break;
        case PASFreeHand::ShapeEllipse:
            sizeReturn = size;
            break;
        case PASFreeHand::ShapeRectangle:
            sizeReturn = size;
            break;
        case PASFreeHand::ShapeCross:
            sizeReturn = size;
            break;
        case PASFreeHand::ShapeImageScaled:
            sizeReturn = image.size();
            break;
        case PASFreeHand::ShapeImage:
            sizeReturn = image.size();
            break;
    }

    return sizeReturn;
}

void PAFreeHand::doPaint( QPainter *pPainter, bool )
{
    // render has already been set in pGraphics
    pPainter->drawImage( r, imagePen );
}

void PAFreeHand::doDraw( QPainter *pPainter, const QRect &r )
{
    switch ( nShape )
    {
        case PASFreeHand::ShapePen:
            return doDrawPen( pPainter, r );
        case PASFreeHand::ShapeEllipse:
            return doDrawEllipse( pPainter, r );
        case PASFreeHand::ShapeRectangle:
            return doDrawRectangle( pPainter, r );
        case PASFreeHand::ShapeCross:
            return doDrawCross( pPainter, r );
        case PASFreeHand::ShapeImageScaled:
            return doDrawImage( pPainter, r );
        case PASFreeHand::ShapeImage:
            return doDrawImage( pPainter, r );
    }
    pLayerGraphics->update();
}

void PAFreeHand::doDrawPen( QPainter *pPainter, const QRect &r )
{
    // draw point
    pPainter->setPen( pen );
    if ( bLineTo )
        pPainter->drawLine( pointLast, r.center() );
    else
        pPainter->drawPoint( r.center() );
}

void PAFreeHand::doDrawEllipse( QPainter *pPainter, const QRect &r )
{
    // draw ellipse
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->drawEllipse( r );
}

void PAFreeHand::doDrawRectangle( QPainter *pPainter, const QRect &r )
{
    // draw rectangle
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->drawRect( r );
}

void PAFreeHand::doDrawCross( QPainter *pPainter, const QRect &r )
{
    // draw cross
    pPainter->setPen( pen );
    pPainter->drawLine( r.left(), r.center().y(), r.right(), r.center().y() );
    pPainter->drawLine( r.center().x(), r.top(), r.center().x(), r.bottom() );
}

void PAFreeHand::doDrawImage( QPainter *pPainter, const QRect &r )
{
    pPainter->drawImage( r.topLeft(), image );
}

void PAFreeHand::doDrawState( const QPoint &point )
{
    r.setSize( getPenSize() );
    r.moveCenter( point );

    nState = StateDraw;
    emit signalPreCommit();
}

void PAFreeHand::doIdleState()
{
    nState = StateIdle;
    emit signalCommitted();
}

void PAFreeHand::slotRefreshPen()
{
    switch ( nShape )
    {
        case PASFreeHand::ShapePen:
            pen = pSettingPen->property( "pen" ).value<QPen>();
            brush.setStyle( Qt::NoBrush );
            break;
        case PASFreeHand::ShapeEllipse:
            break;
        case PASFreeHand::ShapeRectangle:
            break;
        case PASFreeHand::ShapeCross:
            break;
        case PASFreeHand::ShapeImageScaled:
            break;
        case PASFreeHand::ShapeImage:
            break;
    }

    QPoint point = r.center();
    r.setSize( getPenSize() );
    r.moveCenter( point );

    imagePen = getPenImage();
}

void PAFreeHand::slotRefreshBrush()
{
    switch ( nShape )
    {
        case PASFreeHand::ShapePen:
            break;
        case PASFreeHand::ShapeEllipse:
            brush = pSettingBrush->property( "brush" ).value<QBrush>();
            break;
        case PASFreeHand::ShapeRectangle:
            brush = pSettingBrush->property( "brush" ).value<QBrush>();
            break;
        case PASFreeHand::ShapeCross:
            break;
        case PASFreeHand::ShapeImageScaled:
            break;
        case PASFreeHand::ShapeImage:
            break;
    }

    QPoint point = r.center();
    r.setSize( getPenSize() );
    r.moveCenter( point );

    imagePen = getPenImage();
}

void PAFreeHand::slotRefreshFreeHand()
{
    nShape      = (PASFreeHand::Shapes)pSettingFreeHand->property( "nShape" ).toInt();
    size        = pSettingFreeHand->property( "size" ).toSize();
    image       = pSettingFreeHand->property( "image" ).value<QImage>();
    bLineTo     = pSettingFreeHand->property( "bLineTo" ).toInt();

    switch ( nShape )
    {
        case PASFreeHand::ShapePen:
            pen = pSettingPen->property( "pen" ).value<QPen>();
            brush.setStyle( Qt::NoBrush );
            break;
        case PASFreeHand::ShapeEllipse:
            pen.setStyle( Qt::NoPen );
            brush = pSettingBrush->property( "brush" ).value<QBrush>();
            break;
        case PASFreeHand::ShapeRectangle:
            pen.setStyle( Qt::NoPen );
            brush = pSettingBrush->property( "brush" ).value<QBrush>();
            break;
        case PASFreeHand::ShapeCross:
            pen = pSettingPen->property( "pen" ).value<QPen>();
            brush.setStyle( Qt::NoBrush );
            break;
        case PASFreeHand::ShapeImageScaled:
            if ( image.isNull() )
            {
                nShape = PASFreeHand::ShapePen;
                pen = pSettingPen->property( "pen" ).value<QPen>();
                brush.setStyle( Qt::NoBrush );
            }
            else
                image = image.scaled( size );
            break;
        case PASFreeHand::ShapeImage:
            if ( image.isNull() )
            {
                nShape = PASFreeHand::ShapePen;
                pen = pSettingPen->property( "pen" ).value<QPen>();
                brush.setStyle( Qt::NoBrush );
            }
            else
                size = image.size();
            break;
    }

    QPoint point = r.center();
    r.setSize( getPenSize() );
    r.moveCenter( point );

    imagePen = getPenImage().copy();
}


