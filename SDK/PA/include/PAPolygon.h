/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
