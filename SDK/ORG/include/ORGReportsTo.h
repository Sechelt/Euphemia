#ifndef H_ORGReportsTo
#define H_ORGReportsTo

#include <DLine.h>

class ORGDiagram;
class ORGPerson;

/*!
 * \brief A line for connecting a Superior to a Subordinate. 
 *  
 * The Begin is always connected to a Superior. 
 * The End is always connected to a Subordinate. 
 * This will route; 
 *      - south from Superior to Subordinate
 *      - will go east/west (if needed) half way between the two rectangles
 *      - 90 degree elbows as needed  
 * 
 * \author pharvey (4/9/20)
 */
class ORGReportsTo : public DLine
{
    Q_OBJECT
public:
    explicit ORGReportsTo( ORGDiagram *pParent, const QString &stringName = QString() );
    virtual ~ORGReportsTo();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Reports To" ); }
    virtual QPointF     getElbow() { return polygon.at( 1 ); }
    virtual ORGPerson * getSuperior();
    virtual ORGPerson * getSubordinate();

    virtual void doRoute();

    virtual bool hasElbow() { return polygon.count() > 2; }

    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene );  
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );   
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

public slots:
    virtual void slotDelete();

protected:
    virtual void doRoute( DSource *p, const QPointF &pointScene );
};

#endif


