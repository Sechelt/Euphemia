#ifndef H_DLinePolyAuto
#define H_DLinePolyAuto

#include "DLinePoly.h"

class DRouteSource;
class RRouteAStar;

/*!
 * \brief Extends DLinePoly (segmented line) with automatic routing.
 *  
 * This works at the granularity of the grid and will use a mix of 90 degree AND 
 * diagonal routes as needed. 
 *  
 * Uses RRouteAStar. 
 *  
 * \author pharvey (2/26/20)
 */
class DLinePolyAuto : public DLinePoly
{
    Q_OBJECT
public:
    explicit DLinePolyAuto( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLinePolyAuto();

    virtual QPixmap getIcon();
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected:
    DRouteSource *  pRouteSource;
    RRouteAStar *   pRoute;

    virtual void doRoute( DSource *p, const QPointF &pointScene );
};

#endif


