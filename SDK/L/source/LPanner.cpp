/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LPanner.h"

LPanner::LPanner( QGraphicsScene *pScene, QWidget *pParent )
    : QGraphicsView( pScene, pParent )
{
    bPanning = false;
    setCursor( Qt::OpenHandCursor );

    // we want mouse events so we need interactive
    // setInteractive( false );

    // this is pointless as we always have the entire scene in the view
    // setDragMode( QGraphicsView::ScrollHandDrag );
}

LPanner::~LPanner()
{
}

QGraphicsView *LPanner::getView()
{
    QGraphicsView *pView = nullptr;
    QList<QGraphicsView*> listViews = scene()->views();

    if ( listViews.count() < 2 ) return nullptr;

    // find first one that is not us
    foreach( pView, listViews )
    {
        if ( pView != this ) return pView;
    }
    return nullptr;
}

void LPanner::slotUpdate() 
{ 
   viewport()->update();
}

void LPanner::resizeEvent( QResizeEvent *pEvent )
{
    Q_UNUSED(pEvent);
    // scale the entire scene to fit into our viewport
    fitInView( sceneRect(), Qt::KeepAspectRatio );
}

void LPanner::mousePressEvent( QMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;
    bPanning = true;
    pointLast = pEvent->pos();
    setCursor( Qt::ClosedHandCursor );
}

/*!
 * \brief Scroll main view as we drag on panner.
 *  
 * We (the panner) is scaled so we get the Delta and unscale it. 
 * The main view may be scaled as well so we then scale the Delta to the main view.
 *  
 * \author pharvey (2/21/20)
 * 
 * \param pEvent 
 */
void LPanner::mouseMoveEvent( QMouseEvent *pEvent )
{
    if ( !bPanning ) return;
    QGraphicsView *pView = getView();
    if ( !pView ) return;

    // SCALE (of panner)
    qreal nXScale = transform().m11();
    qreal nYScale = transform().m22();

    // SCALE (of main view)
    qreal nXScaleMain = pView->transform().m11();
    qreal nYScaleMain = pView->transform().m22();
    
    // DELTA (in panner and scaled as per panner)
    qreal nXDeltaScaled = pEvent->pos().x() - pointLast.x();
    qreal nYDeltaScaled = pEvent->pos().y() - pointLast.y();

    // DELTA (in panner unscaled)
    qreal nXDeltaUnscaled = nXDeltaScaled / nXScale;
    qreal nYDeltaUnscaled = nYDeltaScaled / nYScale;

    // DELTA (scaled for main view)
    nXDeltaScaled = nXDeltaUnscaled * nXScaleMain;
    nYDeltaScaled = nYDeltaUnscaled * nYScaleMain;

    // APPLY DELTA (to main view)
    int nX = pView->horizontalScrollBar()->value();
    int nY = pView->verticalScrollBar()->value();

    int nXNew = nX + nXDeltaScaled;
    int nYNew = nY + nYDeltaScaled;

    pView->horizontalScrollBar()->setValue( nXNew );
    pView->verticalScrollBar()->setValue( nYNew );

    // \note this is protected and scroll bars are not 
    // pView->scrollContentsBy( nXDelta, nYDelta );

    pointLast = pEvent->pos();
    slotUpdate();
}

void LPanner::mouseReleaseEvent( QMouseEvent *pEvent )
{
    Q_UNUSED(pEvent);
    bPanning = false;
    setCursor( Qt::OpenHandCursor );
    scene()->update(); // scene will be confused without this update
}

/*!
 * \brief Draw a red box representing the main views viewport over the document.
 * 
 * \author pharvey (2/22/20)
 * 
 * \param pPainter 
 * \param rect 
 */
void LPanner::drawForeground( QPainter *pPainter, const QRectF &rect )
{
    Q_UNUSED(rect);
    // \to draw red rect to show area displayed in primary view
    // Not sure how this will work if there are split views for example.
    // Perhaps allow setView( int nIndex ) to tell us to use a certain view?
    // For now; just use the first view.
    QGraphicsView *pView = getView();
    if ( !pView ) return;

    // SCALE (of main view)
    qreal nXScaleMain = pView->transform().m11();
    qreal nYScaleMain = pView->transform().m22();

    // pPainter will automatically scale to panner but still need to apply scale 
    // from main view 
    QRectF r( pView->horizontalScrollBar()->value() / nXScaleMain,
              pView->verticalScrollBar()->value() / nYScaleMain,
              pView->viewport()->width() / nXScaleMain,
              pView->viewport()->height() / nYScaleMain );


    // yellow solid
    QPen pen( Qt::yellow );
    pen.setWidth( 3 );
    pPainter->setPen( pen );
    pPainter->drawRect( r );
    // red dashline
    pen.setStyle( Qt::DashLine );
    pen.setColor( Qt::red );
    pPainter->setPen( pen );
    pPainter->drawRect( r );
}



