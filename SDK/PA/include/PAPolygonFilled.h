/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPolygonFilled
#define H_PAPolygonFilled

#include <SSetting.h>

#include "PAPolygon.h"

class PAPolygonFilled : public PAPolygon
{
    Q_OBJECT
public:
    PAPolygonFilled( PAPaint *pLayer );

protected:
    SSetting *pSettingPolygonFilled;
    Qt::FillRule nFillRule = Qt::OddEvenFill;

    virtual void doPaint( QPainter *, bool bCommit = true );

protected slots:
    void slotRefreshPolygonFilled();
};

#endif
