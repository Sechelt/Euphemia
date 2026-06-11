/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DREllipse
#define H_DREllipse

#include "DRRectangleBase.h"

class DREllipse : public DRRectangleBase
{
    Q_OBJECT
public:
    DREllipse( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap getIcon() override;

protected:
    SSetting *pSettingEllipse;

    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    void slotRefreshEllipse();
};


#endif
