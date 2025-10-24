#include "LibInfo.h"
#include "LMAttributesWidget.h"

#include <WTabDialog.h>
#include <AWObjectSelectWidget.h>

#include "LMModel.h"
#include "LMAttribute.h"
#include "LMDataItem.h"
#include "LMDomain.h"

LMAttributesWidget::LMAttributesWidget( LMEntityBase *pEntity, QWidget *pwidgetParent )
    : AWDataTable( pEntity, "LMAttribute" /* match inherits class */, pwidgetParent )
{
    pModel = (LMModel*)pEntity->getParent( "LMModel" );
    this->pEntity = pEntity;

    // Table
    ptable->setColumnCount( 12 );                                                                                                                                       
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Domain") ) );
    ptable->setHorizontalHeaderItem( 4, new QTableWidgetItem( tr("Type") ) );
    ptable->setHorizontalHeaderItem( 5, new QTableWidgetItem( tr("Length") ) );
    ptable->setHorizontalHeaderItem( 6, new QTableWidgetItem( tr("Precision1") ) );
    ptable->setHorizontalHeaderItem( 7, new QTableWidgetItem( tr("Precision2") ) );
    ptable->setHorizontalHeaderItem( 8, new QTableWidgetItem( tr("Scale") ) );
    ptable->setHorizontalHeaderItem( 9, new QTableWidgetItem( tr("Identifier") ) );
    ptable->setHorizontalHeaderItem( 10, new QTableWidgetItem( tr("Mandatory") ) );
    ptable->setHorizontalHeaderItem( 11, new QTableWidgetItem( tr("Hidden") ) );
                                                                                                                                                                    
    // ToolBar: adjust default
    QAction *pActionNewDataItem = new QAction( QIcon( ":W/RowNewPlus16x16" ), tr("New Data Item"), this );
    connect( pActionNewDataItem, SIGNAL(triggered()), this, SLOT(slotNewDataItem()) );
    ptoolbar->addAction( pActionNewDataItem );
    setNew();
    setEdit();
    setDelete();

    // Load                                                                                                                                                         
    doTableSet();                                                                                                                                                   
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

LMAttributesWidget::~LMAttributesWidget()
{
}

/*!
    slotNew
    
    Create a new LMAttribute(s) based upon selecting an existing LMDataItem(s).
*/    
void LMAttributesWidget::slotNew()
{
    // get list of data items
    QList<ADObject*> listModelsIn = pModel->getObjects( QString(), QString(), "LMDataItem" );

    // create dialog                                                                             
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Data Item..."), "SelectDataItem" );  
    pdialog->setWindowIcon( QIcon( ":W/RowNew16x16" ) );                                                          
                                                                                                 
    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn );     
    pdialog->getTabWidget()->addTab( pmultiselect, "Data Items" );                               
                                                                                                 
    // popup window                                                                              
    if ( pdialog->exec() != QDialog::Rejected )                                                  
    {
        // create new attribute for each selected data item                                      
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();                            
        ADObject *       p;

        foreach( p, listModelsOut )
        {
            LMDataItem *    pDataItem   = (LMDataItem*)p;
            LMAttribute *   pAttribute  = (LMAttribute*)pEntity->getObject( "LMAttribute" );
            pAttribute->doConnect( pDataItem );
            doRowAppend( pAttribute );
        }
    }                                                                                            

    // clean up                                                                                  
    delete pdialog;                                                                              
}

/*!
    slotNew
    
    Create a new LMAttribute based upon a new LMDataItem.
*/    
void LMAttributesWidget::slotNewDataItem()
{
    LMDataItem *    pDataItem   = (LMDataItem*)pModel->getObject( "LMDataItem" );   
    LMAttribute *   pAttribute  = (LMAttribute*)pEntity->getObject( "LMAttribute" );
    pAttribute->doConnect( pDataItem );
                                                                           
    pAttribute->slotObjectDialog( this );                                    
                                                                           
    doRowAppend( pAttribute );                                             
}

/*!

*/    
void LMAttributesWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will 
    //       cause all to be blanked out. So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0                                          
    AWDataTable::doRowSet( nRow, pObject );                                
                                                                          
    // Now we set the remaining cols                                      
    LMAttribute *pAttribute = (LMAttribute*)pObject;                       
                                                                          
    ptable->item( nRow, 1 )->setText( pAttribute->getName() );                    
    ptable->item( nRow, 2 )->setText( pAttribute->getCode() );                    
    if ( pAttribute->getDomain() )                                        
        ptable->item( nRow, 3 )->setText( pAttribute->getDomain()->getName() );   
    else                                                                  
        ptable->item( nRow, 3 )->setText( "" );                                   
    ptable->item( nRow, 4 )->setText( pAttribute->getDataType() );                
    ptable->item( nRow, 5 )->setText( pAttribute->getLength() );                  
    ptable->item( nRow, 6 )->setText( pAttribute->getPrecision1() );              
    ptable->item( nRow, 7 )->setText( pAttribute->getPrecision2() );              
    ptable->item( nRow, 8 )->setText( pAttribute->getScale() );                   
    ptable->item( nRow, 9 )->setText( (pAttribute->getIdentifier() ? "*" : "" ) );
    ptable->item( nRow, 10 )->setText( (pAttribute->getMandatory() ? "*" : "" ) );
    ptable->item( nRow, 11 )->setText( (pAttribute->getHidden() ? "*" : "" ) );   

    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

/*!

*/    
void LMAttributesWidget::doRowGet( int nRow, AWObject *pObject )
{
    LMAttribute *pAttribute = (LMAttribute*)pObject;                        
                                                                           
    pAttribute->setName( ptable->item( nRow, 1 )->text().simplified() );      
    pAttribute->setCode( ptable->item( nRow, 2 )->text().simplified() );      
    // skip domain (3)
    pAttribute->setDataType( ptable->item( nRow, 4 )->text().simplified() );  
    pAttribute->setLength( ptable->item( nRow, 5 )->text().simplified() );    
    pAttribute->setPrecision1( ptable->item( nRow, 6 )->text().simplified() );
    pAttribute->setPrecision2( ptable->item( nRow, 7 )->text().simplified() );
    pAttribute->setScale( ptable->item( nRow, 8 )->text().simplified() );     
    pAttribute->setIdentifier( ptable->item( nRow, 9 )->text().isEmpty() ? 0 : 1 );    
    pAttribute->setMandatory( ptable->item( nRow, 10 )->text().isEmpty() ? 0 : 1 );    
    pAttribute->setHidden( ptable->item( nRow, 11 )->text().isEmpty() ? 0 : 1 );       
}

