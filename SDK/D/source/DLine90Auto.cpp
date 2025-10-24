#include "LibInfo.h"
#include "DLine90Auto.h"

#include <math.h>
#include <RRouteAStar.h>

#include "DDiagram.h"

#include "DRectangle.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"
#include "DRouteSource.h"

DLine90Auto::DLine90Auto( ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
    DDiagram *pDiagram = nullptr;
    if ( pObjectParent->inherits( "DDiagram" ) ) pDiagram = (DDiagram*)pObjectParent;
    else pDiagram = (DDiagram *)getParent( "DDiagram" );
    Q_ASSERT( pDiagram );

    pRouteSource    = nullptr;
    pRoute          = nullptr;

    polygon.append( QPoint( pDiagram->getCell().width(), pDiagram->getCell().height() / 2 ) );

    vectorSegments.append( Qt::Vertical );
    vectorSegments.append( Qt::Horizontal );
}

DLine90Auto::~DLine90Auto()
{
}

QPixmap DLine90Auto::getIcon()
{
    return QPixmap( ":D/Line90Auto" );
}

void DLine90Auto::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // QPointF pointRequested = getProxy()->mapFromScene( pEvent->scenePos() );
    // getProxy()->doPrepareGeometryChange();

    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        if ( !pRoute ) doNewRoute();

        doRoute( getProxy()->mapToScene( polygon.first() ), getProxy()->mapToScene( polygon.last() ) );
        doMoveSource( getSource( pHandle ), pointScene );
        return;
    }

    // must be for moving (ie text handle)
    doHandleMove( pHandle, pointScene );
}

void DLine90Auto::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);
    // life is easier if we can just skip this event
    // so if UI feels right do nothing here

    if ( pRoute ) doDeleteRoute();
}

void DLine90Auto::doRoute( DSource *pSource, const QPointF &pointScene )
{
    Q_ASSERT( !pRoute );

    DDiagram *pDiagram = getDiagram();
    if ( pDiagram->isOffScene( pointScene ) )
    { 
        doMoveSource( pSource, pointScene );
        return;
    }

    doNewRoute();

    // ignore any rectangle (on either end) for blocking etc                                        
    QList<DObject*> listIgnore;                                                 
    listIgnore.append( this );                                                  
    if ( pSource->getRectangle() ) listIgnore.append( pSource->getRectangle() );
    pRouteSource->setIgnore( listIgnore );                                      

    QPointF pointBegin;
    QPointF pointEnd;

    if ( pSource->isBegin() ) 
    {
        pointBegin  = pointScene;
        pointEnd    = getProxy()->mapToScene( polygon.last() );
    }
    else
    {
        pointBegin  = getProxy()->mapToScene( polygon.first() );
        pointEnd    = pointScene;
    }

    DIntersectedLine IntersectedLineBegin   = getIntersectedLine( CBD::EOLBegin );
    DIntersectedLine IntersectedLineEnd     = getIntersectedLine( CBD::EOLEnd );
    if ( !IntersectedLineBegin.p2().isNull() )  pointBegin  = IntersectedLineBegin.p2();
    if ( !IntersectedLineEnd.p2().isNull() )    pointEnd    = IntersectedLineEnd.p2();

    doRoute( pointBegin, pointEnd );
    doDeleteRoute();
    doMoveSource( pSource, pointScene );
}

void DLine90Auto::doNewRoute()
{
    pRouteSource = new DRouteSource( getDiagram(), getDiagram()->getCell() );
    pRoute = new RRouteAStar( pRouteSource );
    pRoute->setHeuristic( RRouteAStar::HeuristicManhatten );
    pRoute->setDiagonal( false );
    pRoute->setStraightLinePreference( true );
    pRoute->setStraightLinePreference( 1.0 );
    pRoute->setReduceRoutePoints( true );
}

void DLine90Auto::doRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache )
{
    QList<QPointF> l = pRoute->getRoute( pointBegin, pointEnd, bCache );
    if ( l.isEmpty() ) return;

    // we do not want Begin and End so...
    l.removeFirst();
    l.removeLast();
    // apply auto route - merge even when list is empty as points may need to get removed
    doMergePoints( l );

    // ensure first and last segments are on a 90 degree angle
    if ( polygon.count() >= 3 )
    {
        // begin
        int n1 = 0;
        int n2 = 1;
        Qt::Orientation n = getOrientation( polygon.at(n1), polygon.at(n2) );
        if ( n == Qt::Vertical ) polygon[n2].setX( polygon.at(n1).x() );
        else polygon[n2].setY( polygon.at(n1).y() );
        // end
        n1 = polygon.count() - 1;    
        n2 = polygon.count() - 2;    
        n = getOrientation( polygon.at(n1), polygon.at(n2) );
        if ( n == Qt::Vertical ) polygon[n2].setX( polygon.at(n1).x() );
        else polygon[n2].setY( polygon.at(n1).y() );
    }
}

void DLine90Auto::doDeleteRoute()
{
    delete pRoute;
    pRoute = nullptr;
    delete pRouteSource;
    pRouteSource = nullptr;
}


