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

    virtual void setTransparentHandles( bool b = true, LHandle *pHandleGrabbed = nullptr ) override;

    virtual QPixmap     getIcon() override;
    virtual QRectF      getRect() const override;
    virtual QPolygonF   getPolygon() const { return polygon; }
    virtual QImage      getImage() override;
    virtual LHandle *   getGrab( const QPointF &, bool bSizing = false ) override;  /*!< manipulating - get handle                      */

    virtual void doMoveBy( const QPointF & ) override;                              /*!< manipulating - move object                     */
    virtual void doMove( LHandle *, const QPointF & ) override;                     /*!< manipulating - move handle                     */
    virtual void doDrop( LHandle *, const QPointF &point = QPointF() ) override;    /*!< manipulating - drop handle                     */

    // DRDraw will call these only during initial drawing to allow points to be appended quickly
    virtual void doPress( const QPointF & );                                        /*!< drawing - append a point                       */
    virtual void doMove( const QPointF & );                                         /*!< drawing - update pointMouse                    */
    virtual void doDoubleClick( const QPointF &point = QPointF() );                 /*!< drawing - append last point or commit/cancel   */

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    QPolygonF   polygon;
    bool        bInit = false;  /*!< indicates that we are using doPress for appending points       */
    QPointF     pointMouse;     /*!< used when bInit to allow doDraw to draw proposed line segment  */

    int nIndexHandleMove = 0;   /*!< first handle is a move handle */

    virtual void doDraw( QPainter * ) override;
    virtual void doCreateHandlesStandard() override;
    virtual void doSyncHandlesStandard() override;
    virtual void doRemovePoint( LHandle *pHandle );

    virtual bool shouldRemovePoint( LHandle *pHandle );

private:
    void doDump();
};

#endif
