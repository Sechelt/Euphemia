/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PAEllipse
#define H_PAEllipse

#include "PARectangle.h"

class PAEllipse : public PARectangle
{
    Q_OBJECT
public:
    PAEllipse( PAPaint *pLayer );

protected:
    SSetting *pSettingEllipse;

    virtual void doPaint( QPainter *, bool bCommit = true );


protected slots:
    void slotRefreshEllipse();
};


#endif
