#include "LibInfo.h"
#include "LMValidationWidget.h"

LMValidationWidget::LMValidationWidget( LMModel *pmodel, QWidget *pwidgetParent )
: QWidget( pwidgetParent )
{
    this->pmodel = pmodel;                                                                                  
                                                                                                            
    QVBoxLayout *playoutTop  = new QVBoxLayout( this );                                                          
                                                                                                            
    // Level                                                                                                
    QHBoxLayout *playout = new QHBoxLayout();                                                      

    QLabel *plabel = new QLabel( "Level", this );                                                  
    playout->addWidget( plabel );                                                                 
                                                                                                            
    pcomboboxLevels = new QComboBox( this );                                                                
    pcomboboxLevels->addItem( "" );                                                                      
    pcomboboxLevels->addItem( "Low" );                                                                   
    pcomboboxLevels->addItem( "Medium" );                                                                
    pcomboboxLevels->addItem( "High" );                                                                  
    pcomboboxLevels->setToolTip( tr("select a validation level and then customize as required") );           
    playout->addWidget( pcomboboxLevels );                                                        
    connect( pcomboboxLevels, SIGNAL(currentTextChanged(const QString &)), SLOT(slotLevelChanged(const QString &)) );

    playoutTop->addLayout( playoutTop );                              
                                                                              
    // Properties                                                                                           
//    pproperties = new CBDProperties( &pmodel->listValidateProperties, this );                               
//    playoutTop->addWidget( pproperties );                                                                   
}

LMValidationWidget::~LMValidationWidget()
{
}

void LMValidationWidget::slotApply()
{
    // auto applied
}

/*!
    slotLevelChanged
    
    Update help text.
*/
void LMValidationWidget::slotLevelChanged( const QString &stringLevel )
{
    if ( stringLevel == "Low" )                                                  
    {                                                                            
/*
        (pmodel->listValidateProperties).clear();                                
        pmodel->getValidations( ADObject::nValidationLevelLow );                 
        delete pproperties;                                                      
        pproperties = new CBDProperties( &pmodel->listValidateProperties, this );
        playoutTop->addWidget( pproperties );                                    
        pproperties->show();                                                     
*/
    }                                                                            
    else if ( stringLevel == "Medium" )                                          
    {                                                                            
/*
        (pmodel->listValidateProperties).clear();                                
        pmodel->getValidations( ADObject::nValidationLevelMedium );              
        delete pproperties;                                                      
        pproperties = new CBDProperties( &pmodel->listValidateProperties, this );
        playoutTop->addWidget( pproperties );                                    
        pproperties->show();                                                     
*/
    }                                                                            
    else if ( stringLevel == "High" )                                            
    {                                                                            
/*
        (pmodel->listValidateProperties).clear();                                
        pmodel->getValidations( ADObject::nValidationLevelHigh );                
        delete pproperties;                                                      
        pproperties = new CBDProperties( &pmodel->listValidateProperties, this );
        playoutTop->addWidget( pproperties );                                    
        pproperties->show();                                                     
*/
    }                                                                            
}


