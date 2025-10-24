#include "RouteWidget.h"

#include <RRouteAStar.h>
#include <RRouteDynamic.h>

#include "RouteScene.h"
#include "RouteSource.h"
#include "RouteControlWidget.h"

RouteWidget::RouteWidget( QWidget *pWidgetParent )
    : QSplitter( pWidgetParent )
{
    setOrientation( Qt::Horizontal );

    pRoute      = nullptr;
    pScene      = new RouteScene( this );
    pView       = new QGraphicsView( pScene, this );
    pSource     = new RouteSource( pScene, this );
    pControl    = new RouteControlWidget( pSource, this );

    addWidget( pControl );
    addWidget( pView );
    
// do this in RouteScene::setRoute
//    connect( pRoute, SIGNAL(signalChanged()), pScene, SLOT(slotUpdate()) );

    connect( pControl, SIGNAL(signalRoute(const QString &)), SLOT(slotRoute(const QString &)) );
    // connect( pControl, SIGNAL(signalRoute()), SLOT(slotRoute()) );
}

RouteWidget::~RouteWidget()
{
}

// select a route...
void RouteWidget::slotRoute( const QString &s )
{
    if ( pRoute ) 
    { 
        pControl->setRoute( nullptr );
        pScene->setRoute( nullptr );
        delete pRoute;
        pRoute = nullptr;
    }

    if ( !pSource ) return;

    if ( s == "A*" )
    {
        pRoute = new RRouteAStar( pSource, this );
        pScene->setRoute( pRoute );
        pControl->setRoute( pRoute ); 
    }
    else if ( s == "Dynamic" )
    { 
        pRoute = new RRouteDynamic( pSource, this );
        pScene->setRoute( pRoute );
        pControl->setRoute( pRoute ); 
    }
}

// execute a route...
void RouteWidget::slotRoute()
{
qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Begin:" << pScene->getBegin() << "End:" << pScene->getEnd();
    QList<QPointF> l = pRoute->getRoute( pScene->getBegin(), pScene->getEnd(), false );
    emit signalRoute( l );
qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__;
}


