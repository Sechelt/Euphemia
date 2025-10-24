#include "LibInfo.h"
#include "LMIdentifierOptionsWidget.h"

LMIdentifierOptionsWidget::LMIdentifierOptionsWidget( LMIdentifier *pIdentifier, QWidget *pParent )
    : QWidget( pParent )
{
    this->pIdentifier = pIdentifier;                                                   
                                                                                       
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
    pCheckBoxPrimary->setChecked( pIdentifier->isPrimary() );                                                                      
    pCheckBoxPrimary->setToolTip( tr("indicate if this is the primary identifier (can only be one)") );                                                             
    pTableWidget->setCellWidget( nRow, 1, pCheckBoxPrimary );                                                                              
    connect( pCheckBoxPrimary, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotPrimary(Qt::CheckState)) );
}

LMIdentifierOptionsWidget::~LMIdentifierOptionsWidget()
{
}


