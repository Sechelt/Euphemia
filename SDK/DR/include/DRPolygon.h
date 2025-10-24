/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
