#ifndef H_DLayoutGrid
#define H_DLayoutGrid

#include <W.h>

#include "DLayout.h"

/*!
 * \brief Grid layout. 
 *  
 * This manages pos/size (layout) of child objects. All child objects are assumed to be derived from 
 * DRectangleBase. It is a fatal error for others to be children here. 
 *  
 * \author pharvey (8/9/20)
 */
class DLayoutGrid : public DLayout
{
    Q_OBJECT
public:
    explicit DLayoutGrid( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLayoutGrid();
 
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Layout Grid" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

    virtual bool doEnter( DRectangleBase *p, const QPointF &pointScene );
    virtual bool doOver( DRectangleBase *p, const QPointF &pointScene );
    virtual bool doExit( DRectangleBase *p );
    virtual void doDrag( DRectangleBase *p );
    virtual bool doDrop( DRectangleBase *p, const QPointF &pointScene );

protected:
    virtual void doLayout();
    virtual void doInsert( DRectangleBase *p, int nX, int nY );
    virtual void doRemove( DRectangleBase *p );
    virtual void doUpdateSelf();
};
 
#endif


