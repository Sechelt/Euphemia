#include "LibInfo.h"
#include "DLineDynamic.h"

#include <RRouteAStar.h>

#include "DRouteSource.h"
#include "DGraphicsProxyItem.h"
#include "DRectangle.h"
#include "DSource.h"

#include "DDiagram.h"

DLineDynamic::DLineDynamic( ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
    pRouteSource    = nullptr;
    pRoute          = nullptr;

    nStyleEnd = W::WLineEndArrow; 

    // default to offset by (route) cell
    nOffset = OffsetCell;
    sizeOffset.setWidth( 48 );
    sizeOffset.setHeight( 48 );
}

DLineDynamic::~DLineDynamic()
{
}

void DLineDynamic::setOffset( OffsetTypes n )
{
    if ( n == nOffset ) return;
    nOffset = n;
    setModified();
}

void DLineDynamic::setOffsetSize( const QSize size )
{
    if ( size == sizeOffset ) return;
    sizeOffset = size;
    setModified(); 
}

QPixmap DLineDynamic::getIcon()
{
    return QPixmap( ":D/Dynamic" );
}

/*!
 * \brief Get point (begin or end) for passing to a route. 
 *  
 * We want to avoid starting/ending a route within a rectangle. 
 * This is done by ensuring that begin/end point is beyond the edge of the rectangle by (at least) a cell. 
 *  
 * This ensures (under the usual case) that the 1st line segment is not entirely within the rectangle - negating truncation. 
 *  
 * This also ensures that there is space for drawing line-end decorations *before* any change of direction. 
 *  
 * \author pharvey (5/16/20)
 * 
 * \param IntersectedLine 
 * 
 * \return QPointF 
 */
QPointF DLineDynamic::getOffset( const DIntersectedLine &IntersectedLine )
{
    Q_ASSERT( pRoute );
    Q_ASSERT( !IntersectedLine.p2().isNull() );

    // calculate offset
    QSize size( 0, 0 );
    switch ( nOffset )
    {
        case OffsetDynamic:
            // \todo Calculate base upon decoration at IntersectedLine::nEOL combined
            // with position of rectangle on grid + IntersectedLine::p1 (chance of routing onto self)
            break;
        case OffsetCell:
            size  = pRouteSource->getCellSize();
            break;
        case OffsetCustom:
            size = sizeOffset;
            break;
        case OffsetNone:
            break;
    }

    // apply offset to rectangle edge
    QPointF point = IntersectedLine.p2();
    switch ( IntersectedLine.nDir )
    {
        case CBD::DirectionN:
            point.setY( point.y() - size.height() );
            break;
        case CBD::DirectionE:
            point.setX( point.x() + size.width() );
            break;
        case CBD::DirectionS:
            point.setY( point.y() + size.height() );
            break;
        case CBD::DirectionW:
            point.setX( point.x() - size.width() );
            break;
        case CBD::DirectionNE:
            point.setY( point.y() - size.height() );
            point.setX( point.x() + size.width() );
            break;
        case CBD::DirectionSE:
            point.setY( point.y() + size.height() );
            point.setX( point.x() + size.width() );
            break;
        case CBD::DirectionSW:
            point.setY( point.y() + size.height() );
            point.setX( point.x() - size.width() );
            break;
        case CBD::DirectionNW:
            point.setY( point.y() - size.height() );
            point.setX( point.x() - size.width() );
            break;
        case CBD::DirectionUnknown:
            break;
    }

    return point;
}

void DLineDynamic::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    DDiagram *pDiagram = getDiagram();

    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        QPointF pointBegin      = getProxy()->mapToScene( polygon.first() );
        QPointF pointEnd        = getProxy()->mapToScene( polygon.last() );
        QPoint pointBeginCell   = pDiagram->mapSceneToCell( pointBegin );
        QPoint pointEndCell     = pDiagram->mapSceneToCell( pointEnd );

        if ( pDiagram->isOffGrid( pointBeginCell ) || pDiagram->isOffGrid( pointEndCell ) )
        { 
           doMoveSource( getSource( pHandle ), pointScene );
           return;
        }

        if ( !pRoute ) doNewRoute();

        DIntersectedLine IntersectedLineBegin   = getIntersectedLine( CBD::EOLBegin );
        DIntersectedLine IntersectedLineEnd     = getIntersectedLine( CBD::EOLEnd );

        if ( !IntersectedLineBegin.p2().isNull() )  pointBegin  = getOffset( IntersectedLineBegin );
        if ( !IntersectedLineEnd.p2().isNull() )    pointEnd    = getOffset( IntersectedLineEnd );

        doRoute( pointBegin, pointEnd, true );
        doMoveSource( getSource( pHandle ), pointScene );
        return;
    }

    // must be for moving (ie text handle)
    doHandleMove( pHandle, pointScene );
}

void DLineDynamic::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);

    if ( pRoute ) doDeleteRoute();
}

void DLineDynamic::doRoute( DSource *pSource, const QPointF &pointScene )
{
    Q_ASSERT( !pRoute );
    
    DDiagram *pDiagram = getDiagram();
        
    // scene coords
    QPointF pointBegin;
    QPointF pointEnd;
    if ( pSource->isBegin() ) 
    {
        pointBegin = pointScene;
        pointEnd   = getProxy()->mapToScene( polygon.last() );
    }              
    else           
    {              
        pointBegin = getProxy()->mapToScene( polygon.first() );
        pointEnd   = pointScene;
    }

    // cell coords
    QPoint pointBeginCell  = pDiagram->mapSceneToCell( pointBegin );
    QPoint pointEndCell    = pDiagram->mapSceneToCell( pointEnd );

    // boundary check
    if ( pDiagram->isOffGrid( pointBeginCell ) || pDiagram->isOffGrid( pointEndCell ) )
    { 
       doMoveSource( pSource, pointScene );
       return;
    }

    doNewRoute();
    // lets ignore self for blocking etc
    // QList<DObject*> listIgnore;
    // listIgnore.append( this );
    // if ( pSource->getRectangle() ) listIgnore.append( pSource->getRectangle() );
    // pRouteSource->setIgnore( listIgnore );

    DIntersectedLine IntersectedLineBegin   = getIntersectedLine( CBD::EOLBegin );
    DIntersectedLine IntersectedLineEnd     = getIntersectedLine( CBD::EOLEnd );

    if ( !IntersectedLineBegin.p2().isNull() )  pointBegin  = getOffset( IntersectedLineBegin );
    if ( !IntersectedLineEnd.p2().isNull() )    pointEnd    = getOffset( IntersectedLineEnd );

    doRoute( pointBegin, pointEnd, true );

    doDeleteRoute();

    doMoveSource( pSource, pointScene );
}

void DLineDynamic::doNewRoute()
{
    QSize size = getDiagram()->getCell();
    pRouteSource = new DRouteSource( getDiagram(), QSize( size.width() / 4, size.height() / 4 ) );
    // RRouteDynamic does not do angle routes well
    /*
    pRoute = new RRouteDynamic( pRouteSource );
    pRoute->setCostBreakStraightLine( 2 );
    pRoute->setCostLeaveBeginAxis( 2 );
    pRoute->setCostLeaveEndAxis( 2 );
    pRoute->setCostCrossLine( 2 );
    */

    pRoute = new RRouteAStar( pRouteSource );
    pRoute->setDiagonal( true ); // enable 8 dir (default is 4)
    // pRoute->setDirectionCost( CBD::DirectionTypes nDirection, double nCost );
    pRoute->setHeuristic( RRouteAStar::HeuristicManhatten );
    pRoute->setStraightLinePreference( true );
    // pRoute->setStraightLinePreference( double n );
    pRoute->setReduceRoutePoints( true );
}

void DLineDynamic::doRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache )
{
    QList<QPointF> l = pRoute->getRoute( pointBegin, pointEnd, bCache );
    if ( l.isEmpty() ) return;

    // apply auto route - merge even when list is empty as points may need to get removed
    doMergePoints( l );
    // ensure first and last segments are on a 90 degree angle
/*
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
*/
}

void DLineDynamic::doDeleteRoute()
{
    delete pRoute;
    pRoute = nullptr;
    delete pRouteSource;
    pRouteSource = nullptr;
}



