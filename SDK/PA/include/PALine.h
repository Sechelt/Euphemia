/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PALine
#define H_PALine

#include <SSetting.h>
#include <LSLine.h>

#include "PATool.h"

class PALine : public PATool
{
    Q_OBJECT
public:

    PALine( PAPaint *pLayer );
    virtual ~PALine();

    virtual QRect  getRect() override { return QRect( pointBegin, pointEnd ).normalized(); }
    virtual QImage getCopy() override;

    virtual void doDoubleClick( PMouseEvent *pEvent ) override;
    virtual void doPress( PMouseEvent *pEvent ) override;
    virtual void doMove( PMouseEvent *pEvent ) override; 
    virtual void doRelease( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;

protected:
    SSetting *              pSettingLine;
    QPoint                  pointBegin;
    QPoint                  pointEnd;
    LSLine::LineEndStyles   nStyleBegin = LSLine::LineEndNone;
    LSLine::LineEndStyles   nStyleEnd   = LSLine::LineEndNone;

    virtual void doPaint( QPainter *, bool bCommit = true );

    virtual void doDrawLineEnds( QPainter *pPainter );
    virtual void doDrawLineEnd( QPainter *pPainter, LSLine::LineEndStyles nStyle, const QLineF &line );
    virtual void doDrawLineEndArrow( QPainter *pPainter, const QLineF &line );
    virtual void doDrawLineEndBall( QPainter *pPainter, const QLineF &line );

    virtual void doDrawState( const QPoint & ) override;
    virtual void doManipulateState() override;
    virtual void doIdleState() override;

    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doMoveHandle( const QPoint &pointPos );

protected slots:
    virtual void slotRefreshLine();
};

#endif
