/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
