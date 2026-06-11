/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRPolyline
#define H_DRPolyline

#include "DRObject.h"

class DRPolyline : public DRObject
{
    Q_OBJECT
public:
    DRPolyline( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRPolyline();

    virtual QPainterPath shape() const;

    virtual QPixmap     getIcon() override;
    virtual QRectF      getRect() const override;
    virtual QPolygonF   getPolygon() const { return polygon; }
    virtual QImage      getImage() override;
    virtual LHandle *   getGrab( const QPointF &, bool bSizing = false ) override;

    virtual void doMoveBy( const QPointF & ) override;
    virtual void doMove( LHandle *, const QPointF & ) override;
    virtual void doDrop( LHandle *, const QPointF &point = QPointF() ) override;

    // DRDraw will call these only during initial drawing to allow points to be appended quickly
    virtual void doPress( const QPointF & );                            /*!< append a point                     */
    virtual void doMove( const QPointF & );                             /*!< update pointMouse                  */
    virtual void doDoubleClick( const QPointF &point = QPointF() );     /*!< append last point or commit/cancel */
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    QPolygonF   polygon;
    bool        bInit = false;  /*!< indicates that we are using doPress for appending points       */
    QPointF     pointMouse;     /*!< used when bInit to allow doDraw to draw proposed line segment  */

    virtual void doDraw( QPainter * ) override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doRemovePoint( LHandle *pHandle );

    virtual bool shouldRemovePoint( LHandle *pHandle );

private:
    void doDump();
};

#endif
