#ifndef H_DLayout
#define H_DLayout

#include "DRectangleBase.h"

/*!
 * \brief Layout content. 
 *  
 * A layout can have 0-n cells. Each cell contains a reference to an object (pObject)
 * and a seperate rectangle (rect). 
 *  
 * pObject 
 *  
 *      This is a reference to an object the layout is managing (a child of the layout). 
 *  
 * rect 
 *  
 *      This is the pos/size of the cell in item (layout) coordinates.
 *      This always lies within the layout.
 *      This contains 0-1 pObject.
 *      The pObject may be smaller than this to account for margins.
 * 
 * \author pharvey (9/25/20)
 */
class DLayoutContent
{
public:
    DLayoutContent( DRectangleBase *p = nullptr, QRectF r = QRectF() )
    {
        pObject = p;
        rect = r;
    }

    DRectangleBase *pObject = nullptr;  ///< reference to a child object
    QRectF          rect;               ///< cell pos/size in item coords ( pObject 'floats' within this rect)
};

/*!
 * \brief Content index.
 * 
 * DLayoutHorizontal will use nCol (nRow will always be 0).
 * DLayoutVertical will use nRow (nCol will always be 0).
 * DLayoutGrid will use nRow and nCol.
 * 
 * \note Was using QPoint but this is more intuitive.
 * 
 * \author pharvey (2026-02-19)
 */
class DLayoutContentIndex
{
public:
    DLayoutContentIndex( int nRow = -1, int nCol = -1 )
    {
        this->nRow = nRow;
        this->nCol = nCol;
    }

    int nRow = -1;
    int nCol = -1;

    bool isNull() { return nRow < 0 || nCol < 0; } 
};

/*!
 * \brief Base class for all diagram layouts.
 *  
 * This is the base class for all layouts. 
 * Layouts manage pos/size of child objects (content). 
 * Layouts take ownership of the objects.
 *  
 * \sa DGroup 
 * \sa DLayoutLinear 
 * \sa DLayoutHorizontal 
 * \sa DLayoutVertical 
 * \sa DLayoutGrid 
 *  
 * \author pharvey (8/9/20)
 */
class DLayout : public DRectangleBase
{
    Q_OBJECT
public:
    explicit DLayout( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayout();

    virtual void setPos( const QPointF &point );      
    virtual void setSize( const QSizeF &size );
    virtual void setSelected( bool b = true );

    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );

    // The scene will call these. Consider them as 'events'. 
    virtual bool doEnter( DRectangleBase *p, const QPointF &pointScene ) = 0;
    virtual bool doOver( DRectangleBase *p, const QPointF &pointScene ) = 0;
    virtual bool doExit( DRectangleBase *p ) = 0;
    virtual void doDrag( DRectangleBase *p ) = 0;
    virtual bool doDrop( DRectangleBase *p, const QPointF &pointScene ) = 0;

    virtual void doGroupSync();

    virtual bool isEmpty() = 0;
    virtual bool isSingleCell() = 0;

public slots:
    // intercept these controller requests
    virtual void slotSizeHint( const QSizeF & );
    virtual void slotSizeConstraints( const QSizeF &, const QSizeF & );
    // catch signals that tell us an object as been removed
    virtual void slotDeleted( ADObject * );             // deleted
    virtual void slotChildRemoved( ADObject * );        // reparented
    // catch signal that one of our objects has changed their layout info
    virtual void slotChangedContent();

protected:
    DHandle *pHandleInsert;

    virtual DLayoutContentIndex getIndex( const QPointF &pointItem ) = 0;
    virtual DLayoutContentIndex getIndex( DRectangleBase * ) = 0;
    virtual CBD::EdgeCenters    getEdge( const DLayoutContentIndex &indexCell, const QPointF &pointItem ) = 0;
    virtual QPointF             getEdgePoint( const DLayoutContentIndex &indexCell, CBD::EdgeCenters nEdge ) = 0;

    virtual bool doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge ) = 0;
    virtual bool doSet( DRectangleBase *p, DLayoutContentIndex indexCell ) = 0;
    virtual void doRemove( DRectangleBase *p ) = 0;

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesDelete();
    virtual void doHandlesSync();

    virtual void doLayout() = 0;
    virtual void doInitResize();
    virtual void doInitLayout() = 0;
};

#endif


