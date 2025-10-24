#ifndef H_DLayout
#define H_DLayout

#include "DRectangleBase.h"

/*!
 * \brief Base class for all diagram layouts.
 *  
 * This is the base class for all layouts. 
 * Layouts manage pos/size of child objects (content). 
 * Layouts take ownership of the objects.
 *  
 * \sa DGroup 
 * \sa DLayoutLinear 
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

    // The scene will call these. Consider them as 'events'. 
    virtual bool doEnter( DRectangleBase *p, const QPointF &pointScene ) = 0;
    virtual bool doOver( DRectangleBase *p, const QPointF &pointScene ) = 0;
    virtual bool doExit( DRectangleBase *p ) = 0;
    virtual void doDrag( DRectangleBase *p ) = 0;
    virtual bool doDrop( DRectangleBase *p, const QPointF &pointScene ) = 0;

    virtual void doGroupSync();

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

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesDelete();
    virtual void doHandlesSync();

    virtual void doLayout() = 0;
    virtual void doInitResize();
    virtual void doUpdateSelf() = 0;
};

#endif


