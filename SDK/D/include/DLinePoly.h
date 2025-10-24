#ifndef H_DLinePoly
#define H_DLinePoly

#include "DLine.h"

/*!
 * \brief Extends DLine to allow 2-n points.
 *  
 * A handle is added between each pair of points (middle of a line segment). 
 * A new point is created when/if this handle is dragged. 
 *  
 * A point can be removed by right-clicking on its handle and selecting Delete from the menu. 
 *  
 * \author pharvey (2/26/20)
 */
class DLinePoly : public DLine
{
    Q_OBJECT
public:
    enum EOLTypes
    {
        EOLBegin = 0,
        EOLEnd,
        EOLNeither
    };

    explicit DLinePoly( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLinePoly();

    virtual QPixmap getIcon();
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void contextMenu( DHandle *pHandle, QGraphicsSceneContextMenuEvent *pEvent );

protected slots:
    virtual void slotDeleteHandle();

protected:
    DHandle *  pHandleRemoveLineSegment;

    virtual void doHandlesCreate(); // intermediate handles (if any)
    virtual void doHandlesSync();   // intermediate handles (if any)
};

#endif


