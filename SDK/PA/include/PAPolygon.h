/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPolygon
#define H_PAPolygon

#include "PAPolyline.h"

class PAPolygon : public PAPolyline
{
    Q_OBJECT
public:
    PAPolygon( PAPaint *pLayer );

protected:
    virtual void doPaint( QPainter *, bool bCommit = true ) override;
};

#endif
