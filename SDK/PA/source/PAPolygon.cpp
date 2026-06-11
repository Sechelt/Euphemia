/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAPolygon.h"

#include "PAPaint.h"

PAPolygon::PAPolygon( PAPaint *pLayer )
    : PAPolyline( pLayer )
{
}

/*!
 * \brief Paint the polygon. 
 *  
 * \author pharvey (2/1/23)
 * 
 * \param pPainter 
 * \param polygon 
 */
void PAPolygon::doPaint( QPainter *pPainter, bool )
{
    if ( nState == StateIdle ) return;

    // apply context
    pPainter->setPen( pen );

    // paint
    if ( nState == StateDraw )
    {
        QPolygon poly = polygon;
        poly.append( pointMouse );
        pPainter->drawPolygon( poly );                         
    }
    else
    {
        pPainter->drawPolygon( polygon );                         
    }
}

