/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRPolygon
#define H_DRPolygon

#include "DRPolyline.h"

class DRPolygon : public DRPolyline
{
    Q_OBJECT
public:
    DRPolygon( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap getIcon() override;

protected:
    virtual void doDraw( QPainter * ) override;
};

#endif
