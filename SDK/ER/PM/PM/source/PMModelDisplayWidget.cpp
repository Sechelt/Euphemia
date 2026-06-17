#include "LibInfo.h"
#include "PMModelDisplayWidget.h"

PMModelDisplayWidget::PMModelDisplayWidget( PMModel *pDiagram, QWidget *pParent )
    : QWidget( pParent )
{
    this->pDiagram = pDiagram;                                                    
                                                                              
    QVBoxLayout *playoutTop      = new QVBoxLayout( this );        
                                                                              
    pcheckboxDisplayDataType    = new QCheckBox( "Data Type", this ); 
    pcheckboxDisplayDomain      = new QCheckBox( "Domain", this );    
    pcheckboxDisplayParams      = new QCheckBox( "Parameters", this );
    pcheckboxDisplayDefault     = new QCheckBox( "Default", this );   
                                                                              
    playoutTop->addWidget( pcheckboxDisplayDataType );                    
    playoutTop->addWidget( pcheckboxDisplayDomain );                      
    playoutTop->addWidget( pcheckboxDisplayParams );                      
    playoutTop->addWidget( pcheckboxDisplayDefault );                     
                                                                              
    if ( pDiagram->getDisplayFlags() & PMModel::DisplayDataType ) pcheckboxDisplayDataType->setChecked( true );                           
    if ( pDiagram->getDisplayFlags() & PMModel::DisplayDomain ) pcheckboxDisplayDomain->setChecked( true );                             
    if ( pDiagram->getDisplayFlags() & PMModel::DisplayParams ) pcheckboxDisplayParams->setChecked( true );                             
    if ( pDiagram->getDisplayFlags() & PMModel::DisplayDefault ) pcheckboxDisplayDefault->setChecked( true );                            

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxDisplayDataType, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDomain(Qt::CheckState)) );
    connect( pcheckboxDisplayDomain, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDataType(Qt::CheckState)) );
    connect( pcheckboxDisplayParams, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotParams(Qt::CheckState)) );
    connect( pcheckboxDisplayDefault, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDefault(Qt::CheckState)) );
#else
    connect( pcheckboxDisplayDataType, SIGNAL(stateChanged(int)), SLOT(slotDomain(int)) );
    connect( pcheckboxDisplayDomain, SIGNAL(stateChanged(int)), SLOT(slotDataType(int)) );
    connect( pcheckboxDisplayParams, SIGNAL(stateChanged(int)), SLOT(slotParams(int)) );
    connect( pcheckboxDisplayDefault, SIGNAL(stateChanged(int)), SLOT(slotDefault(int)) );
#endif
}

PMModelDisplayWidget::~PMModelDisplayWidget()
{
}

void PMModelDisplayWidget::slotDomain( Qt::CheckState n )
{
    if ( n )                  
        pDiagram->setDisplayFlag( PMModel::DisplayDataType );    
    else                                                          
        pDiagram->doClearDisplayFlag( PMModel::DisplayDataType );
}

void PMModelDisplayWidget::slotDataType( Qt::CheckState n )
{
    if ( n )                    
        pDiagram->setDisplayFlag( PMModel::DisplayDomain );      
    else                                                          
        pDiagram->doClearDisplayFlag( PMModel::DisplayDomain );  
}

void PMModelDisplayWidget::slotParams( Qt::CheckState n )
{
    if ( n )                    
        pDiagram->setDisplayFlag( PMModel::DisplayParams );      
    else                                                          
        pDiagram->doClearDisplayFlag( PMModel::DisplayParams );  
}

void PMModelDisplayWidget::slotDefault( Qt::CheckState n )
{
    if ( n )                   
        pDiagram->setDisplayFlag( PMModel::DisplayDefault );     
    else                                                          
        pDiagram->doClearDisplayFlag( PMModel::DisplayDefault ); 
}
     
