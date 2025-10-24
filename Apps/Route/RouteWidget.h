#ifndef H_RouteWidget
#define H_RouteWidget

#include <CBD.h>

#include <RRoute.h>

class RouteControlWidget;
class RouteScene;
class RouteSource;

class RouteWidget : public QSplitter
{
    Q_OBJECT
public:
    RouteWidget( QWidget *pWidgetParent );
    ~RouteWidget();

signals:
    void signalRoute( QList<QPointF> );

public slots:
    void slotRoute(const QString &);
    void slotRoute();

protected:
    QGraphicsView *pView;
    RouteControlWidget *pControl;
    RouteScene *pScene;
    RouteSource *pSource;
    RRoute *pRoute;
};

#endif
