#ifndef H_DRouteSource
#define H_DRouteSource

#include <RSource.h>

class DDiagram;
class DObject;

/*!
 * \brief RSource for use with DDiagram 
 *  
 * In this case we maintain a cache as getting some of the information can cost a bit.
 * 
 * \author pharvey (3/4/20)
 */
class DRouteSource : public RSource
{
    Q_OBJECT
public:
    DRouteSource( DDiagram *pDiagram, const QSize &sizeCell, QObject *pObjectParent = nullptr );

    virtual void setBegin( const QPointF &point ) { pointBegin = point; }
    virtual void setEnd( const QPointF &point ) { pointEnd = point; }
    virtual void setCache( bool b = true ) { bCache = b; }
    virtual void setIgnore( QList<DObject*> l ) { listIgnore = l; }

    virtual QPointF getBegin() { return pointBegin; }
    virtual QPointF getEnd() { return pointEnd; }
    // map/scene width/height in cells  
    virtual QSize getGridSize();
    // cell width/height in pixels
    virtual QSize getCellSize();
    // cell contents
    virtual CellContentTypes getCellContent( const QPoint &pointCell );
    // used to indicate that there is no information in the source
    virtual bool isEmpty() { return !pDiagram; }

    //
    virtual QWidget *getControls( QWidget *pParent ) { Q_UNUSED(pParent); return nullptr; }

protected:
    DDiagram *      pDiagram;
    QPointF         pointBegin;     // begin pos (in scene/pixel coord) 
    QPointF         pointEnd;       // end pos (in scene/pixel coord) 
    QSize           sizeCell;
    QSize           sizeGrid;
    RSourceGrid     grid;           // cache
    bool            bCache;
    QList<DObject*> listIgnore;     // DIObjects to ignore (probably because manipulating DObject (line and possibly connected shapes))

    void doCellLoad( const QPoint &pointCell );
};

#endif

