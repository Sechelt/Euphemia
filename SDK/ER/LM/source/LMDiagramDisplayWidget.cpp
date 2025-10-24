#include "LibInfo.h"
#include "LMDiagramDisplayWidget.h"

LMDiagramDisplayWidget::LMDiagramDisplayWidget( LMModel *pDiagram, QWidget *pParent )
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
                                                                              
    if ( pDiagram->getDisplayFlags() & LMModel::DisplayDataType ) pcheckboxDisplayDataType->setChecked( true );                           
    if ( pDiagram->getDisplayFlags() & LMModel::DisplayDomain ) pcheckboxDisplayDomain->setChecked( true );                             
    if ( pDiagram->getDisplayFlags() & LMModel::DisplayParams ) pcheckboxDisplayParams->setChecked( true );                             
    if ( pDiagram->getDisplayFlags() & LMModel::DisplayDefault ) pcheckboxDisplayDefault->setChecked( true );                            

    connect( pcheckboxDisplayDataType, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDomain(Qt::CheckState)) );
    connect( pcheckboxDisplayDomain, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDataType(Qt::CheckState)) );
    connect( pcheckboxDisplayParams, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotParams(Qt::CheckState)) );
    connect( pcheckboxDisplayDefault, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDefault(Qt::CheckState)) );
}

LMDiagramDisplayWidget::~LMDiagramDisplayWidget()
{
}

void LMDiagramDisplayWidget::slotDomain( Qt::CheckState n )
{
    if ( n )                  
        pDiagram->setDisplayFlag( LMModel::DisplayDataType );    
    else                                                          
        pDiagram->doClearDisplayFlag( LMModel::DisplayDataType );
}

void LMDiagramDisplayWidget::slotDataType( Qt::CheckState n )
{
    if ( n )                    
        pDiagram->setDisplayFlag( LMModel::DisplayDomain );      
    else                                                          
        pDiagram->doClearDisplayFlag( LMModel::DisplayDomain );  
}

void LMDiagramDisplayWidget::slotParams( Qt::CheckState n )
{
    if ( n )                    
        pDiagram->setDisplayFlag( LMModel::DisplayParams );      
    else                                                          
        pDiagram->doClearDisplayFlag( LMModel::DisplayParams );  
}

void LMDiagramDisplayWidget::slotDefault( Qt::CheckState n )
{
    if ( n )                   
        pDiagram->setDisplayFlag( LMModel::DisplayDefault );     
    else                                                          
        pDiagram->doClearDisplayFlag( LMModel::DisplayDefault ); 
}
     
