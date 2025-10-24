#include "LibInfo.h"
#include "DLinePolyAuto.h"

#include <math.h>
#include <WLineEndComboBox.h>
#include <AWPropWidget.h>
#include <RRouteAStar.h>

#include "DDiagram.h"
#include "DRectangle.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"
#include "DRouteSource.h"
#include "DSourcesWidget.h"

DLinePolyAuto::DLinePolyAuto( ADObject *pObjectParent, const QString &stringName )
    : DLinePoly( pObjectParent, stringName )
{
    pRouteSource    = nullptr;
    pRoute          = nullptr;
}

DLinePolyAuto::~DLinePolyAuto()
{
}

QPixmap DLinePolyAuto::getIcon()
{
    return QPixmap( ":D/Polyline" );
}

void DLinePolyAuto::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // is the handle a source?
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        if ( !pRoute )
        {
            DDiagram *pDiagram = getDiagram();
            QSize size = pDiagram->getCell();
            pRouteSource = new DRouteSource( pDiagram, QSize( size.width() / 4, size.height() / 4 ) );
            pRoute = new RRouteAStar( pRouteSource );
            pRoute->setHeuristic( RRouteAStar::HeuristicDiagonal );
            pRoute->setDiagonal( true );
            pRoute->setStraightLinePreference( true );
            pRoute->setReduceRoutePoints( true );
        }

        QList<QPointF> l = pRoute->getRoute( getProxy()->mapToScene( polygon.first() ), getProxy()->mapToScene( polygon.last() ) );
        if ( !l.isEmpty() )
        {
            // we do not want Begin and End so...
            l.removeFirst();
            l.removeLast();
            // apply auto route - merge even when list is empty as points may need to get removed
            doMergePoints( l );
        }
        doMoveSource( getSource( pHandle ), pointScene );
        return;
    }

    // is the handle for creating a new line segment?
    if ( pHandle->getFunction() == DHandle::FunctionNewLineSegment )
    {
        // insert a point into polygon

        // this is the point before the handle (this handle is between two points but we use point before for reference)
        int nPoint = pHandle->getIndex();

        // insert before the point AFTER the handle this will break our line segment into two
        polygon.insert( nPoint + 1, getProxy()->mapFromScene( pHandle->pos() ) );
        
        // our new point  
        int nPointNew = nPoint + 1;

        // bump all of the values above ours by 1 freeing up our index 
        {
            DHandle *p;
            foreach( p, vectorHandles )
            {
                if ( p->getIndex() > nPoint )
                    p->setIndex( p->getIndex() + 1 );
            }
        }

        // remember that the begin point of segment is the reference point for a FunctionNewLineSegment
        // insert a new FunctionNewLineSegment handle before this handle
        {
            int nHandle = vectorHandles.indexOf( pHandle ); 
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ));
            DHandle *p = new DHandle( this, DHandle::FunctionNewLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            p->setIndex( nPoint );
            vectorHandles.insert( nHandle, p );
            getProxy()->scene()->addItem( p );
            // p->stackBefore( pHandle );
            p->setVisible( true );
        }

        // insert a new FunctionNewLineSegment handle after this handle
        {
            int nHandle = vectorHandles.indexOf( pHandle );
            QRectF r( polygon.at( nPointNew ), polygon.at( nPointNew + 1 ));
            DHandle *p = new DHandle( this, DHandle::FunctionNewLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            p->setIndex( nPointNew );
            vectorHandles.insert( nHandle + 1, p );
            getProxy()->scene()->addItem( p );
            // p->stackBefore( pHandle );
            p->setVisible( true );
        }

        // change *this* handle from FunctionNewLineSegment to FunctionRemoveLineSegment
        // it now represents an intermediate point
        pHandle->setFunction( DHandle::FunctionRemoveLineSegment );

        // we are the new point
        pHandle->setIndex( nPointNew );

        // move this handle
        doHandleMove( pHandle, pointScene );

        return;
    }

    // must be for moving an intermediate point (DHandle::FunctionRemoveLineSegment)
    doHandleMove( pHandle, pointScene );
}

void DLinePolyAuto::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);
    // life is easier if we can just skip this event
    // so if UI feels right do nothing here

    if ( pRoute ) 
    {
        delete pRoute;
        pRoute = nullptr;
        delete pRouteSource;
        pRouteSource = nullptr;
    }
}

void DLinePolyAuto::doRoute( DSource *pSource, const QPointF &pointScene )
{
    Q_ASSERT( !pRoute );

    DDiagram *pDiagram = getDiagram();
    if ( pDiagram->isOffScene( pointScene ) )
    { 
        doMoveSource( pSource, pointScene );
        return;
    }

    QSize size = pDiagram->getCell();
    pRouteSource = new DRouteSource( pDiagram, QSize( size.width() / 4, size.height() / 4 ) );
    pRoute = new RRouteAStar( pRouteSource );
    pRoute->setHeuristic( RRouteAStar::HeuristicDiagonal );
    pRoute->setDiagonal( true );
    pRoute->setStraightLinePreference( false );
    pRoute->setReduceRoutePoints( true );

    // lets ignore self for blocking etc
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

    QList<QPointF> l = pRoute->getRoute( pointBegin, pointEnd );
    if ( !l.isEmpty() )
    {
        // we do not want Begin and End so...
        l.removeFirst();
        l.removeLast();
        // apply auto route - merge even when list is empty as points may need to get removed
        doMergePoints( l );
    }

    delete pRoute;
    pRoute = nullptr;
    delete pRouteSource;
    pRouteSource = nullptr;

    doMoveSource( pSource, pointScene );
}

