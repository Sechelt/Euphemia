#include "LibInfo.h"
#include "DLine90.h"

#include <math.h>

#include "DDiagram.h"

#include "DRectangle.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"

DLine90::DLine90( Qt::Orientation nOrientation, ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
    this->nOrientation = nOrientation;

    // we have inherited a straight line... turn it into a right angle
    QPointF pointBegin = polygon.at( 0 );
    QPointF pointEnd = polygon.at( 1 );
    if ( nOrientation == Qt::Horizontal )
    {
        polygon.insert( 1, QPointF( pointEnd.x(), pointBegin.y() ) );
        vectorSegments.append( Qt::Horizontal );
        vectorSegments.append( Qt::Vertical );
    }
    else
    {
        polygon.insert( 1, QPointF( pointBegin.x(), pointEnd.y() ) );
        vectorSegments.append( Qt::Vertical );
        vectorSegments.append( Qt::Horizontal );
    }
}

DLine90::~DLine90()
{
}

QPixmap DLine90::getIcon()
{
    return QPixmap( ":D/Line" );
}

void DLine90::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    QPointF pointRequested = getProxy()->mapFromScene( pointScene );
    getProxy()->doPrepareGeometryChange();

    // SLIDE and/or STRETCH
    // - here we move the source (which is either the begin or the end point of the line)
    // - a source can be moved on x and y axis so it will simply adopt the new pos - what happens next depends upon the line segment orientation
    // - the length of the line segment may change
    // - the length of the next or prev line segment may change 
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        // SOURCE
        // we always have exactly 2 sources (0=begin, 1=end) so check which one we are dealing with
        int nSource = -1;
        if ( pHandle == vectorSources.at( 0 )->getHandle() ) nSource = 0;
        else if ( pHandle == vectorSources.at( 1 )->getHandle() ) nSource = 1;
        Q_ASSERT( nSource >= 0 );

        // POINTS
        // get source point and point on other end of segment
        int nPointSource = ( nSource == 0 ? 0 : polygon.count() - 1 );
        int nPointOther = ( nSource == 0 ? 1 : polygon.count() - 2 );
        int nPointBegin = ( nSource == 0 ? 0 : polygon.count() - 2 );
        // int nPointEnd = ( nSource == 0 ? 1 : polygon.count() - 1 );

        // QPointF pointSource = polygon.at( nPointSource );
        QPointF pointOther  = polygon.at( nPointOther );

        // ORIENTATION
        // - adjust other point on line segment
        if ( vectorSegments.at( nPointBegin ) == Qt::Vertical )
             pointOther.setX( pointRequested.x() ); 
        else
             pointOther.setY( pointRequested.y() ); 
 
        polygon[nPointSource]   = pointRequested;  // source always accepts new position unqualified
        polygon[nPointOther]    = pointOther;          // slide line segment in one axis

        if ( isSelected() ) doHandlesSync();
        doSourcesSync();
        return;
    }

    // SLIDE
    // - here we slide a line segment
    // - the adjacent line segment(s) have their length changed
    // - may cause a source to move (and disconnect if need be)
    if ( pHandle->getFunction() == DHandle::FunctionSlide )
    {
        // get point before and after handle
        int nPoint1 = pHandle->getIndex();
        int nPoint2 = nPoint1 + 1;
        QPointF point1 = polygon.at( nPoint1 );
        QPointF point2 = polygon.at( nPoint2 );

        // are we moving on horizontal or vertical axis?
        if ( vectorSegments.at( nPoint1 ) == Qt::Vertical )
        {
            point1.setX( pointRequested.x() ); 
            point2.setX( pointRequested.x() ); 
        }
        else
        {
            point1.setY( pointRequested.y() ); 
            point2.setY( pointRequested.y() ); 
        }

        polygon[nPoint1] = point1;
        polygon[nPoint2] = point2;

        if ( isSelected() ) doHandlesSync();
        doSourcesSync();
        return;
    }

    // is corner handle?
    if ( pHandle->getFunction() == DHandle::FunctionSlideBoth )
    {
        // get the 3 points we need for working with segment before and segment after
        int nPoint2 = pHandle->getIndex();
        int nPoint1 = nPoint2 - 1;
        int nPoint3 = nPoint2 + 1;

        QPointF point1 = polygon.at( nPoint1 );
        // QPointF point2 = polygon.at( nPoint2 );
        QPointF point3 = polygon.at( nPoint3 );

        if ( vectorSegments.at( nPoint1 ) == Qt::Vertical )
        {
            point1.setX( pointRequested.x() ); 
            point3.setY( pointRequested.y() ); 
        }
        else
        {
            point1.setY( pointRequested.y() ); 
            point3.setX( pointRequested.x() ); 
        }

        polygon[nPoint1] = point1;
        polygon[nPoint2] = pointRequested;  // corner always accepts new position unqualified
        polygon[nPoint3] = point3;

        if ( isSelected() ) doHandlesSync();
        doSourcesSync();
        return;
    }
}

void DLine90::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);
    // life is easier if we can just skip this event
    // so if UI feels right do nothing here
}

/*!
 * \brief Route line. 
 *  
 * In this case we always accept pointScene. The only thing we are concerned about is adjusting 
 * the position of the single intermediate point - the elbow. 
 *  
 * \author pharvey (4/20/20)
 * 
 * \param pSource 
 * \param pointScene 
 */
void DLine90::doRoute( DSource *pSource, const QPointF &pointScene )
{
    Q_ASSERT(polygon.count() == 3);

    QPointF pointBegin   = polygon.at( 0 );
    QPointF pointEnd     = polygon.at( 2 );

    if ( pSource->getType() == CBD::EOLBegin )
        pointBegin = getProxy()->mapFromScene( pointScene ); 
    else
        pointEnd = getProxy()->mapFromScene( pointScene ); 

    if ( nOrientation == Qt::Horizontal )
        polygon[1] = QPoint( pointEnd.x(), pointBegin.y() );
    else
        polygon[1] = QPoint( pointBegin.x(), pointEnd.y() );

    doMoveSource( pSource, pointScene );
}

void DLine90::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // resize so we have;
    // - one for each point in polygon (except first and last) (polygon.count() - 2)
    // - one for each line segment (polygon.count() - 1)
    vectorHandles.resize( polygon.count() * 2 - 1 - 2 );

    int nHandle = 0;
    // process polygon - skip last point
    for ( int nPoint = 0; nPoint < (polygon.count() - 1); nPoint++ )
    {
        // FunctionSlide
        { 
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ) );
            DHandle *pHandle = new DHandle( this, DHandle::FunctionSlide, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            pHandle->setIndex( nPoint );
            vectorHandles[nHandle] = pHandle;
            getProxy()->scene()->addItem( pHandle );
            nHandle++;
        }
        
        // begin has a source handle (not in vectorHandles) so skip
        if ( nPoint == 0 ) continue;

        // we skipped the first point (above) and we do not process the last point
        // so we must be an intermediate point - a corner
        {
            DHandle *pHandle = new DHandle( this, DHandle::FunctionSlideBoth, DHandle::ConnectTypeNone, getProxy()->mapToScene( polygon.at( nPoint ) ) );
            pHandle->setIndex( nPoint );
            vectorHandles[nHandle] = pHandle;
            getProxy()->scene()->addItem( pHandle );
            nHandle++;
        }
    }

    // Center handle for floating text
    { 
        DHandle *pHandle = new DHandle( this, DHandle::FunctionFloatingTextCenter, DHandle::ConnectTypeNone, getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );
        vectorHandles.append( pHandle );
        getProxy()->scene()->addItem( pHandle );
    }
}

void DLine90::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    DHandle *pHandle;
    foreach( pHandle, vectorHandles )
    {
        int nPoint = pHandle->getIndex();
        if ( pHandle->getFunction() == DHandle::FunctionSlide )
        {
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ));
            pHandle->setPos( getProxy()->mapToScene( r.center() ) );
        }
        else if ( pHandle->getFunction() == DHandle::FunctionSlideBoth )
            pHandle->setPos( getProxy()->mapToScene( polygon.at(nPoint) ) );
        else if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
        {
            pHandle->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter ); 
            pFloatingTextCenter->setPos( pHandle->pos() );
        }
    }
}


