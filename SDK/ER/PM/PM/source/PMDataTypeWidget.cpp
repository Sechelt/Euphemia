#include "LibInfo.h"
#include "PMDataTypeWidget.h"

#include "PMModel.h"
#include "PMDomain.h"
#include "PMDataTypeArgsWidget.h"

PMDataTypeWidget::PMDataTypeWidget( PMDomain *pDomain, QWidget *pParent )
    : QWidget( pParent )
{
    this->pColumn   = nullptr;
    this->pDomain   = pDomain;

    PMModel *pModel = (PMModel*)pDomain->getParent( "PMModel" );

    QVBoxLayout *playoutTop = new QVBoxLayout( this );                                                      

    // row 1
    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        // DATA TYPE                                                                                                 
        {
            pLayout->addWidget( new QLabel( tr("Data Type"), this ) );                                                                                                            
            pComboBoxDataType = new QComboBox( this );                                                                   
            pComboBoxDataType->addItems( pModel->getTypeNames() );
            pComboBoxDataType->addItem( "" );

            if ( !pDomain->getDataType().isEmpty() ) pComboBoxDataType->setCurrentText( pDomain->getDataType() );
            else pComboBoxDataType->setCurrentText( "" );                                                          

            pComboBoxDataType->setToolTip( tr("select a data type here, edit any arguments below") );                     
            pLayout->addWidget( pComboBoxDataType );                                                             
        }
        pLayout->addStretch( 10 );                                                                           
        playoutTop->addLayout( pLayout );                                                           
    }
                                                                                                                 
    //                                                                                                           
    pDataTypeArgs = new PMDataTypeArgsWidget( pDomain, this );                                     
    playoutTop->addWidget( pDataTypeArgs );                                                                
                                                                                                                 
    //                                                                                                           
    pTextEditHelp = new QTextEdit( this );                                                                       
    pTextEditHelp->setReadOnly( true );                                                                          
    playoutTop->addWidget( pTextEditHelp );                                                                      
                                                                                                                 
    slotDataTypeChanged( pComboBoxDataType->currentText() );                                                 

    connect( pComboBoxDataType, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDataTypeChanged(const QString &)) );
}

PMDataTypeWidget::PMDataTypeWidget( PMColumn *pColumn, QWidget *pParent )
    : QWidget( pParent )
{   
    this->pColumn   = pColumn;
    this->pDomain   = nullptr;

    PMModel *pModel = (PMModel*)pColumn->getParent( "PMModel" );

    QVBoxLayout *playoutTop = new QVBoxLayout( this );                                                      

    // row 1
    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        // DOMAIN
        {
            pLayout->addWidget( new QLabel( tr("Domain"), this ) );                                                                                                            

            pComboBoxDomain = new QComboBox( this );                                                                 
            listDomains     = pModel->getObjects( QString(), QString(), "PMDomain" );                                
            ADObject *pObject;
            foreach( pObject, listDomains )
            {                                                                                                        
                pComboBoxDomain->addItem( ((PMDomain*)pObject)->getName() );                                                   
            }                                                                                                        
            pComboBoxDomain->addItem( "" );                                                                       
                                                                                                                     
            if ( pColumn->getDomain() ) pComboBoxDomain->setCurrentIndex( listDomains.indexOf( pColumn->getDomain() ) );                    
            else pComboBoxDomain->setCurrentText( "" );                                              

            pComboBoxDomain->setToolTip( tr("custom data type") );                                                        
            pLayout->addWidget( pComboBoxDomain );                                                             
        }                                                                                                            
                                                                                                                             
        // DATA TYPE                                                                                                 
        {
            pLayout->addWidget( new QLabel( tr("Data Type"), this ) );                                                                                                            
            pComboBoxDataType = new QComboBox( this );                                                                   
            pComboBoxDataType->addItems( pModel->getTypeNames() );
            pComboBoxDataType->addItem( "" );

            if ( !pColumn->getDataType().isEmpty() ) pComboBoxDataType->setCurrentText( pColumn->getDataType() );
            else pComboBoxDataType->setCurrentText( "" );                                                          

            pComboBoxDataType->setToolTip( tr("select a data type here, edit any arguments below") );                     
            pLayout->addWidget( pComboBoxDataType );                                                             
        }
        pLayout->addStretch( 10 );                                                                           
        playoutTop->addLayout( pLayout );                                                           
    }
                                                                                                                     
    //                                                                                                           
    pDataTypeArgs = new PMDataTypeArgsWidget( pColumn, this );                                     
    playoutTop->addWidget( pDataTypeArgs );                                                                
                                                                                                                     
    //                                                                                                           
    pTextEditHelp = new QTextEdit( this );                                                                       
    pTextEditHelp->setReadOnly( true );                                                                          
    playoutTop->addWidget( pTextEditHelp );                                                                      
                                                                                                                 
    //                                                                                                           
    if ( pDomain ) slotDomainChanged( pComboBoxDomain->currentText() );                                                     
    else slotDataTypeChanged( pComboBoxDataType->currentText() );                                                 

    connect( pComboBoxDomain, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDomainChanged(const QString &)) );    
    connect( pComboBoxDataType, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDataTypeChanged(const QString &)) );
}

PMDataTypeWidget::~PMDataTypeWidget()
{
}

void PMDataTypeWidget::slotDomainChanged( const QString & /* stringDomain */ )
{
    PMDomain *pDomain = nullptr;
    if ( pComboBoxDomain->currentIndex() >= 0 && pComboBoxDomain->currentIndex() < pComboBoxDomain->count() - 1 )
    {
        pDomain = (PMDomain*)listDomains.at( pComboBoxDomain->currentIndex() );                                                                                                           
    }
                                                                                                                                                                                               
    pColumn->setDomain( pDomain );
                                                                                                                                                                                                   
    if ( pDomain )                                                                                                                                                                             
        pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp; <B>" ) + pDomain->getName() + "</B><BR>" + pDomain->getDataTypeWithArgs() + "<BR><BR>" + pDomain->getComment() );
    else                                                                                                                                                                                       
        pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp;" ) );                                                                                                            
}

void PMDataTypeWidget::slotDataTypeChanged( const QString &stringDataType )
{
    DATADataTypeSpec *pDataTypeSpec;

    if ( pDomain )
    {
        pDomain->setDataType( stringDataType );
        pDataTypeSpec = pDomain->getDataTypeSpec();
    }
    else
    {
        pColumn->setDataType( stringDataType );
        pDataTypeSpec = pColumn->getDataTypeSpec();
    }
    if ( pDataTypeSpec ) pDataTypeArgs->setDataTypeSyntax( pDataTypeSpec->vectorSyntax );

    if ( pDataTypeSpec && pDataTypeSpec->stringDescription.isEmpty() ) pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp;" ) );
    else if ( pDataTypeSpec ) pTextEditHelp->setText( pDataTypeSpec->stringDescription );
}


