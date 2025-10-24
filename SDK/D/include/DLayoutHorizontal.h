#ifndef H_DLayoutHorizontal
#define H_DLayoutHorizontal

#include "DLayoutLinear.h"

class DLayoutHorizontal : public DLayoutLinear
{
    Q_OBJECT
public:
    explicit DLayoutHorizontal( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayoutHorizontal();
 
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Layout Horizontal" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

protected:
    virtual QPointF getEdge( const QPointF &pointScene );
    virtual int     indexOf( const QPointF &pointScene );

    virtual void doLayout();
    virtual void doUpdateSelf();
    virtual void doUpdateCellWidths();
    virtual void doShrinkCells( qreal nAdjust, bool bStretch );
    virtual void doExpandCells( qreal nAdjust, bool bStretch );
};
 
#endif


