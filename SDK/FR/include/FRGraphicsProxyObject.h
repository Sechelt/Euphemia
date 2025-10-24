#ifndef H_FRGraphicsItemProxy
#define H_FRGraphicsItemProxy

#include <DGraphicsProxyItem.h>

#include "FRObject.h"

/*!
 * \brief Extends DGraphicsProxyItem.
 *  
 * This implements doCreateGraphicsObject() to create a \sa FRGraphicsObject. 
 *  
 * \author pharvey (9/16/20)
 */
class FRGraphicsProxyObject : public DGraphicsProxyItem
{
public:
    explicit FRGraphicsProxyObject( FRObject *p );

    virtual void setWidget( QWidget *p );
    virtual void setGeometry( const QRectF rect );

    virtual QWidget *   widget();
    virtual QRectF      geometry();
    virtual QSizeF      size();

    virtual void doCreateGraphicsObject( QGraphicsItem *pItemParent = nullptr );
    virtual void doPrepareGeometryChange();

    virtual void resize( const QSizeF size );
};

/*!
 * \brief Wrapper for QGraphicsProxyWidget. 
 *  
 * QGraphicsProxyWidget allows us to introduce a QWidget to the scene. 
 *  
 * \author pharvey (7/20/20)
 */
class FRGraphicsObject : public QGraphicsProxyWidget
{
    Q_OBJECT
public:
    FRGraphicsObject( FRGraphicsProxyObject *p, QGraphicsItem *pItemParent = nullptr );

    virtual FRGraphicsProxyObject *getProxy() { return p; }

    // to expose protected method
    virtual void doPrepareGeometryChange();

protected:
    FRGraphicsProxyObject *p;
};


#endif


