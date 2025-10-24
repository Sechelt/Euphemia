/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
