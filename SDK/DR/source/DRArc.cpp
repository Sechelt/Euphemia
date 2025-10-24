/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRArc.h"

#include <CBDMath.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

#define DRRectangleBegin 0
#define DRRectangleMove 1
#define DRRectangleEnd 2
#define DRArcStartAngle 3
#define DRArcSpanAngle 4

    /*
        if ( bBind )                                                                       
        {                                                                                  
            QPoint pointDelta = pEvent->pos() - r.bottomRight();                           
            int nDelta = pointDelta.x() > pointDelta.y() ? pointDelta.x() : pointDelta.y();
            r.setTopLeft( r.topLeft() - QPoint( nDelta, nDelta ) );                        
            r.setBottomRight( r.bottomRight() + QPoint( nDelta, nDelta ) );                
        }                                                                                  
        else                                                                               
        {                                                                                  
            r.setBottomRight( pEvent->pos() );                                             
        }                                                                                  
        // ensure these points are consistent with nStartAngle=0-E and nEndAngle=180=W     
        // the handles will take over during manipulate                                    
        pointStart.setX( r.topRight().x() );                                               
        pointStart.setY( r.center().y() );                                                 
        pointStop.setX( r.x() );                                                           
        pointStop.setY( r.center().y() );                                                  
        nState = StateManipulate;                                                          
        update();                                                                          
    */

DRArc::DRArc( DRDraw *pLayer, const QPointF &pointPos )
    : DRRectangleBase( pLayer, pointPos )
{
    pointStart = r.center();
    pointStop = r.center();
    // angles: we use positive values only - so span is always clockwise
    nStartAngle = 0;               // 0 (east)
    nEndAngle   = 180;             // 180 degrees (west)
}

QPixmap DRArc::getIcon()
{
    return QPixmap( ":DR/Arc" );
}

QDomElement DRArc::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

    // start
    domElemThis.setAttribute( "StartX", pointStart.x() );
    domElemThis.setAttribute( "StartY", pointStart.y() );
    // stop
    domElemThis.setAttribute( "StopX", pointStop.x() );
    domElemThis.setAttribute( "StopY", pointStop.y() );
    // angle
    domElemThis.setAttribute( "AngleStart", nStartAngle );
    domElemThis.setAttribute( "AngleEnd", nEndAngle );

    return domElemThis;
}

bool DRArc::doLoad( QDomElement *pdomElemThis )
{
    DRRectangleBase::doLoad( pdomElemThis );

    // start
    pointStart.setX( pdomElemThis->attribute( "StartX", "0" ).toDouble() ); 
    pointStart.setY( pdomElemThis->attribute( "StartY", "0" ).toDouble() ); 
    // stop
    pointStop.setX( pdomElemThis->attribute( "StopX", "0" ).toDouble() ); 
    pointStop.setY( pdomElemThis->attribute( "StopY", "0" ).toDouble() ); 
    // angle
    nStartAngle = pdomElemThis->attribute( "AngleStart", "0" ).toInt();
    nEndAngle   = pdomElemThis->attribute( "AngleEnd", "0" ).toInt();

    return true;
}

void DRArc::doMove( LHandle *pHandle, const QPointF &pointPos )
{
    Q_ASSERT( pHandle );

    getGraphics()->doPrepareGeometryChange();

    QPointF pointView = getView()->mapFromScene( pointPos );

    // adjusting angles?
    if ( pHandle == vectorHandles[DRArcStartAngle] )
    {
        pHandle->setCenter( pointView );
        pointStart = pointPos;
        // change origin to rect center and get polar
        QPointF pointFromCenter = CBDMath::getTopLeftToOrigin( pLayer->boundingRect(), r.center(), pointStart );
        CBDMath::Polar polar = CBDMath::getCartesianToPolar( pointFromCenter.x(), pointFromCenter.y() );
        // update our start angle
        nStartAngle = int(CBDMath::getRadiansToDegrees( polar.angle ));
        // update();
        return;
    }

    if ( pHandle == vectorHandles[DRArcSpanAngle] )
    {
        pHandle->setCenter( pointView );
        pointStop = pointPos;
        QPointF pointFromCenter = CBDMath::getTopLeftToOrigin( pLayer->boundingRect(), r.center(), pointStop );

        // change origin to rect center and get polar
        CBDMath::Polar polar = CBDMath::getCartesianToPolar( pointFromCenter.x(), pointFromCenter.y() );
        // update our span
        nEndAngle = int(CBDMath::getRadiansToDegrees( polar.angle ));
        // update();
        return;
    }

    //
    // adjust rect
    //

    // adjust our geometry
    if ( pHandle == vectorHandles[DRRectangleBegin] )
    {
        // todo
    }
    else if ( pHandle == vectorHandles[DRRectangleMove] )
    {
        QPointF pointDiff = pointPos - r.center();
        r.setTopLeft( r.topLeft() + pointDiff );
        r.setBottomRight( r.bottomRight() + pointDiff );

        pointStart += pointDiff;
        pointStop += pointDiff;
    }
    else if ( pHandle == vectorHandles[DRRectangleEnd] )
    {
        // todo
    }

    //
    DRRectangleBase::doMove( pHandle, pointPos );
}

void DRArc::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // paint
    // - factors when calculating the span;
    //      - arc will be drawn from nStartAngle, counter-clockwise, span degrees
    //      - span degrees should be calculated from nStartAngle to nEndAngle in the counter-clockwise direction
    //      - span could cross 0 degrees
    //      - 0 degrees is East (3 o'clock) and increases counter-clockwise
    int nSpan;
    if ( nEndAngle > nStartAngle )
        nSpan = nEndAngle - nStartAngle;
    else
        nSpan = 360 - ( nStartAngle - nEndAngle );

    // draw shape
    pPainter->drawArc( r, nStartAngle * 16, nSpan * 16 );                         

    // draw manipulation indicators
    if ( nSelectionState == SelectionStateStandard )
    {
        QPen pen( Qt::white );
        pPainter->setPen( pen );
        pPainter->drawLine( r.center(), pointStart );
        pPainter->drawLine( r.center(), pointStop );

        pen.setColor( Qt::darkGray );
        pen.setStyle( Qt::DashLine );
        pPainter->setPen( pen );
        pPainter->drawLine( r.center(), pointStart );
        pPainter->drawLine( r.center(), pointStop );
    }
    // draw outline as selected
    else if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawArc( r, nStartAngle * 16, nSpan * 16 );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawArc( r, nStartAngle * 16, nSpan * 16 );                         
    }
}

void DRArc::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    DRRectangleBase::doCreateHandles();

    // angle handles must be after rect handles

    // DRArcStartAngle
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( pointStart ) );
    pHandle->setToolTip( tr("start angle") );
    vectorHandles.append( pHandle );
    pHandle->show();

    // DRArcSpanAngle
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( pointStop ) );
    pHandle->setToolTip( tr("stop angle") );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void DRArc::doSyncHandles()
{
    DRRectangleBase::doSyncHandles();

    vectorHandles[DRArcStartAngle]->setCenter( getView()->mapFromScene( pointStart ) );
    vectorHandles[DRArcSpanAngle]->setCenter( getView()->mapFromScene( pointStop ) );
}


