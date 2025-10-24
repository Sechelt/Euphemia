#ifndef H_DLineDynamic
#define H_DLineDynamic

#include "DLine.h"

class DDiagram;
class DRouteSource;
class RRouteAStar;

/*!
 * \brief A general purpose auto-routing line which favours 90 degree angles.
 *  
 * This is experimental. 
 *  
 * This works at the granularity of the grid and will NOT use diagonal routes with 
 * exception of arrival and departure. 
 *  
 * Uses RRouteDynamic. 
 *  
 * \author pharvey (5/14/20)
 */
class DLineDynamic : public DLine
{
    Q_OBJECT
public:
    enum OffsetTypes
    {
        OffsetDynamic = 0,
        OffsetCell,
        OffsetCustom,
        OffsetNone
    };

    explicit DLineDynamic( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLineDynamic();

    virtual void setOffset( OffsetTypes nOffset );
    virtual void setOffsetSize( const QSize size );

    virtual QPixmap     getIcon();
    virtual OffsetTypes getOffset() { return nOffset; }
    virtual QSize       getOffsetSize() { return sizeOffset; }
    virtual QPointF     getOffset( const DIntersectedLine &IntersectedLine );

    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected:
    DRouteSource *  pRouteSource;
    RRouteAStar *   pRoute;
    // for calculating start/end of route \sa getRoutePoint
    OffsetTypes     nOffset;                    
    QSize           sizeOffset;

    virtual void doRoute( DSource *p, const QPointF &pointScene );

    virtual void doNewRoute();
    virtual void doRoute( const QPointF &pointBegin, const QPointF &pointEnd, bool bCache = false );
    virtual void doDeleteRoute();
};

#endif


