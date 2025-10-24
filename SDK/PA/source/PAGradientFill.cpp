/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PAGradientFill.h"

#include <CBDMath.h>
#include <SColorsPickerDialog.h>
#include <SSettings.h>

#include "PAPaint.h"

// constant index values for vectorHandles
#define PAGradientFillFactory 0                      /*!< indicates fill area, can create/delete intermediate handles, is center for radial gradient */

#define PAGradientFillLinearStart 1
#define PAGradientFillLinearStop 2
#define PAGradientFillLinearIntermediates 3          /*!< index of first (if any) intermediate stop point handle                                     */

#define PAGradientFillRadialFocal 1
#define PAGradientFillRadialRadius 2
#define PAGradientFillRadialIntermediates 3          /*!< index of first (if any) intermediate stop point handle                                     */

#define PAGradientFillConicalAngle 1
#define PAGradientFillConicalIntermediates 2         /*!< index of first (if any) intermediate stop point handle                                     */

//
//
//
PAGradientFill::PAGradientFill( PAPaint *pLayer )
    : PATool( pLayer )
{
    pSetting = g_SSettings->getSetting( "PASGradient" ); Q_ASSERT( pSetting );

    slotRefresh();

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
}

PAGradientFill::~PAGradientFill()
{
    doCancel();
}

QRect PAGradientFill::getRect() 
{ 
    return polygon.boundingRect(); 
}

/*!
 * \brief Return a copy of the CURRENT SHAPE.
 *  
 * This is done by getting doPaint to draw on an empty, temp, canvas and then 
 * copying from that canvas. 
 *  
 * \author pharvey (2/6/23)
 * 
 * \return QImage 
 */
QImage PAGradientFill::getCopy()
{
    Q_ASSERT( nState == StateManipulate );

    QImage image( pLayer->getImageRef()->size(), QImage::Format_ARGB32);
    image.fill( Qt::transparent );
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    return image.copy( polygon.boundingRect() );
}

/*!
 * \brief Double click on handle to change color (in some cases).
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pEvent 
 * 
 * \return QRect 
 */
void PAGradientFill::doDoubleClick( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    if ( nState != StateManipulate ) return;                                // does not make sense unless we are in manipulate state as there would be no handles
    LHandle *pHandle = getHandle( pEvent->pos() );
    if ( !pHandle ) return;                                                 // we are only going to process a double click on a handle

    if ( nType == StandardGradientLinear )
    {
        if ( pHandle == vectorHandles[PAGradientFillLinearStart] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStart.second, 0, true );
            if ( !bOk ) return;
            stopStart.second = color;
            update();
            return;
        }
        if ( pHandle == vectorHandles[PAGradientFillLinearStop] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStop.second, 0, true );
            if ( !bOk ) return;
            stopStop.second = color;
            update();
            return;
        }
        // must be a stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillLinearIntermediates;
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stops[n].second, 0, true );
            if ( !bOk ) return;
            stops[n].second = color;
            update();
            return;
        }
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        if ( pHandle == vectorHandles[PAGradientFillFactory] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStart.second, 0, true );
            if ( !bOk ) return;
            stopStart.second = color;
            update();
            return;
        }

        if ( pHandle == vectorHandles[PAGradientFillRadialRadius] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStop.second, 0, true );
            if ( !bOk ) return;
            stopStop.second = color;
            update();
            return;
        }
        if ( pHandle == vectorHandles[PAGradientFillRadialFocal] )
        {
            return;
        }
        // must be a stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillLinearIntermediates;
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stops[n].second, 0, true );
            if ( !bOk ) return;
            stops[n].second = color;
            update();
            return;
        }
        return;
    }

    if ( nType == StandardGradientConical )
    {
        if ( pHandle == vectorHandles[PAGradientFillFactory] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStart.second, 0, true );
            if ( !bOk ) return;
            stopStart.second = color;
            update();
            return;
        }

        if ( pHandle == vectorHandles[PAGradientFillConicalAngle] )
        {
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stopStop.second, 0, true );
            if ( !bOk ) return;
            stopStop.second = color;
            update();
            return;
        }
        // must be a stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillConicalIntermediates;
            bool bOk = false;
            QColor color = SColorsPickerDialog::getColor( &bOk, stops[n].second, 0, true );
            if ( !bOk ) return;
            stops[n].second = color;
            update();
            return;
        }
        return;
    }
}

/*!
 * \brief Create a new point (drawing mode) or start to move a point (manipulating mode).
 * 
 * \author pharvey (2/1/23)
 * 
 * \param pEvent 
 * 
 * \return bool 
 */
void PAGradientFill::doPress( PMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        doDrawState( pEvent->pos() );
        break;
    case StateDraw:
        break;
    case StateManipulate:
        pHandle = getHandle( pEvent->pos() );
        if ( !pHandle ) doCommit();
        break;
    }
}

void PAGradientFill::doMove( PMouseEvent *pEvent ) 
{
    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        if ( pHandle ) doMoveHandle( pEvent->pos() );
        break;
    }
}

void PAGradientFill::doRelease( PMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->button() != Qt::LeftButton ) return;

    switch ( nState )
    {
    case StateIdle:
        break;
    case StateDraw:
        break;
    case StateManipulate:
        if ( pHandle )
        {
            if ( shouldRemoveStop() ) 
            {
                doRemoveStop();
                update();
            }
        }
        pHandle = nullptr;
        break;
    }
}

void PAGradientFill::doCommit()
{
    Q_ASSERT( nState == StateManipulate );

    emit signalPreCommit();

    QPainter painter( pLayer->getImageRef() );
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    doPaint( &painter );

    emit signalCommitted();

    doIdleState();
}

void PAGradientFill::slotRefresh()
{
    if ( nType != pSetting->property( "nType" ).toInt() )
    {
        if ( nState != StateIdle ) doCancel();
        nType = pSetting->property( "nType" ).toInt();
    }

    nSpread = (QGradient::Spread)pSetting->property( "nSpread" ).toInt();
    update();
}

void PAGradientFill::doPaint( QPainter *pPainter, bool bCommit )
{
    if ( nState != StateManipulate ) return;

    // re. intermediate stops
    // - we do not use gradient.setStops( stops ); because that would have to be in asc order by qreal value
    // - our stops are not sorted - their index position is important for our use in other areas
    // - so we add them one-at-a-time.


    if ( nType == StandardGradientLinear )
    {
        QLinearGradient gradient( linear.pointStart, linear.pointStop );
        gradient.setSpread( nSpread );

        // add intermediate stops
        gradient.setColorAt( stopStart.first, stopStart.second );
        for ( QGradientStop stop : stops )
        {
            gradient.setColorAt( stop.first, stop.second );
        }
        gradient.setColorAt( stopStop.first, stopStop.second );

        pPainter->setBrush( QBrush( gradient ) );
        pPainter->setPen( QPen( Qt::NoPen ) );
        pPainter->drawPolygon( polygon );

        if ( !bCommit )
        {
            QPen pen( Qt::darkGray );
            pen.setStyle( Qt::DashLine );
            pPainter->setPen( pen );
            pPainter->drawLine( linear.pointStart, linear.pointStop );
        }
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        qreal nRadius = qAbs( pointFactory.x() - radialSimple.pointCenterRadius.x() );
        QRadialGradient gradient( radialSimple.pointCenter, nRadius, radialSimple.pointFocal );
        gradient.setSpread( nSpread );

        // add intermediate stops
        gradient.setColorAt( stopStart.first, stopStart.second );       // center
        for ( QGradientStop stop : stops )
        {
            gradient.setColorAt( stop.first, stop.second );
        }
        gradient.setColorAt( stopStop.first, stopStop.second );         // focal point

        pPainter->setBrush( QBrush( gradient ) );
        pPainter->setPen( QPen( Qt::NoPen ) );
        pPainter->drawPolygon( polygon );

        if ( !bCommit )
        {
            QPen pen( Qt::darkGray );
            pen.setStyle( Qt::DashLine );
            pPainter->setPen( pen );
            pPainter->drawLine( radialSimple.pointCenter, radialSimple.pointCenterRadius );
        }
        return;
    }

    if ( nType == StandardGradientConical )
    {
        // the origin of the polar coordinates is pointCenter.
        // get pointAngle relative to polar origin
        QPointF pointAngle = conical.pointAngle - conical.pointCenter;
        // get polar coordinate to get radians and then convert radians to degrees
        CBDMath::Polar polar = CBDMath::getCartesianToPolar( pointAngle.x(), pointAngle.y() );
        qreal nDegrees = CBDMath::getRadiansToDegrees( polar.angle );
        // init gradient
        QConicalGradient gradient( conical.pointCenter, nDegrees );

        // add intermediate stops
        gradient.setColorAt( stopStart.first, stopStart.second );
        for ( QGradientStop stop : stops )
        {
            gradient.setColorAt( stop.first, stop.second );
        }
        gradient.setColorAt( stopStop.first, stopStop.second );  

        pPainter->setBrush( QBrush( gradient ) );
        pPainter->setPen( QPen( Qt::NoPen ) );
        pPainter->drawPolygon( polygon );

        if ( !bCommit )
        {
            QPen pen( Qt::darkGray );
            pen.setStyle( Qt::DashLine );
            pPainter->setPen( pen );
            pPainter->drawLine( conical.pointCenter, conical.pointAngle );
        }
        return;
    }

    // 
    if ( nType > 0 )
    {
        QGradient   gradient( (QGradient::Preset)nType );
        gradient.setSpread( nSpread );
        pPainter->setBrush( QBrush( gradient ) );
        pPainter->setPen( QPen( Qt::NoPen ) );
        pPainter->drawPolygon( polygon );
        return;
    }
}

/*!
 * \brief Go to StateDraw. 
 *  
 * We init. Notably we set; pointFactory (initial press point) and polygon (outline of fill area). 
 *  
 * We then move to StateManipulate. So we do not stay in StateDraw - we just transition 
 * through it. 
 * 
 * \author pharvey (4/19/23)
 * 
 * \param point  
 */
void PAGradientFill::doDrawState( const QPoint &point )
{
    Q_ASSERT( nState == StateIdle );

    // pointFactory 
    // - indicates the fill area. 
    // - its handle can not be moved.
    // - can produce intermediate 'stop' points when an attempt is made to drag it.
    pointFactory = point;

    if ( nType == StandardGradientLinear )
    {
        // pointStart is the [Start]
        // pointStop is the [Stop]
        // intermediate points will be between [Start] and [Stop]
        stopStart.first = 0;
        stopStart.second = QColor( Qt::black );
        stopStop.first = 1;
        stopStop.second = QColor( Qt::white );

        linear.pointStart = linear.pointStop = point;
    }
    else if ( nType == StandardGradientRadial )
    {
        // pointCenter is set but can not be moved. it is the [Start] 
        // pointCenterRadius can be moved. is is the [Stop]
        // intermediate points will be between [Start] and [Stop]
        // pointFocal can be moved (should stay within the 'center circle' and has no color associated with it
        stopStart.first = 0;
        stopStart.second = QColor( Qt::black );
        stopStop.first = 1;
        stopStop.second = QColor( Qt::white );

        radialSimple.pointCenter = point;
        radialSimple.pointCenterRadius = point;
        radialSimple.pointFocal = point;
    }
    else if ( nType == StandardGradientConical )
    {
        // pointCenter is set but can not be moved. it is the [Start] 
        // pointAngle can be moved. is is the [Stop]
        // intermediate points will be between [Start] and [Stop]
        stopStart.first = 0;
        stopStart.second = QColor( Qt::black );
        stopStop.first = 1;
        stopStop.second = QColor( Qt::white );

        conical.pointCenter = point;
        conical.pointAngle  = point;
    }

    // polygon
    // - this is an outline of the area to be filled
    polygon = getPolygon( pointFactory );

    nState = StateDraw;
    doManipulateState();
}

/*!
 * \brief Go to StateManipulate. 
 *  
 * Create handles and notify that we have 'changed'. 
 * 
 * \author pharvey (4/19/23)
 */
void PAGradientFill::doManipulateState()
{
    Q_ASSERT( nState == StateDraw );
    doCreateHandles();
    nState = StateManipulate;
    emit signalChanged();
}

/*!
 * \brief Go to StateIdle.
 *  
 * Reinit ourself but leave nType and nSpread alone. 
 * A press will get things started again. 
 *  
 * \author pharvey (4/19/23)
 */
void PAGradientFill::doIdleState()
{
    Q_ASSERT( nState != StateIdle );

    doDeleteHandles();
    stops.clear();
    vectorStopPoints.clear();
    polygon.clear();
    nState = StateIdle;
    update();

    // Warning: Emit this signal here and the app will crash.
    //          Did not determine why but commenting it out seems to be fine.
    // emit signalChanged();
}


/*!
 * \brief Create handles.
 *  
 * Order matters when handles share a position. Last handle will be found first. 
 *  
 */
void PAGradientFill::doCreateHandles()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // Order matters when handles share a position. Last handle will be found first.
    LHandle *pHandle;

    if ( nType == StandardGradientLinear )
    {
        pHandle = new LHandle( this, pLayerView, LHandle::TypePointFactory, pLayerView->mapFromScene( pointFactory ) );
        pHandle->setToolTip( tr("Drag to create a new point.") );
        vectorHandles.append( pHandle );
        pHandle->show();

        pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientStart, pLayerView->mapFromScene( linear.pointStart ) );
        pHandle->setToolTip( tr("Start point.\nDouble-Click to set color.\nDrop point here to remove.") );
        vectorHandles.append( pHandle );
        pHandle->show();

        pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientStop, pLayerView->mapFromScene( linear.pointStop ) );
        pHandle->setToolTip( tr("Stop point.\nDouble-Click to set color.\nDrop point here to remove.") );
        vectorHandles.append( pHandle );
        pHandle->show();
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        pHandle = new LHandle( this, pLayerView, LHandle::TypePointFactory, pLayerView->mapFromScene( pointFactory ) );
        pHandle->setToolTip( tr("Center\nDouble-Click to set color.\nDrag to create a new point.\nDrop point here to remove.") );
        vectorHandles.append( pHandle );
        pHandle->show();

        pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientFocal, pLayerView->mapFromScene( radialSimple.pointFocal ) );
        pHandle->setToolTip( tr("Focal point") );
        vectorHandles.append( pHandle );
        pHandle->show();

        pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientRadius, pLayerView->mapFromScene( radialSimple.pointCenterRadius ) );
        pHandle->setToolTip( tr("Radius\nDouble-Click to set color.\nDrop point here to remove.") );
        vectorHandles.append( pHandle );
        pHandle->show();
        return;
    }

    if ( nType == StandardGradientConical )
    {
        pHandle = new LHandle( this, pLayerView, LHandle::TypePointFactory, pLayerView->mapFromScene( conical.pointCenter ) );
        // pHandle->setToolTip( tr("Drag to create a new point.\nDrop point here to remove.") );
        vectorHandles.append( pHandle );
        pHandle->show();

        pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientAngle, pLayerView->mapFromScene( conical.pointAngle ) );
        pHandle->setToolTip( tr("Start/stop angle and radius") );
        vectorHandles.append( pHandle );
        pHandle->show();
        return;
    }
}


// react to zoom
void PAGradientFill::doSyncHandles()
{
    vectorHandles[PAGradientFillFactory]->setCenter( pLayerView->mapFromScene( pointFactory ) );

    if ( nType == StandardGradientLinear )
    {
        vectorHandles[PAGradientFillLinearStart]->setCenter( pLayerView->mapFromScene( linear.pointStart ) );
        vectorHandles[PAGradientFillLinearStop]->setCenter( pLayerView->mapFromScene( linear.pointStop ) );

        for ( int n = 0; n < vectorStopPoints.count(); n++ )
        {
            vectorHandles[PAGradientFillLinearIntermediates + n]->setCenter( pLayerView->mapFromScene( vectorStopPoints[n] ) );
        }
    }
    else if ( nType == StandardGradientRadial )
    {
        vectorHandles[PAGradientFillRadialRadius]->setCenter( pLayerView->mapFromScene( radialSimple.pointCenterRadius ) );
        vectorHandles[PAGradientFillRadialFocal]->setCenter( pLayerView->mapFromScene( radialSimple.pointFocal ) );

        for ( int n = 0; n < vectorStopPoints.count(); n++ )
        {
            vectorHandles[PAGradientFillRadialIntermediates + n]->setCenter( pLayerView->mapFromScene( vectorStopPoints[n] ) );
        }
    }
    else if ( nType == StandardGradientConical )
    {
        vectorHandles[PAGradientFillConicalAngle]->setCenter( pLayerView->mapFromScene( conical.pointAngle ) );
    }
}

void PAGradientFill::doMoveHandle( const QPoint &pointPos )
{
    Q_ASSERT( pHandle );

    QPoint pointViewPos = pLayerView->mapFromScene( pointPos );

    // standard handles
    if ( nType == StandardGradientLinear )
    {
        if ( pHandle == vectorHandles[PAGradientFillLinearStart] )
        {
            pHandle->setCenter( pointViewPos );
            linear.pointStart = pointPos;
            doUpdateStops();
            update();
            return;
        }
        if ( pHandle == vectorHandles[PAGradientFillLinearStop] )
        {
            pHandle->setCenter( pointViewPos );
            linear.pointStop = pointPos;
            doUpdateStops();
            update();
            return;
        }
        // create intermediate stop point
        if ( pHandle == vectorHandles[PAGradientFillFactory] )
        {
            // create handle
            pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientIntermediate, pLayerView->mapFromScene( pointFactory ) );
            pHandle->setToolTip( tr("Color point.\nDouble-Click to set color.\nDrop on Start/Stop handle to delete.") );
            pHandle->show();
            // add point
            vectorStopPoints.append( pointFactory );
            // add stop
            stops.append( QGradientStop() );
            // add handle
            vectorHandles.append( pHandle );
            // ensure point and handle on line
            // set qreal in stop
            doUpdateStop( vectorStopPoints.count() - 1 );
            update();
            return;
        }
        // move intermediate stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillLinearIntermediates;
            vectorStopPoints[n] = pointPos;
            doUpdateStop( n );
            update();
            return;
        }
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        if ( pHandle == vectorHandles[PAGradientFillRadialRadius] )
        {
            pHandle->setCenter( pointViewPos );
            radialSimple.pointCenterRadius = pointPos;
            doUpdateStops();
            update();
            return;
        }
        if ( pHandle == vectorHandles[PAGradientFillRadialFocal] )
        {
            pHandle->setCenter( pointViewPos );
            radialSimple.pointFocal = pointPos;
            doUpdateStops();
            update();
            return;
        }
        // create intermediate stop point
        if ( pHandle == vectorHandles[PAGradientFillFactory] )
        {
            // create handle
            pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientIntermediate, pLayerView->mapFromScene( pointFactory ) );
            pHandle->setToolTip( tr("Color point.\nDouble-Click to set color.\nDrop on Start/Stop handle to delete.") );
            pHandle->show();
            // add point
            vectorStopPoints.append( pointFactory );
            // add stop
            stops.append( QGradientStop() );
            // add handle
            vectorHandles.append( pHandle );
            // ensure point and handle on line
            // set qreal in stop
            doUpdateStop( vectorStopPoints.count() - 1 );
            update();
            return;
        }
        // move intermediate stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillRadialIntermediates;
            vectorStopPoints[n] = pointPos;
            doUpdateStop( n );
            update();
            return;
        }
        return;
    }

    if ( nType == StandardGradientConical )
    {
        if ( pHandle == vectorHandles[PAGradientFillConicalAngle] )
        {
            pHandle->setCenter( pointViewPos );
            conical.pointAngle = pointPos;
            doUpdateStops();
            update();
            return;
        }
        // create intermediate stop point
        if ( pHandle == vectorHandles[PAGradientFillFactory] )
        {
            // create handle
            pHandle = new LHandle( this, pLayerView, LHandle::TypeGradientIntermediate, pLayerView->mapFromScene( pointFactory ) );
            pHandle->setToolTip( tr("Color point.\nDouble-Click to set color.\nDrop on Start/Stop handle to delete.") );
            pHandle->show();
            // add point
            vectorStopPoints.append( pointFactory );
            // add stop
            stops.append( QGradientStop() );
            // add handle
            vectorHandles.append( pHandle );
            // ensure point and handle on line
            // set qreal in stop
            doUpdateStop( vectorStopPoints.count() - 1 );
            update();
            return;
        }
        // move intermediate stop point
        {
            int n = vectorHandles.indexOf( pHandle ) - PAGradientFillConicalIntermediates;
            vectorStopPoints[n] = pointPos;
            doUpdateStop( n );
            update();
            return;
        }
    }
}

// assumes the vectorHandles, stops, and vectorStopPoints have point represented
void PAGradientFill::doUpdateStop( int n )
{
    // ensure that all of our vectors are in harmony
    Q_ASSERT( n >= 0 );
    Q_ASSERT( n < vectorStopPoints.count() );
    Q_ASSERT( n < stops.count() );
    Q_ASSERT( vectorStopPoints.count() == stops.count() );


    if ( nType == StandardGradientLinear )
    {
        Q_ASSERT( vectorHandles.count() - PAGradientFillLinearIntermediates == stops.count() ); 

        QPointF pointOnLine = CBDMath::getNearestPointOnLine( vectorStopPoints[n], linear.pointStart, linear.pointStop );

        qreal nDistance             = CBDMath::getDistance( linear.pointStart, linear.pointStop );
        qreal nDistanceFromStart    = CBDMath::getDistance( linear.pointStart, pointOnLine );
        stops[n].first = nDistanceFromStart / nDistance; // value should always be 0-1
        vectorStopPoints[n] = pointOnLine.toPoint();
        vectorHandles[PAGradientFillLinearIntermediates + n]->setCenter( pLayerView->mapFromScene( pointOnLine ) );
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        Q_ASSERT( vectorHandles.count() - PAGradientFillRadialIntermediates == stops.count() ); 

        QPointF pointOnLine = CBDMath::getNearestPointOnLine( vectorStopPoints[n], radialSimple.pointCenter, radialSimple.pointCenterRadius );

        qreal nDistance             = CBDMath::getDistance( radialSimple.pointCenter, radialSimple.pointCenterRadius );
        qreal nDistanceFromStart    = CBDMath::getDistance( radialSimple.pointCenter, pointOnLine );
        stops[n].first = nDistanceFromStart / nDistance; // value should always be 0-1
        vectorStopPoints[n] = pointOnLine.toPoint();
        vectorHandles[PAGradientFillRadialIntermediates + n]->setCenter( pLayerView->mapFromScene( pointOnLine ) );
        return;
    }

    if ( nType == StandardGradientConical )
    {
        Q_ASSERT( vectorHandles.count() - PAGradientFillConicalIntermediates == stops.count() ); 

        QPointF pointOnLine = CBDMath::getNearestPointOnLine( vectorStopPoints[n], conical.pointCenter, conical.pointAngle );

        qreal nDistance             = CBDMath::getDistance( conical.pointCenter, conical.pointAngle );
        qreal nDistanceFromStart    = CBDMath::getDistance( conical.pointCenter, pointOnLine );
        stops[n].first = nDistanceFromStart / nDistance; // value should always be 0-1
        vectorStopPoints[n] = pointOnLine.toPoint();
        vectorHandles[PAGradientFillConicalIntermediates + n]->setCenter( pLayerView->mapFromScene( pointOnLine ) );
        return;
    }
}

// start or 'final' stop have moved so...
// - assumes we are only doing linear at the moment
void PAGradientFill::doUpdateStops()
{
    for ( int n = 0; n < vectorStopPoints.count(); n++ )
    {
        doUpdateStop( n );
    }
}

// call shouldRemoveStop() before calling here
void PAGradientFill::doRemoveStop()
{
    if ( nType == StandardGradientLinear )
    {
        // this is based off of a handle being dropped so...
        if ( !pHandle ) return;
        // we can only remove TypeGradientIntermediate handles
        if ( pHandle->getType() != LHandle::TypeGradientIntermediate ) return;

        // remove handle
        int nHandle = vectorHandles.indexOf( pHandle );
        delete vectorHandles.takeAt( nHandle );
        // remove from stops
        stops.remove( nHandle - PAGradientFillLinearIntermediates );
        vectorStopPoints.remove( nHandle - PAGradientFillLinearIntermediates );
        return;
    }

    if ( nType == StandardGradientRadial )
    {
        // this is based off of a handle being dropped so...
        if ( !pHandle ) return;
        // we can only remove TypeGradientIntermediate handles
        if ( pHandle->getType() != LHandle::TypeGradientIntermediate ) return;

        // remove handle
        int nHandle = vectorHandles.indexOf( pHandle );
        delete vectorHandles.takeAt( nHandle );
        // remove from stops
        stops.remove( nHandle - PAGradientFillRadialIntermediates );
        vectorStopPoints.remove( nHandle - PAGradientFillRadialIntermediates );
        return;
    }

    if ( nType == StandardGradientConical )
    {
        // this is based off of a handle being dropped so...
        if ( !pHandle ) return;
        // we can only remove TypeGradientIntermediate handles
        if ( pHandle->getType() != LHandle::TypeGradientIntermediate ) return;

        // remove handle
        int nHandle = vectorHandles.indexOf( pHandle );
        delete vectorHandles.takeAt( nHandle );
        // remove from stops
        stops.remove( nHandle - PAGradientFillConicalIntermediates );
        vectorStopPoints.remove( nHandle - PAGradientFillConicalIntermediates );
        return;
    }
}

bool PAGradientFill::shouldRemoveStop()
{
    // this is based off of a handle being dropped so...
    if ( !pHandle ) return false;
    // we can only remove TypeGradientIntermediate handles
    if ( pHandle->getType() != LHandle::TypeGradientIntermediate ) return false;
    // deleted by dropping handle on a TypePointFactory handle (same place it was created)
    if ( nType == StandardGradientLinear )
    {
        if ( getHandleUnder( pHandle, LHandle::TypeGradientStart ) ) return true;
        if ( getHandleUnder( pHandle, LHandle::TypeGradientStop ) ) return true;
    }
    else if ( nType == StandardGradientRadial )
    {
        if ( getHandleUnder( pHandle, LHandle::TypeGradientRadius ) ) return true;
        if ( getHandleUnder( pHandle, LHandle::TypePointFactory ) ) return true;
    }
    else if ( nType == StandardGradientConical )
    {
        if ( getHandleUnder( pHandle, LHandle::TypeGradientAngle ) ) return true;
        if ( getHandleUnder( pHandle, LHandle::TypePointFactory ) ) return true;
    }

    return false;
}

/*!
 * \brief Returns a polygon representing a boundary. 
 *  
 * The boundary is defined by not being the color at pointFactory starting at pointFactory. 
 *  
 * Uses the 'Square tracing algorithm'. 
 *  
 * \author pharvey (2/17/23)
 * 
 * \param point  
 * 
 * \return QPolygon The inside of the polygon is the area outlined.
 */
QPolygon PAGradientFill::getPolygon( const QPoint &pointFactory )
{
    QImage *    pImage      = pLayer->getImageRef();
    int         nX          = pointFactory.x();
    int         nY          = pointFactory.y();
    QColor      colorSeed   = pImage->pixelColor( pointFactory );
    QPolygon    polygon;

    // go west until we hit a boundary (or go off image)
    do
    {
        nX--;
        if ( isBoundary( pImage, colorSeed, QPoint( nX, nY ) ) ) break; 
    } while ( 1 );

    // start point
    QPoint pointStart( nX, nY );
    polygon.append( pointStart );

    //
    QPoint pointStep = getLeft( QPoint( 0, 1 ) );               
    QPoint point = pointStart + pointStep;
    while ( point != pointStart )
    {
        if ( isBoundary( pImage, colorSeed, point ) )
        {
            polygon.append( point );
            pointStep = getLeft( pointStep );
            point = point + pointStep;
        }
        else
        {
            point = point - pointStep;
            pointStep = getRight( pointStep );
            point = point + pointStep;
        }
    }

    return polygon;
}


