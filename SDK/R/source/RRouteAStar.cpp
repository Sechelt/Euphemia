#include "LibInfo.h"
#include "RRouteAStar.h"

#include "RSource.h"
#include "RRouteAStarControl.h"

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
RRouteAStar::RRouteAStar( RSource *pSource, QObject *pObjectParent )
: RRoute( pSource, pObjectParent )
{
    vectorDirectionCosts.resize( 8 );
    vectorDirectionCosts.fill( 0 );
    bDiagonal               = true;
    nDelay                  = 0;
    bShowExploreLines       = false;
    bShowRouteLines         = false;
    bShowDetails            = false;
    bReduceRoutePoints      = true;
    nHeuristic              = HeuristicManhatten;
    bStraightLinePreference = true;
    nStraightLinePreference = 1; // 0.001;
    d = 1.0;
    d2= 1.414;
}

void RRouteAStar::setDiagonal( bool b ) 
{
    if ( bDiagonal == b ) return;
    bDiagonal = b;
    emit signalChangedConfig();
}

void RRouteAStar::setDirectionCost( CBD::DirectionTypes nDirection, double nCost ) 
{ 
    if ( vectorDirectionCosts[nDirection] == nCost ) return; 
    vectorDirectionCosts[nDirection] = nCost; 
    emit signalChangedConfig();
}

void RRouteAStar::setHeuristic( HeuristicTypes n )
{
    if ( nHeuristic == n ) return;

    switch ( n )
    {
        case HeuristicNone:
            nHeuristic = n;
            emit signalChangedConfig();
            break;
        case HeuristicManhatten:
            nHeuristic = n;
            emit signalChangedConfig();
            break;
        case HeuristicDiagonal:
            nHeuristic = n;
            emit signalChangedConfig();
            break;
        case HeuristicEuclidean:
            nHeuristic = n;
            emit signalChangedConfig();
            break;
    }
}

void RRouteAStar::setStraightLinePreference( bool b ) 
{ 
    if ( bStraightLinePreference == b ) return;
    bStraightLinePreference = b;
    emit signalChangedConfig();
}

void RRouteAStar::setStraightLinePreference( double n ) 
{ 
    if ( nStraightLinePreference == n ) return;
    nStraightLinePreference = n;
    emit signalChangedConfig();
}

void RRouteAStar::setReduceRoutePoints( bool b ) 
{ 
    if ( bReduceRoutePoints == b ) return;
    bReduceRoutePoints = b;
    emit signalChangedConfig();
}

void RRouteAStar::setDelay( uint n ) 
{ 
    if ( nDelay == n ) return; 
    nDelay = n; 
    emit signalChangedConfig();
}

void RRouteAStar::setExploreLines( int b )
{
    if ( bShowExploreLines == b ) return;
    bShowExploreLines = b;
    emit signalChangedConfig();
    emit signalChanged();
}

void RRouteAStar::setRouteLines( int b )
{
    if ( bShowRouteLines == b ) return;
    bShowRouteLines = b;
    emit signalChangedConfig();
    emit signalChanged();
}

void RRouteAStar::setDetails( int b )
{
    if ( bShowDetails == b ) return;
    bShowDetails = b;
    emit signalChangedConfig();
    emit signalChanged();
}

QStringList RRouteAStar::getHeuristics()
{
    QStringList l;
    l << "Manhatten (4-way + prox)" << "Diagonal (8-way + prox)" << "Euclidean (any angle)";
    return l;
}

QWidget *RRouteAStar::getControls( QWidget *pParent )
{
    return new RRouteAStarControl( this, pParent );
}

/*!
 * \brief Returns a route from Begin to End. 
 *  
 * The route is empty if we failed to find one. 
 *  
 * Pixel coordinates in and pixel coordinates out BUT all between is done in cell coordinates. 
 * This optimizes processing with acceptable granularity for our purposes. 
 *  
 * There is a note-worthy side affect of the begin/end points being calculated in pixel coordinates and the intermediate 
 * points being calculated in cell coordinates. It can result in the first/last line segments deviating from desired 
 * vertical/horizontal alignment. This happens when the begin/end points are not the center of a cell. 
 * Not an issue for us.
 *  
 * \author pharvey (3/3/20)
 * 
 * \param pointBegin 
 * \param pointEnd 
 * 
 * \return QList&lt;QPoint&gt; 
 */
QList<QPointF> RRouteAStar::getRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache )
{
    if ( bCache && (this->pointBegin == pSource->mapSceneToCell( pointBegin )) && (this->pointEnd == pSource->mapSceneToCell( pointEnd )) ) return listRoute;

    this->pointBegin    = pSource->mapSceneToCell( pointBegin );
    this->pointEnd      = pSource->mapSceneToCell( pointEnd );

    if ( !doGrid() ) return listRoute;

    // translate route to a list of cells 

    // starts at End and nav back following parent until parent is equal to current cell
    QPoint point = pSource->mapSceneToCell( pointEnd );
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

        // are intermediate points on a straight line redundant?
        if ( bReduceRoutePoints )
        {
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
        else 
            listRoute << pSource->mapCellToScene( point );
    }

    if ( pointPrev1.x() >= 0 ) listRoute << pSource->mapCellToScene( pointPrev1 );
    if ( pointPrev2.x() >= 0 ) listRoute << pSource->mapCellToScene( pointPrev2 );

    emit signalChanged();

    return listRoute;
}

void RRouteAStar::doPaint( QPainter *pPainter, const QPoint &pointCell )
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
        if ( bShowExploreLines )
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
            double f = gridRoute[pointCell.y()][pointCell.x()].f;
            double g = gridRoute[pointCell.y()][pointCell.x()].g;
            double h = gridRoute[pointCell.y()][pointCell.x()].h;

            pen.setColor( Qt::black );
            pen.setWidth( 1 );
            pPainter->setPen( pen );
            pPainter->drawText( rect, QString( "f:%1\ng:%2\nh:%3" ).arg( f ).arg( g ).arg( h ), Qt::AlignLeft | Qt::AlignTop );
        }
    }
}

void RRouteAStar::doInfo( const QPoint &pointCell )
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

void RRouteAStar::slotHeuristic( int n )
{
    setHeuristic( HeuristicTypes(n) );
}

void RRouteAStar::slotDiagonal( Qt::CheckState b )
{
    setDiagonal( bool(b) );
}

void RRouteAStar::slotDelay( int n )
{
    setDelay( uint( n ) );
}

void RRouteAStar::slotExploreLines( Qt::CheckState b )
{
    setExploreLines( bool(b)  );
}

void RRouteAStar::slotRouteLines( Qt::CheckState b )
{
    setRouteLines( bool(b) );
}

void RRouteAStar::slotDetails( Qt::CheckState b )
{
    setDetails( bool(b) );
}

void RRouteAStar::slotStraightLine( Qt::CheckState n )
{
    setStraightLinePreference( bool(n) );
}

void RRouteAStar::slotStraightLine( double n )
{
    setStraightLinePreference( n );
}

void RRouteAStar::setClosed( const QPoint &pointCell )
{
    gridRoute[pointCell.y()][pointCell.x()].bClosed = true;
}

double RRouteAStar::getStepCost( const QPoint &pointCell )
{
    // this is in addition to any cost for diagonal steps
    // - in gaming world this is a terrain cost
    // - here we just discourage crossing a line
    if ( pSource->getCellContent( pointCell ) == RSource::CellContentLine ) return d * 10;
    return d;
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
bool RRouteAStar::isValid( const QPoint &pointCell )
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
bool RRouteAStar::isBlocked( const QPoint &pointCell )
{
    if ( pSource->getCellContent( pointCell ) > RSource::CellContentLine ) return true;

    return false;
}

bool RRouteAStar::isClosed( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return false;
    return gridRoute[pointCell.y()][pointCell.x()].bClosed;
}

bool RRouteAStar::isRoute( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return false;
    return gridRoute[pointCell.y()][pointCell.x()].bRoute;
}

bool RRouteAStar::isEnd( const QPoint &pointCell )
{
    return pointCell == pointEnd;
}

bool RRouteAStar::isOpen( const QPoint &pointCell )
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
bool RRouteAStar::doGrid()
{
    // If the source is out of range
    if ( !isValid( pointBegin ) )
    {
        qDebug() << "Source is invalid\n";
        return false;
    }

    // If the destination is out of range
    if ( !isValid( pointEnd ) )
    {
        qDebug() << "Destination is invalid\n";
        return false;
    }

    // If the destination cell is the same as source cell
    if ( isEnd( pointBegin ) )
    {
        qDebug() << "We are already at the destination\n";
        return false;
    }

    // clear any past route
    listRoute.clear();

    // init or reinit route grid
    gridRoute.clear();
    for ( int nRow = 0; nRow < pSource->getGridSize().height(); nRow++ )
    {
        gridRoute.append( RRouteAStarRow( pSource->getGridSize().width() ) );
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

    // diagonal costs more to move so step cost is a bit higher
    double nDiagonal = d2;
    double nRightAngle = d;

    while ( !listOpen.isEmpty() )
    {
        // take best cell (highest f value) from listOpen
        QPoint pointCell = getCellNext();

        // flag as closed 
        setClosed( pointCell );

        // process adjacent (8) cells
        if ( doCell( pointCell, QPoint( pointCell.x(), pointCell.y() - 1 ), nRightAngle + vectorDirectionCosts[CBD::DirectionN] ) ) return true;    // N
        if ( doCell( pointCell, QPoint( pointCell.x(), pointCell.y() + 1 ), nRightAngle + vectorDirectionCosts[CBD::DirectionS] ) ) return true;    // S
        if ( doCell( pointCell, QPoint( pointCell.x() + 1, pointCell.y() ), nRightAngle + vectorDirectionCosts[CBD::DirectionE] ) ) return true;    // E
        if ( doCell( pointCell, QPoint( pointCell.x() - 1, pointCell.y() ), nRightAngle + vectorDirectionCosts[CBD::DirectionW] ) ) return true;    // W
        if ( bDiagonal )
        { 
            if ( doCell( pointCell, QPoint( pointCell.x() - 1, pointCell.y() - 1 ), nDiagonal + vectorDirectionCosts[CBD::DirectionNW] ) ) return true;  // NW
            if ( doCell( pointCell, QPoint( pointCell.x() + 1, pointCell.y() - 1 ), nDiagonal + vectorDirectionCosts[CBD::DirectionNE] ) ) return true;  // NE
            if ( doCell( pointCell, QPoint( pointCell.x() - 1, pointCell.y() + 1 ), nDiagonal + vectorDirectionCosts[CBD::DirectionSW] ) ) return true;  // SW
            if ( doCell( pointCell, QPoint( pointCell.x() + 1, pointCell.y() + 1 ), nDiagonal + vectorDirectionCosts[CBD::DirectionSE] ) ) return true;  // SE
        }

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
bool RRouteAStar::doCell( const QPoint &pointCell, const QPoint &pointCellAdjacent, double nStep )
{
    if ( !isValid( pointCellAdjacent ) ) return false;

    if ( isEnd( pointCellAdjacent ) )
    {
        gridRoute[pointCellAdjacent.y()][pointCellAdjacent.x()].pointParent = pointCell;
        return true; 
    }
    else if ( !isClosed( pointCellAdjacent ) && !isBlocked( pointCellAdjacent ) )
    {
        // do we want to consider this, adjacent, cell?  
        double f, g, h;
        g = gridRoute[pointCell.y()][pointCell.x()].g + nStep + getStepCost( pointCellAdjacent );
        h = getHeuristicValue( pointCellAdjacent );
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

void RRouteAStar::doInsertOpen( double n, const QPoint &pointCell )
{
    listOpen[n].append( pointCell );
}

void RRouteAStar::doRemoveOpen( double n, const QPoint &pointCell )
{
    listOpen[n].removeAll( pointCell );
    if ( listOpen[n].isEmpty() ) listOpen.remove( n );
}

void RRouteAStar::doClearOpen()
{
    listOpen.clear();
}

QPoint RRouteAStar::getParent( const QPoint &pointCell )
{
    if ( !gridRoute.count() ) return QPoint();
    return gridRoute[pointCell.y()][pointCell.x()].pointParent;
}

QRectF RRouteAStar::getCellRect( const QPoint &pointCell )
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
QPoint RRouteAStar::getCellNext()
{
    double f        = listOpen.firstKey();
    QPoint point    = listOpen[f].first();

    doRemoveOpen( f, point );

    return point;
}

/*!
 * \brief Returns a heuristic value based upon proximity to destination.
 * 
 * \author pharvey (3/3/20)
 * 
 * \param pointCell 
 * 
 * \return double 
 */
double RRouteAStar::getHeuristicValue( const QPoint &pointCell )
{
    double f = 0.5;

    if ( nHeuristic == HeuristicManhatten )
    {
        // manhatten - best default for grid with 4 direction search
        double dx = abs( pointCell.x() - pointEnd.x() );
        double dy = abs( pointCell.y() - pointEnd.y() );
        f = d * ( dx + dy );
    }
    else if ( nHeuristic == HeuristicDiagonal )
    {
        // diagonal distance - best default for grid with 8 direction search
        double dx = abs( pointCell.x() - pointEnd.x() );
        double dy = abs( pointCell.y() - pointEnd.y() );
        f = d * (dx + dy) + (d2 - 2 * d) * qMin(dx, dy);
    }
    else if ( nHeuristic == HeuristicEuclidean )
    {
        // Euclidean (straight line) distance - best when you can move at angle 
        double dx = abs( pointCell.x() - pointEnd.x() );
        double dy = abs( pointCell.y() - pointEnd.y() );
        f = d * sqrt( dx * dx + dy * dy );
    }

    if ( bStraightLinePreference ) f += getStraightLinePreference( pointCell );

    return f;
}

// can add this to f
double RRouteAStar::getStraightLinePreference( const QPoint &pointCell )
{
    // favours route continueing a horizontal or vertical direction
    {
        double nF = 0;
        RRouteAStarCell rc = gridRoute[pointCell.y()][pointCell.x()];
        if ( rc.pointParent.x() == pointCell.x() ) nF += 10;
        if ( rc.pointParent.y() == pointCell.y() ) nF += 10;
        return nF;
    }

    // favours routing on End x/y axis
    {
        double nF = 0;
        if ( pointCell.x() == pointEnd.x() ) nF += 10;
        if ( pointCell.y() == pointEnd.y() ) nF += 10;
        return nF;
    }

    // favours route heading straight for End incl. diagonal
    // - not so great for HeuristicManhatten
    // - relative to End... on the same scale as getHeuristicValue
    {
        double dx1 = pointCell.x() - pointEnd.x();
        double dy1 = pointCell.y() - pointEnd.y();
        double dx2 = pointBegin.x() - pointEnd.x();
        double dy2 = pointBegin.y() - pointEnd.y();
        double cross = abs(dx1*dy2 - dx2*dy1);
        return cross * nStraightLinePreference; // 0.001;
    }
}


