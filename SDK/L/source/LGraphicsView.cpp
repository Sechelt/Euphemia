/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LGraphicsView.h"

#include <SSettings.h>

#include "LFile.h"

LGraphicsView::LGraphicsView( LFile *pFile, QWidget *pParent )
    : QGraphicsView( pFile->getGraphics(), pParent )
{
    this->pFile = pFile;

    pSettingRender = g_SSettings->getSetting( "LSRender" ); Q_ASSERT( pSettingRender );
    slotRefreshRender();
    connect( pSettingRender, SIGNAL(signalChanged()), SLOT(slotRefreshRender()) );

    pSettingPointer = g_SSettings->getSetting( "LSPointer" ); Q_ASSERT( pSettingPointer );
    slotRefreshPointer();
    connect( pSettingPointer, SIGNAL(signalChanged()), SLOT(slotRefreshPointer()) );

    setAlignment( Qt::AlignCenter ); // setAlignment( Qt::AlignLeft | Qt::AlignTop );
    
    // \todo only way to catch this?
    // - when document size changes and not triggering other events because all is still in viewport
    connect( pFile->getGraphics(), SIGNAL(sceneRectChanged(const QRectF &)), SLOT(slotSceneRectChanged(const QRectF &)) );

//    setBackgroundBrush( QBrush( Qt::gray ) );
    centerOn( pFile->getGraphics()->sceneRect().center() );

    // We always have mouse tracking on because we want to provide
    // an ability to report current coordinate as the mouse moves. 
    setMouseTracking( true );
}

QWidget *LGraphicsView::getViewportTop()    
{ 
    return viewport();
}

QPoint LGraphicsView::getTopLeft()
{
    return mapFromScene( sceneRect().topLeft() );
}

QPoint LGraphicsView::getBottomRight()
{
    return mapFromScene( sceneRect().bottomRight() );
}

QPointF LGraphicsView::getScale()
{
    return QPointF( transform().m11(), transform().m22() );
}

/*!
 * \brief The X offset between viewport.topleft and scene.topleft. 
 *  
 * Created to notify the ruler. 
 *  
 * \author pharvey (2/21/20)
 * 
 * \return int 
 */
qreal LGraphicsView::getOffsetH()
{
/*
    // default to case where scene is larger than viewport - we rely on scroll bar values       
    qreal nOffsetH = -horizontalScrollBar()->value();                                           
                                                                                                
    // handle case where entire width is in viewport - we assume scene is centered in viewport  
    qreal nSceneWidth = sceneRect().width() * getScaleH();                                      
    if ( nSceneWidth < viewport()->width() ) nOffsetH = (viewport()->width() - nSceneWidth) / 2;
                                                                                                
    return nOffsetH;                                                                            
*/

    return mapFromScene( sceneRect().topLeft() ).x();
}

/*!
 * \brief Horizontal scale in play. 
 *  
 * m11 = horizontal 
 * m22 = vertical 
 * 
 * \author pharvey (2/25/20)
 * 
 * \return qreal 
 */
qreal LGraphicsView::getScaleH()
{
    return transform().m11();
}

qreal LGraphicsView::getLengthH()
{
    return scene()->width();
}

/*!
 * \brief The Y offset between viewport.topleft and scene.topleft. 
 *  
 * Created to notify the ruler. 
 *  
 * \author pharvey (2/21/20)
 * 
 * \return qreal 
 */
qreal LGraphicsView::getOffsetV()
{
/*
    // default to case where scene is larger than viewport - we rely on scroll bar values           
    qreal nOffsetV = -verticalScrollBar()->value();                                                 
                                                                                                    
    // handle case where entire height is in viewport - we assume scene is centered in viewport     
    qreal nSceneHeight = sceneRect().height() * getScaleV();                                        
    if ( nSceneHeight < viewport()->height() ) nOffsetV = (viewport()->height() - nSceneHeight) / 2;
                                                                                                    
    return nOffsetV;                                                                                
*/

    return mapFromScene( sceneRect().topLeft() ).y();
}

/*!
 * \brief Vertical scale in play. 
 *  
 * m11 = horizontal 
 * m22 = vertical 
 * 
 * \author pharvey (2/25/20)
 * 
 * \return qreal 
 */
qreal LGraphicsView::getScaleV()
{
    return transform().m22();
}

qreal LGraphicsView::getLengthV()
{
    return scene()->height();
}

void LGraphicsView::slotZoomChanged( WZoomWidget::FitTypes nFit, int n )
{
    if ( nFit == nZoomFit && n == nZoom ) return;

    nZoomFit    = nFit;
    nZoom       = n;

    // get scale
    qreal nScale = 1.0;
// qInfo() << scene()->sceneRect();
// qInfo() << viewport()->width() << viewport()->height();
    switch ( nFit )
    {
    case WZoomWidget::FitWidth:
        // if ( scene()->sceneRect().width() <= viewport()->width() ) return;
        nScale = viewport()->width() / scene()->sceneRect().width();
        break;
    case WZoomWidget::FitHeight:
        // if ( scene()->sceneRect().height() <= viewport()->height() ) return;
        nScale = viewport()->height() / scene()->sceneRect().height();
        break;
    case WZoomWidget::FitAll:
        {
            qreal nScaleWidth   = 0;
            qreal nScaleHeight  = 0;
            // if ( scene()->sceneRect().width() > viewport()->width() ) nScaleWidth = viewport()->width() / scene()->sceneRect().width();
            // if ( scene()->sceneRect().height() > viewport()->height() ) nScaleHeight = viewport()->height() / scene()->sceneRect().height();
            nScaleWidth = viewport()->width() / scene()->sceneRect().width();
            nScaleHeight = viewport()->height() / scene()->sceneRect().height();
            nScale = nScaleWidth > nScaleHeight ? nScaleHeight : nScaleWidth;
        }
        break;
    case WZoomWidget::FitIgnore:
        nScale = qreal(nZoom) / 100;
        break;
    }

    if ( nScale == 0 ) return;

    // apply scale
    setScale( nScale );
    pFile->slotZoomChanged( nZoom );

    emit signalZoomChanged( nFit, nZoom );
}

void LGraphicsView::slotSceneRectChanged( const QRectF &r )
{
    Q_UNUSED( r );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
    emit signalChanged( getTopLeft(), getBottomRight(), getScale() );
}

void LGraphicsView::slotRefreshRender()
{
    setRenderHint( QPainter::Antialiasing, pSettingRender->property( "nAntialiasing" ).toBool() );
    setRenderHint( QPainter::TextAntialiasing, pSettingRender->property( "nTextAntialiasing" ).toBool() );
    setRenderHint( QPainter::SmoothPixmapTransform, pSettingRender->property( "nSmoothPixmapTransform" ).toBool() );
    setRenderHint( QPainter::LosslessImageRendering, pSettingRender->property( "nLosslessImageRendering" ).toBool() );
    update();
}

void LGraphicsView::slotRefreshPointer()
{
    setCursor( pSettingPointer->property( "cursor" ).value<QCursor>() );
}

void LGraphicsView::setScale( qreal n )
{
    if ( n == getScale().x() ) return;

    setTransform( QTransform::fromScale( n, n ) );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
    emit signalChanged( getTopLeft(), getBottomRight(), getScale() );
}

void LGraphicsView::resizeEvent( QResizeEvent *pEvent )
{
    QGraphicsView::resizeEvent( pEvent );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
    emit signalChanged( getTopLeft(), getBottomRight(), getScale() );
}

void LGraphicsView::scrollContentsBy( int nX, int nY )
{
    QGraphicsView::scrollContentsBy( nX, nY );
    pFile->slotScrollChanged( getOffsetH(), getOffsetV() );

    emit signalChangedH( getOffsetH(), getScaleH(), getLengthH() );
    emit signalChangedV( getOffsetV(), getScaleV(), getLengthV() );
    emit signalChanged( getTopLeft(), getBottomRight(), getScale() );
}

/*
void LGraphicsView::drawForeground( QPainter *pPainter, const QRectF & )                                            
{                                                                                                                   
    // This behaves exactly the same as QGraphicsScene::drawBackground including                                    
    // scaling its lines etc (they get fat as you zoom in).                                                         
    // QGraphicsScene::drawBackground gets called by default.                                                       
    // Implementing this (or setting our foreground brush) means QGraphicsScene::drawBackground will not get called.
    // Leave this comment and example code for future reference.                                                    
    //                                                                                                              
    int nCellSize = 32;                                                                                             
    int nWidth = scene()->sceneRect().width();                                                                      
    int nHeight = scene()->sceneRect().height();                                                                    
    for ( int nX = 0; nX <= nWidth; nX += nCellSize )                                                               
    {                                                                                                               
        pPainter->drawLine( QPointF( nX, 0 ), QPointF( nX, nHeight ) );                                             
    }                                                                                                               
    for ( int nY = 0; nY <= nHeight; nY += nCellSize )                                                              
    {                                                                                                               
        pPainter->drawLine( QPointF( 0, nY ), QPointF( nWidth, nY ) );                                              
    }                                                                                                               
}                                                                                                                   
*/

