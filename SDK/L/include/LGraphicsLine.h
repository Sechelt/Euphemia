#ifndef H_LGraphicsLine
#define H_LGraphicsLine

#include <CBD.h>

/*!
 * \class LGraphicsLine 
 * \brief A line for utility use. 
 *  
 * We can not use a QGraphicsLineItem as we want it based upon QGraphicsObject because inherits() is 
 * important in how we filter objects.
 *  
 * Uses; 
 *  
 * 1. used when indicating angle during object rotation 
 * 
 * \author pharvey (2/14/20)
 */
class LGraphicsLine : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit LGraphicsLine( const QLineF &line );
    ~LGraphicsLine();

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


