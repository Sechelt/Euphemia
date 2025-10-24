#ifndef H_RRouteAStar
#define H_RRouteAStar

#include "RRoute.h"

#ifndef Q_OS_WIN
	#include <bits/stdc++.h>
#endif


// ROUTE GRID
// - same dim as source grid but with routing info in each cell
// - calculated as-needed
class RRouteAStarCell
{ 
public:
	RRouteAStarCell()
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

typedef QVector<RRouteAStarCell> RRouteAStarRow;
typedef QVector<RRouteAStarRow> RRouteAStarGrid;

// OPEN LIST
// - this is a list of cells we are currently considering
// - cells added during update of a RRouteAStarCell
// - cell removed once considered (and bClosed set to true)
// - the double = f value - giving us a preference
// - ordered by f (the key)
// - there can be more than one value for a key in a MultiHash
typedef QMap<double,QList<QPoint>> RRouteAStarOpenList;		// 1-n

/*!
 * \class RRouteAStar
 * \brief Calculates route using A* method.
 *
 * Options
 *
 * setHeuristic
 *
 * 		HeuristicManhatten - prefers to advance in general direction of objective
 * 		HeuristicDiagonal  - use 8 directions instead of 4
 * 		HeuristicEuclidean - 
 *
 * setReduceRoutePoints
 *
 *  	On 	= reduce points (when x or y does not change between 2 cells)
 *  	Off = point for each cell
 *
 * setStraightLinePreference
 *
 *  	On 		= will prefer to maintain a straight line (existing course)
 *  	Off		= will have no preference for a straight line
 *  	weight	= the strength of the preference is increased when this is a higher value
 *
 * setDirectionCost
 *
 * 		weight	= added to step cost in given direction to make certain directions less desirable 
 *
 * \author pharvey (3/4/20)
 */
class RRouteAStar : public RRoute
{
	Q_OBJECT
public:
	enum HeuristicTypes
	{
		HeuristicNone,
		HeuristicManhatten,
		HeuristicDiagonal,
		HeuristicEuclidean
	};

    RRouteAStar( RSource *pSource, QObject *pObjectParent = nullptr );

	virtual void setDiagonal( bool b = false );
	virtual void setDirectionCost( CBD::DirectionTypes nDirection, double nCost );
	virtual void setHeuristic( HeuristicTypes n );
	virtual void setStraightLinePreference( bool b = false );
	virtual void setStraightLinePreference( double n );
	virtual void setReduceRoutePoints( bool b = true );
	virtual void setDelay( uint n = 0 );
	virtual void setExploreLines( int b );
	virtual void setRouteLines( int b );
	virtual void setDetails( int b );

	virtual bool			isDiagonal() { return bDiagonal; }
	virtual double 			getDirectionCost( CBD::DirectionTypes nDirection ) { return vectorDirectionCosts[nDirection]; }
	virtual HeuristicTypes	getHeuristic() { return nHeuristic; }
	virtual bool 			isStraightLinePreference() { return bStraightLinePreference; }
	virtual double			getStraightLinePreference() { return nStraightLinePreference; }
	virtual bool 			isReduceRoutePoints() { return bReduceRoutePoints; }
	virtual uint 			getDelay() { return nDelay; }
	virtual bool 			isExploreLines() { return bShowExploreLines; }
	virtual bool 			isRouteLines() { return bShowRouteLines; }
	virtual bool 			isDetails() { return bShowDetails; }
	virtual QStringList 	getHeuristics();
	virtual QWidget *		getControls( QWidget *pParent );
	virtual QList<QPointF> 	getRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache = true );
	virtual bool 			isEmpty() { return !gridRoute.count(); } 

	virtual void doPaint( QPainter *pPainter, const QPoint &pointCell );
	virtual void doInfo( const QPoint &pointCell );

public slots:
	virtual void slotHeuristic( int n );
	virtual void slotDiagonal( Qt::CheckState b );
	virtual void slotDelay( int n );
	virtual void slotExploreLines( Qt::CheckState b );
	virtual void slotRouteLines( Qt::CheckState b );
	virtual void slotDetails( Qt::CheckState b );
	virtual void slotStraightLine( Qt::CheckState );
	virtual void slotStraightLine( double );

protected:
	QPoint 			pointBegin;
	QPoint 			pointEnd;
	QVector<double> vectorDirectionCosts;	// added to step value when considering adjacent cell (default is 0)
	bool			bDiagonal;				// true if 8 dir else 4 dir
	uint 			nDelay;
	bool 			bShowExploreLines;
	bool			bShowRouteLines;
	bool			bShowDetails;
	bool			bReduceRoutePoints;
	HeuristicTypes	nHeuristic;
	bool			bStraightLinePreference;
	double			nStraightLinePreference;
	double 			d;  			// cost of one step (to move from one square to next) horizontal or vertical
	double 			d2; 			// cost of one step (to move from one square to next) diagonaly
	double 			w;  			// weight of heuristic - decrease as cell is closer to End - making heuristic more important f = g + w * h
	RRouteAStarGrid	gridRoute;

	RRouteAStarOpenList	listOpen;			// key=f

	void setClosed( const QPoint &pointCell );
	double getStepCost( const QPoint &pointCell );
	bool isValid( const QPoint &pointCell ); 
	bool isBlocked( const QPoint &pointCell ); 
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
	double getHeuristicValue( const QPoint &pointCell );	
	double getStraightLinePreference( const QPoint &pointCell );


	bool isStraight( const QPoint &point1, const QPoint &point2, const QPoint &point3 )
	{
		QLineF l1 = QLineF( point1.x(), point1.y(), point2.x(), point2.y() );
		QLineF l2 = QLineF( point1.x(), point1.y(), point3.x(), point3.y() );
		return ( l1.angle() == l2.angle() );
	}
};

#endif
 
