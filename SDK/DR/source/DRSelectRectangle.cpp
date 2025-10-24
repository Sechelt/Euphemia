/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRSelectRectangle.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRSelectRectangle::DRSelectRectangle( DRDraw *pLayer, const QPointF &pointPos )
    : DRRectangleBase( pLayer, pointPos )
{
    bBind = false; 
}

QPixmap DRSelectRectangle::getIcon()
{
    return QPixmap( ":DR/SelectRectangle" );
}

/*!
 * \brief Return a copy of the SELECTED AREA.
 * 
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage DRSelectRectangle::getImage()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QImage();
}

QDomElement DRSelectRectangle::getXML( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    Q_UNUSED( pdomDoc );
    Q_UNUSED( pdomElemParent );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QDomElement();
}

void DRSelectRectangle::doMove( LHandle *pHandle, const QPointF &point )
{
    QPointF pointDiff = point - r.center();
    DRRectangleBase::doMove( pHandle, point );
    if ( bDrawing ) return;
    QVector<DRObject *> vectorSelected = pLayer->getSelected();

    // move
    if ( pHandle->getType() == LHandle::TypeDrag ) 
    {
        foreach( DRObject *pObject, vectorSelected )
        {
            if ( pObject != this ) pObject->doMoveBy( pointDiff );
        }
        return;
    }
    // resize - stretch and move all selected objects
    if ( pHandle->getType() == LHandle::TypeSizeTopLeft ) 
    {
        // pObject->doResize();
        return;
    }
    if ( pHandle->getType() == LHandle::TypeSizeBottomRight ) 
    {
        return;
    }
}

void DRSelectRectangle::doDrop( LHandle *pHandle, const QPointF &point )
{
    DRRectangleBase::doDrop( pHandle, point );
    // the main purpose for this method is to do this...
    if ( bDrawing ) 
    {
        bDrawing = false;
        return;
    }
    // doMove will get called by DRRectangleBase::doDrop so no need to transform here
}

void DRSelectRectangle::doDraw( QPainter *pPainter )
{
    if ( bDrawing )
        pPainter->setPen( QPen( Qt::white ) );
    else
        pPainter->setPen( QPen( Qt::green ) );

    pPainter->drawRect( r.normalized() );                         
    pPainter->setPen( QPen( Qt::DashLine ) );
    pPainter->drawRect( r.normalized() );                         
}

void DRSelectRectangle::doConnectSettings()
{
    // bypass DRRectangleBase to avoid changing bBind
    DRObject::doConnectSettings();
}

void DRSelectRectangle::doDisconnectSettings()
{
    // bypass DRRectangleBase to avoid changing bBind
    DRObject::doDisconnectSettings();
}

