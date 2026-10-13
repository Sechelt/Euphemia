/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRLine.h"

#include <LGraphicsObject.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

#define DRLineBegin 0
#define DRLineMove 1
#define DRLineEnd 2

DRLine::DRLine( DRDraw *pCanvas, const QPointF &pointPos )
    : DRObject( pCanvas, pointPos )
{
    if ( pointPos.isNull() )
        pointBegin = pointEnd = QPointF( 0, 0 );
    else
        pointBegin = pointEnd = pointPos;

    pSettingLine = g_SSettings->getSetting( "LSLine" ); Q_ASSERT( pSettingLine );
    slotRefreshLine();
    connect( pSettingLine, SIGNAL(signalChanged()), SLOT(slotRefreshLine()) );
}

DRLine::~DRLine()
{
}

QPainterPath DRLine::shape() const
{
    QPolygonF polygon;
    polygon << pointBegin << pointEnd;
    // draw our line into a path (perhaps make the path reusable to speed up code in the future)
    QPainterPath path;
    path.addPolygon( polygon );
    // use a stroker to increase the width a bit to make the line easier to click on
    QPainterPathStroker stroker;
    stroker.setWidth( pen.width() + 10 );
    // return the, now slightly fatter, line
    return stroker.createStroke( path );
}

QPixmap DRLine::getIcon()
{
    return QPixmap( ":DR/Line" );
}

/*!
 * \brief Return a copy of the CURRENT SHAPE.
 *  
 * This is done by getting doDraw to draw on an empty, temp, canvas and then 
 * copying from that canvas. 
 *  
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage DRLine::getImage()
{
#ifdef CBD_TODO
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    QRect r;                                                              
    r.setTopLeft( pointBegin );                                           
    r.setBottomRight( pointEnd );                                         
                                                                          
    QImage image( pCanvas->getImageRef()->size(), QImage::Format_ARGB32 );
    image.fill( Qt::transparent );                                        
    QPainter painter( &image );                                           
    painter.setRenderHint( QPainter::Antialiasing );                      
    painter.setRenderHint( QPainter::SmoothPixmapTransform );             
    doDraw( &painter );                                                   
                                                                          
    return image.copy( r );                                               
*/
    return QImage();
}

LHandle *DRLine::getGrab( const QPointF &pointPos, bool bSizing )
{
    if ( bSizing ) 
    {
        if ( nSelectionState != SelectionStateStandard ) return nullptr;
        return vectorHandles.at( DRLineEnd );
    }

    return DRObject::getGrab( pointPos ); 
}

void DRLine::doMoveBy( const QPointF &pointOffset )
{
}

void DRLine::doMove( LHandle *pHandle, const QPointF &pointPos )
{
    Q_ASSERT( pHandle );

    pGraphics->doPrepareGeometryChange();

    // adjust our geometry
    if ( pHandle == vectorHandles[DRLineBegin] )
    {
        // move the begin 
        pointBegin = pointPos;
    }
    else if ( pHandle == vectorHandles[DRLineMove] )
    {
        // get diff
        QRectF r( pointBegin, pointEnd );
        r = r.normalized();
        QPointF pointDiff = pointPos - r.center();
        // update points
        pointBegin += pointDiff;
        pointEnd += pointDiff;
    }
    else if ( pHandle == vectorHandles[DRLineEnd] )
    {
        // move the end 
        pointEnd = pointPos;
    }

    // adjust handles
    QPointF  pointViewBegin  = getView()->mapFromScene( pointBegin );
    QPointF  pointViewPos    = getView()->mapFromScene( pointPos );
    QPointF  pointViewEnd    = getView()->mapFromScene( pointEnd );

    if ( pHandle == vectorHandles[DRLineBegin] )
    {
        pHandle->setCenter( pointViewBegin );
        // get center
        QRectF r;
        r.setTopLeft( pointViewBegin );
        r.setBottomRight( pointViewEnd );
        // adjust move handle
        vectorHandles[DRLineMove]->setCenter( r.center() );
    }
    else if ( pHandle == vectorHandles[DRLineMove] )
    {
        vectorHandles[DRLineBegin]->setCenter( pointViewBegin );
        vectorHandles[DRLineMove]->setCenter( pointViewPos );
        vectorHandles[DRLineEnd]->setCenter( pointViewEnd );
    }
    else if ( pHandle == vectorHandles[DRLineEnd] )
    {
        pHandle->setCenter( pointViewEnd );
        // get center
        QRectF r;
        r.setTopLeft( pointViewBegin );
        r.setBottomRight( pointViewEnd );
        // adjust move handle
        vectorHandles[DRLineMove]->setCenter( r.center() );
    }
    // update();
}

void DRLine::doDrop( LHandle *pHandle, const QPointF &point )
{
    if ( point.isNull() ) return;
    doMove( pHandle, point );
}

QDomElement DRLine::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRObject::doSave( pdomDoc, pdomElemParent );

    // begin
    domElemThis.setAttribute( "x1", pointBegin.x() );
    domElemThis.setAttribute( "y1", pointBegin.y() );
    // end
    domElemThis.setAttribute( "x2", pointEnd.x() );
    domElemThis.setAttribute( "y2", pointEnd.y() );
    // line ends
    domElemThis.setAttribute( "BeginStyle", nBeginStyle );
    domElemThis.setAttribute( "EndStyle", nEndStyle );
    domElemThis.setAttribute( "BeginSize", nBeginSize );
    domElemThis.setAttribute( "EndSize", nEndSize );

    return domElemThis;
}

bool DRLine::doLoad( QDomElement *pdomElemThis )
{
    DRObject::doLoad( pdomElemThis );

    // begin
    pointBegin.setX( pdomElemThis->attribute( "x1", "0" ).toDouble() ); 
    pointBegin.setY( pdomElemThis->attribute( "y1", "0" ).toDouble() ); 
    // end
    pointEnd.setX( pdomElemThis->attribute( "x2", "0" ).toDouble() ); 
    pointEnd.setY( pdomElemThis->attribute( "y2", "0" ).toDouble() ); 
    // line-ends
    nBeginStyle = LSLine::LineEndStyles(pdomElemThis->attribute( "BeginStyle", QString::number( int(nBeginStyle) ) ).toInt());
    nEndStyle   = LSLine::LineEndStyles(pdomElemThis->attribute( "EndStyle", QString::number( int(nEndStyle) ) ).toInt());
    nBeginSize  = pdomElemThis->attribute( "BeginSize", "5" ).toDouble(); 
    nEndSize    = pdomElemThis->attribute( "EndSize", "5" ).toDouble(); 

    return true;
}

void DRLine::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw shape (could be fat line)
    pPainter->drawLine( pointBegin, pointEnd );                         

    // draw as selected (thin line)
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawLine( pointBegin, pointEnd );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawLine( pointBegin, pointEnd );                         
    }

    // draw line ends (if any)
    doDrawLineEnds( pPainter );
}

void DRLine::doDrawLineEnds( QPainter *pPainter )
{
    doDrawLineEnd( pPainter, nBeginStyle, QLineF( pointBegin, pointEnd ) );
    // provide reversed line because doDrawLineEnd always draws at p1
    doDrawLineEnd( pPainter, nEndStyle, QLineF( pointEnd, pointBegin ) );
}

void DRLine::doDrawLineEnd( QPainter *pPainter, LSLine::LineEndStyles nStyle, const QLineF &line )
{
    switch ( nStyle )
    {
        case LSLine::LineEndArrow:
            doDrawLineEndArrow( pPainter, line );
            break;
        case LSLine::LineEndBall:
            doDrawLineEndBall( pPainter, line );
            break;
        case LSLine::LineEndNone:
            break;
    };
}

void DRLine::doDrawLineEndArrow( QPainter *pPainter, const QLineF &line )
{
    // draw at p1
    const qreal Pi = 3.14;
    double angle = ::acos( line.dx() / line.length() );

    qreal nSize = 6;
    if ( line.p1() == pointBegin )
        nSize = nBeginSize;
    else
        nSize = nEndSize;

    if ( line.dy() >= 0 )
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line.p1() + QPointF( sin( angle + Pi / 3 ) * nSize,
                                           cos( angle + Pi / 3 ) * nSize );
    QPointF arrowP2 = line.p1() + QPointF( sin( angle + Pi - Pi / 3 ) * nSize,
                                           cos( angle + Pi - Pi / 3 ) * nSize );

    QPolygonF polygonArrow;
    polygonArrow << line.p1() << arrowP1 << arrowP2;
    pPainter->drawPolygon( polygonArrow );
}

void DRLine::doDrawLineEndBall( QPainter *pPainter, const QLineF &line )
{
    // draw at p1
    qreal nSize = 6;
    if ( line.p1() == pointBegin )
        nSize = nBeginSize;
    else
        nSize = nEndSize;

    pPainter->drawEllipse( line.p1(), nSize, nSize );
}

void DRLine::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // their parent will be the viewport so...
    QPointF pointViewBegin   = getView()->mapFromScene( pointBegin );
    QPointF pointViewEnd     = getView()->mapFromScene( pointEnd );

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    // DRLineBegin
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, pointViewBegin );
    vectorHandles.append( pHandle );
    pHandle->show();

    // DRLineMove
    QRectF r;
    r.setTopLeft( pointViewBegin );
    r.setBottomRight( pointViewEnd );
    r = r.normalized();

    pHandle = new LHandle( this, getView(), LHandle::TypeDrag, r.center() );
    vectorHandles.append( pHandle );
    pHandle->show();

    // DRLineEnd
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, pointViewEnd );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void DRLine::doSyncHandles()
{
    QRectF rectView = getView()->mapFromScene( QRectF( pointBegin, pointEnd ).normalized() ).boundingRect();

    vectorHandles[DRLineBegin]->setCenter( rectView.topLeft() );
    vectorHandles[DRLineMove]->setCenter( rectView.center() );
    vectorHandles[DRLineEnd]->setCenter( rectView.bottomRight() );
}

void DRLine::slotRefreshLine()
{
    nBeginStyle = LSLine::LineEndStyles(pSettingLine->property( "nBegin" ).toInt());
    nBeginSize  = pSettingLine->property( "nBeginSize" ).toReal();

    nEndStyle = LSLine::LineEndStyles(pSettingLine->property( "nEnd" ).toInt());
    nEndSize  = pSettingLine->property( "nEndSize" ).toReal();

    if ( pGraphics ) pGraphics->update();
}

