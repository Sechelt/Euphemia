#ifndef H_DRGroup
#define H_DRGroup

#include "DRRectangleBase.h"

/*!
 * \brief Group. 
 *  
 * Groups the selected objects so that they can be moved and sized together. 
 * The objects can be ungrouped if desired. 
 *  
 * Implementation 
 *  
 * 1.   Adopt the selected objects (we become parent). 
 * 2.   Moving group causes content to be moved (implied). 
 *  
 *      re. lines which are a child of group are not adjusted 
 *      re. auto routed lines connected to a child of group and are NOT a child of group; connected point is adjusted and line is re-routed.
 *      re. other lines connected to a child of group and are NOT a child of group - connected point is adjusted
 *  
 * 3.   Resize group causes pos/size of objects to be altered to effect a scaling of the content. 
 *  
 *      re. auto routed lines which are child of group; both end points are adjusted and then line re-routed.
 *      re. auto routed lines which are NOT child of group but are connected to a child of group; connected point is adjusted then line re-routed.
 *      re. other lines which are child of group; all points are adjusted
 *      re. other lines which are NOT child of group but are connected to a child of group; connected point is adjusted
 *  
 * \author pharvey (9/29/20)
 */
class DRGroup : public DRRectangleBase
{
    Q_OBJECT
public:
    explicit DRGroup( DRDraw *pLayer, const QPointF &pointPos = QPointF() );
    virtual ~DRGroup();

    virtual void setPos( const QPointF &point );      
    virtual void setSize( const QSizeF &size );
    virtual void setSelected( bool b = true );

    virtual QPixmap     getIcon() override;
    virtual QImage      getImage() override;
    virtual DRObject *  getObject( const QString &stringClass, DRObject *pObjectParent = nullptr );

    virtual bool        isGrouped() { return children().count(); }

    // we alter contents here
    virtual bool        doGroup( QList<DRObject*> listObjects );
    virtual void        doUngroup();
    virtual void        doGroupSync();
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

protected:
    virtual void doInitResize();
    virtual void doScale( const QSizeF &sizeFactor );
};

#endif


