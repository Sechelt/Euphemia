#include "LibInfo.h"
#include "LMEntityOptionsWidget.h"

LMEntityOptionsWidget::LMEntityOptionsWidget( LMEntityBase *pEntity, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pEntity = pEntity;                                                                                                            
                                                                                                                                        
    int             nRow    = 0;                                                                                                                       
    QVBoxLayout *   playout = new QVBoxLayout( this );                                                                                     
    QStringList     l;
    l << tr("Property") << tr("Value");

    pTableWidget = new QTableWidget( 0, 2, this );
    pTableWidget->setHorizontalHeaderLabels( l );
    pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTableWidget->setSelectionMode( QAbstractItemView::NoSelection );    
    playout->addWidget( pTableWidget );                                                                                                       
                                                                                                                                        
    // GENERATE                                                                                                                         
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Generate") ) );
    pGenerate = new QCheckBox( this );                                                                                          
    pGenerate->setChecked( pEntity->getGenerate() );                                                                            
    pGenerate->setToolTip( tr("generate a table in the Physical Model") );                                                       
    pTableWidget->setCellWidget( nRow, 1, pGenerate );                                                                                
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pGenerate, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotGenerate(Qt::CheckState)) );
#else
    connect( pGenerate, SIGNAL(stateChanged(int)), SLOT(slotGenerate(int)) );
#endif                                                                                                                                        
    // RECORDS                                                                                                                          
    nRow++;                                                                                                                             
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Records") ) );
    pRecords     = new QLineEdit( this );                                                                                       
    pRecords->setText( pEntity->getRecords() );                                                                                 
    pRecords->setToolTip( tr("estimate of the number of records expected") );                                                    
    pTableWidget->setCellWidget( nRow, 1, pRecords );                                                                                 
    connect( pRecords, SIGNAL(textChanged(const QString&)), SLOT(slotRecords(const QString&)) );

    // restore geometry                                                                                                                 
/*
    QSettings settings;                                                                                                                       
    int nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    pTableWidget->setColumnWidth( 0, nW );                                                                                                    
    nW = settings.readNumEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );    
    pTableWidget->setColumnWidth( 1, nW );                                                                                                    
*/
}

LMEntityOptionsWidget::~LMEntityOptionsWidget()
{
/*
    QSettings settings;                                                                                                      
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col0/Width", pTableWidget->columnWidth( 0 ) );
    settings.writeEntry( "/CodeByDesign/" + QString( metaObject()->className() ) + "/Col1/Width", pTableWidget->columnWidth( 1 ) );
*/
}


