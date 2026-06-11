/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
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
