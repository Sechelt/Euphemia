#ifndef H_RSource
#define H_RSource

#include <CBD.h>

/*!
 * \class RSourceCell
 * \brief Can be used by RSource to cache source info or as basis for that info. 
 *
 * RSource does not have to use; RSourceCell, RSourceRow, or RSourceGrid but they
 * can be used to cache source information.
 *
 * They can also be used *as* the source information if needed.
 *
 * \author pharvey (3/4/20)
 */
class RSourceCell
{
public:
	enum CellContentTypes
	{
		CellContentUnknown = -1, // in case where we are using this as a cache
		CellContentClear = 0,
		CellContentLine = 1,
		CellContentShape = 2
	};

	RSourceCell()
	{
		nContent = CellContentUnknown;
	}

	CellContentTypes nContent;
};

typedef QVector<RSourceCell> RSourceRow;
typedef QVector<RSourceRow> RSourceGrid;

/*!
 * \class RSource
 * \brief Interface to a; map, scene or whatever the source is.
 *
 * Derive a new class from this and implement the pure methods.
 * Then provide an object of that class to the RRoute based object.
 * RRoute will then use the information provided by RSource to
 * determine the; grid size, cell size, and cell content.
 *
 * \author pharvey (3/4/20)
 */
class RSource : public QObject
{
    Q_OBJECT
public:
	enum CellContentTypes
	{
		CellContentClear = 0,
		CellContentLine = 1,
		CellContentShape = 2
	};
    RSource( QObject *pObjectParent );

	// Begin/End in scene/pixels coords because not all routes use cell coordinates and/or some need more precision for these
	virtual QPointF 			getBegin() = 0;
	virtual QPointF 			getEnd() = 0;

	// map/scene width/height in cells  
    virtual QSize 				getGridSize() = 0;
    // cell width/height in pixels
    virtual QSize 				getCellSize() = 0;
    // cell contents
    virtual CellContentTypes	getCellContent( const QPoint &point ) = 0;
	// map coordinates
	virtual QPoint              mapSceneToCell( const QPointF &pointScene );
	virtual QPointF             mapCellToScene( const QPoint &pointCell );
	virtual QRect               mapSceneToCell( const QRectF &rectScene );
	virtual QRectF              getCellRect( const QPoint &pointCell );
	// used to indicate that there is no information in the source
	virtual bool 				isEmpty() = 0;

	// Widget with controls for adjusting our behaviour.
	// The controls will be connected to this.
	// This is used by 'Route' testing application.
	virtual QWidget *getControls( QWidget *pParent ) = 0;

	// this is called when a new route is about to be calculated - RSource may want to clear its cache
	virtual void doPrepareForRoute() {}

signals:
	void signalGridSizeChanged( QSize );
	void signalCellSizeChanged( QSize );
	void signalChanged( const QPoint &pointCell, RSource::CellContentTypes n );
};

#endif

