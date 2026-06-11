/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAPasteRectangle.h"

#include <SSettings.h>

#include "PAPaint.h"

//
// PAPasteRectangle
//
PAPasteRectangle::PAPasteRectangle( PAPaint *pLayer )
    : PARectangle( pLayer )
{
    pSettingPaste = g_SSettings->getSetting( "PASPaste" ); Q_ASSERT( pSettingPaste );
    slotRefreshPaste();
    connect( pSettingPaste, SIGNAL(signalChanged()), SLOT(slotRefreshPaste()) );

    image = QGuiApplication::clipboard()->image();
    doDrawState( QPoint( 10, 10 ) );
    doManipulateState();

    r.moveCenter( pLayerView->mapToScene( pLayerView->viewport()->rect().center() ).toPoint() );

    doSyncHandles();
}

PAPasteRectangle::PAPasteRectangle( PAPaint *pLayer, const QImage &i )
    : PARectangle( pLayer )
{
    pSettingPaste = g_SSettings->getSetting( "PASPaste" ); Q_ASSERT( pSettingPaste );
    slotRefreshPaste();
    connect( pSettingPaste, SIGNAL(signalChanged()), SLOT(slotRefreshPaste()) );

    image = i;
    doDrawState( QPoint( 10, 10 ) );
    doManipulateState();

    r.moveCenter( pLayerView->mapToScene( pLayerView->viewport()->rect().center() ).toPoint() );

    doSyncHandles();
}

void PAPasteRectangle::setImage( const QImage &i )
{
    doCancel();
    image = i;
    doDrawState( QPoint( 10, 10 ) );
    doManipulateState();
}

void PAPasteRectangle::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        pHandle = getHandle( pEvent->pos() );
        if ( !pHandle ) doCommit();
        break;
    }
}

void PAPasteRectangle::doCommit()
{
    Q_ASSERT( nState == StateDraw || nState == StateManipulate );

    emit signalPreCommit();

    QPainter painter( pLayer->getImageRef() );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    emit signalCommitted();

    doIdleState();
}

bool PAPasteRectangle::doScale( qreal nX, qreal nY, Qt::AspectRatioMode nRatio )
{
    image = image.scaled( (int)nX, (int)nY, nRatio, Qt::SmoothTransformation );
    r.setSize( image.size() );
    doSyncHandles();
    update();
    return true;
}

bool PAPasteRectangle::doFlipX()
{
    image = image.mirrored( true, false );
    update();
    return true;
}

bool PAPasteRectangle::doFlipY()
{
    image = image.mirrored( false, true );
    update();
    return true;
}

bool PAPasteRectangle::doRotate( int nDegrees )
{
    QPoint pointCenter = r.center();
    image = image.transformed( QTransform().rotate( nDegrees ) );
    r.setSize( image.size() );
    r.moveCenter( pointCenter );
    doSyncHandles();
    update();
    return true;
}

bool PAPasteRectangle::canCommit()
{
    if ( image.isNull() ) return false;
//    return PARectangle::canCommit();
    return true;
}

bool PAPasteRectangle::canCancel()
{
    if ( image.isNull() ) return false;
//    return PARectangle::canCommit();
    return true;
}

void PAPasteRectangle::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    if ( nCompositionMode != QPainter::CompositionMode_SourceOver )
    {
        // dest
        QImage imageDestination = pLayer->getImageRef()->copy( r );
        int nWidth = r.width();
        int nHeight = r.height();

        for ( int nX = 0; nX < nWidth; nX++ )                                                                                  
        {                                                                                                                           
            for ( int nY = 0; nY < nHeight; nY++ )                                                                              
            {                                                                                                                       
                if ( image.pixelColor( nX, nY ).alpha() == 0 ) imageDestination.setPixelColor( nX, nY, image.pixelColor( nX, nY ) );
            }                                                                                                                       
        }                                                                                                                           

        pPainter->drawImage( r, imageDestination );
        pPainter->setRenderHint( QPainter::Antialiasing, false );               // probably not relevant for drawImage but will not hurt
        pPainter->setCompositionMode( nCompositionMode );
    }
    // src
    pPainter->drawImage( r, image );
}

void PAPasteRectangle::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );
    r.setTopLeft( point );
    r.setSize( image.size() );
    nState = StateDraw;
    update();
    emit signalChanged();
}

void PAPasteRectangle::slotRefreshPaste()
{
    nCompositionMode = (QPainter::CompositionMode)pSettingPaste->property( "nCompositionMode" ).toInt();
    bStamp = pSettingPaste->property( "bStamp" ).toInt();
}

