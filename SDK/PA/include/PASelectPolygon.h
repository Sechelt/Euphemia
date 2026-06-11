/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PASelectPolygon
#define H_PASelectPolygon

#include "PAPolygon.h"

class PASelectPolygon : public PAPolygon
{
    Q_OBJECT
public:
    PASelectPolygon( PAPaint *pLayer );

    virtual QImage getCopy() override;

    virtual void doDoubleClick( PMouseEvent * ) override;
    virtual void doCommit() override;
    virtual void doCut() override;
    virtual void doDelete() override;

    virtual bool canCommit() override   { return false; }
    virtual bool canCut() override      { return ( nState == StateManipulate ); }
    virtual bool canDelete() override   { return ( nState == StateManipulate ); }

    virtual bool isSelector() override { return true; }

protected:
    virtual QImage getMask() override;

    virtual void doPaint( QPainter *, bool bCommit = true ) override;

};

#endif
