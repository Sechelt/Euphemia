#ifndef H_DLayoutVertical
#define H_DLayoutVertical

#include "DLayoutLinear.h"

class DLayoutVertical : public DLayoutLinear
{
    Q_OBJECT
public:
    explicit DLayoutVertical( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayoutVertical();
 
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Layout Vertical" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

protected:
    virtual QPointF getEdge( const QPointF &pointScene );
    virtual int     indexOf( const QPointF &pointScene );

    virtual void doLayout();
    virtual void doUpdateSelf();
    virtual void doUpdateCellHeights();
    virtual void doShrinkCells( qreal nAdjust, bool bStretch );
    virtual void doExpandCells( qreal nAdjust, bool bStretch );
};
 
#endif


