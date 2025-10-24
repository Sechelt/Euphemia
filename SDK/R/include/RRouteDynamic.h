#ifndef H_RRouteDynamic
#define H_RRouteDynamic

#include "RRoute.h"

#ifndef Q_OS_WIN
	#include <bits/stdc++.h>
#endif

// ROUTE GRID
// - same dim as source grid but with routing info in each cell
// - calculated as-needed
class RRouteDynamicCell
{ 
public:
	RRouteDynamicCell()
	{
		pointParent.setX( -1 );
		pointParent.setY( -1 );
		f	= FLT_MAX;	// init to 'infiniti'		
		g	= FLT_MAX;	// init to 'infiniti'		
		h	= FLT_MAX;	// init to 'infiniti'
		bClosed = false;
		bRoute	= false;
	}

	QPoint 	pointParent;	// used to navigate backward
	double 	f;   	   		// total cost of the node (g+h)
	double 	g;   	   		// distance between the current node and the start node
	double 	h;   	   		// heuristic — estimated distance from the current node to the end node
	bool	bClosed;		// true if we have considered this cell and can now ignore
	bool 	bRoute;			// true if part of the determined route
};

typedef QVector<RRouteDynamicCell> RRouteDynamicRow;
typedef QVector<RRouteDynamicRow> RRouteDynamicGrid;

// OPEN LIST
// - this is a list of cells we are currently considering
// - cells added during update of a RRouteDynamicCell
// - cell removed once considered (and bClosed set to true)
// - the double = f value - giving us a preference
// - ordered by f (the key)
// - there can be more than one value for a key in a MultiHash
typedef QMap<double,QList<QPoint>> RRouteDynamicOpenList;		// 1-n

class RRouteDynamic : public RRoute
{
	Q_OBJECT
public:
    RRouteDynamic( RSource *pSource, QObject *pObjectParent = nullptr );

	virtual void setCostBreakStraightLine( int n );
	virtual void setCostCrossLine( int n );
	virtual void setCostLeaveBeginAxis( int n );
	virtual void setCostLeaveEndAxis( int n );
	virtual void setDepart( CBD::DirectionTypes n );
	virtual void setArrive( CBD::DirectionTypes n );
	virtual void setDelay( uint n );
	virtual void setExploreLines( bool b = true );
	virtual void setRouteLines( bool b = true );
	virtual void setDetails( bool b = true );

	virtual int 				getCostBreakStraightLine() { return nCostBreakStraightLine; }
	virtual int 				getCostCrossLine() { return nCostCrossLine; }
	virtual int 				getCostLeaveBeginAxis() { return nCostLeaveBeginAxis; }
	virtual int 				getCostLeaveEndAxis() { return nCostLeaveEndAxis; }
	virtual CBD::DirectionTypes getDepart() { return nDepart; }
	virtual CBD::DirectionTypes getArrive() { return nArrive; }
	virtual uint 				getDelay() { return nDelay; }
	virtual bool 				isExploreLines() { return bShowExploreLines; }
	virtual bool 				isRouteLines() { return bShowRouteLines; }
	virtual bool 				isDetails() { return bShowDetails; }
	virtual QList<QPointF> 		getRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache = true );
	virtual bool 				isEmpty() { return !gridRoute.count(); } 
	virtual QWidget *			getControls( QWidget *pParent );

	virtual void doPaint( QPainter *pPainter, const QPoint &pointCell );
	virtual void doInfo( const QPoint &pointCell );

public slots:
	virtual void slotRoute();
	virtual void slotCostBreakStraightLine( int n );
	virtual void slotCostCrossLine( int n );
	virtual void slotCostLeaveBeginAxis( int n );
	virtual void slotCostLeaveEndAxis( int n );
	virtual void slotDepart( int n );
	virtual void slotArrive( int n );
	virtual void slotDelay( int n );
	virtual void slotExploreLines( Qt::CheckState b );
	virtual void slotRouteLines( Qt::CheckState b );
	virtual void slotDetails( Qt::CheckState b );

protected:
	// Begin/End **CELL** as provided, via setDepartCell(), before call to getRoute().
	// These can be same as used in call to getRoute() or be an offset cell.
	QPoint 			pointBegin;
	QPoint 			pointEnd;

	// Grid
	// Visualize this as a grid layed over the diagram. We store route details in each cell
	// as we explore from Begin to End. The idea is to not have to explore all cells so 
	// many cells may never get details assigned to them.
	RRouteDynamicGrid		gridRoute;

	// Open List
	// List of all of the cells we should consider for our next step.
	// This is ordered by f value so we can quickly find the most desirable cell.
	// The cell is considered and then removed from here. The corresponding cell in the 'grid' is flagged as 'closed'.
	RRouteDynamicOpenList	listOpen;

	// Cost to move 1 cell.
	// We can increase this value to discourage routing through a cell.
	// For example; 	+1 to discourage going through lines
	//					+1 to discourage breaking a straight line
	//					+1 to discourage moving off of an End point axis
	double d;

	// Weight of heuristic.
	// Decrease as cell is closer to End - making heuristic more important f = g + w * h.
	// Not used at this time.
	double w;		
			
	// Step costs we can adjust to influence routing.	
	// int rather than double means we can adjust via QSpinBox		
	int	nCostBreakStraightLine;
	int	nCostCrossLine;
	int	nCostLeaveBeginAxis;
	int	nCostLeaveEndAxis;

	// slotRoute() will use these to determine offset cell (if any)
	CBD::DirectionTypes nDepart;
	CBD::DirectionTypes nArrive;

	bool isBreakStraightLine( const QPoint &pointCell, const QPoint &pointAdjacent );
	bool isCrossLine( const QPoint &pointAdjacent );
	bool isLeaveBeginAxis( const QPoint &pointCell, const QPoint &pointAdjacent );
	bool isLeaveEndAxis( const QPoint &pointCell, const QPoint &pointAdjacent );
	bool isBeginAxis( const QPoint &pointAdjacent );
	bool isEndAxis( const QPoint &pointAdjacent );

	// Force Depart (begin cell) and Arrive (end cell) directions.
	// QPoint pointCellBeginOrig;
	// QPoint pointCellEndOrig;
					
	// View options
	// - for CBDRoute
	uint nDelay;
	bool bShowExploreLines;
	bool bShowRouteLines;
	bool bShowDetails;

	void setClosed( const QPoint &pointCell );
	double getStepCost( const QPoint &pointCell, const QPoint &pointAdjacent );
	bool isValid( const QPoint &pointCell ); 
	bool isBlocked( const QPoint &pointCellCurrent, const QPoint &pointAdjacent ); 
	bool isClosed( const QPoint &pointCell );
	bool isRoute( const QPoint &pointCell );
	bool isEnd( const QPoint &pointCell ); 
	bool isOpen( const QPoint &pointCell ); 
	bool doGrid(); 
	bool doCell( const QPoint &pointCell, const QPoint &pointCellAdjacent, double nStep );

	void doInsertOpen( double n, const QPoint &pointCell );
	void doRemoveOpen( double n, const QPoint &pointCell );
	void doClearOpen();

	QPoint getParent( const QPoint &pointCell );
	QRectF getCellRect( const QPoint &pointCell );
	QPoint getCellNext();		
	double getProximity( const QPoint &pointCell );	

	bool isStraight( const QPoint &point1, const QPoint &point2, const QPoint &point3 )
	{
		QLineF l1 = QLineF( point1.x(), point1.y(), point2.x(), point2.y() );
		QLineF l2 = QLineF( point1.x(), point1.y(), point3.x(), point3.y() );
		return ( l1.angle() == l2.angle() );
	}
};

#endif
 
