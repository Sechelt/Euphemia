#include "LibInfo.h"
#include "LMDataTypeWidget.h"

#include "LMModel.h"
#include "LMDomain.h"
#include "LMDataTypeArgsWidget.h"

LMDataTypeWidget::LMDataTypeWidget( QWidget *pParent, LMModel *pModel, LMDomain *pDomain, const QString &stringDataType, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale )
    : QWidget( pParent )
{
    this->pModel = pModel;
    QVBoxLayout *playoutTop = new QVBoxLayout( this );                                                      

    // row 1
    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        // DOMAIN
        {
            pLayout->addWidget( new QLabel( tr("Domain"), this ) );                                                                                                            

            pComboBoxDomain = new QComboBox( this );                                                                 
            listDomains     = pModel->getObjects( QString(), QString(), "LMDomain" );                                
            ADObject *pObject;
            foreach( pObject, listDomains )
            {                                                                                                        
                pComboBoxDomain->addItem( ((LMDomain*)pObject)->getName() );                                                   
            }                                                                                                        
            pComboBoxDomain->addItem( "" );                                                                       
                                                                                                                     
            if ( pDomain )                                                                            
                pComboBoxDomain->setCurrentIndex( listDomains.indexOf( pDomain ) );                    
            else                                                                                                     
                pComboBoxDomain->setCurrentIndex( listDomains.count() );                                              
        }                                                                                                            
        pComboBoxDomain->setToolTip( tr("custom data type") );                                                        
        pLayout->addWidget( pComboBoxDomain );                                                             
                                                                                                                     
        // DATA TYPE                                                                                                 
        // (syntax formatted)                                                                                        
        int nDataType = pModel->getDataType( stringDataType );                                             
                                                                                                                     
        pLayout->addWidget( new QLabel( tr("Data Type"), this ) );                                                                                                            
        pComboBoxDataType = new QComboBox( this );                                                                   
        pComboBoxDataType->addItem( "" );
        const char **ap = pModel->getDataTypeSyntax();
        while ( *ap )
        {
            pComboBoxDataType->addItem( *ap );
            ap++;
        }
        pComboBoxDataType->setCurrentIndex( nDataType + 1 );                                                          
        pComboBoxDataType->setToolTip( tr("select a data type here, edit any arguments below") );                     
        pLayout->addWidget( pComboBoxDataType );                                                             
        pLayout->addStretch( 10 );                                                                           

        playoutTop->addLayout( pLayout );                                                           
    }
                                                                                                                 
    //                                                                                                           
    pDataTypeArgs = new LMDataTypeArgsWidget( this, stringLength, stringPrecision1, stringPrecision2, stringScale );                                     
    playoutTop->addWidget( pDataTypeArgs );                                                                
                                                                                                                 
    //                                                                                                           
    pTextEditHelp = new QTextEdit( this );                                                                       
    pTextEditHelp->setReadOnly( true );                                                                          
    playoutTop->addWidget( pTextEditHelp );                                                                      
                                                                                                                 
    //                                                                                                           
    if ( pDomain )                                                                                
        slotDomainChanged( pComboBoxDomain->currentText() );                                                     
    else                                                                                                         
        slotDataTypeChanged( pComboBoxDataType->currentText() );                                                 

    connect( pComboBoxDomain, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDomainChanged(const QString &)) );    
    connect( pComboBoxDataType, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDataTypeChanged(const QString &)) );

    connect( pDataTypeArgs, SIGNAL(signalLengthChanged(const QString&)), SIGNAL(signalLengthChanged(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalPrecision1Changed(const QString&)), SIGNAL(signalPrecision1Changed(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalPrecision2Changed(const QString&)), SIGNAL(signalPrecision2Changed(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalScaleChanged(const QString&)), SIGNAL(signalScaleChanged(const QString&)) );
}

LMDataTypeWidget::LMDataTypeWidget( QWidget *pParent, LMModel *pModel, const QString &stringDataType, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale )
    : QWidget( pParent )
{
    this->pModel = pModel;
    QVBoxLayout *playoutTop = new QVBoxLayout( this );                                                      

    // row 1
    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        // DATA TYPE                                                                                                 
        // (syntax formatted)                                                                                        
        int nDataType = pModel->getDataType( stringDataType );                                             
                                                                                                                     
        pLayout->addWidget( new QLabel( tr("Data Type"), this ) );                                                                                                            
        pComboBoxDataType = new QComboBox( this );                                                                   
        pComboBoxDataType->addItem( "" );
        const char **ap = pModel->getDataTypeSyntax();
        while ( *ap )
        {
            pComboBoxDataType->addItem( *ap );
            ap++;
        }
        pComboBoxDataType->setCurrentIndex( nDataType + 1 );                                                          
        pComboBoxDataType->setToolTip( tr("select a data type here, edit any arguments below") );                     
        pLayout->addWidget( pComboBoxDataType );                                                             
        pLayout->addStretch( 10 );                                                                           

        playoutTop->addLayout( pLayout );                                                           
    }
                                                                                                                 
    //                                                                                                           
    pDataTypeArgs = new LMDataTypeArgsWidget( this, stringLength, stringPrecision1, stringPrecision2, stringScale );                                     
    playoutTop->addWidget( pDataTypeArgs );                                                                
                                                                                                                 
    //                                                                                                           
    pTextEditHelp = new QTextEdit( this );                                                                       
    pTextEditHelp->setReadOnly( true );                                                                          
    playoutTop->addWidget( pTextEditHelp );                                                                      
                                                                                                                 
    slotDataTypeChanged( pComboBoxDataType->currentText() );                                                 

    connect( pComboBoxDataType, SIGNAL(currentTextChanged(const QString &)), SLOT(slotDataTypeChanged(const QString &)) );

    connect( pDataTypeArgs, SIGNAL(signalLengthChanged(const QString&)), SIGNAL(signalLengthChanged(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalPrecision1Changed(const QString&)), SIGNAL(signalPrecision1Changed(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalPrecision2Changed(const QString&)), SIGNAL(signalPrecision2Changed(const QString&)) );
    connect( pDataTypeArgs, SIGNAL(signalScaleChanged(const QString&)), SIGNAL(signalScaleChanged(const QString&)) );
}

LMDataTypeWidget::~LMDataTypeWidget()
{
}

void LMDataTypeWidget::slotDomainChanged( const QString & /* stringDomain */ )
{
    LMDomain *pDomain = nullptr;
    if ( pComboBoxDomain->currentIndex() >= 0 && pComboBoxDomain->currentIndex() < pComboBoxDomain->count() - 1 )
    {
        pDomain = (LMDomain*)listDomains.at( pComboBoxDomain->currentIndex() );                                                                                                           
    }
                                                                                                                                                                                               
    emit signalDomainChanged( pDomain );    
                                                                                                                                                                                                   
    if ( pDomain )                                                                                                                                                                             
        pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp; <B>" ) + pDomain->getName() + "</B><BR>" + pDomain->getDataTypeWithArgs() + "<BR><BR>" + pDomain->getComment() );
    else                                                                                                                                                                                       
        pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp;" ) );                                                                                                            
}

void LMDataTypeWidget::slotDataTypeChanged( const QString &stringDataType )
{
    //                                                                                                                   
    pDataTypeArgs->slotDataTypeSyntax( stringDataType );                                                           

    if ( pComboBoxDataType->currentIndex() > 0 ) // 0 = blank (not selected)
    { 
        {
            const char ** p = pModel->getDataTypes();                                                                        
            emit signalDataTypeChanged( p[pComboBoxDataType->currentIndex() - 1] ); // -1 because we inserted a blank at start of combobox list
        }
        {
            const char ** p = pModel->getDataTypeDescs();                                                                        
            pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp;" ) + p[pComboBoxDataType->currentIndex() - 1] ); // -1 because we inserted a blank at start of combobox list
        }
    }
    else
    {
        emit signalDataTypeChanged( QString() );
        pTextEditHelp->setText( QString( "<img src=\":W/Information16x16\"/>&nbsp;" ) );
    }
}


