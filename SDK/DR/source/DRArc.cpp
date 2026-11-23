/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRArc.h"

#include <CBDMath.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

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
    // juggle around handles
    nHandlesAlloc           = 5;
    nIndexHandleStartAngle  = 0;
    nIndexHandleSpanAngle   = 1;
    nIndexHandleMove        = 2;
    nIndexHandleTopLeft     = 3;
    nIndexHandleBottomRight = 4;

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
    if ( nSelectionState != SelectionStateStandard ) return DRRectangleBase::doMove( pHandle, pointPos );

    getGraphics()->doPrepareGeometryChange();

    QPointF pointView = getView()->mapFromScene( pointPos );

    // adjusting angles?
    if ( pHandle == vectorHandles[nIndexHandleStartAngle] )
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

    if ( pHandle == vectorHandles[nIndexHandleSpanAngle] )
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
    if ( pHandle == vectorHandles[nIndexHandleTopLeft] )
    {
        // todo
    }
    else if ( pHandle == vectorHandles[nIndexHandleMove] )
    {
        QPointF pointDiff = pointPos - r.center();
        r.setTopLeft( r.topLeft() + pointDiff );
        r.setBottomRight( r.bottomRight() + pointDiff );

        pointStart += pointDiff;
        pointStop += pointDiff;
    }
    else if ( pHandle == vectorHandles[nIndexHandleBottomRight] )
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

void DRArc::doCreateHandlesStandard()
{
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() == 0 );

    DRRectangleBase::doCreateHandlesStandard();

    vectorHandles[nIndexHandleStartAngle] = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( pointStart ) );
    vectorHandles[nIndexHandleSpanAngle] = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( pointStop ) );

    vectorHandles[nIndexHandleStartAngle]->setToolTip( tr("start angle") );
    vectorHandles[nIndexHandleSpanAngle]->setToolTip( tr("stop angle") );

    // make z-order the same order as vectorHandles
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
        vectorHandles[n]->raise();
    }

    vectorHandles[nIndexHandleStartAngle]->show();
    vectorHandles[nIndexHandleSpanAngle]->show();
}

void DRArc::doSyncHandlesStandard()
{
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() > 0 );

    DRRectangleBase::doSyncHandlesStandard();

    vectorHandles[nIndexHandleStartAngle]->setCenter( getView()->mapFromScene( pointStart ) );
    vectorHandles[nIndexHandleSpanAngle]->setCenter( getView()->mapFromScene( pointStop ) );
}



