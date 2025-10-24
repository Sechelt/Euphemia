#ifndef H_RouteScene
#define H_RouteScene

// we need RSourceGrid and RSourceCell...
#include <RRoute.h>
#include <RSource.h>

/*!
 * \brief A scene for testing, debugging, tweaking, and visualizing routing.
 *  
 * This scene translates pixel coordinates to grid/cell coordinates ASAP and then 
 * operates in grid/cell coordinates. 
 *  
 * \author pharvey (3/4/20)
 */
class RouteScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum ModeTypes
    {
        ModeClear,
        ModeBegin,
        ModeEnd,
        ModeLine,
        ModeShape,
        ModeInfo
    };

    RouteScene( QObject *pObject = nullptr );
    ~RouteScene();

    void setMode( ModeTypes n ) { nMode = n; }
    void setRoute( RRoute *p );

    QWidget *   getControls( QWidget *pParent );
    QSize       getGridSize() { return sizeGrid; }
    QSize       getCellSize() { return sizeCell; }
    RSourceCell getCell( const QPoint &pointCell );
    QPoint      getBegin();
    QPoint      getEnd();
    bool        isValid( const QPoint &pointCell );

    QPoint  mapSceneToCell( const QPointF &pointScene );
    QPointF mapCellToScene( const QPoint &pointCell );

public slots:
    void slotMode( const QString &s );
    void slotUpdate();

protected:
    QSize       sizeGrid;   // in cells
    QSize       sizeCell;   // in pixels 
    RSourceGrid grid;       // our data/content
    RRoute *    pRoute;     // current route object - we use this, if available, when painting scene to show routing info
    ModeTypes   nMode;
    // Begin/End in cell coords because that is the system we work with here
    QPoint      pointBegin;
    QPoint      pointEnd;

    void mousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    void drawBackground( QPainter *painter, const QRectF &rect );
    void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent );
};

#endif


