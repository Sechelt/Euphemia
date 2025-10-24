#ifndef H_DGraphicsProxyItem
#define H_DGraphicsProxyItem

#include "DObject.h"

/*!
 * \class DGraphicsProxyItem 
 * \brief Represents as DObject on a QGraphicsScene.  
 *  
 * Abstract base class for proxy. 
 *  
 * It is assumed that the Qt graphics class we are working with inherits QGraphicsObject. 
 * In other words - the object created by calling \sa doCreateGraphicsObject() creates 
 * an object which inherits QGraphicsObject. 
 *  
 * This proxy provides some abstraction between QGraphics* and D* classes. 
 * It was originally introduced to help port from Qt3 to Qt4 - not sure 
 * if this abstraction (which is only partial) is useful moving forward.
 *  
 * \author pharvey (2/16/20)
 */
class DGraphicsProxyItem
{
public:
    explicit DGraphicsProxyItem( DObject *p );
    virtual ~DGraphicsProxyItem();

    virtual void setScene( QGraphicsScene *pScene );
    virtual void setVisible( bool b );
    virtual void setPos( const QPointF &point );
    virtual void setGeometry( const QRectF rect ) = 0;
    virtual void setTransform( const QTransform &t, bool b = false );

    virtual QPointF             pos();
    virtual QRectF              geometry() = 0;
    virtual QSizeF              size() = 0;
    virtual QGraphicsScene *    scene();
    virtual DObject *           getObject();
    virtual QGraphicsObject *   getItem();
    virtual QPointF             mapToParent(const QPointF &point);
    virtual QPointF             mapFromParent(const QPointF &point);
    virtual QPointF             mapToScene(const QPointF &point);
    virtual QPolygonF           mapToScene( const QRectF &rect );
    virtual QPolygonF           mapToScene( const QPolygonF &polygon );
    virtual QRectF              mapRectToScene( const QRectF &rect );
    virtual QRectF              mapRectFromScene( const QRectF &rect ) { return mapFromScene( rect ).boundingRect(); }
    virtual QPointF             mapFromScene( const QPointF &point );
    virtual QPolygonF           mapFromScene( const QRectF &rect );
    virtual QPolygonF           mapFromScene( const QPolygonF &polygon );
    virtual QTransform          transform();

    virtual void doCreateGraphicsObject( QGraphicsItem *pItemParent = nullptr ) = 0;
    virtual void doToBack();
    virtual void doToFront();
    virtual void doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBefore( DObject *pObject );
    virtual void doToAfter( DObject *pObject );
    virtual void doReparent( ADObject *pObject );
    virtual void doPrepareGeometryChange() = 0; // to expose a protected method

    virtual void update();
    virtual void resize( const QSizeF size ) = 0;
    virtual QRectF boundingRect() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    virtual QPainterPath shape() const;

protected:
    DObject *           pObject;
    QGraphicsObject *   pGraphicsObject;
};

/*!
 * \brief Extends DGraphicsProxyItem.
 *  
 * This implements doCreateGraphicsObject() to create a \sa DGraphicsObject. 
 *  
 * \author pharvey (9/16/20)
 */
class DGraphicsProxyObject : public DGraphicsProxyItem
{
public:
    explicit DGraphicsProxyObject( DObject *p );

    virtual void setGeometry( const QRectF ) { qWarning( "DGraphicsProxyObject::setGeometry() not supported" ); }

    virtual QRectF      geometry() { qWarning( "DGraphicsProxyObject::geometry() not supported" ); return QRectF(); }
    virtual QSizeF      size() { qWarning( "DGraphicsProxyObject::size() not supported" ); return QSizeF(); }

    virtual void doCreateGraphicsObject( QGraphicsItem *pItemParent = nullptr );
    virtual void doPrepareGeometryChange();

    virtual void resize( const QSizeF ) { qWarning( "DGraphicsProxyObject::resize() not supported" ); }
};

/*!
 * \brief Extends QGraphicsObject. 
 *  
 * We create a two-way interface between \sa DObject and \sa QGraphicsObject. 
 *  
 * \author pharvey (9/16/20)
 */
class DGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    DGraphicsObject( DGraphicsProxyItem *p, QGraphicsItem *pItemParent = nullptr );

    virtual DGraphicsProxyItem *getProxy() { return p; }

    virtual void doPrepareGeometryChange();

    virtual QRectF boundingRect() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    virtual QPainterPath shape() const;

protected:
    DGraphicsProxyItem *p;
};

#endif


