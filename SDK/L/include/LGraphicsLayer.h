/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LGraphicsLayer
#define H_LGraphicsLayer

#include "L.h"

class LLayer;

/*!
 * \brief A QGraphicsObject which reperesents a \sa LLayer based object.
 *  
 * \author pharvey (4/6/23)
 */
class LGraphicsLayer : public QGraphicsObject
{
    Q_OBJECT
public:
    LGraphicsLayer( LLayer *pLayer );

    virtual void doPrepareGeometryChange();

    virtual QRectF boundingRect() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    virtual QPainterPath shape() const;

protected:
    LLayer *pLayer = nullptr;
};

#endif
