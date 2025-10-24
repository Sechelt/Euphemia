#ifndef H_LCrossHairLine
#define H_LCrossHairLine

#include "LSPointer.h"

/*!
 * \class LCrossHairLine 
 * \brief A crosshair line.
 *  
 * \author pharvey (4/28/23)
 */
class LCrossHairLine : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit LCrossHairLine( Qt::Orientation n );
    ~LCrossHairLine();

    void setLine( const QPointF &point, const QLineF &line );

//    QLineF getLine() { return line; }

    QRectF boundingRect() const override;
    void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    LSPointer *     pSettingPointer;
    Qt::Orientation nOrientation;
    QPen            pen;
    QPointF         point;
    QLineF          line;
    QSize           sizeCursor  = QSize( 0, 0 );

protected slots:
    void slotRefreshPointer();
};


#endif


