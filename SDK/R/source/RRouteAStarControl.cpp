#include "RRouteAStarControl.h"

RRouteAStarControl::RRouteAStarControl( RRouteAStar *p, QWidget *pParent )
: QWidget( pParent )
{
    pRoute = p;

    QVBoxLayout *pLayout = new QVBoxLayout( this  );

    pLayout->addWidget( new QLabel( "View", this  ) );
    pExploreLines = new QCheckBox( "Explore Lines", this  );
    pExploreLines->setChecked( pRoute->isExploreLines() );
    connect( pExploreLines, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotExploreLines(Qt::CheckState)) );
    pLayout->addWidget( pExploreLines );

    pRouteLines = new QCheckBox( "Route Lines", this  );
    pRouteLines->setChecked( pRoute->isRouteLines() );
    connect( pRouteLines, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotRouteLines(Qt::CheckState)) );
    pLayout->addWidget( pRouteLines );

    pDetails = new QCheckBox( "Details", this  );
    pDetails->setChecked( pRoute->isDetails() );
    connect( pDetails, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotDetails(Qt::CheckState)) );
    pLayout->addWidget( pDetails );

    pLayout->addWidget( new QLabel( "Heuristic", this  ) );
    pHeuristic = new QComboBox( this  );
    pHeuristic->addItem( "none" );
    pHeuristic->addItem( "Manhatten (4-way + prox)" );
    pHeuristic->addItem( "Diagonal (8-way + prox)" );
    pHeuristic->addItem( "Euclidean (any angle)" );
    pHeuristic->setCurrentIndex( pRoute->getHeuristic() );
    connect( pHeuristic, SIGNAL(currentIndexChanged(int)), pRoute, SLOT(slotHeuristic(int)) );
    pLayout->addWidget( pHeuristic );

    pDiagonal = new QCheckBox( "Diagonal", this  );
    pDiagonal->setChecked( pRoute->isDiagonal() );
    connect( pDiagonal, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotDiagonal(Qt::CheckState)) );
    pLayout->addWidget( pDiagonal );

    pStraightLine = new QCheckBox( "Straight Line", this );
    pStraightLine->setChecked( pRoute->isStraightLinePreference() );
    connect( pStraightLine, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotStraightLine(Qt::CheckState)) );
    pLayout->addWidget( pStraightLine );

    pLayout->addWidget( new QLabel( "Straight Line", this  ) );
    pStraighLineValue = new QDoubleSpinBox( this );
    pStraighLineValue->setMinimum( 0 );
    pStraighLineValue->setMaximum( 2 );
    pStraighLineValue->setSingleStep( 0.1 );
    pStraighLineValue->setValue( pRoute->getStraightLinePreference() );
    connect( pStraighLineValue, SIGNAL(valueChanged(double)), pRoute, SLOT(slotStraightLine(double)) );
    pLayout->addWidget( pStraighLineValue );

    pLayout->addWidget( new QLabel( "Delay", this ) );
    pDelay = new QSpinBox( this  );
    pDelay->setMinimum( 0 );
    pDelay->setMaximum( 5 );
    pDelay->setValue( pRoute->getDelay() );
    connect( pDelay, SIGNAL(valueChanged(int)), pRoute, SLOT(slotDelay(int)) );
    pLayout->addWidget( pDelay );

    QPushButton *pRouteButton = new QPushButton( "Route", this  );
    connect( pRouteButton, SIGNAL(clicked()), pRoute, SLOT(slotRoute()) );
    pLayout->addWidget( pRouteButton );

    pLayout->addStretch( 10 );

    connect( pRoute, SIGNAL(signalChangedConfig()), SLOT(slotModified()) );
}

void RRouteAStarControl::slotModified()
{
    pExploreLines->setChecked( pRoute->isExploreLines() );
    pRouteLines->setChecked( pRoute->isRouteLines() );
    pDetails->setChecked( pRoute->isDetails() );
    pHeuristic->setCurrentIndex( pRoute->getHeuristic() );
    pDiagonal->setChecked( pRoute->isDiagonal() );
    pStraightLine->setChecked( pRoute->isStraightLinePreference() );
    pStraighLineValue->setValue( pRoute->getStraightLinePreference() );
    pDelay->setValue( pRoute->getDelay() );
}


