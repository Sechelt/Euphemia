/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAText
#define H_PAText

#include <SSetting.h>

#include "PARectangle.h"

class PAText : public PARectangle
{
    Q_OBJECT
public:
    PAText( PAPaint *pLayer );

    virtual void doRelease( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;

protected:
    SSetting *pSettingText;
    QString stringText;
    Qt::AlignmentFlag nHAlign = Qt::AlignHCenter;
    Qt::AlignmentFlag nVAlign = Qt::AlignVCenter;

    virtual void doPaint( QPainter *, bool bCommit = true );

protected slots:
    void slotRefreshText();
};

#endif
