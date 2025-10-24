#ifndef H_FLFlow
#define H_FLFlow

#include <DLine.h>

class FLDiagram;

class FLFlow : public DLine
{
    Q_OBJECT
public:
    explicit FLFlow( FLDiagram *pParent, const QString &stringName = QString() );
    virtual ~FLFlow();

    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Flow" ); }
    virtual QPointF getElbow() { return polygon.at( 1 ); }
     
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


