#include "LibInfo.h"
#include "PMDataTypeArgsWidget.h"

#include "PMModel.h"

PMDataTypeArgsWidget::PMDataTypeArgsWidget( PMDomain *pDomain, QWidget *pParent )
    : QWidget( pParent )
{
    this->pDomain   = pDomain;
    this->pColumn   = nullptr;

    pEditor     = nullptr;                                                              
    pLength     = nullptr;                                                              
    pPrecision1 = nullptr;                                                              
    pPrecision2 = nullptr;                                                              
    pScale      = nullptr;                                                              
                                                  
    pLayout     = new QVBoxLayout( this );

    DATADataTypeSpec *p = pDomain->getDataTypeSpec();
    if ( p ) setDataTypeSyntax( p->vectorSyntax );
}

PMDataTypeArgsWidget::PMDataTypeArgsWidget( PMColumn *pColumn, QWidget *pParent )
    : QWidget( pParent )
{
    this->pDomain   = nullptr;
    this->pColumn   = pColumn;

    pEditor     = nullptr;                                                              
    pLength     = nullptr;                                                              
    pPrecision1 = nullptr;                                                              
    pPrecision2 = nullptr;                                                              
    pScale      = nullptr;                                                              
                                                  
    pLayout     = new QVBoxLayout( this );

    DATADataTypeSpec *p = pColumn->getDataTypeSpec();
    if ( p ) setDataTypeSyntax( p->vectorSyntax );
}

PMDataTypeArgsWidget::~PMDataTypeArgsWidget()
{
}

/*!
 * \brief create controls according to syntax. 
 * 
 * \author pharvey (5/28/20)
 * 
 */
void PMDataTypeArgsWidget::setDataTypeSyntax( const QVector<QString> &vectorSyntax )
{
    // clear                                                                                  
    if ( pEditor )                                                                      
    {
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

    QString s;
    foreach( s, vectorSyntax )
    {
        if ( s == "n" )
        {          
            Q_ASSERT(!pLength);                                                                       
            pLength = new QLineEdit( pEditor );                             
            pLength->setValidator( new QIntValidator( pLength ) );        
            pLength->setToolTip( tr("length (n)") );
            playoutEdit->addWidget( pLength );                                    
            pLength->setText( pDomain ? pDomain->getLength() : pColumn->getLength() );                             
            pLength->show();                                                      
            connect( pLength, SIGNAL(textEdited(const QString &)), SLOT(slotLengthChanged(const QString &)) );  
        }                                                                                 
        else if ( s == "p" )
        {                                                                                 
            Q_ASSERT(!pPrecision1);                                                                       
            pPrecision1 = new QLineEdit( pEditor );                         
            pPrecision1->setValidator( new QIntValidator( pPrecision1 ) );
            pPrecision1->setToolTip( tr("precision (p)") );                        
            playoutEdit->addWidget( pPrecision1 );                                
            pPrecision1->setText(  pDomain ? pDomain->getPrecision1() : pColumn->getPrecision1()  );                     
            pPrecision1->show();                                                  
            connect( pPrecision1, SIGNAL(textEdited(const QString &)), SLOT(slotPrecision1Changed(const QString &)) );  
        }                                                                                 
        else if ( s == "q" )
        {                                                                                 
            Q_ASSERT(!pPrecision2);
            pPrecision2 = new QLineEdit( pEditor );                         
            pPrecision2->setValidator( new QIntValidator( pPrecision2 ) );
            pPrecision2->setToolTip( tr("precision (q)") );                        
            playoutEdit->addWidget( pPrecision2 );                                
            pPrecision2->setText(  pDomain ? pDomain->getPrecision2() : pColumn->getPrecision2()  );                     
            pPrecision2->show();                                                  
            connect( pPrecision2, SIGNAL(textEdited(const QString &)), SLOT(slotPrecision2Changed(const QString &)) );  
        }                                                                                 
        else if ( s == "s" )
        {
            Q_ASSERT(!pScale);
            pScale = new QLineEdit( pEditor );                              
            pScale->setValidator( new QIntValidator( pScale ) );          
            pScale->setToolTip( tr("scale (s)") );                                 
            playoutEdit->addWidget( pScale );                                     
            pScale->setText(  pDomain ? pDomain->getScale() : pColumn->getScale()  );                               
            pScale->show();
            connect( pScale, SIGNAL(textEdited(const QString &)), SLOT(slotScaleChanged(const QString &)) );  
        }
        else 
        {
            plabel = new QLabel( pEditor );                                             
            plabel->setFont( font );                                                          
            playoutEdit->addWidget( plabel );                                                 
            plabel->setText( s );                                                    
            plabel->show();                                                                   
        }
    }

    playoutEdit->addStretch( 10 );                                                            

    playout->addStretch( 10 );                                                             

    pEditor->show();                                                                    
}

void PMDataTypeArgsWidget::slotLengthChanged( const QString &s )
{
    if ( pDomain ) pDomain->setLength( s );
    else pColumn->setLength( s );
}

void PMDataTypeArgsWidget::slotPrecision1Changed( const QString &s )
{
    if ( pDomain ) pDomain->setPrecision1( s );
    else pColumn->setPrecision1( s );
}

void PMDataTypeArgsWidget::slotPrecision2Changed( const QString &s )
{
    if ( pDomain ) pDomain->setPrecision2( s );
    else pColumn->setPrecision2( s );
}

void PMDataTypeArgsWidget::slotScaleChanged( const QString &s )
{
    if ( pDomain ) pDomain->setScale( s );
    else pColumn->setScale( s );
}


