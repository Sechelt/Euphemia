/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRRectangle.h"

#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRRectangle::DRRectangle( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    // juggle around handles
    nHandlesAlloc           = 5;
    nIndexHandleHRound      = 0;
    nIndexHandleVRound      = 1;
    nIndexHandleMove        = 2;
    nIndexHandleTopLeft     = 3;
    nIndexHandleBottomRight = 4;

    // we share LSRectangle with DRRectangleBase
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
    if ( nSelectionState == SelectionStateStandard )
    {
        if ( pHandle->getType() == LHandle::TypeHRound )
        {
            pSettingRectangle->setProperty( "nXRadius", getXRadiusValidated( getXRadius( pointPos ) ) );
            return;
        }
        else if ( pHandle->getType() == LHandle::TypeVRound )
        {
            pSettingRectangle->setProperty( "nYRadius", getYRadiusValidated( getYRadius( pointPos ) ) );
            return;
        }
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

void DRRectangle::doCreateHandlesStandard()
{
    // sanity checks
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() >= 5 );

    DRRectangleBase::doCreateHandlesStandard();

    vectorHandles[nIndexHandleHRound] = new LHandle( this, getView(), LHandle::TypeHRound, getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
    vectorHandles[nIndexHandleVRound] = new LHandle( this, getView(), LHandle::TypeVRound, getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );

    // make z-order the same order as vectorHandles
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
        vectorHandles[n]->raise();
    }

    vectorHandles[nIndexHandleHRound]->setToolTip( tr("horizontal round") );
    vectorHandles[nIndexHandleVRound]->setToolTip( tr("vertical round") );

    vectorHandles[nIndexHandleHRound]->show();
    vectorHandles[nIndexHandleVRound]->show();
}

void DRRectangle::doSyncHandlesStandard()
{
    // sanity checks
    Q_ASSERT( nSelectionState == SelectionStateStandard );
    Q_ASSERT( vectorHandles.count() >= 5 );

    DRRectangleBase::doSyncHandlesStandard();

    vectorHandles[nIndexHandleHRound]->setCenter( getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
    vectorHandles[nIndexHandleVRound]->setCenter( getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );
}

void DRRectangle::doConnectSettings()
{
    DRRectangleBase::doConnectSettings();
    pSettingRectangle->setSelected( nSizeMode, nXRadius, nYRadius );
    connect( pSettingRectangle, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangle::doDisconnectSettings()
{
    DRRectangleBase::doDisconnectSettings();
    disconnect( pSettingRectangle, SIGNAL(signalChanged()), this, SLOT(slotRefreshRectangle()) );
}

void DRRectangle::slotRefreshRectangle()
{
    DRRectangleBase::slotRefreshRectangle();

    // accept
    nSizeMode   = (Qt::SizeMode)pSettingRectangle->property( "nSizeMode" ).toInt();
    nXRadius    = getXRadiusValidated( pSettingRectangle->property( "nXRadius" ).toDouble() );
    nYRadius    = getYRadiusValidated( pSettingRectangle->property( "nYRadius" ).toDouble() );

    if ( !vectorHandles.count() ) return;

    if ( nSelectionState == SelectionStateStandard )
    {
        vectorHandles[nIndexHandleHRound]->setCenter( getView()->mapFromScene( getXRadiusPoint( nXRadius ) ) );
        vectorHandles[nIndexHandleVRound]->setCenter( getView()->mapFromScene( getYRadiusPoint( nYRadius ) ) );
    }

    getGraphics()->update();
}


