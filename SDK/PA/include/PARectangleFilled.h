/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
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
