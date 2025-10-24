#ifndef H_MainWindow
#define H_MainWindow

#include <CBD.h>

class RouteWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

protected slots:
    void slotRoute( QList<QPointF> );

protected:
    RouteWidget *   pWidgetRoute;
    QTextEdit *     pOutput;

};

#endif



