/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRRectangleBase
#define H_DRRectangleBase

#include <SSetting.h>

#include "DRObject.h"

class DRRectangleBase : public DRObject
{
    Q_OBJECT
public:
    DRRectangleBase( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRRectangleBase();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    virtual void setRect( const QRectF & );

    virtual QRectF      getRect() const override { return r.normalized(); }
    virtual QImage      getImage() override;
    virtual LHandle *   getGrab( const QPointF &pointPos, bool bSizing = false ) override;

    virtual void        doMoveBy( const QPointF & ) override;
    virtual void        doMove( LHandle *, const QPointF & ) override;
    virtual void        doDrop( LHandle *, const QPointF &point = QPointF() ) override;
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    SSetting *      pSettingRectangle;
    bool            bBind       = false;    /*!< bind x and y (ensure we have a square)     */

    QRectF r;

    // index of handles
    int nIndexHandleMove    = 0;
    int nIndexHandleBegin   = 1;
    int nIndexHandleEnd     = 2;

    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    virtual void slotRefreshRectangle();
};

#endif
