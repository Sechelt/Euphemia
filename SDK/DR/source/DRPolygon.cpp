/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRPolygon.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRPolygon::DRPolygon( DRDraw *pCanvas, const QPointF &pointPos )
    : DRPolyline( pCanvas, pointPos )
{
}

QPixmap DRPolygon::getIcon()
{
    return QPixmap( ":DR/Polygon" );
}

/*!
 * \brief Draw the polygon. 
 *  
 * \author pharvey (2/1/23)
 * 
 * \param pPainter 
 * \param polygon 
 */
void DRPolygon::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    if ( bInit )
    {
        // draw the polygon with proposed line segment
        QPolygonF poly = polygon;
        poly.append( pointMouse );
        pPainter->drawPolygon( poly );                         
    }
    else
    {
        // draw the polygon
        pPainter->drawPolygon( polygon );                         
    }

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawPolygon( polygon );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawPolygon( polygon );                         
    }
}

