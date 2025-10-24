#ifndef H_DPanner
#define H_DPanner

#include <QtGui>
#include <CBD.h>

#include "DGraphicsSceneProxy.h"

class DPanner : public QGraphicsView
{
    Q_OBJECT
public:
    DPanner( DGraphicsSceneProxy *pScene, QWidget *pParent );
    ~DPanner();

    QGraphicsView *getView(); // gets the main view

public slots:
    virtual void slotUpdate();

protected:
    bool bPanning;
    QPoint pointLast;

    // events
    void resizeEvent( QResizeEvent *pEvent );
    void mousePressEvent( QMouseEvent *pMouseEvent );
    void mouseMoveEvent( QMouseEvent *pMouseEvent );
    void mouseReleaseEvent( QMouseEvent *pMouseEvent );

    void drawForeground( QPainter *pPainter, const QRectF &rect );
    void drawBackground( QPainter *pPainter, const QRectF &rect );
};

#endif


