/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PAPixelSelection
#define H_PAPixelSelection

#include "PATool.h"

class PAPixelSelection : public PATool
{
    Q_OBJECT
public:
    PAPixelSelection( PAPaint *pLayer );
    virtual ~PAPixelSelection();

    virtual QRect  getRect() override { return r.normalized(); }

    virtual void doDoubleClick( PMouseEvent *pEvent ) override;
    virtual void doPress( PMouseEvent *pEvent ) override;
    virtual void doMove( PMouseEvent *pEvent ) override; 
    virtual void doRelease( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;

    virtual bool    canCommit() override { return false; }
    virtual bool    canCut()    override { return false; }
    virtual bool    canCopy()   override { return false; }

signals:
    void signalMoved( const QPoint & );

protected:
    QRect r;

    virtual void doPaint( QPainter *, bool bCommit = true );
    virtual void doDrawState( const QPoint & ) override;
    virtual void doManipulateState() override;
    virtual void doIdleState() override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doMoveHandle( const QPoint &pointPos );
};

#endif
