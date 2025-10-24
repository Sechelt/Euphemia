/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_DRChord
#define H_DRChord

#include "DRArc.h"

class DRChord : public DRArc
{
    Q_OBJECT
public:
    DRChord( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap getIcon() override;

protected:
    virtual void doDraw( QPainter * ) override;
};


#endif
