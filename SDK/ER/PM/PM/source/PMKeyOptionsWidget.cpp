#include "LibInfo.h"
#include "PMKeyOptionsWidget.h"

PMKeyOptionsWidget::PMKeyOptionsWidget( PMKey *pKey, QWidget *pParent )
    : QWidget( pParent )
{
    this->pKey = pKey;                                                   
                                                                                       
    int             nRow    = 0;                                                                                                                       
    QVBoxLayout *   playout = new QVBoxLayout( this );                                                                                     
    QStringList     l;
    l << tr("Property") << tr("Value");

    pTableWidget = new QTableWidget( 0, 2, this );                                                                                                 
    pTableWidget->setHorizontalHeaderLabels( l );
    pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTableWidget->setSelectionMode( QAbstractItemView::NoSelection );    
    playout->addWidget( pTableWidget );                                                                                                       
                                                                                       
    // PRIMARY
    pTableWidget->setRowCount( nRow + 1 );
    pTableWidget->setItem( nRow, 0, new QTableWidgetItem( tr("Primary") ) );
    pCheckBoxPrimary  = new QCheckBox( this );                                                                                       
    pCheckBoxPrimary->setChecked( pKey->isPrimary() );                                                                      
    pCheckBoxPrimary->setToolTip( tr("indicate if this is the primary key (can only be one)") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pCheckBoxPrimary );                                                                              
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxPrimary, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotPrimary(Qt::CheckState)) );
#else
    connect( pCheckBoxPrimary, SIGNAL(stateChanged(int)), SLOT(slotPrimary(int)) );
#endif
}

PMKeyOptionsWidget::~PMKeyOptionsWidget()
{
}


