#ifndef H_DLayoutVertical
#define H_DLayoutVertical

#include "DLayoutLinear.h"

/*!
 * \brief Manages layout in a single column of cells.
 * 
 * \sa DLayoutHorizontal
 * \sa DLayout
 * \author pharvey (2026-02-19)
 */
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
    virtual CBD::EdgeCenters    getEdge( const DLayoutContentIndex &indexContent, const QPointF &pointItem );
    virtual QPointF             getEdgePoint( const DLayoutContentIndex &indexContent, CBD::EdgeCenters nEdge );

    virtual bool doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge );

    virtual void doLayout();
    virtual void doInitLayout();
    virtual void doUpdateCellHeights();
    virtual void doShrinkCells( qreal nAdjust, bool bStretch );
    virtual void doExpandCells( qreal nAdjust, bool bStretch );

    virtual int getIndex( const DLayoutContentIndex & );
};
 
#endif


