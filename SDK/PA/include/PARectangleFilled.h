/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PARectangleFilled
#define H_PARectangleFilled

#include "PARectangle.h"

class PARectangleFilled : public PARectangle
{
    Q_OBJECT
public:
    PARectangleFilled( PAPaint *pLayer );

protected:
    virtual void doPaint( QPainter *, bool bCommit = true ) override;
};

#endif
