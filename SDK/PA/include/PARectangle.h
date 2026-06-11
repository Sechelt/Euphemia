/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PARectangle
#define H_PARectangle

#include <SSetting.h>

#include "PATool.h"

class PARectangle : public PATool
{
    Q_OBJECT
public:
    PARectangle( PAPaint *pLayer );
    virtual ~PARectangle();

    virtual QRect  getRect() override { return r.normalized(); }
    virtual QImage getCopy() override;

    virtual void doDoubleClick( PMouseEvent *pEvent ) override;
    virtual void doPress( PMouseEvent *pEvent ) override;
    virtual void doMove( PMouseEvent *pEvent ) override; 
    virtual void doRelease( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;

protected:
    SSetting *      pSettingRectangle;
    bool            bBind;
    Qt::SizeMode    nSizeMode;
    qreal           nXRadius;
    qreal           nYRadius;

    QRect r;

    virtual void doPaint( QPainter *, bool bCommit = true );
    virtual void doDrawState( const QPoint & ) override;
    virtual void doManipulateState() override;
    virtual void doIdleState() override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doMoveHandle( const QPoint &pointPos );
    virtual void doSyncHandleTypes();

protected slots:
    void slotRefreshRectangle();
};

#endif
