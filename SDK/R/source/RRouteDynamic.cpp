#include "LibInfo.h"
#include "RRouteDynamic.h"
#include "RRouteDynamicControl.h"

#include "RSource.h"

/*!
 * \brief Construct a route object. 
 *  
 * Instantiate one of these when a DSource is 'picked up'.
 * Call getRoute() each time the DSource changes location. 
 * Delete this object when the DSource is 'dropped'. 
 * 
 * \author pharvey (3/3/20)
 * 
 * \param pSource 
 * \param pObjectParent 
 */
RRouteDynamic::RRouteDynamic( RSource *pSource, QObject *pObjectParent )
    : RRoute( pSource, pObjectParent )
{
    nCostBreakStraightLine  = 0;
    nCostCrossLine          = 0;
    nCostLeaveBeginAxis     = 0;
    nCostLeaveEndAxis       = 0;
    nDelay                  = 0;
    bShowExploreLines       = false;
    bShowRouteLines         = false;
    bShowDetails            = false;
    d                       = 1.0;
    w                       = 0;
    nDepart                 = CBD::DirectionUnknown;
    nArrive                 = CBD::DirectionUnknown;

}

void RRouteDynamic::setCostBreakStraightLine( int n )
{
    if ( nCostBreakStraightLine == n ) return;
    nCostBreakStraightLine = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setCostCrossLine( int n )
{
    if ( nCostCrossLine == n ) return;
    nCostCrossLine = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setCostLeaveBeginAxis( int n )
{
    if ( nCostLeaveBeginAxis == n ) return;
    nCostLeaveBeginAxis = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setCostLeaveEndAxis( int n )
{
    if ( nCostLeaveEndAxis == n ) return;
    nCostLeaveEndAxis = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setDepart( CBD::DirectionTypes n )
{
    if ( nDepart == n ) return;
    nDepart = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setArrive( CBD::DirectionTypes n )
{
    if ( nArrive == n ) return;
    nArrive = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setDelay( uint n )
{
    if ( nDelay == n ) return;
    nDelay = n;
    emit signalChangedConfig();
}

void RRouteDynamic::setExploreLines( bool b )
{
    if ( bShowExploreLines == b ) return;
    bShowExploreLines = b;
    emit signalChangedConfig();
    emit signalChanged();
}

void RRouteDynamic::setRouteLines( bool b )
{
    if ( bShowRouteLines == b ) return;
    bShowRouteLines = b;
    emit signalChangedConfig();
    emit signalChanged();
}

void RRouteDynamic::setDetails( bool b )
{
    if ( bShowDetails == b ) return;
    bShowDetails = b;
    emit signalChangedConfig();
    emit signalChanged();
}

/*!
 * \brief Returns a route from Begin cell to End cell. 
 *  
 * The route is empty if we failed to find one. 
 * 
 * \author pharvey (3/3/20)
 * 
 * \param pointCellBegin 
 * \param pointCellEnd 
 * 
 * \return QList&lt;QPoint&gt; 
 */
QList<QPointF> RRouteDynamic::getRoute( const QPointF &pointB, const QPointF &pointE, bool bCache )
{
    if ( bCache && (this->pointBegin == pSource->mapSceneToCell( pointB )) && (this->pointEnd == pSource->mapSceneToCell( pointE )) ) return listRoute;

    listRoute.clear();

    if ( pointB == pointE ) return listRoute;

    pointBegin  = pSource->mapSceneToCell( pointB );                                       
    pointEnd    = pSource->mapSceneToCell( pointE );                                       

    // do the routing
    if ( !doGrid() ) return listRoute;

    // translate route to a list of cells 

    // starts at End and nav back following parent until parent is equal to current cell
    QPoint point = pSource->mapSceneToCell( pointE );
    QStack<QPoint> stackRoute;
    while ( gridRoute[point.y()][point.x()].pointParent != point )
    {
        stackRoute.push( point );
        gridRoute[point.y()][point.x()].bRoute = true;
        point = gridRoute[point.y()][point.x()].pointParent;
    }
    stackRoute.push( point ); // ensure Begin is included

    // we have the route but it is in reverse order
    // the following solves that

    QPoint pointPrev1( -1, -1 );
    QPoint pointPrev2( -1, -1 );

    while ( !stackRoute.empty() )
    {
        point = stackRoute.top();
        stackRoute.pop();

        // skip redundant points
        // - we need 3 points; prev1, prev2, and current
        // - then we decide if prev2 is redundant

        // prime
        if ( pointPrev1.x() < 0 )
        { 
            pointPrev1 = point;
            continue;
        }
        if ( pointPrev2.x() < 0 )
        { 
            pointPrev2 = point;
            continue;
        }

        // check cur against prev1 and prev2
        if ( isStraight( pointPrev1, point, pointPrev2 ) )
        {
            pointPrev2 = point;
            continue;
        }

        listRoute << pSource->mapCellToScene( pointPrev1 );
        pointPrev1 = pointPrev2;
        pointPrev2 = point;
    }

    if ( pointPrev1.x() >= 0 ) listRoute << pSource->mapCellToScene( pointPrev1 );
    if ( pointPrev2.x() >= 0 ) listRoute << pSource->mapCellToScene( pointPrev2 );

    emit signalChanged();

    return listRoute;
}

QWidget *RRouteDynamic::getControls( QWidget *pParent )
{
    return new RRouteDynamicControl( this, pParent );
}

void RRouteDynamic::doPaint( QPainter *pPainter, const QPoint &pointCell )
{
    QRectF rectCell = getCellRect( pointCell );
    QRectF rect( 0, 0, pSource->getCellSize().width() - 8, pSource->getCellSize().height() - 8 );
    rect.moveCenter( rectCell.center() );

    if ( isClosed( pointCell ) )
    {
        pPainter->setPen( Qt::black );
        pPainter->setBrush( Qt::lightGray );
        pPainter->drawRect( rect );
    }

    if ( isOpen( pointCell ) )
    {
        pPainter->setPen( Qt::black );
        pPainter->setBrush( Qt::green );
        pPainter->drawRect( rect );
    }

    QPoint pointParent = getParent( pointCell );
    if ( pointParent.x() >= 0 )
    {
        QRectF rectParent = getCellRect( pointParent ); 
        QPen pen;
        if ( bShowExploreLines && gridRoute.count() )
        { 
            pen.setColor( Qt::black );
            pen.setWidth( 4 );
            pPainter->setPen( pen );
            pPainter->drawLine( rectCell.center(), rectParent.center() );
        }
        if ( bShowRouteLines && isRoute( pointCell ) )
        { 
            pen.setColor( Qt::red );
            pen.setWidth( 4 );
            pPainter->setPen( pen );
            pPainter->drawLine( rectCell.center(), rectParent.center() );
        }
        if ( bShowDetails && gridRoute.count() )
        { 
            QString stringF = ( gridRoute[pointCell.y()][pointCell.x()].f == FLT_MAX ? "-" : QString::number( gridRoute[pointCell.y()][pointCell.x()].f ) );
            QString stringG = ( gridRoute[pointCell.y()][pointCell.x()].g == FLT_MAX ? "-" : QString::number( gridRoute[pointCell.y()][pointCell.x()].g ) );
            QString stringH = ( gridRoute[pointCell.y()][pointCell.x()].h == FLT_MAX ? "-" : QString::number( gridRoute[pointCell.y()][pointCell.x()].h ) );


            pen.setColor( Qt::black );
            pen.setWidth( 1 );
            pPainter->setPen( pen );
            pPainter->drawText( rect, QString( "f:%1\ng:%2\nh:%3" ).arg( stringF ).arg( stringG ).arg( stringH ), Qt::AlignLeft | Qt::AlignTop );
        }
    }
}

void RRouteDynamic::doInfo( const QPoint &pointCell )
{
    Q_UNUSED(pointCell);
    if ( !gridRoute.count() ) return;

    // popup a dialog

/*
    QTableWidget *pTable = new QTableWidget();                                                                    
    pTable->setWindowFlags( Qt::ToolTip );                                                                        
    pTable->setColumnCount( 2 );                                                                                  
    pTable->setRowCount( 3 );                                                                                     
                                                                                                                  
    pTable->setItem( 0, 0, new QTableWidgetItem("f") );                                                           
    pTable->setItem( 1, 0, new QTableWidgetItem("g") );                                                           
    pTable->setItem( 2, 0, new QTableWidgetItem("h") );                                                           
                                                                                                                  
    pTable->setItem( 0, 1, new QTableWidgetItem( QString::number( gridRoute[pointCell.y()][pointCell.x()].f ) ) );
    pTable->setItem( 1, 1, new QTableWidgetItem( QString::number( gridRoute[pointCell.y()][pointCell.x()].g ) ) );
    pTable->setItem( 2, 1, new QTableWidgetItem( QString::number( gridRoute[pointCell.y()][pointCell.x()].h ) ) );
*/
}

/*!
 * \brief Determine a route. 
 *  
 * This method is to support the 'Route' testing app. 
 * As such it has to calculate any offset needed based upon nDepart and nArrive. 
 *  
 * The normal case is that the app sets the desired offet (having more knowledge about the scene) and calls getRoute(). 
 * 
 * \author pharvey (5/15/20)
 */
void RRouteDynamic::slotRoute() 
{
    if ( !pSource ) return;
    QSize   sizeCell        = pSource->getCellSize();
    QPointF pointBegin      = pSource->getBegin();
    QPointF pointEnd        = pSource->getEnd();

    // adjust for offset?
    switch ( nDepart )
    {
        case CBD::DirectionN:
            pointBegin.setY( pointBegin.y() - sizeCell.height() );
            break;
        case CBD::DirectionE:
            pointBegin.setX( pointBegin.x() + sizeCell.width() );
            break;
        case CBD::DirectionS:
            pointBegin.setY( pointBegin.y() + sizeCell.height() );
            break;
        case CBD::DirectionW:
            pointBegin.setX( pointBegin.x() - sizeCell.width() );
            break;
        case CBD::DirectionNE:
            pointBegin.setY( pointBegin.y() - sizeCell.height() );
            pointBegin.setX( pointBegin.x() + sizeCell.width() );
            break;
        case CBD::DirectionSE:
            pointBegin.setY( pointBegin.y() + sizeCell.height() );
            pointBegin.setX( pointBegin.x() + sizeCell.width() );
            break;
        case CBD::DirectionSW:
            pointBegin.setY( pointBegin.y() + sizeCell.height() );
            pointBegin.setX( pointBegin.x() - sizeCell.width() );
            break;
        case CBD::DirectionNW:
            pointBegin.setY( pointBegin.y() - sizeCell.height() );
            pointBegin.setX( pointBegin.x() - sizeCell.width() );
            break;
        case CBD::DirectionUnknown:
            break;
    }
    switch ( nArrive )
    {
        case CBD::DirectionN:
            pointEnd.setY( pointEnd.y() - sizeCell.height() );
            break;
        case CBD::DirectionE:
            pointEnd.setX( pointEnd.x() + sizeCell.width() );
            break;
        case CBD::DirectionS:
            pointEnd.setY( pointEnd.y() + sizeCell.height() );
            break;
        case CBD::DirectionW:
            pointEnd.setX( pointEnd.x() - sizeCell.width() );
            break;
        case CBD::DirectionNE:
            pointEnd.setY( pointEnd.y() - sizeCell.height() );
            pointEnd.setX( pointEnd.x() + sizeCell.width() );
            break;
        case CBD::DirectionSE:
            pointEnd.setY( pointEnd.y() + sizeCell.height() );
            pointEnd.setX( pointEnd.x() + sizeCell.width() );
            break;
        case CBD::DirectionSW:
            pointEnd.setY( pointEnd.y() + sizeCell.height() );
            pointEnd.setX( pointEnd.x() - sizeCell.width() );
            break;
        case CBD::DirectionNW:
            pointEnd.setY( pointEnd.y() - sizeCell.height() );
            pointEnd.setX( pointEnd.x() - sizeCell.width() );
            break;
        case CBD::DirectionUnknown:
            break;
    }

    getRoute( pointBegin, pointEnd, false ); 
}

void RRouteDynamic::slotCostBreakStraightLine( int n )
{
    setCostBreakStraightLine( n );
}

void RRouteDynamic::slotCostCrossLine( int n )
{
    setCostCrossLine( n );
}

void RRouteDynamic::slotCostLeaveBeginAxis( int n )
{
    setCostLeaveBeginAxis( n );
}

void RRouteDynamic::slotCostLeaveEndAxis( int n )
{
    setCostLeaveEndAxis( n );
}

void RRouteDynamic::slotDepart( int n )
{
    setDepart( (CBD::DirectionTypes)n );
}

void RRouteDynamic::slotArrive( int n )
{
    setArrive( (CBD::DirectionTypes)n );
}

void RRouteDynamic::slotDelay( int n )
{
    setDelay( n );
}

void RRouteDynamic::slotExploreLines( Qt::CheckState b )
{
    setExploreLines( bool(b) );
}

void RRouteDynamic::slotRouteLines( Qt::CheckState b )
{
    setRouteLines( bool(b) );
}

void RRouteDynamic::slotDetails( Qt::CheckState b )
{
    setDetails( bool(b) );
}

bool RRouteDynamic::isBreakStraightLine( const QPoint &pointCell, const QPoint &pointAdjacent )
{
    QPoint pointParent = gridRoute[pointCell.y()][pointCell.x()].pointParent;
    if ( pointParent == pointCell ) return false; // begin

    return ( !isStraight( pointParent, pointCell, pointAdjacent ) );
}

bool RRouteDynamic::isCrossLine( const QPoint &pointAdjacent )
{
    return (pSource->getCellContent( pointAdjacent ) == RSource::CellContentLine);
}

bool RRouteDynamic::isLeaveBeginAxis( const QPoint &pointCell, const QPoint &pointAdjacent )
{
    if ( pointCell.x() == pointBegin.x() && pointAdjacent.x() != pointBegin.x() ) return true;
    if ( pointCell.y() == pointBegin.y() && pointAdjacent.y() != pointBegin.y() ) return true;
   return false;
}

bool RRouteDynamic::isLeaveEndAxis( const QPoint &pointCell, const QPoint &pointAdjacent )
{
    if ( pointCell.x() == pointEnd.x() && pointAdjacent.x() != pointEnd.x() ) return true;
    if ( pointCell.y() == pointEnd.y() && pointAdjacent.y() != pointEnd.y() ) return true;
    return false;
}

bool RRouteDynamic::isBeginAxis( const QPoint &pointAdjacent )
{
    if ( pointAdjacent.x() == pointBegin.x() || pointAdjacent.y() == pointBegin.y() ) return true;
    return false;
}

bool RRouteDynamic::isEndAxis( const QPoint &pointAdjacent )
{
    if ( pointAdjacent.x() == pointEnd.x() || pointAdjacent.y() == pointEnd.y() ) return true;
    return false;
}

void RRouteDynamic::setClosed( const QPoint &pointCell )
{
    gridRoute[pointCell.y()][pointCell.x()].bClosed = true;
}

double RRouteDynamic::getStepCost( const QPoint &pointCell, const QPoint &pointAdjacent )
{
    double nStep = 0.0;
    if ( nCostBreakStraightLine && isBreakStraightLine( pointCell, pointAdjacent ) ) nStep += nCostBreakStraightLine;
    if ( nCostCrossLine && isCrossLine( pointAdjacent ) ) nStep += nCostCrossLine;
    if ( nCostLeaveBeginAxis && isLeaveBeginAxis( pointCell, pointAdjacent ) && (!isEndAxis( pointAdjacent )) ) nStep += nCostLeaveBeginAxis;
    if ( nCostLeaveEndAxis && isLeaveEndAxis( pointCell, pointAdjacent ) && (!isBeginAxis( pointAdjacent )) ) nStep += nCostLeaveEndAxis;
   return nStep;
}

/*!
 * \brief Returns whether the cell is valid or not.
 *  
 * A cell is invalid if it is 'off-scene'.
 * 
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 * 
 * \return bool 
 */
bool RRouteDynamic::isValid( const QPoint &pointCell )
{
    int nRow = pointCell.y();
    int nCol = pointCell.x();
    return ( nRow >= 0 && nRow < pSource->getGridSize().height() && nCol >= 0 && nCol < pSource->getGridSize().width() );
}

/*!
 * \brief Returns whether the cell is blocked or not. 
 *  
 * The cell is blocked if; any part of the cell contains a DRectangle (a shape) based object.
 * DLine based objects do not block. 
 *  
 * It is an ERROR (terminal) to call here with an invalid cell. 
 *  
 * \sa isInvalid 
 * 
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 * 
 * \return bool 
 */
bool RRouteDynamic::isBlocked( const QPoint &pointCellCurrent, const QPoint &pointAdjacent )
{
    if ( pSource->getCellContent( pointAdjacent ) > RSource::CellContentLine ) return true;

    // we are trying to depart/arrive from a different side so block and force route around
    CBD::DirectionTypes nDir = CBD::DirectionUnknown;
    QPoint point1;
    QPoint point2;

    if ( pointCellCurrent.isNull() || pointCellCurrent == pointBegin )
    {
        nDir    = nDepart;
        point1  = pointAdjacent;
        point2  = pointBegin;
    }
    else if ( pointAdjacent == pointEnd )
    { 
        nDir    = nDepart;
        point1  = pointEnd;
        point2  = pointCellCurrent;
    }

    switch ( nDir )
    {
        case CBD::DirectionN:
            if ( !(point1.x() == point2.x() && point1.y() == point2.y() - 1) ) return true;
            break;
        case CBD::DirectionE:
            if ( !(point1.y() == point2.y() && point1.x() == point2.x() + 1) ) return true;
            break;
        case CBD::DirectionS:
            if ( !(point1.x() == point2.x() && point1.y() == point2.y() + 1) ) return true;
            break;
        case CBD::DirectionW:
            if ( !(point1.y() == point2.y() && point1.x() == point2.x() - 1) ) return true;
            break;
        case CBD::DirectionNE:
            break;
        case CBD::DirectionSE:
            break;
        case CBD::DirectionSW:
            break;
        case CBD::DirectionNW:
            break;
        case CBD::DirectionUnknown:
            break;
    }

    return false;
}

bool RRouteDynamic::isClosed( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return false;
    return gridRoute[pointCell.y()][pointCell.x()].bClosed;
}

bool RRouteDynamic::isRoute( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return false;
    return gridRoute[pointCell.y()][pointCell.x()].bRoute;
}

bool RRouteDynamic::isEnd( const QPoint &pointCell )
{
    return pointCell == pointEnd;
}

bool RRouteDynamic::isOpen( const QPoint &pointCell )
{
    QMapIterator<double, QList<QPoint>> i( listOpen );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( i.value().contains( pointCell ) ) return true;
    }

    return false;
}

/*!
 * \brief Process the grid. 
 *  
 * This is where the search logic exists. 
 * 
 * \author pharvey (3/3/20)
 * 
 * \return bool 
 */
bool RRouteDynamic::doGrid()
{
    // If the source is out of range
    if ( !isValid( pointBegin ) )
    {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "Source out of range: " << pointBegin;
        return false;
    }

    // If the destination is out of range
    if ( !isValid( pointEnd ) )
    {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "Destination out of range: " << pointEnd;
        return false;
    }

    // If the destination cell is the same as source cell
    if ( isEnd( pointBegin ) )
    {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "We are already at destination.";
        return false;
    }

    // clear any past route
    listRoute.clear();

    // init or reinit route grid
    gridRoute.clear();
    for ( int nRow = 0; nRow < pSource->getGridSize().height(); nRow++ )
    {
        gridRoute.append( RRouteDynamicRow( pSource->getGridSize().width() ) );
    }

    // init or reinit list of cells being considered
    doClearOpen();

    // init a starting cell
    // - this cell is unique - pointParent points to self
    int nRow    = pointBegin.y();
    int nCol    = pointBegin.x();
    gridRoute[nRow][nCol].pointParent = pointBegin;
    gridRoute[nRow][nCol].f = 0.0;
    gridRoute[nRow][nCol].g = 0.0;
    gridRoute[nRow][nCol].h = 0.0;

    doInsertOpen( 0.0, pointBegin );

    while ( !listOpen.isEmpty() )
    {
        // take best cell (highest f value) from listOpen
        QPoint pointCell = getCellNext();

        // flag as closed 
        setClosed( pointCell );

        // process adjacent (8) cells
        if ( doCell( pointCell, QPoint( pointCell.x(), pointCell.y() - 1 ), d ) ) return true;    // N
        if ( doCell( pointCell, QPoint( pointCell.x(), pointCell.y() + 1 ), d ) ) return true;    // S
        if ( doCell( pointCell, QPoint( pointCell.x() + 1, pointCell.y() ), d ) ) return true;    // E
        if ( doCell( pointCell, QPoint( pointCell.x() - 1, pointCell.y() ), d ) ) return true;    // W

        if ( nDelay ) doDelay( nDelay );
    }

    return false;
}

/*!
 * \brief Load our neighbours and update our status.
 *  
 * Set the cells direction flags as either blocked - or not. 
 * Once a direction is flagged as blocked  - it can not be unblocked. 
 *  
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 * 
 * \return bool 
 */
bool RRouteDynamic::doCell( const QPoint &pointCell, const QPoint &pointCellAdjacent, double nStep )
{
    if ( !isValid( pointCellAdjacent ) ) return false;

    if ( isEnd( pointCellAdjacent ) )
    {
        gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].pointParent = pointCell;
        return true; 
    }
    else if ( !isClosed( pointCellAdjacent ) && !isBlocked( pointCell, pointCellAdjacent ) )
    {
        // do we want to consider this, adjacent, cell?  
        double f, g, h;
        // add step cost to total step cost to this cell
        g = gridRoute[pointCell.y()][pointCell.x()].g + nStep + getStepCost( pointCell, pointCellAdjacent );
        h = getProximity( pointCellAdjacent );
        f = g + h;

        // f not calculated yet or its better than current
        if ( gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].f == FLT_MAX ||
             gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].f > f )
        { 
            // yes! so add to listOpen and update adjacent cell with our newly calculated values
            // we will come back to this cell later
            doInsertOpen( f, pointCellAdjacent );

            gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].f = f;
            gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].g = g;
            gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].h = h;
            gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].pointParent = pointCell;

            emit signalChanged();
            emit signalChanged( pointCell );
            emit signalChanged( pointCellAdjacent );
        }
    }

    return false;
}

void RRouteDynamic::doInsertOpen( double n, const QPoint &pointCell )
{
    listOpen[n].append( pointCell );
}

void RRouteDynamic::doRemoveOpen( double n, const QPoint &pointCell )
{
    listOpen[n].removeAll( pointCell );
    if ( listOpen[n].isEmpty() ) listOpen.remove( n );
}

void RRouteDynamic::doClearOpen()
{
    listOpen.clear();
}

QPoint RRouteDynamic::getParent( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return QPoint();
    return gridRoute[pointCell.y()][pointCell.x()].pointParent;
}

QRectF RRouteDynamic::getCellRect( const QPoint &pointCell )
{
    return QRectF( pointCell.x() * pSource->getCellSize().width(), pointCell.y() * pSource->getCellSize().height(), pSource->getCellSize().width(), pSource->getCellSize().height() );
}

/*!
 * \brief Returns the best cell to be considered from the 'open' list.
 *  
 * The best cell is the one with the best 'f' value. 
 * The cell is removed from listOpen and returned. 
 * It is an ERROR to call here when listOpen is empty. 
 *  
 * \author pharvey (3/3/20)
 * 
 * \return QPoint 
 */
QPoint RRouteDynamic::getCellNext()
{
    double f        = listOpen.firstKey();
    QPoint point    = listOpen[f].first();

    doRemoveOpen( f, point );

    return point;
}

/*!
 * \brief Returns a heuristic value based upon proximity to destination.
 *  
 * This uses 'Manhatten' method to calculate proximity. 
 *  
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 * 
 * \return double 
 */
double RRouteDynamic::getProximity( const QPoint &pointCell )
{
    double f = 0.5;
    double dx = abs( pointCell.x() - pointEnd.x() );
    double dy = abs( pointCell.y() - pointEnd.y() );
    f = d * ( dx + dy );

    return f;
}


