/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPolyline
#define H_PAPolyline

#include "PATool.h"

class PAPolyline : public PATool
{
    Q_OBJECT
public:
    PAPolyline( PAPaint *pLayer );
    virtual ~PAPolyline();

    virtual QRect  getRect() override { return polygon.boundingRect(); }
    virtual QImage getCopy() override;

    virtual void doDoubleClick( PMouseEvent * ) override;
    virtual void doPress( PMouseEvent * ) override;
    virtual void doMove( PMouseEvent * ) override; 
    virtual void doRelease( PMouseEvent * ) override;
    virtual void doCommit() override;

protected:
    QPolygon    polygon;
    QPoint      pointMouse;     /*!< when moving in draw state we have a temp line following the mouse from the last polygon point */

    virtual void doPaint( QPainter *, bool bCommit = true );
    virtual void doDrawState( const QPoint & ) override;
    virtual void doManipulateState() override;
    virtual void doIdleState() override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doMoveHandle( const QPoint &pointPos );
    virtual void doRemovePoint();

    virtual bool shouldRemovePoint();

private:
    void doDump();
};

#endif
