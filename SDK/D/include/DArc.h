#ifndef H_DArc
#define H_DArc

#include "DLine.h"

// number of steps in curve
#define DARC_N_STEPS 64
#define DARC_SEG_MAX 128

/*!
 * \brief Arc 
 *  
 * This does not use the QPainter::drawArc as that is rect based and we want a line 
 * where the focus is on connecting two rectangles. 
 *  
 * This is good for Data Flow Diagrams (for example). 
 *  
 * This extends DLine; 
 *  
 * - limit line/polygon to just 2 points (1st and 2nd points for arc)
 * - add a handle for expressing arc (3rd point for arc)
 *  
 * \author pharvey (11/22/19)
 */
class DArc : public DLine
{
    Q_OBJECT
public:
    explicit DArc( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DArc();

    // SETTERS
    virtual void setPos( const QPointF &point );
    // virtual void setPos( DSource *pSource, const QPointF &pointScene );
    virtual void setStyle( CBD::EOLTypes nEOL, W::WLineEndStyles nStyle );
    
    // GETTERS
    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Arc" ); }
    virtual DHandle *   getHandleArc() { return vectorHandles.at( 0 ); }

    // DO'RS
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemLine );

protected slots:
    virtual void slotTruncateBegin( Qt::CheckState n );
    virtual void slotTruncateEnd( Qt::CheckState n );

protected:
    QPolygonF   polygonArc;             // calculated arc - created/recreated using getArc() as needed so its ready to paint
    QPointF     pointArcDelta;          // delta from center of item boundingRect to handle position (in scene coords) for expressing arc
    QLineF      lineIntersectedBegin;   // side affect of getArc() that we can use when drawing line end 
    QLineF      lineIntersectedEnd;     // side affect of getArc() that we can use when drawing line end 

    virtual void doMoveSource( DSource *pSource, const QPointF &pointScenePos );
    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesSync();

    virtual QPolygonF getArc();
    virtual QPolygonF getArc2( const QPointF &pointBegin, const QPointF &pointEnd, const QPointF &pointArc );

private:
    // not used but could be of use in the future so tucked away here
    bool objectUnderPoint( const QPoint &point );
    float get_angle(QPoint v);
};


#endif


