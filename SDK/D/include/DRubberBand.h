#ifndef H_DRubberBand
#define H_DRubberBand

#include "DHandle.h"

class DRubberBand : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit DRubberBand();
    virtual ~DRubberBand();

    virtual void setPos( const QPointF &pointScene );

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

    virtual void mouseMoveHandle( QGraphicsSceneMouseEvent *pEvent );
    virtual void mouseReleaseHandle( QGraphicsSceneMouseEvent *pEvent );

protected:
    QRectF  rect;
    DHandle *pHandle;

    virtual void doHandleMove( const QPointF &pointScenePos );
};

#endif


