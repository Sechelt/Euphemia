#ifndef H_DGraphicsSceneProxy
#define H_DGraphicsSceneProxy

#include <QtGui>
#include <CBD.h>

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

class DGraphicsSceneProxy : public QGraphicsScene
{
    Q_OBJECT
public:
    DGraphicsSceneProxy( DDiagram *p, const QRectF &r, QObject *pObject = nullptr );
    ~DGraphicsSceneProxy();

    DDiagram *getDiagram() { return pDiagram; }

protected:
    DDiagram *pDiagram;

    // events
    void mousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void drawBackground( QPainter *painter, const QRectF &rect );
    void dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent );
    void dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent );
    void dropEvent( QGraphicsSceneDragDropEvent *pEvent );
    void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent );
};

#endif


