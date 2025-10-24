#include "LibInfo.h"
#include "DGraphicsSceneProxy.h"

DGraphicsSceneProxy::DGraphicsSceneProxy( DDiagram *p, const QRectF &r, QObject *pObject )
    : QGraphicsScene( r, pObject )
{
    Q_ASSERT( p );
    pDiagram = p;
    setBackgroundBrush( Qt::lightGray );
}

DGraphicsSceneProxy::~DGraphicsSceneProxy()
{
}

void DGraphicsSceneProxy::mousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    pDiagram->mousePressEvent( pMouseEvent );
    QGraphicsScene::mousePressEvent( pMouseEvent );
}

void DGraphicsSceneProxy::mouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    pDiagram->mouseMoveEvent( pMouseEvent );
    QGraphicsScene::mouseMoveEvent( pMouseEvent );
}

void DGraphicsSceneProxy::mouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    pDiagram->mouseReleaseEvent( pMouseEvent );
    QGraphicsScene::mouseReleaseEvent( pMouseEvent );
}

void DGraphicsSceneProxy::drawBackground( QPainter *pPainter, const QRectF &rect )
{
    pDiagram->drawBackground( pPainter, rect );
}

void DGraphicsSceneProxy::dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pDiagram->dragEnterEvent( pEvent );
}

void DGraphicsSceneProxy::dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pDiagram->dragMoveEvent( pEvent );
}

void DGraphicsSceneProxy::dropEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    pDiagram->dropEvent( pEvent );
}

void DGraphicsSceneProxy::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
    pDiagram->contextMenuEvent( pEvent );
}

void DGraphicsSceneProxy::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    pDiagram->mouseDoubleClickEvent( pEvent );
}


