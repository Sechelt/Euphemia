#include "LibInfo.h"
#include "LMDomainsWidget.h"

#include "LMDomain.h"

LMDomainsWidget::LMDomainsWidget( LMModel *pmodelParent, QWidget *pwidgetParent )
    : AWDataTable( pmodelParent, "LMDomain", pwidgetParent, false )
{
    // TABLE                                      
    ptable->setColumnCount( 8 );                      
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Type") ) );
    ptable->setHorizontalHeaderItem( 4, new QTableWidgetItem( tr("Length") ) );
    ptable->setHorizontalHeaderItem( 5, new QTableWidgetItem( tr("Precision1") ) );
    ptable->setHorizontalHeaderItem( 6, new QTableWidgetItem( tr("Precision2") ) );
    ptable->setHorizontalHeaderItem( 7, new QTableWidgetItem( tr("Scale") ) );
                                                  
    // TOOLBAR                                    
    setNew();
    setEdit();
    setDelete();

    // Load                                       
    doTableSet();                                 
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

LMDomainsWidget::~LMDomainsWidget()
{
}

void LMDomainsWidget::slotNew()
{
    LMModel *   pModel  = (LMModel*)pmodelParent;          
    LMDomain *  pDomain = (LMDomain*)pModel->getObject( "LMDomain" );
                                                          
    pDomain->slotObjectDialog( this );                      
                                                          
    doRowAppend( pDomain );                               
}

void LMDomainsWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0                         
    AWDataTable::doRowSet( nRow, pObject );               
                                                         
    // Now we set the remaining cols                     
    LMDomain *pDomain = (LMDomain*)pObject;               
                                                         
    ptable->item( nRow, 1 )->setText( pDomain->getName() );      
    ptable->item( nRow, 2 )->setText( pDomain->getCode() );      
    ptable->item( nRow, 3 )->setText( pDomain->getDataType() );  
    ptable->item( nRow, 4 )->setText( pDomain->getLength() );    
    ptable->item( nRow, 5 )->setText( pDomain->getPrecision1() );
    ptable->item( nRow, 6 )->setText( pDomain->getPrecision2() );
    ptable->item( nRow, 7 )->setText( pDomain->getScale() );     
}

void LMDomainsWidget::doRowGet( int nRow, AWObject *pmodel )
{
    LMDomain *pdomain = (LMDomain*)pmodel;                         
                                                                   
    pdomain->setCode( ptable->item( nRow, 2 )->text().simplified() );      
    pdomain->setLength( ptable->item( nRow, 4 )->text().simplified() );    
    pdomain->setPrecision1( ptable->item( nRow, 5 )->text().simplified() );
    pdomain->setPrecision2( ptable->item( nRow, 6 )->text().simplified() );
    pdomain->setScale( ptable->item( nRow, 7 )->text().simplified() );     
                                                                   
    // do last                                                     
    pdomain->setDataType( ptable->item( nRow, 3 )->text().simplified() );  
    pdomain->setName( ptable->item( nRow, 1 )->text().simplified() );      
}


