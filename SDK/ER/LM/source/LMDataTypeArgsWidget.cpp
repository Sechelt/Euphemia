#include "LibInfo.h"
#include "LMDataTypeArgsWidget.h"

LMDataTypeArgsWidget::LMDataTypeArgsWidget( QWidget *pParent, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale )
    : QWidget( pParent )
{
    this->stringLength      = stringLength;
    this->stringPrecision1  = stringPrecision1;
    this->stringPrecision2  = stringPrecision2;
    this->stringScale       = stringScale;

    pEditor     = nullptr;                                                              
    pLength     = nullptr;                                                              
    pPrecision1 = nullptr;                                                              
    pPrecision2 = nullptr;                                                              
    pScale      = nullptr;                                                              
                                                  
    pLayout     = new QVBoxLayout( this );
}

LMDataTypeArgsWidget::~LMDataTypeArgsWidget()
{
}

void LMDataTypeArgsWidget::slotDataTypeSyntax( const QString &stringDataTypeSyntax )
{
    // clear                                                                                  
    if ( pEditor )                                                                      
    {
        if ( pLength ) stringLength = pLength->text();
        if ( pPrecision1 ) stringPrecision1 = pPrecision1->text();
        if ( pPrecision2 ) stringPrecision2 = pPrecision2->text();
        if ( pScale ) stringScale = pScale->text();
                                                                                                 
        delete pEditor;                                                                 
        pEditor     = nullptr;                                                              
        pLength     = nullptr;                                                              
        pPrecision1 = nullptr;                                                              
        pPrecision2 = nullptr;                                                              
        pScale      = nullptr;                                                              
    }                                                                                         
                                                                                              
    //                                                                                        
    pEditor = new QGroupBox( this );                                                    
    pEditor->setTitle( tr("Arguments") );                                                   
    pLayout->addWidget( pEditor );                                                      

    //                                                                                        
    QVBoxLayout *playout = new QVBoxLayout( pEditor );                       
    playout->addStretch( 10 );                                                             

    QHBoxLayout *playoutEdit= new QHBoxLayout();
    playout->addLayout( playoutEdit );                                  
    QLabel *    plabel      = new QLabel( pEditor );                                    
    QFont       font        = plabel->font();                                                 

    font.setBold( true );                                                                     
    plabel->setFont( font );                                                                  
    playoutEdit->addStretch( 10 );                                                            
    playoutEdit->addWidget( plabel );                                                         
    plabel->show();                                                                           

    // parse syntax                                                                           
    QString     stringChar;                                                                   
    bool        bInArgs = false;                                                              

    for ( int n=0; ; n++)                                                                     
    {                                                                                         
        stringChar = stringDataTypeSyntax.mid( n, 1 );                                        
        if ( stringChar.isNull() )                                                            
            break;                                                                            

        if ( stringChar == "(" )                                                              
        {                                                                                     
            plabel->setText( plabel->text() + stringChar );                                   
            bInArgs = true;                                                                   
            continue;                                                                         
        }                                                                                     

        if ( stringChar == ")" )                                                              
        {                                                                                     
            plabel = new QLabel( pEditor );                                             
            plabel->setFont( font );                                                          
            playoutEdit->addWidget( plabel );                                                 
            plabel->setText( stringChar );                                                    
            plabel->show();                                                                   
            bInArgs = false;                                                                  
            continue;                                                                         
        }                                                                                     

        if ( bInArgs )                                                                        
        {                                                                                     
            if ( stringChar == "n" ) // length                                                
            {                                                                                 
                pLength = new QLineEdit( pEditor );                             
                pLength->setValidator( new QIntValidator( pLength ) );        
                pLength->setToolTip( tr("length (n)") );
                playoutEdit->addWidget( pLength );                                    
                pLength->setText( stringLength );                             
                pLength->show();                                                      
                connect( pLength, SIGNAL(textEdited(const QString &)), SIGNAL(signalLengthChanged(const QString &)) );  
            }                                                                                 
            else if ( stringChar == "p" ) // precision1                                       
            {                                                                                 
                pPrecision1 = new QLineEdit( pEditor );                         
                pPrecision1->setValidator( new QIntValidator( pPrecision1 ) );
                pPrecision1->setToolTip( tr("precision (p)") );                        
                playoutEdit->addWidget( pPrecision1 );                                
                pPrecision1->setText( stringPrecision1 );                     
                pPrecision1->show();                                                  
                connect( pPrecision1, SIGNAL(textEdited(const QString &)), SIGNAL(signalPrecision1Changed(const QString &)) );  
            }                                                                                 
            else if ( stringChar == "q" ) // precision2                                       
            {                                                                                 
                pPrecision2 = new QLineEdit( pEditor );                         
                pPrecision2->setValidator( new QIntValidator( pPrecision2 ) );
                pPrecision2->setToolTip( tr("precision (q)") );                        
                playoutEdit->addWidget( pPrecision2 );                                
                pPrecision2->setText( stringPrecision2 );                     
                pPrecision2->show();                                                  
                connect( pPrecision2, SIGNAL(textEdited(const QString &)), SIGNAL(signalPrecision2Changed(const QString &)) );  
            }                                                                                 
            else if ( stringChar == "s" ) // scale                                            
            {                                                                                 
                pScale = new QLineEdit( pEditor );                              
                pScale->setValidator( new QIntValidator( pScale ) );          
                pScale->setToolTip( tr("scale (s)") );                                 
                playoutEdit->addWidget( pScale );                                     
                pScale->setText( stringScale );                               
                pScale->show();
                connect( pScale, SIGNAL(textEdited(const QString &)), SIGNAL(signalScaleChanged(const QString &)) );  
            }                                                                                 
            else if ( stringChar == "," ) // literal                                          
            {                                                                                 
                plabel = new QLabel( pEditor );                                         
                plabel->setFont( font );                                                      
                playoutEdit->addWidget( plabel );                                             
                plabel->setText( stringChar );                                                
                plabel->show();                                                               
            }                                                                                 
        }                                                                                     
        else                                                                                  
            plabel->setText( plabel->text() + stringChar );                                   

    } // for                                                                                  

    playoutEdit->addStretch( 10 );                                                            

    playout->addStretch( 10 );                                                             

    pEditor->show();                                                                    
}

