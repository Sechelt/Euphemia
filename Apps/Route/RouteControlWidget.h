#ifndef H_RouteControlWidget
#define H_RouteControlWidget

#include <CBD.h>
#include <RRoute.h>

#include "RouteSource.h"

class RouteControlWidget : public QWidget
{
    Q_OBJECT
public:
    RouteControlWidget( RouteSource *pSource, QWidget *pWidgetParent );
    ~RouteControlWidget();

    void setSource( RouteSource *pSource );
    void setRoute( RRoute *pRoute );

signals:
    void signalRoute( const QString &s );

protected:
    QWidget *pSourceControls;
    QWidget *pControls;
    QWidget *pRouteControls;

    QWidget *getControls( QWidget *pParent );
};

#endif
