#ifndef H_DLayoutLinear
#define H_DLayoutLinear

#include "DLayout.h"
#include "DSpacer.h"

/*!
 * \brief Base class for all linear (single row or single column) layouts.
 *  
 * This manages pos/size (layout) of child objects. All child objects are assumed to be derived from 
 * DRectangleBase. It is a fatal error for others to be children here. 
 *  
 * Content is stored in a vector (\sa vectorContents) of cells (\sa DLayoutContent) and this could be empty.
 * The layout will *appear* to have have a single, empty, cell when no content.
 * 
 * Cells are added to accomodate a new object and removed when the cell is empty.
 * 
 * This is an asbtract base class. 
 *  
 * \sa DLayoutVertical 
 * \sa DLayoutHorizontal 
 *  
 * \author pharvey (8/9/20)
 */
class DLayoutLinear : public DLayout
{
    Q_OBJECT
public:
    explicit DLayoutLinear( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayoutLinear();
 
    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual bool doEnter( DRectangleBase *p, const QPointF &pointScene );
    virtual bool doOver( DRectangleBase *p, const QPointF &pointScene );
    virtual bool doExit( DRectangleBase *p );
    virtual void doDrag( DRectangleBase *p );
    virtual bool doDrop( DRectangleBase *p, const QPointF &pointScene );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    virtual bool isEmpty();
    virtual bool isSingleCell();

public slots:
    virtual void slotChildRemoved( ADObject * );

protected:
    // We manage layout by using a vector of DLayoutContent's. This vector indicates the order of the
    // objects and provides a geometry for each layout cell. The order, as maintained by QObject, of 
    // the children objects and the order of their proxy is of no concern here. It is enough to simply
    // know that all of the objects to be managed are children of the layout.
    //
    // \note    Using the children objects directly by reordering them etc does not work well - we 
    //          end up trying to layout child objects that are about to be deleted or are deleted.
    //
    // The layout contains (0-n) cells. Each cell is in item coords and resides within the layout rect.
    // 
    // Objects are added to this in;
    //      \sa doLoad
    //      \sa doInsert            - to support doDrop
    // Objects are removed from this in;
    //      \sa doRemove            - to support doDrag - sets parent to null - triggering slotChildRemoved
    // Objects are reordered by;
    //      \sa QVector::remove()
    //      \sa QVector::insert()
    // External changes to children are caught via;
    //      \sa slotDeleted         - object is removed from vectorObjects 
    //      \sa slotChildRemoved    - object is removed from vectorObjects
    //  
    QVector<DLayoutContent> vectorContents;

    // We need to have an index of objects, based upon stretch factor, when we aportion space. 
    // This is ordered lowest to highest.
    // This is temp storage - loaded and used in \sa doLayout() and supporting calls.
    // 
    // <stretch,index> - where index is into vectorContents
    QMultiMap<int,int> mapStretchFactors;

    virtual DLayoutContentIndex getIndex( DRectangleBase * );
    virtual DLayoutContentIndex getIndex( const QPointF &pointItem );
    virtual int                 getIndex( const DLayoutContentIndex & ) = 0;

    virtual bool doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge ) = 0;
    virtual bool doSet( DRectangleBase *p, DLayoutContentIndex indexCell );
    virtual void doRemove( DRectangleBase *p );

};
 
#endif


