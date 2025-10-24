#include "LibInfo.h"
#include "LMDomainChecksWidget.h"

LMDomainChecksWidget::LMDomainChecksWidget( LMDomain *pDomain, QWidget *pParent )
    : QWidget( pParent )
{
    this->pDomain = pDomain;                                                                                                            
                                                                                                                                        
    int             nRow    = 0;                                                                                                                       
    QVBoxLayout *   playout = new QVBoxLayout( this );                                                                                     
    QStringList     l;
    l << tr("Property") << tr("Value");

    pTableWidget = new QTableWidget( 8, 2, this );                                                                                                 
    pTableWidget->setHorizontalHeaderLabels( l );
    pTableWidget->setColumnCount( 2 );
    pTableWidget->setRowCount( 0 );
    pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTableWidget->setSelectionMode( QAbstractItemView::NoSelection );    
    playout->addWidget( pTableWidget );                                                                                                       
                                                                                                                                        
    // DEFAULT                                                                                                                          
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Default") ) );
    plineeditDefault = new QLineEdit( pTableWidget );                                                                                         
    plineeditDefault->setText( pDomain->getDefault() );                                                                                 
    plineeditDefault->setToolTip( tr("specifies a default column value that is entered when no other entry is made") );                  
    pTableWidget->setCellWidget( nRow, 1, plineeditDefault );                                                                                 
    connect( plineeditDefault, SIGNAL(textChanged(const QString&)), SLOT(slotDefault(const QString&)) );
                                                                                                                                        
    // MINIMUM                                                                                                                          
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Minimum") ) );
    plineeditMinimum = new QLineEdit( pTableWidget );                                                                                         
    plineeditMinimum->setText( pDomain->getMinimum() );                                                                                 
    pTableWidget->setCellWidget( nRow, 1, plineeditMinimum );                                                                                 
    connect( plineeditMinimum, SIGNAL(textChanged(const QString&)), SLOT(slotMinimum(const QString&)) );
                                                                                                                                        
    // MAXIMIMUM                                                                                                                        
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Maximum") ) );
    plineeditMaximum = new QLineEdit( pTableWidget );                                                                                         
    plineeditMaximum->setText( pDomain->getMaximum() );                                                                                 
    pTableWidget->setCellWidget( nRow, 1, plineeditMaximum );                                                                                 
    connect( plineeditMaximum, SIGNAL(textChanged(const QString&)), SLOT(slotMaximum(const QString&)) );
                                                                                                                                        
    // FORMAT                                                                                                                           
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Format") ) );
    plineeditFormat = new QLineEdit( pTableWidget );                                                                                          
    plineeditFormat->setText( pDomain->getFormat() );                                                                                   
    pTableWidget->setCellWidget( nRow, 1, plineeditFormat );                                                                                  
    connect( plineeditFormat, SIGNAL(textChanged(const QString&)), SLOT(slotFormat(const QString&)) );
                                                                                                                                        
    // UNIT                                                                                                                             
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Unit") ) );
    plineeditUnit = new QLineEdit( pTableWidget );                                                                                            
    plineeditUnit->setText( pDomain->getUnit() );                                                                                       
    pTableWidget->setCellWidget( nRow, 1, plineeditUnit );                                                                                    
    connect( plineeditUnit, SIGNAL(textChanged(const QString&)), SLOT(slotUnit(const QString&)) );
                                                                                                                                        
    // UPPER CASE                                                                                                                       
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Upper Case") ) );
    pcheckboxForceUpperCase = new QCheckBox( this );                                                                                    
    pcheckboxForceUpperCase->setChecked( pDomain->getForceUpperCase() );                                                                
    pcheckboxForceUpperCase->setToolTip( tr("force values to upper case") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pcheckboxForceUpperCase );                                                                          
    connect( pcheckboxForceUpperCase, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotForceUpper(Qt::CheckState)) );
                                                                                                                                        
    // LOWER CASE                                                                                                                       
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Lower Case") ) );
    pcheckboxForceLowerCase = new QCheckBox( this );                                                                                    
    pcheckboxForceLowerCase->setChecked( pDomain->getForceLowerCase() );                                                                
    pcheckboxForceLowerCase->setToolTip( tr("force values to lower case") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pcheckboxForceLowerCase );                                                                          
    connect( pcheckboxForceLowerCase, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotForceLower(Qt::CheckState)) );
                                                                                                                                        
    // RESTRICT MODIFY                                                                                                                  
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Restrict Modify") ) );
    pcheckboxRestrictModify = new QCheckBox( this );                                                                                    
    pcheckboxRestrictModify->setChecked( pDomain->getRestrictModify() );                                                                
    pcheckboxRestrictModify->setToolTip( tr("restrict ability to modify value") );                                                       
    pTableWidget->setCellWidget( nRow, 1, pcheckboxRestrictModify );                                                                          
    connect( pcheckboxRestrictModify, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRestrictModify(Qt::CheckState)) );

    // restore geometry                                                                                                                 
/*
    QSettings settings;                                                                                                                       
    int nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    pTableWidget->setColumnWidth( 0, nW );                                                                                                    
    nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );    
    pTableWidget->setColumnWidth( 1, nW );                                                                                                    
*/
}

LMDomainChecksWidget::~LMDomainChecksWidget()
{
/*
    QSettings settings;                                                                                                            
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );
*/
}                                                                                                                            


