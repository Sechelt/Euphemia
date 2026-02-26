#ifndef H_DLayoutHorizontal
#define H_DLayoutHorizontal

#include "DLayoutLinear.h"

/*!
 * \brief Manages layout in a single row of cells.
 * 
 * \sa DLayoutVertical 
 * \sa DLayout
 * \author pharvey (2026-02-19)
 */
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
    virtual CBD::EdgeCenters    getEdge( const DLayoutContentIndex &indexContent, const QPointF &pointItem );
    virtual QPointF             getEdgePoint( const DLayoutContentIndex &indexContent, CBD::EdgeCenters nEdge );

    virtual bool doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge );

    virtual void doLayout();
    virtual void doInitLayout();
    virtual void doUpdateCellWidths();
    virtual void doShrinkCells( qreal nAdjust, bool bStretch );
    virtual void doExpandCells( qreal nAdjust, bool bStretch );

    virtual int getIndex( const DLayoutContentIndex & );
};
 
#endif


