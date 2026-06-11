/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LGraphicsScene.h"
#include "LFile.h"

LGraphicsScene::LGraphicsScene( LFile *p, const QRectF &r, QObject *pObject )
    : QGraphicsScene( r, pObject )
{
    Q_ASSERT( p );
    pFile = p;

    connect( this, SIGNAL(signalPos(const QPointF&)), pFile, SLOT(slotPos(const QPointF&)) );
}

LGraphicsScene::~LGraphicsScene()
{
}

void LGraphicsScene::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    emit signalPos( pEvent->scenePos() );

    pFile->doMousePressEvent( pEvent );
//    QGraphicsScene::mousePressEvent( pMouseEvent );
}

void LGraphicsScene::mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
    emit signalPos( pEvent->scenePos() );

    pFile->doMouseMoveEvent( pEvent );
//    QGraphicsScene::mouseMoveEvent( pMouseEvent );
}

void LGraphicsScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
    emit signalPos( pEvent->scenePos() );

    pFile->doMouseReleaseEvent( pEvent );
//    QGraphicsScene::mouseReleaseEvent( pMouseEvent );
}

void LGraphicsScene::dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pFile->doDragEnterEvent( pEvent );
}

void LGraphicsScene::dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pFile->doDragMoveEvent( pEvent );
}

void LGraphicsScene::dropEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pFile->doDropEvent( pEvent );
}

void LGraphicsScene::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
    pFile->doContextMenuEvent( pEvent );
}

void LGraphicsScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    pFile->doMouseDoubleClickEvent( pEvent );
}

void LGraphicsScene::drawBackground( QPainter *pPainter, const QRectF &rect )
{
   pFile->doDrawBackground( pPainter, rect );
}

void LGraphicsScene::drawForeground( QPainter *pPainter, const QRectF &rect )
{
 // This will not get called if QGraphicsView::drawForeground() is implemented.
   pFile->doDrawForeground( pPainter, rect );
}

