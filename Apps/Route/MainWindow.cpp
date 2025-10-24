#include "MainWindow.h"

#include "RouteWidget.h"

MainWindow::MainWindow() 
    : QMainWindow()
{
    setWindowTitle( "Pathfinding" );

    pWidgetRoute = new RouteWidget( this );                                                      
    setCentralWidget( pWidgetRoute );                                                           
    connect( pWidgetRoute, SIGNAL(signalRoute(QList<QPointF>)), SLOT(slotRoute(QList<QPointF>)) );

    QDockWidget *pDock = new QDockWidget( "Output", this );
    pDock->setObjectName( "Output" );
    pOutput = new QTextEdit( pDock );
    pDock->setWidget( pOutput );
    addDockWidget( Qt::BottomDockWidgetArea, pDock );
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotRoute( QList<QPointF> l )
{
    pOutput->clear();
    QPointF point;
    QString s;
    foreach( point, l )
    {
        s += QString( "(%1,%2)" ).arg( point.x() ).arg( point.y() );
    }
    pOutput->setText( s );
}

