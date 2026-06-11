/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LPixelsWidget.h"

#include <SSettings.h>

#include "LFile.h"

LPixelsWidget::LPixelsWidget( QWidget *pParent )
    : QWidget( pParent )
{
    pSettingPen             = g_SSettings->getSetting( "SPen" ); Q_ASSERT( pSettingPen );
    pSettingBackground      = g_SSettings->getSetting( "LSBackground" ); Q_ASSERT( pSettingBackground );

    slotRefreshPen();
    slotRefreshBackground();

    setMinimumSize( 100, 100 );

    connect( pSettingPen, SIGNAL(signalChanged()), SLOT(slotRefreshPen()) );
    connect( pSettingBackground, SIGNAL(signalChanged()), SLOT(slotRefreshBackground()) );
}

void LPixelsWidget::setLayers( LFile *p )
{
    if ( pLayers ) doDisconnectLayers();
    if ( p ) doConnectLayers( p );
}

void LPixelsWidget::mousePressEvent( QMouseEvent *pEvent )
{
    if ( !pImage ) return;
    LLayer *pLayer = pLayers->getLayerCurrent();
     
    // init
    doCalcMatrix();
    if ( pointTopLeft.isNull() ) return pEvent->ignore();

    // get cell
    QPoint pointCell = mapWidgetToCell( pEvent->pos() );
    if ( pointCell.x() >= nXCells ) return pEvent->ignore();
    if ( pointCell.y() >= nYCells ) return pEvent->ignore();

    // get point in scene/image
    QPoint pointScene = mapCellToScene( pointCell );
    if ( pointScene.x() >= pImage->width() ) return pEvent->ignore();
    if ( pointScene.y() >= pImage->height() ) return pEvent->ignore();

    // add to undo?
    if ( bPushUndo )
    {
        pLayer->doPushUndo();
        bPushUndo = false;
    }

    // do it
    pImage->setPixelColor( pointScene, pen.color() );

    // basically we are doing a commit here
    pLayers->setModified();
    pLayers->getGraphics()->update();

    // fini
    update();
}

void LPixelsWidget::mouseMoveEvent( QMouseEvent *pEvent )
{
    if ( !pImage ) return;
    if ( pointTopLeft.isNull() ) return pEvent->ignore();

    // get cell
    QPoint pointCell = mapWidgetToCell( pEvent->pos() );
    if ( pointCell.x() >= nXCells ) return pEvent->ignore();
    if ( pointCell.y() >= nYCells ) return pEvent->ignore();

    // get point in scene/image
    QPoint pointScene = mapCellToScene( pointCell );
    if ( pointScene.x() >= pImage->width() ) return pEvent->ignore();
    if ( pointScene.y() >= pImage->height() ) return pEvent->ignore();

    // do it
    pImage->setPixelColor( pointScene, pen.color() );

    // fini
    pLayers->getGraphics()->update();
    update();
}

void LPixelsWidget::mouseReleaseEvent( QMouseEvent *pEvent )
{
    if ( !pImage ) return;
    if ( pointTopLeft.isNull() ) return pEvent->ignore();
}

void LPixelsWidget::paintEvent( QPaintEvent *pEvent )
{
    if ( !pImage ) return;
    // init
    doCalcMatrix();
    if ( pointTopLeft.isNull() ) return pEvent->ignore();

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );

    QPen pen( Qt::white );
    painter.setPen( pen );

    for ( int nXCell = 0; nXCell < nXCells; nXCell++ )
    {
        for ( int nYCell = 0; nYCell < nYCells; nYCell++ )
        {
            QColor color = pImage->pixelColor( nXImage + nXCell, nYImage + nYCell );
            if ( color.alpha() > 0 && color != Qt::transparent )
                painter.setBrush( color );
            else 
                painter.setBrush( brushTransparency );
            painter.drawRect( nXCell * nSize, nYCell * nSize, nSize, nSize );
            painter.setBrush( QBrush() );

        }
    }
}

void LPixelsWidget::wheelEvent( QWheelEvent *pEvent )
{
    if ( !pImage ) return;
    if ( pointTopLeft.isNull() ) return pEvent->ignore();

    QPoint pointPixels  = pEvent->pixelDelta();
    QPoint pointDegrees = pEvent->angleDelta() / 8;

    if ( !pointPixels.isNull() ) 
    {
        nSize += pointPixels.y() * 2; 
        if ( nSize > 48 ) nSize = 48;
        if ( nSize < 16 ) nSize = 16;
    } 
    else if ( !pointDegrees.isNull() ) 
    {
        QPoint pointSteps = pointDegrees / 15;
        nSize += pointSteps.y() * 2; 
        if ( nSize > 48 ) nSize = 48;
        if ( nSize < 16 ) nSize = 16;
    }

    pEvent->accept();
    update();
}

void LPixelsWidget::doCalcMatrix()
{
    // no active ToolMagnifierSelection shape?
    if ( pointTopLeft.isNull() )
    {
        nXImage = 0;
        nYImage = 0;
        nXCells = 0;
        nYCells = 0;
        return;
    }

    // do it
    nXImage = pointTopLeft.x();
    nYImage = pointTopLeft.y();
    nXCells = rect().width() / nSize;
    nYCells = rect().height() / nSize;

    if ( nXCells > pImage->width() - nXImage ) nXCells = pImage->width() - nXImage;
    if ( nYCells > pImage->height() - nYImage ) nYCells = pImage->height() - nYImage;
}

QPoint LPixelsWidget::mapCellToScene( const QPoint &pointCell )
{
    // can be out of range so caller should check
    return QPoint( nXImage + pointCell.x(), nYImage + pointCell.y() );
}

QPoint LPixelsWidget::mapWidgetToCell( const QPoint &pointWidget )
{
    // can be out of range so caller should check
    return QPoint( pointWidget.x() / nSize, pointWidget.y() / nSize );
}

void LPixelsWidget::doConnectLayers( LFile *p )
{
    Q_ASSERT( p );
    Q_ASSERT( !pLayers );

    pLayers = p;
    connect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrent()) );
    if ( pLayers->getLayerCurrent() ) doConnectLayer( pLayers->getLayerCurrent() );
}

void LPixelsWidget::doDisconnectLayers()
{
    Q_ASSERT( pLayers );

    disconnect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrent()) );
    if ( pLayer ) doDisconnectLayer();
    pLayers = nullptr;
}

void LPixelsWidget::doConnectLayer( LLayer *p )
{
    Q_ASSERT( pLayers );
    Q_ASSERT( p );
    Q_ASSERT( !pLayer );

    pLayer = p;
    pImage = pLayer->getImageRef();
    connect( pLayer, SIGNAL(signalPixelRefreshImage()), this, SLOT(slotRefreshImage()) );
    connect( pLayer, SIGNAL(signalPixelSelectionMoved(const QPoint &)), this, SLOT(slotSelectionMoved(const QPoint &)) );

    update();
}

void LPixelsWidget::doDisconnectLayer()
{
    Q_ASSERT( pLayer );

    // catch case where pLayer was deleted
    if ( pLayers->hasLayer( pLayer ) )
    {
        disconnect( pLayer, SIGNAL( signalPixelRefreshImage() ), this, SLOT( slotRefreshImage() ) );
        disconnect( pLayer, SIGNAL(signalPixelSelectionMoved(const QPoint &)), this, SLOT(slotSelectionMoved(const QPoint &)) );
    }

    pLayer          = nullptr;
    pImage          = nullptr;
    pointTopLeft    = QPoint();
    bPushUndo       = false;

    update();
}

void LPixelsWidget::slotRefreshPen()
{
    pen = pSettingPen->property( "pen" ).value<QPen>();
}

void LPixelsWidget::slotRefreshBackground()
{
    brushTransparency = pSettingBackground->property( "brushTransparency" ).value<QBrush>();
}

void LPixelsWidget::slotSelectionMoved( const QPoint &pointTopLeft )
{
    if ( !bPushUndo )
    {
        // No need to call some form of 'commit' as we have done the steps at each pixel change
        // but lets call precommit if a pixel is changed after this \sa mousePressEvent.
        // In this way we group the undo/redo based upon this slot being called.
        // by doing things this way we also avoid dealing with a dangling precommit when file or layer changed.
        bPushUndo = true;
    }
    this->pointTopLeft = pointTopLeft;
    update();
}

/*!
 * \brief Replace our image. 
 *  
 * Presumably the original has changed due to something like 
 * an undo or redo. 
 *  
 * Does not handle a case where image size has changed. 
 *  
 * \author pharvey (4/18/23)
 */
void LPixelsWidget::slotRefreshImage()
{
    Q_ASSERT( pLayers );
    LLayer *pLayer = pLayers->getLayerCurrent();
    Q_ASSERT( pLayer );
    pImage = pLayer->getImageRef();
    bPushUndo = true;
    update();
}

/*!
 * \brief Handle case where current layer has changed.
 * 
 * \author pharvey (4/18/23)
 */
void LPixelsWidget::slotChangedCurrent()
{
    Q_ASSERT( pLayers );

    if ( pLayer ) doDisconnectLayer();
    if ( pLayers->getLayerCurrent() ) doConnectLayer( pLayers->getLayerCurrent() );
}


