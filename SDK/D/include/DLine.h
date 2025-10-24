#ifndef H_DLine
#define H_DLine

#include <W.h>

#include "DObject.h"

class DDiagram;
class DRectangle;
class DSource;
class DSink;

/*!
 * \brief A line segment that is within a rectangle. 
 *  
 * p1   = first point (ie source/sink)
 * p2   = point that lies on the rectangle edge 
 * nDir = rectangle edge of p2 (N,S,E or W) 
 *  
 * \note There may be a case where p1==p2. 
 * \note Check QLine::isNull() to see if this is valid. 
 *  
 * \author pharvey (5/16/20)
 */
class DIntersectedLine : public QLineF
{
public:
    DIntersectedLine()
    : QLineF()
    {
        nEOL = CBD::EOLNeither;
        nDir = CBD::DirectionUnknown;
    }
    DIntersectedLine( CBD::EOLTypes nEOL, const QPointF &p1, const QPointF &p2, CBD::DirectionTypes nDir )
    : QLineF( p1, p2 )
    {
        this->nEOL = nEOL;
        this->nDir = nDir;
    }

    CBD::EOLTypes nEOL;
    CBD::DirectionTypes nDir;
};

/*!
 * \brief A simple, 2 point, line. 
 *  
 * Lines are most often used to connect DRectangle based objects. 
 *  
 * This is the base class for all lines - as such - it supports many features beyond what is needed here. 
 *  
 * Some of these features are; 
 *  
 * - can have 2-n points 
 * - can have 0-n sources (sources can connect to a sink) 
 * - can have 0-n handles (used to manipulate the line) - in practice there will be at least one handle 
 * - can have a label 
 *  
 * This class implements; 
 *  
 * - exactly 2 points 
 * - a source on each end (incl. a handle for moving line end)
 * - a floating text item that is placed relative to the center of the line 
 * - a handle for the floating text to allow changing its relative position 
 *  
 * \note An ADObject can have references to other objects. These references can be created 
 * without connecting a line to a rectangle (as we can do here). However; connecting a line 
 * to a rectangle will create a reference. 
 *  
 * \author pharvey (2/26/20)
 */
class DLine : public DObject
{
    Q_OBJECT
public:
    explicit DLine( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLine();

    virtual void setName( const QString &sringName );
    virtual void setCode( const QString &sringCode );
    virtual void setComment( const QString &sringComment );
    virtual void setPos( const QPointF &point );
    virtual void setPosRaw( const QPointF &point );
    virtual void setPos( DSource *pSource, const QPointF &pointScene );
    virtual void setProxy( DGraphicsProxyItem *p );
    virtual void setSelected( bool b = true );
    virtual void setVisible( bool b = true );
    virtual void setBegin( const QPointF &pointScene );
    virtual void setEnd( const QPointF &pointScene );
    virtual void setStyle( CBD::EOLTypes nEOL, W::WLineEndStyles nStyle );

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Line" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QPolygonF           getPolygon() { return polygon; }
    virtual QPointF             getBegin() { return polygon.first(); }
    virtual QPointF             getEnd() { return polygon.last(); }
    virtual DIntersectedLine    getIntersectedLine( CBD::EOLTypes n );
    virtual DSource *           getSource( CBD::EOLTypes n );
    virtual DSource *           getSource( const QString &stringName );
    virtual DSource *           getSource( DHandle *p );
    virtual QVector<DSource*>   getSources() { return vectorSources; }
    virtual DRectangle *        getRectangle( CBD::EOLTypes n );
    virtual W::WLineEndStyles   getStyle( CBD::EOLTypes n ) { return (n == CBD::EOLBegin ? nStyleBegin : nStyleEnd); }
    virtual bool                isTruncate( CBD::EOLTypes n ) { return (n == CBD::EOLBegin ? bTruncateBegin : bTruncateEnd); }
    virtual bool                isAutoRouted() { return bAutoRouted; }
    // virtual bool                isConnected( CBD::EOLTypes n ) { return getRectangle( n ); }

    virtual void doRoute();
    virtual void doGroupSync();
    virtual void doGroupScale( const QSizeF &sizeFactor );
    virtual void doGroupIn( const QPointF &pointDelta );
    virtual void doGroupOut( const QPointF &pointDelta );
    virtual bool doConnect( DSource *pSource, DSink *pSink );
    virtual bool doConnect( ADObject *pObject, const QString &stringName = QString() );                             // create a reference to pObject 
    virtual bool doDisconnect( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual void doDisconnect( CBD::EOLTypes n );

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

    // save/load item/element of a file
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual void doPostLoad();

    // REMOVE THESE? (legacy from when model and presentation were separated)
    virtual void doUsesAdd( ADObject *pmodel, const QString &stringName = QString() );
    virtual void doUsesRemove( ADObject *pmodel, const QString &stringName = QString() );

    //
    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected slots:
    virtual void slotStyleBegin( W::WLineEndStyles n );
    virtual void slotStyleEnd( W::WLineEndStyles n );
    virtual void slotTruncateBegin( Qt::CheckState nState );
    virtual void slotTruncateEnd( Qt::CheckState nState );
    virtual void slotTextDisplay( int n );

protected:
    bool                bAutoRouted;    // defaults is false - derived classes can change this if they support auto routing
    qreal               nPadding;       // amount to pad boundingRect to make clicking/selecting line easier
    // our line **IN ITEM COORDS**
    QPolygonF           polygon;
    bool                bTruncateBegin;
    bool                bTruncateEnd;
    W::WLineEndStyles   nStyleBegin;
    W::WLineEndStyles   nStyleEnd;

    // floating text relative to center
    QGraphicsTextItem * pFloatingTextCenter;
    QPointF             pointDeltaFloatingTextCenter;

    virtual QPointF getFloatingTextCenterPos(); // in scene coords
    virtual QString getFloatingTextText();

    virtual void doMoveSource( DSource *pSource, const QPointF &pointScenePos );
    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate(); // intermediate handles (if any)
    virtual void doHandlesDelete();
    virtual void doHandlesSync();   // intermediate handles (if any)

    // SOURCES 
    // We extend DObject connections by providing a location/coordinate and a handle.
    // Typical situation is that a source exists at begin/end of line. The handle of the source
    // is then representing the begin/end of the line and NOT in the usual vectorHandles list. 
    QVector<DSource*> vectorSources;  

    virtual void doSourcesCreate();
    virtual void doSourcesVisible( bool b = true );
    virtual void doSourcesDelete();
    virtual void doSourcesSync();

    virtual void doRoute( DSource *p, const QPointF &pointScene );
    virtual void doMergePoints( const QList<QPoint> &listCells );
    virtual void doMergePoints( const QList<QPointF> &listPoints );

    virtual void doNormalize();
    //
    virtual void doDrawBegin( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected );
    virtual void doDrawEnd( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected );
    virtual void doDrawArrow( QPainter *pPainter, const QLineF &lineSegment );
    virtual void doDrawBall( QPainter *pPainter, const QLineF &lineSegment );

    virtual QPolygonF getTruncated( QLineF &lineIntersectedBegin, QLineF &lineIntersectedEnd );
    virtual QPointF getIntersect( const QLineF &line, const QRectF &rect, QLineF &lineIntersected );
    virtual Qt::Orientation getOrientation( const QPointF &point1, const QPointF &point2 );

    // debugging purposes
    void doDumpHandlesVector();
    void doDumpPoints();
    void doDumpPointsScene();
};

#endif


