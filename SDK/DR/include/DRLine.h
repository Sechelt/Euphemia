/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRLine
#define H_DRLine

#include "DRObject.h"

class DRLine : public DRObject
{
    Q_OBJECT
public:
    DRLine( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRLine();

    virtual QPainterPath shape() const;

    virtual QPixmap     getIcon() override;
    virtual QRectF      getRect() const override { return QRectF( pointBegin, pointEnd ).normalized(); }
    virtual QImage      getImage() override;
    virtual LHandle *   getGrab( const QPointF &pointPos, bool bSizing = false ) override;
    QPointF             getBegin()  { return pointBegin;    }
    QPointF             getEnd()    { return pointEnd;      }

    virtual void        doMoveBy( const QPointF & ) override;
    virtual void        doMove( LHandle *, const QPointF & ) override;
    virtual void        doDrop( LHandle *, const QPointF &point = QPointF() ) override;
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    QPointF pointBegin;
    QPointF pointEnd;

    virtual void doDraw( QPainter * ) override;

    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
};

#endif
