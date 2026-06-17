#include "LibInfo.h"
#include "LMAttributeChecksWidget.h"

LMAttributeChecksWidget::LMAttributeChecksWidget( LMAttribute *pAttribute, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pAttribute = pAttribute;                                                                                                      
                                                                                                                                        
    int             nRow    = 0;                                                                                                                       
    QVBoxLayout *   playout = new QVBoxLayout( this );                                                                                     
    QStringList     l;
    l << tr("Property") << tr("Value");

    pTableWidget = new QTableWidget( 0, 2, this );                                                                                                 
    pTableWidget->setHorizontalHeaderLabels( l );
    pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTableWidget->setSelectionMode( QAbstractItemView::NoSelection );    
    playout->addWidget( pTableWidget );                                                                                                       
                                                                                                                                        
    // DEFAULT                                                                                                                          
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Default") ) );
    plineeditDefault = new QLineEdit( pTableWidget );                                                                                         
    plineeditDefault->setText( pAttribute->getDefault() );                                                                                 
    plineeditDefault->setToolTip( tr("specifies a default column value that is entered when no other entry is made") );                  
    pTableWidget->setCellWidget( nRow, 1, plineeditDefault );                                                                                 
    connect( plineeditDefault, SIGNAL(textChanged(const QString&)), SLOT(slotDefault(const QString&)) );

    // IDENTIFIER                                                                                                                       
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Identifier") ) );
    pcheckboxIdentifier  = new QCheckBox( this );                                                                                       
    pcheckboxIdentifier->setChecked( pAttribute->getIdentifier() );                                                                      
    pcheckboxIdentifier->setToolTip( tr("identifies a specific instance") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pcheckboxIdentifier );                                                                              
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxIdentifier, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotIdentifier(Qt::CheckState)) );
#else
    connect( pcheckboxIdentifier, SIGNAL(stateChanged(int)), SLOT(slotIdentifier(int)) );
#endif                                                                                                                                        
    // MANDATORY                                                                                                                        
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Mandatory") ) );
    pcheckboxMandatory  = new QCheckBox( this );                                                                                        
    pcheckboxMandatory->setChecked( pAttribute->getMandatory() );                                                                       
    pcheckboxMandatory->setToolTip( tr("column must have a value") );                                                                    
    pTableWidget->setCellWidget( nRow, 1, pcheckboxMandatory );                                                                               
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxMandatory, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotMandatory(Qt::CheckState)) );
#else
    connect( pcheckboxMandatory, SIGNAL(stateChanged(int)), SLOT(slotMandatory(int)) );
#endif                                                                                                                                        
                                                                                                                                        
    // HIDDEN                                                                                                                           
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Hidden") ) );
    pcheckboxHidden  = new QCheckBox( this );                                                                                           
    pcheckboxHidden->setChecked( pAttribute->getHidden() );                                                                             
    pcheckboxHidden->setToolTip( tr("column is seen by User") );                                                                         
    pTableWidget->setCellWidget( nRow, 1, pcheckboxHidden );                                                                                  
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxHidden, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotHidden(Qt::CheckState)) );
#else
    connect( pcheckboxHidden, SIGNAL(stateChanged(int)), SLOT(slotHidden(int)) );
#endif                                                                                                                                        
                                                                                                                                        
    // MINIMUM                                                                                                                          
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Minimum") ) );
    plineeditMinimum = new QLineEdit( pTableWidget );                                                                                         
    plineeditMinimum->setText( pAttribute->getMinimum() );                                                                                 
    pTableWidget->setCellWidget( nRow, 1, plineeditMinimum );                                                                                 
    connect( plineeditMinimum, SIGNAL(textChanged(const QString&)), SLOT(slotMinimum(const QString&)) );

    // MAXIMIMUM                                                                                                                        
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Maximum") ) );
    plineeditMaximum = new QLineEdit( pTableWidget );                                                                                         
    plineeditMaximum->setText( pAttribute->getMaximum() );                                                                                 
    pTableWidget->setCellWidget( nRow, 1, plineeditMaximum );                                                                                 
    connect( plineeditMaximum, SIGNAL(textChanged(const QString&)), SLOT(slotMaximum(const QString&)) );

    // FORMAT                                                                                                                           
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Format") ) );
    plineeditFormat = new QLineEdit( pTableWidget );                                                                                          
    plineeditFormat->setText( pAttribute->getFormat() );                                                                                   
    pTableWidget->setCellWidget( nRow, 1, plineeditFormat );                                                                                  
    connect( plineeditFormat, SIGNAL(textChanged(const QString&)), SLOT(slotFormat(const QString&)) );

    // UNIT                                                                                                                             
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Unit") ) );
    plineeditUnit = new QLineEdit( pTableWidget );                                                                                            
    plineeditUnit->setText( pAttribute->getUnit() );                                                                                       
    pTableWidget->setCellWidget( nRow, 1, plineeditUnit );                                                                                    
    connect( plineeditUnit, SIGNAL(textChanged(const QString&)), SLOT(slotUnit(const QString&)) );

    // UPPER CASE                                                                                                                       
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Upper Case") ) );
    pcheckboxForceUpperCase = new QCheckBox( this );                                                                                    
    pcheckboxForceUpperCase->setChecked( pAttribute->getForceUpperCase() );                                                                
    pcheckboxForceUpperCase->setToolTip( tr("force values to upper case") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pcheckboxForceUpperCase );                                                                          
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxForceUpperCase, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotForceUpper(Qt::CheckState)) );
#else
    connect( pcheckboxForceUpperCase, SIGNAL(stateChanged(int)), SLOT(slotForceUpper(int)) );
#endif
    // LOWER CASE                                                                                                                       
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Lower Case") ) );
    pcheckboxForceLowerCase = new QCheckBox( this );                                                                                    
    pcheckboxForceLowerCase->setChecked( pAttribute->getForceLowerCase() );                                                                
    pcheckboxForceLowerCase->setToolTip( tr("force values to lower case") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pcheckboxForceLowerCase );                                                                          
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxForceLowerCase, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotForceLower(Qt::CheckState)) );
#else
    connect( pcheckboxForceLowerCase, SIGNAL(stateChanged(int)), SLOT(slotForceLower(int)) );
#endif
    // RESTRICT MODIFY                                                                                                                  
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Restrict Modify") ) );
    pcheckboxRestrictModify = new QCheckBox( this );                                                                                    
    pcheckboxRestrictModify->setChecked( pAttribute->getRestrictModify() );                                                                
    pcheckboxRestrictModify->setToolTip( tr("restrict ability to modify value") );                                                       
    pTableWidget->setCellWidget( nRow, 1, pcheckboxRestrictModify );                                                                          
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pcheckboxRestrictModify, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRestrictModify(Qt::CheckState)) );
#else
    connect( pcheckboxRestrictModify, SIGNAL(stateChanged(int)), SLOT(slotRestrictModify(int)) );
#endif                                                                                                                                        
    // restore geometry                                                                                                                 
/*
    QSettings settings;                                                                                                                       
    int nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    pTableWidget->setColumnWidth( 0, nW );                                                                                                    
    nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );    
    pTableWidget->setColumnWidth( 1, nW );                                                                                                    
*/
}

LMAttributeChecksWidget::~LMAttributeChecksWidget()
{
/*
    QSettings settings;                                                                                                      
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );
*/
}


