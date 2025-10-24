/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRRectangle.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

#define DRRectangleBegin 0
#define DRRectangleMove 1
#define DRRectangleEnd 2
#define DRRectangleHRound 3
#define DRRectangleVRound 4

DRRectangle::DRRectangle( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    slotRefreshRectangle();
}

DRRectangle::~DRRectangle()
{
}

QPixmap DRRectangle::getIcon()
{
    return QPixmap( ":DR/Rectangle" );
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
QImage DRRectangle::getImage()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QImage();
}

void DRRectangle::doMove( LHandle *pHandle, const QPointF &pointPos )
{
    Q_ASSERT( pHandle );

    getGraphics()->doPrepareGeometryChange();

    // rounded corners
    if ( pHandle == vectorHandles[DRRectangleHRound] )
    {
        pSettingRectangle->setProperty( "nXRadius", getXRadiusValidated( getXRadius( pointPos ) ) );
        return;
    }
    else if ( pHandle == vectorHandles[DRRectangleVRound] )
    {
        pSettingRectangle->setProperty( "nYRadius", getYRadiusValidated( getYRadius( pointPos ) ) );
        return;
    }

    //
    DRRectangleBase::doMove( pHandle, pointPos );
}

QDomElement DRRectangle::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

    // radius
    domElemThis.setAttribute( "XRadius", nXRadius );
    domElemThis.setAttribute( "YRadius", nYRadius );
    domElemThis.setAttribute( "SizeMode", nSizeMode );

    return domElemThis;
}

bool DRRectangle::doLoad( QDomElement *pdomElemThis )
{
    DRRectangleBase::doLoad( pdomElemThis );

    // radius
    nSizeMode   = (Qt::SizeMode)pdomElemThis->attribute( "SizeMode", QString::number( int(Qt::AbsoluteSize) ) ).toInt();
    nXRadius    = getXRadiusValidated( pdomElemThis->attribute( "XRadius", "0" ).toDouble() );
    nYRadius    = getYRadiusValidated( pdomElemThis->attribute( "YRadius", "0" ).toDouble() );

    return true;
}

/*!
 * \brief Get X Radius given the context.
 * 
 * \author pharvey (4/1/23)
 * 
 * \param point  proposed location for the handle in scene coordinate
 * 
 * \return qreal XRadius for given context (may not match point due to validation)
 */
qreal DRRectangle::getXRadius( const QPointF &point )
{
    qreal nX = point.x();
    if ( nSizeMode == Qt::AbsoluteSize )
    {
        // calc nXRadius using abs value (0-width/2)
        if ( nX > r.topRight().x() ) nX = r.topRight().x();
        if ( nX < r.topRight().x() - r.width() / 2 ) nX = r.topRight().x() - r.width() / 2;
        return ( r.topRight().x() - nX );
    }
    // calc nXRadius using % of half-width (0-100)
    qreal nWidth    = r.width() / 2;
    qreal nLength   = r.right() - nX;
    qreal nPercent  = nLength / nWidth * 100;
    if ( nPercent > 100 ) nPercent = 100;
    if ( nPercent < 0 ) nPercent = 0;
    return nPercent;
}

/*!
 * \brief Get Y Radius given the context.
 * 
 * \author pharvey (4/1/23)
 * 
 * \param point  proposed location for the handle in scene coordinate
 * 
 * \return qreal XRadius for given context (may not match point due to validation)
 */
qreal DRRectangle::getYRadius( const QPointF &point )
{
    qreal nY = point.y();
    if ( nSizeMode == Qt::AbsoluteSize )
    {
        // calc nYRadius using abs value (0-width/2)
        if ( nY > r.topRight().y() + r.height() / 2 ) nY = r.topRight().y() + r.height() / 2;
        if ( nY < r.topRight().y() ) nY = r.topRight().y();
        return ( nY - r.topRight().y() );
    }
    // calc nYRadius using % of half-width (0-100)
    qreal nHeight   = r.height() / 2;
    qreal nLength   = nY - r.top();
    qreal nPercent  = nLength / nHeight * 100;
    if ( nPercent > 100 ) nPercent = 100;
    if ( nPercent < 0 ) nPercent = 0;
    return nPercent;
}

qreal DRRectangle::getXRadiusValidated( qreal nXRadius )
{
    if ( nSizeMode == Qt::AbsoluteSize )
    {
        if ( nXRadius > r.width() / 2 ) nXRadius = r.width() / 2;
        if ( nXRadius < 0 ) nXRadius = 0;
    }
    else
    {
        if ( nXRadius > 100 ) nXRadius = 100;
        if ( nXRadius < 0 ) nXRadius = 0;
    }
    return nXRadius;
}

qreal DRRectangle::getYRadiusValidated( qreal nYRadius )
{
    if ( nSizeMode == Qt::AbsoluteSize )
    {
        if ( nYRadius > r.height() / 2 ) nYRadius = r.height() / 2;
        if ( nYRadius < 0 ) nYRadius = 0;
    }
    else
    {
        if ( nYRadius > 100 ) nYRadius = 100;
        if ( nYRadius < 0 ) nYRadius = 0;
    }
    return nYRadius;
}

QPointF DRRectangle::getXRadiusPoint( qreal nXRadius )
{
    if ( nSizeMode == Qt::AbsoluteSize ) return QPointF( r.right() - nXRadius, r.top() );
    qreal nWidth    = r.width() / 2;
    qreal nLength   = nWidth * ( nXRadius / 100 );
    return QPointF( r.right() - nLength, r.top() );
}

QPointF DRRectangle::getYRadiusPoint( qreal nYRadius )
{
    if ( nSizeMode == Qt::AbsoluteSize ) return QPointF( r.right(), r.top() + nYRadius );
    qreal nHeight   = r.height() / 2;
    qreal nLength   = nHeight * ( nYRadius / 100 );
    return QPointF( r.right(), r.top() + nLength );
}

void DRRectangle::doDraw( QPainter *pPainter )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw shape
    pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRoundedRect( r, nXRadius, nYRadius, nSizeMode );
    }
}

void DRRectangle::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    DRRectangleBase::doCreateHandles();

    // DRRectangleHRound
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
    vectorHandles.append( pHandle );
    pHandle->show();

    // DRRectangleVRound
    pHandle = new LHandle( this, getView(), LHandle::TypeMovePoint, getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );
    vectorHandles.append( pHandle );
    pHandle->show();
}

void DRRectangle::doSyncHandles()
{
    DRRectangleBase::doSyncHandles();

    vectorHandles[DRRectangleHRound]->setCenter( getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
    vectorHandles[DRRectangleVRound]->setCenter( getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );
}

void DRRectangle::slotRefreshRectangle()
{
    DRRectangleBase::slotRefreshRectangle();

    // accept
    nSizeMode   = (Qt::SizeMode)pSettingRectangle->property( "nSizeMode" ).toInt();
    nXRadius    = getXRadiusValidated( pSettingRectangle->property( "nXRadius" ).toDouble() );
    nYRadius    = getYRadiusValidated( pSettingRectangle->property( "nYRadius" ).toDouble() );

    if ( !vectorHandles.count() ) return;

    vectorHandles[DRRectangleHRound]->setCenter( getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
    vectorHandles[DRRectangleVRound]->setCenter( getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );

    getGraphics()->update();
}


