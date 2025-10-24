/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PASelectRectangle
#define H_PASelectRectangle

#include "PARectangle.h"

class PASelectRectangle : public PARectangle
{
    Q_OBJECT
public:
    PASelectRectangle( PAPaint *pLayer, bool bAll = false );

    virtual QImage getCopy() override;

    virtual void doRelease( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;
    virtual void doCut() override;
    virtual void doDelete() override;
    virtual void doSelectAll();

    virtual bool canCommit() override   { return false; }
    virtual bool canCut() override      { return ( nState == StateManipulate ); }
    virtual bool canDelete()            { return ( nState == StateManipulate ); }

    virtual bool isSelector() override { return true; }

protected:
    virtual QImage getMask() override;

    virtual void doPaint( QPainter *, bool bCommit = true ) override;
};

#endif
