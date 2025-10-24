/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
