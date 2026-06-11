/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRSelectPolygon.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRSelectPolygon::DRSelectPolygon( DRDraw *pCanvas, const QPointF &pointPos )
    : DRPolygon( pCanvas, pointPos )
{
}

QPixmap DRSelectPolygon::getIcon()
{
    return QPixmap( ":DR/SelectPolygon" );
}

void DRSelectPolygon::doDraw( QPainter *pPainter )
{
    if ( bInit )
    {
        // draw the polygon with proposed line segment
        QPolygonF poly = polygon;
        poly.append( pointMouse );

        // temp outline of area
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawPolygon( poly );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawPolygon( poly );                         
    }
    else
    {
        // draw the polygon
        pPainter->setPen( QPen( Qt::green ) );
        pPainter->drawPolygon( polygon );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawPolygon( polygon );                         
    }
}



