#include "RRouteDynamicControl.h"

RRouteDynamicControl::RRouteDynamicControl( RRouteDynamic *p, QWidget *pParent )
: QWidget( pParent )
{
    pRoute = p;

    QVBoxLayout *pLayout = new QVBoxLayout( this  );

    pLayout->addWidget( new QLabel( "View", this ) );     
                                     
    pExploreLines = new QCheckBox( "Explore Lines", this );                     
    pExploreLines->setChecked( pRoute->isExploreLines() );
    connect( pExploreLines, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotExploreLines(Qt::CheckState)) );     
    pLayout->addWidget( pExploreLines );                                                      
                                                                                              
    pRouteLines = new QCheckBox( "Route Lines", this );
    pRouteLines->setChecked( pRoute->isRouteLines() );                         
    connect( pRouteLines, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotRouteLines(Qt::CheckState)) );         
    pLayout->addWidget( pRouteLines );                                                        
                                                                                              
    pDetails = new QCheckBox( "Details", this );      
    pDetails->setChecked( pRoute->isDetails() );                          
    connect( pDetails, SIGNAL(checkStateChanged(Qt::CheckState)), pRoute, SLOT(slotDetails(Qt::CheckState)) );               
    pLayout->addWidget( pDetails );                                                           
                                                                                              
    pLayout->addWidget( new QLabel( "Depart", this ) );                                    
    pDepart = new QComboBox( this );                                            
    pDepart->addItem( "North" );                                                              
    pDepart->addItem( "East" );                                                              
    pDepart->addItem( "South" );                                                              
    pDepart->addItem( "West" );                                                              
    pDepart->addItem( "NorthEast" );                                                               
    pDepart->addItem( "SouthEast" );                                                               
    pDepart->addItem( "SouthWest" );                                                            
    pDepart->addItem( "NorthWest" );                                                              
    pDepart->addItem( "unknown" );                                                 
    pDepart->setCurrentIndex( pRoute->getDepart() );           
    connect( pDepart, SIGNAL(currentIndexChanged(int)), pRoute, SLOT(slotDepart(int)) );
    pLayout->addWidget( pDepart );                                                            
                                                                                              
    pLayout->addWidget( new QLabel( "Arrive", this ) );                                    
    pArrive = new QComboBox( this );                                            
    pArrive->addItem( "North" );                                                              
    pArrive->addItem( "East" );                                                              
    pArrive->addItem( "South" );                                                              
    pArrive->addItem( "West" );                                                              
    pArrive->addItem( "NorthEast" );                                                               
    pArrive->addItem( "SouthEast" );                                                               
    pArrive->addItem( "SouthWest" );                                                            
    pArrive->addItem( "NorthWest" );                                                              
    pArrive->addItem( "unknown" );                                                 
    pArrive->setCurrentIndex( pRoute->getArrive() );           
    connect( pArrive, SIGNAL(currentIndexChanged(int)), pRoute, SLOT(slotArrive(int)) );
    pLayout->addWidget( pArrive );                                                            
                                                                                              
    pLayout->addWidget( new QLabel( "Cost to Break Straight Line", this ) );               
    pStraightLine = new QSpinBox( this );                                                          
    pStraightLine->setMinimum( 0 );                                                                   
    pStraightLine->setMaximum( 5 );                                                                   
    pStraightLine->setValue( pRoute->getCostBreakStraightLine() );                                                                     
    connect( pStraightLine, SIGNAL(valueChanged(int)), pRoute, SLOT(slotCostBreakStraightLine(int)) );        
    pLayout->addWidget( pStraightLine );                                                              
                                                                                              
    pLayout->addWidget( new QLabel( "Cost to Cross Line", this ) );                        
    pCrossLine = new QSpinBox( this );                                                          
    pCrossLine->setMinimum( 0 );                                                                   
    pCrossLine->setMaximum( 5 );                                                                   
    pCrossLine->setValue( pRoute->getCostCrossLine() );                                                                     
    connect( pCrossLine, SIGNAL(valueChanged(int)), pRoute, SLOT(slotCostCrossLine(int)) );                
    pLayout->addWidget( pCrossLine );                                                              
                                                                                              
    pLayout->addWidget( new QLabel( "Cost to Leave Begin Axis", this ) );                  
    pLeaveBeginAxis = new QSpinBox( this );                                                          
    pLeaveBeginAxis->setMinimum( 0 );                                                                   
    pLeaveBeginAxis->setMaximum( 5 );                                                                   
    pLeaveBeginAxis->setValue( pRoute->getCostLeaveBeginAxis() );                                                                     
    connect( pLeaveBeginAxis, SIGNAL(valueChanged(int)), pRoute, SLOT(slotCostLeaveBeginAxis(int)) );           
    pLayout->addWidget( pLeaveBeginAxis );                                                              
                                                                                              
    pLayout->addWidget( new QLabel( "Cost to Leave End Axis", this ) );                    
    pLeaveEndAxis = new QSpinBox( this );                                                          
    pLeaveEndAxis->setMinimum( 0 );                                                                   
    pLeaveEndAxis->setMaximum( 5 );                                                                   
    pLeaveEndAxis->setValue( pRoute->getCostLeaveEndAxis() );                                                                     
    connect( pLeaveEndAxis, SIGNAL(valueChanged(int)), pRoute, SLOT(slotCostLeaveEndAxis(int)) );             
    pLayout->addWidget( pLeaveEndAxis );                                                              
                                                                                              
    pLayout->addWidget( new QLabel( "Delay", this ) );                                     
    pDelay = new QSpinBox( this );                                               
    pDelay->setMinimum( 0 );                                                                  
    pDelay->setMaximum( 5 );                                                                  
    pDelay->setValue( pRoute->getDelay() );                                                                    
    connect( pDelay, SIGNAL(valueChanged(int)), pRoute, SLOT(slotDelay(int)) );                       
    pLayout->addWidget( pDelay );                                                             
                                                                                              
    QPushButton *pRouteButton = new QPushButton( "Route", this );                                
    connect( pRouteButton, SIGNAL(clicked()), pRoute, SLOT(slotRoute()) );                                  
    pLayout->addWidget( pRouteButton );                                                             

    pLayout->addStretch( 10 );

    connect( pRoute, SIGNAL(signalChangedConfig()), SLOT(slotModified()) );
}

void RRouteDynamicControl::slotModified()
{
    pExploreLines->setChecked( pRoute->isExploreLines() );
    pRouteLines->setChecked( pRoute->isRouteLines() );                         
    pDetails->setChecked( pRoute->isDetails() );                          
    pDepart->setCurrentIndex( pRoute->getDepart() );           
    pArrive->setCurrentIndex( pRoute->getArrive() );           
    pStraightLine->setValue( pRoute->getCostBreakStraightLine() );                                                                     
    pCrossLine->setValue( pRoute->getCostCrossLine() );                                                                     
    pLeaveBeginAxis->setValue( pRoute->getCostLeaveBeginAxis() );                                                                     
    pLeaveEndAxis->setValue( pRoute->getCostLeaveEndAxis() );                                                                     
    pDelay->setValue( pRoute->getDelay() );                                                                    
}


