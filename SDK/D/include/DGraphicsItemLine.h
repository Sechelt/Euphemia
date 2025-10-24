#ifndef H_DGraphicsItemLine
#define H_DGraphicsItemLine

#include <CBD.h>

/*!
 * \class DGraphicsItemLine 
 * \brief A line for utility use. 
 *  
 * We can not use a QGraphicsLineItem as we want it based upon QGraphicsObject because inherits() is 
 * important in how we filter objects.
 *  
 * Uses; 
 *  
 * 1. used when indicating angle during object rotation 
 * 2. two of these are used for cross hairs 
 * 
 * \author pharvey (2/14/20)
 */
class DGraphicsItemLine : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit DGraphicsItemLine( const QLineF &line );
    ~DGraphicsItemLine();

    void setLine( const QLineF &line );
    void setBegin( const QPointF &point );
    void setEnd( const QPointF &point );

    void setPen( const QPen &pen );



    QLineF getLine() { return line; }
    QPen getPen() { return pen; }

    QRectF boundingRect() const override;
    void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    QPainterPath shape() const;

protected:
    QPen    pen;
    QLineF  line;
};


#endif


