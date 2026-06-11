/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LGraphicsLayer.h"

#include "LLayer.h"

LGraphicsLayer::LGraphicsLayer( LLayer *p )
    : QGraphicsObject()
{
    Q_ASSERT( p );
    pLayer = p;
}

void LGraphicsLayer::doPrepareGeometryChange()
{
    prepareGeometryChange();
}

QRectF LGraphicsLayer::boundingRect() const                                                                
{                                                                                                           
    return pLayer->boundingRect();                                                                  
}                                                                                                           
                                                                                                            
void LGraphicsLayer::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{                                                                                                           
    pLayer->paint( pPainter, nOption, pWidget );                                                    
}                                                                                                           
                                                                                                            
QPainterPath LGraphicsLayer::shape() const                                                                 
{                                                                                                           
    return pLayer->shape();                                                                         
}                                                                                                           


