#ifndef H_RRoute
#define H_RRoute

#include <CBD.h>

class RSource;

/*!
 * \class RRoute
 * \brief Base class for a route.
 *
 * This is the common interface for all routing.
 * Derive a class from this to implement specific route finding algorithms.
 * For example; RRouteAStar.
 *
 * Uses RSource to get source (map, scene, whatever) information such as sizes and cell content.
 *
 * \sa RSource
 * \sa RRouteAStar
 * \author pharvey (3/4/20)
 */
class RRoute : public QObject
{
	Q_OBJECT
public:
    RRoute( RSource *pSource, QObject *pObjectParent = nullptr );

	// calculate a route and return it in (0-based pixel coordinates in and out)
	virtual QList<QPointF> getRoute( const QPointF &pointPixelBegin, const QPointF &pointPixelEnd, bool bCache = true ) = 0;
	// the source for cell content etc
	virtual RSource *getSource() { return pSource; }
	// indicates if we have routing information 
	virtual bool isEmpty() = 0;
	// Get control widget (ie for CBDRoute)
	virtual QWidget *getControls( QWidget *pParent ) = 0;
	// Paint a cell (ie for CBDRoute)
	virtual void doPaint( QPainter *pPainter, const QPoint &pointCell ) = 0;
	// Provide cell details (ie for CBDRoute)
	virtual void doInfo( const QPoint &pointCell ) = 0;

signals:
	void signalChanged();
	void signalChanged( const QPoint &pointCell );
	void signalChangedConfig();

public slots:
	virtual void slotRoute();

protected:
	// Interface to source info such as cell content.
	RSource *pSource;

	// Route 
	// - the result from slotRoute() or getRoute()
	// - kept here as a cache
	QList<QPointF> 	listRoute;

	void doDelay( int nSeconds );
};

#endif
 
