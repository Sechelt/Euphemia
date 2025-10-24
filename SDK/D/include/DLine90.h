#ifndef H_DLine90
#define H_DLine90

#include "DLine.h"

/*!
 * \brief A line which maintains a single 90.
 *  
 * This supports \sa DLine90H and \sa DLine90V and is not meant to be used directly.
 *  
 * \author pharvey (2/27/20)
 */
class DLine90 : public DLine
{
    Q_OBJECT
public:
    explicit DLine90( Qt::Orientation nOrientation, ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLine90();

    virtual QPixmap getIcon();
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected:
    Qt::Orientation nOrientation;

    // We have to keep track of the line segment orientation. 
    // Comparing coordinates does not work when points are the same (crossing over each other for example).
    // Storing in handles does not work either as handles are transient.
    // This should always have a count of 1 less than polygon.count.
    QVector<Qt::Orientation> vectorSegments;

    virtual void doRoute( DSource *p, const QPointF &pointScene );

    virtual void doHandlesCreate(); // intermediate handles (if any)
    virtual void doHandlesSync();   // intermediate handles (if any)
};

#endif


