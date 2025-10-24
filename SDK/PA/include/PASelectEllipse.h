/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PASelectEllipse
#define H_PASelectEllipse

#include "PASelectRectangle.h"

class PASelectEllipse : public PASelectRectangle
{
    Q_OBJECT
public:
    PASelectEllipse( PAPaint *pLayer );

    virtual QImage getCopy() override;

    virtual void  doCut() override;
    virtual void  doCopy() override;
    virtual void  doDelete() override;

protected:
    virtual QImage getMask() override;

    virtual void doPaint( QPainter *, bool bCommit = true ) override;
};

#endif
