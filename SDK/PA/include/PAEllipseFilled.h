/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAEllipseFilled
#define H_PAEllipseFilled

#include "PAEllipse.h"

class PAEllipseFilled : public PAEllipse
{
    Q_OBJECT
public:
    PAEllipseFilled( PAPaint *pLayer );

protected:
    void doPaint( QPainter *, bool bCommit = true ) override;
};

#endif
