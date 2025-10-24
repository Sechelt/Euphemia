#ifndef H_DLine90Auto
#define H_DLine90Auto

#include "DLine.h"

class DRouteSource;
class RRouteAStar;

/*!
 * \brief A line with (0-n) 90 degree angles and auto-routing.
 *  
 * This is experimental and may be merged into DLineDynamic at some point in the future.
 *  
 * This works at the granularity of the grid and will NOT use diagonal routes with 
 * the exception of arrival and departure. 
 *  
 * Uses RRouteAStar. 
 *  
 * \author pharvey (2/27/20)
 */
class DLine90Auto : public DLine
{
    Q_OBJECT
public:
    explicit DLine90Auto( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLine90Auto();

    virtual QPixmap getIcon();
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected:
    DRouteSource *  pRouteSource;
    RRouteAStar *   pRoute;

    virtual void doRoute( DSource *p, const QPointF &pointScene );
    virtual void doNewRoute();
    virtual void doRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache = false );
    virtual void doDeleteRoute();

    // We have to keep track of the line segment orientation. 
    // Comparing coordinates does not work when points are the same (crossing over each other for example).
    // Storing in handles does not work either as handles are transient.
    // This should always have a count of 1 less than polygon.count.
    QVector<Qt::Orientation> vectorSegments;
};

#endif


