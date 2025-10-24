#include "RouteControlWidget.h"

RouteControlWidget::RouteControlWidget( RouteSource *pSource, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    pSourceControls = nullptr;
    pControls = getControls( this );
    pRouteControls = nullptr;

    setSource( pSource );
}

RouteControlWidget::~RouteControlWidget()
{
}

void RouteControlWidget::setSource( RouteSource *pSource )
{
    if ( pSourceControls )
    { 
        delete pSourceControls;
        pSourceControls = nullptr;
    }

    if ( !pSource ) return;

    pSourceControls = pSource->getControls( this );

    if ( layout() ) delete layout();

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pLayout->addWidget( pSourceControls );
    pLayout->addWidget( pControls );
    if ( pRouteControls ) pLayout->addWidget( pRouteControls );

    pLayout->addStretch( 10 );
}

void RouteControlWidget::setRoute( RRoute *pRoute )
{
    if ( pRouteControls )
    { 
        delete pRouteControls;
        pRouteControls = nullptr;
    }

    if ( !pRoute ) return;

    pRouteControls = pRoute->getControls( this );

    if ( layout() ) delete layout();

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    if ( pSourceControls ) pLayout->addWidget( pSourceControls );
    pLayout->addWidget( pControls );
    pLayout->addWidget( pRouteControls );

    pLayout->addStretch( 10 );
}

QWidget *RouteControlWidget::getControls( QWidget *pParent )
{
    QWidget *pWidget = new QWidget( pParent );
    QVBoxLayout *pLayout = new QVBoxLayout( pWidget );

    pLayout->addWidget( new QLabel( "Route", pWidget ) );
    QComboBox *pRoutes = new QComboBox( pWidget );
    pRoutes->addItem( "none" );
    pRoutes->addItem( "A*" );
    pRoutes->addItem( "Dynamic" );
    connect( pRoutes, SIGNAL(textActivated(const QString &)), SIGNAL(signalRoute(const QString &)) );
    pLayout->addWidget( pRoutes );

    return pWidget;
}

