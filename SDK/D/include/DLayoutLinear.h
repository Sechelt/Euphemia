#ifndef H_DLayoutLinear
#define H_DLayoutLinear

#include "DLayout.h"

#include "DSpacer.h"

/*!
 * \brief Layout cell. 
 *  
 * A layout can have 0-n cells. Each cell contains a reference to an object (pObject)
 * and a seperate rectangle (rect). 
 *  
 * pObject 
 *  
 * pObject is a reference to an object the layout is managing (a child of the layout). 
 *  
 * rect 
 *  
 * rect is the pos/size of the cell. All cells reside within the layout rect. 
 * The cell rect is larger than pObject when pObject has margins. 
 * The bulk of what a layout does is to calc the cell rect. This is then used to; 
 *  
 * 1. determine the pos/size of the pObject
 * 2. paint the layout 
 * 3. calc pos of an insert indicator 
 *  
 * \author pharvey (9/25/20)
 */
class DLayoutCell
{
public:
    DLayoutCell( DRectangleBase *p = nullptr, QRectF r = QRectF() )
    {
        pObject = p;
        rect = r;
    }

    DRectangleBase *pObject;    // reference to a child object
    QRectF          rect;       // cell pos/size in item coords
};

/*!
 * \brief Base class for all linear diagram layouts.
 *  
 * This manages pos/size (layout) of child objects. All child objects are assumed to be derived from 
 * DRectangleBase. It is a fatal error for others to be children here. 
 *  
 * All linear layouts work with a vector of DLayoutCell's. 
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

public slots:
    virtual void slotDeleted( ADObject * );
    virtual void slotChildRemoved( ADObject * );

protected:
    // We manage layout by using a vector of DLayoutCell's. This vector indicates the order of the
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
    QVector<DLayoutCell> vectorContents;

    // We need to have an index of objects, based upon stretch factor, when we aportion space. 
    // This is ordered lowest to highest.
    // This is temp storage - loaded and used in \sa doLayout() and supporting calls.
    // 
    // <stretch,index> - where index is into vectorContents
    QMultiMap<int,int> mapStretchFactors;

    virtual QPointF     getEdge( const QPointF &pointScene ) = 0;
    virtual int         indexOf( const QPointF &pointScene ) = 0;
    virtual int         indexOf( DRectangleBase * );

    virtual void doInsert( DRectangleBase *p, int nIndex );
    virtual void doRemove( DRectangleBase *p );
};
 
#endif


