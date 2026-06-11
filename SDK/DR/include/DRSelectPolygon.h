/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRSelectPolygon
#define H_DRSelectPolygon

#include "DRPolygon.h"

class DRSelectPolygon : public DRPolygon
{
    Q_OBJECT
public:
    DRSelectPolygon( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap getIcon() override;

    virtual bool isSelector() override { return true; }

protected:
    virtual void doDraw( QPainter * ) override;

};

#endif
