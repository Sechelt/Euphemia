/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PATool.h"

#include <SSettings.h>
#include <LGraphicsLayer.h>
#include <LGraphicsScene.h>

#include "PAPaint.h"

PATool::PATool( PAPaint *pLayer )
    : QGraphicsObject( pLayer->getGraphics() )
{
    nState          = StateIdle;

    this->pLayer    = pLayer;
    pLayerGraphics  = pLayer->getGraphics();
    pLayerScene     = (LGraphicsScene*)pLayerGraphics->scene();   
    pLayerView      = pLayerScene->views().first();

    pSettingPen     = g_SSettings->getSetting( "SPen" ); Q_ASSERT( pSettingPen );
    pSettingBrush   = g_SSettings->getSetting( "SBrush" ); Q_ASSERT( pSettingBrush );
    pSettingFont    = g_SSettings->getSetting( "SFont" ); Q_ASSERT( pSettingFont );
    pSettingGeneral = g_SSettings->getSetting( "LSGeneral" ); Q_ASSERT( pSettingGeneral );
    pSettingRender  = g_SSettings->getSetting( "LSRender" ); Q_ASSERT( pSettingRender );

    slotRefreshPen();
    slotRefreshBrush();
    slotRefreshFont();
    slotRefreshGeneral();
    slotRefreshRender();

    connect( pSettingPen, SIGNAL(signalChanged()), SLOT(slotRefreshPen()) );
    connect( pSettingBrush, SIGNAL(signalChanged()), SLOT(slotRefreshBrush()) );
    connect( pSettingFont, SIGNAL(signalChanged()), SLOT(slotRefreshFont()) );
    connect( pSettingGeneral, SIGNAL(signalChanged()), SLOT(slotRefreshGeneral()) );
    connect( pSettingRender, SIGNAL(signalChanged()), SLOT(slotRefreshRender()) );
}

PATool::~PATool()
{
}

/*!
 * \brief Scene wants this to paint itself. 
 *  
 * Ignore if this is StateIdle otherwise strip down the call and pass to doPaint. 
 * 
 * \author pharvey (2/10/23)
 * 
 * \param pPainter 
 * \param nOption  
 * \param pWidget  
 */
void PATool::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    doPaint( pPainter, false );
}

/*!
 * \brief Scene wants to know the geometry. 
 *  
 * We are a temp object which always covers the entire; scene/canvas/image. 
 *  
 * Derived classes should not need to change this. 
 * 
 * \author pharvey (2/10/23)
 * 
 * \return QRectF 
 */
QRectF PATool::boundingRect() const
{
    return pLayerScene->sceneRect();
}

void PATool::doCancel()
{
    if ( nState != StateIdle ) doIdleState();
}

void PATool::doCopy()
{
     QClipboard *clipboard = QGuiApplication::clipboard();
     clipboard->setImage( getCopy() );
}

void PATool::doZoomChanged( int nZoom )
{
    Q_UNUSED( nZoom );
    if ( nState != StateManipulate ) return;
    doSyncHandles();
}

void PATool::doScrollChanged( int nX, int nY )
{
    Q_UNUSED( nX );
    Q_UNUSED( nY );
    if ( nState != StateManipulate ) return;
    doSyncHandles();
}

LHandle *PATool::getHandle( const QPoint &pointPos )
{
    // search in REVERSE order to reflect z-order
    for ( int n = vectorHandles.count() - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        // NOTE: The handles are children of the viewport. 
        // This is done so that they do not get scaled during a zoom.
        QPolygonF polygon = pLayerView->mapToScene( p->geometry() );
        if ( polygon.containsPoint( pointPos, Qt::OddEvenFill ) ) return p; 
    }
    return nullptr;
}

/*!
 * \brief Return a handle of given type that is below the given handle.
 * 
 * \author pharvey (2/20/23)
 * 
 * \param pHandle 
 * \param nType   
 * 
 * \return LHandle* 
 */
LHandle *PATool::getHandleUnder( LHandle *pHandle, LHandle::Type nType )
{
    QPoint point = pHandle->geometry().center();

    // search in REVERSE order to reflect z-order
    for ( int n = vectorHandles.count() - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() != nType ) continue;              // looking for a handle this is the same type
        if ( p == pHandle ) continue;                       // and not given handle
        if ( p->geometry().contains( point ) ) return p;    // and at same pos (roughly)
    }
    return nullptr;
}

LHandle *PATool::getHandleNext( LHandle *pHandle, LHandle::Type nType )
{
    for ( int n = vectorHandles.indexOf( pHandle ) + 1; n < vectorHandles.count(); n++ )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() == nType ) return p;
    }
    return nullptr;
}

LHandle *PATool::getHandlePrev( LHandle *pHandle, LHandle::Type nType )
{
    for ( int n = vectorHandles.indexOf( pHandle ) - 1; n >= 0; n-- )
    {
        LHandle *p = vectorHandles.at( n );
        if ( p->getType() == nType ) return p;
    }
    return nullptr;
}

/*!
 * \brief Returns a trimmed version of a copied image. The trimmed area becomes transparent.
 *  
 * This can trim around an ellipse and around a complex polygon with an appropriate mask. 
 * Scans every pixel in imageCopied. 
 *  
 * r 
 *  
 * This is a rectangle, in canvas coordinates, which corresponds to the area copied. It is used 
 * when bErase is true. 
 *  
 * imageCopied 
 *  
 * This is the image copied from the canvas. A trimmed version of this is returned. 
 * Is the same size as r. 
 *  
 * imageMask 
 *  
 * This is a mask used to determine which pixels to trim. It only has two colors for the pixels - black and white. 
 * The black pixels indicate that the corresponding pixel in imageCopied should be left. 
 * The white pixels indicate that the corresponding pixel in imageCopied should be trimmed (set to transparent). 
 * Is the same size as r. 
 *  
 * bErase 
 *  
 * Indicates whether or not the copied image should be erased from the canvas. 
 * true = erase (set transparent) in canvas where mask has black pixels 
 * false = NoOp 
 *  
 * \author pharvey (2/5/23)
 * 
 * \param r bounding rect of copied image (in canvas coordinates)
 * \param imageCopied image copied from canvas
 * \param imageMask mask used to trim (same size as imageCopied with black being pixels to keep and white the pixels to trim)
 * \param bErase true to erase (where mask pixels are black) from canvas 
 * 
 * \return QImage 
 */
QImage PATool::getTrimmed( const QRect &r, QImage &imageCopied, const QImage &imageMask, bool bErase )
{
    Q_ASSERT( imageCopied.size() == imageMask.size() );

    QColor      colorMask( Qt::black );                         // assume black is for the area we want to keep
    QColor      colorBackground( Qt::transparent );             // assume canvas background is transparent
    QImage *    pImageCanvas    = pLayer->getImageRef();
    int         nWidth          = imageCopied.size().width();
    int         nHeight         = imageCopied.size().height();

    // Seems that creating QColor with Qt::transparent does not properly set alpha channel so...
    colorBackground.setAlpha( 0 );

    for ( int nX = 0; nX < nWidth; nX++ )
    {
        for ( int nY = 0; nY < nHeight; nY++ )
        {
            // trim by setting pixel to mask trim color - probably transparent but definately not colorMask
            if ( imageMask.pixelColor( nX, nY ) != colorMask ) 
                imageCopied.setPixelColor( nX, nY, imageMask.pixelColor( nX, nY ) );
            // not in trim area so consider erasing (ie for a cut operation)
            else if ( bErase ) 
                pImageCanvas->setPixelColor( nX + r.x(), nY + r.y(), colorBackground );
        }
    }

    return imageCopied;
}

void PATool::doDeleteHandles()
{
    pHandle = nullptr;
    while ( !vectorHandles.isEmpty() )
    {
        delete vectorHandles.takeLast();
    }
}

void PATool::doShowHandles( bool b )
{
    for ( LHandle *p : vectorHandles )
    {
        p->setVisible( b );
    }
}

void PATool::slotRefreshPen()
{
    pen = pSettingPen->property( "pen" ).value<QPen>();
    update();
}

void PATool::slotRefreshBrush()
{
    brush = pSettingBrush->property( "brush" ).value<QBrush>();
    update();
}

void PATool::slotRefreshFont()
{
    font = pSettingFont->property( "font" ).value<QFont>();
    update();
}

void PATool::slotRefreshGeneral()
{
    bAutoCommit = pSettingGeneral->property( "bAutoCommit" ).toInt();
}

void PATool::slotRefreshRender()
{
    nAntialiasing           = pSettingRender->property( "nAntialiasing" ).toBool();
    nTextAntialiasing       = pSettingRender->property( "nTextAntialiasing" ).toBool();
    nSmoothPixmapTransform  = pSettingRender->property( "nSmoothPixmapTransform" ).toBool();
    nLosslessImageRendering = pSettingRender->property( "nLosslessImageRendering" ).toBool();
    update();
}

