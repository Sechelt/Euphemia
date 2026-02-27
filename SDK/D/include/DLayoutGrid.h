#ifndef H_DLayoutGrid
#define H_DLayoutGrid

#include <W.h>

#include "DLayout.h"
#include "DSpacer.h"

/*!
 * \brief Aggregated geometry factors.
 * 
 * \author pharvey (2026-02-19)
 */
class DLayoutGeometry
{
public:
    // set in doLayoutAggregate
    qreal nMaximum  = 0;    ///< lowest in row/col
    qreal nMinimum  = 0;    ///< highest in row/col
    qreal nStretch  = 0;    ///< highest in row/col
    qreal nHint     = 0;    ///< highest in row/col 

    // doUpdateRowHeights and doUpdateColWidths uses above values to set these
    union                 
    {
        qreal nWidth;       ///< use when col
        qreal nHeight;      ///< use when row
    } WH;  
};

/*!
 * \brief Grid layout. 
 *  
 * This manages pos/size (layout) of child objects. All child objects are assumed to be derived from 
 * DRectangleBase. It is a fatal error for others to be children here. 
 * 
 * Content is stored in a vector-of-vector (\sa vectorContents) of cells (\sa DLayoutContent) and this could be empty.
 * The layout will *appear* to have have a single, empty, cell when no content.
 * 
 * INSERT/DELETE
 * 
 * Rows and Columns are automatically added as needed.
 * Rows and Columns are automatically removed when empty. Use a strut or spring to keep an empty row or column.
 * 
 * SIZING
 * 
 * All cells in a col must have same width.
 * All cells in a row must have same height.
 * 
 * Cells
 * 
 * Factors in determining cell size (in order of prefvectorColGeometrieserence);
 *  - hint
 *  - max
 *  - min
 *  - stretch
 *      - This is a modifier that is weighted among other cells in col or row to determine
 *      how much space beyond hint that will be used by each cell. An object in cell will
 *      only use up to max.
 * 
 * An object in a cell can be smaller than cell but can never be larger.
 * A strut object is fixed (hint=min=max and stretch=0).
 * 
 * \author pharvey (8/9/20)
 */
class DLayoutGrid : public DLayout
{
    Q_OBJECT
public:
    explicit DLayoutGrid( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayoutGrid();

    virtual void setSize( const QSizeF &size ) override;

    virtual QPixmap         getIcon() override;
    virtual QString         getFriendlyClassName() override { return tr( "Layout Grid" ); }
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;
    virtual int             getRows() { return vectorContents.size();     }
    virtual int             getCols() { return vectorContents[0].size();  }


    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos ) override;

    virtual bool doEnter( DRectangleBase *p, const QPointF &pointScene ) override;
    virtual bool doOver( DRectangleBase *p, const QPointF &pointScene ) override;
    virtual bool doExit( DRectangleBase *p ) override;
    virtual void doDrag( DRectangleBase *p ) override;
    virtual bool doDrop( DRectangleBase *p, const QPointF &pointScene ) override;

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool doLoad( QDomElement *pdomElem ) override;

    virtual bool isEmpty();
    virtual bool isSingleCell();

public slots:
    virtual void slotChildRemoved( ADObject * );

protected:
    QVector<QVector<DLayoutContent>>       vectorContents;         ///< content                  - <rows<cols>>
    QVector<DLayoutGeometry>            vectorRowGeometries;    ///< aggregated row geometry  - mostly used when layout resized
    QVector<DLayoutGeometry>            vectorColGeometries;    ///< aggregated col geometry  - mostly used when layout resized

    QMultiMap<qreal,int>                mapStretchFactorRows;   ///< <stretch,row>            - group rows by stretch - used when calc row heights during ie layout resize
    QMultiMap<qreal,int>                mapStretchFactorCols;   ///< <stretch,col>            - group cols by stretch - used when calc col widths during ie layout resize

    virtual DLayoutContentIndex getIndex( const QPointF &pointItem );
    virtual DLayoutContentIndex getIndex( DRectangleBase * );
    virtual CBD::EdgeCenters    getEdge( const DLayoutContentIndex &indexContent, const QPointF &pointItem );
    virtual QPointF             getEdgePoint( const DLayoutContentIndex &indexContent, CBD::EdgeCenters nEdge );

    virtual bool doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge );
    virtual bool doSet( DRectangleBase *p, DLayoutContentIndex indexCell );
    virtual void doRemove( DRectangleBase *p );
    virtual void doLayout();
    virtual void doInitLayout();

    virtual void doUpdateRowHeights();
    virtual void doShrinkRowHeights( qreal nAdjust, bool bStretch );
    virtual void doExpandRowHeights( qreal nAdjust, bool bStretch );

    virtual void doUpdateColWidths();
    virtual void doShrinkColWidths( qreal nAdjust, bool bStretch );
    virtual void doExpandColWidths( qreal nAdjust, bool bStretch );

    // we automatically remove empty rows/cols (except when last cell)
    virtual bool isRowEmpty( int nRow );
    virtual bool isColEmpty( int nCol );
    virtual bool doRowDelete( int nRow );
    virtual bool doColDelete( int nCol );
    virtual bool doRowInsert( int nRow /* 0=prepend size()=append */ );
    virtual bool doColInsert( int nCol /* 0=prepend size()=append */ );

private:
    void doPrintRects();
};
 
#endif


