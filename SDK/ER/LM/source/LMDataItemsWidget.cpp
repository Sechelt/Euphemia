#include "LibInfo.h"
#include "LMDataItemsWidget.h"

#include "LMDataItem.h"
#include "LMDomain.h"

LMDataItemsWidget::LMDataItemsWidget( LMModel *ptableParent, QWidget *pwidgetParent )
    : AWDataTable( ptableParent, "LMDataItem", pwidgetParent )
{
    // TABLE                                      
    ptable->setColumnCount( 9 );                      
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Domain") ) );
    ptable->setHorizontalHeaderItem( 4, new QTableWidgetItem( tr("Type") ) );
    ptable->setHorizontalHeaderItem( 5, new QTableWidgetItem( tr("Length") ) );
    ptable->setHorizontalHeaderItem( 6, new QTableWidgetItem( tr("Precision1") ) );
    ptable->setHorizontalHeaderItem( 7, new QTableWidgetItem( tr("Precision2") ) );
    ptable->setHorizontalHeaderItem( 8, new QTableWidgetItem( tr("Scale") ) );

    // TOOLBAR                                    
    setNew();
    setEdit();
    setDelete();

    // Load                                       
    doTableSet();                                 
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

LMDataItemsWidget::~LMDataItemsWidget()
{
}

/*!

*/    
void LMDataItemsWidget::slotNew()
{
    LMModel *       pModel      = (LMModel*)pmodelParent;          
    LMDataItem *    pDataItem   = (LMDataItem*)pModel->getObject( "LMDataItem" );
                                                          
    pDataItem->slotObjectDialog( this );                    
                                                          
    doRowAppend( pDataItem );                             
}

/*!
    slotDelete
    
    With replace this because we have a special relationship with LMAttribute(s) where LMAttribute
    must have a LMDataItem.
*/
void LMDataItemsWidget::slotDelete()
{
    int nRow = getRowNumStrict();                                                                                                                                                                               
                                                                                                                                                                                                                
    if ( nRow < 0 )                                                                                                                                                                                             
        return;                                                                                                                                                                                                 
                                                                                                                                                                                                                
    // make edit widget go away                                                                                                                                                                                 
    ptable->setCurrentCell( -1, -1 );                                                                                                                                                                           
                                                                                                                                                                                                                
    LMDataItem *pDataItem = (LMDataItem*)getObject( nRow );                                                                                                                                                      
    if ( !pDataItem ) return;                                                                                                                                                                                                 
                                                                                                                                                                                                                
    // get the number of LMAttribute(s) which Use this LMDataItem                                                                                                                                               
    int nAttributes = 0;                                                                                                                                                                                        
    {
        QList<ADObjectReferenceKey> l = pDataItem->getUsedByKeys();
        ADObjectReferenceKey        Key;
        foreach( Key, l )
        {                                                                                                                                                                                                       
            if ( Key.pObject->inherits( "LMAttribute" ) )                                                                                                                                                            
                nAttributes++;                                                                                                                                                                                  
        }                                                                                                                                                                                                       
    }                                                                                                                                                                                                           
                                                                                                                                                                                                                
    if ( nAttributes )                                                                                                                                                                                          
    {                                                                                                                                                                                                           
        QString stringPrompt = QString( tr("%d Attribute(s) use this Data Item (%s).\nAny attributes using this Data Item will also be removed if you proceed.") ).arg( nAttributes ).arg( pDataItem->getName() );
        // stringPrompt = stringPrompt.sprintf( "%d Attribute(s) use this Data Item (%s).\nAny attributes using this Data Item will also be removed if you proceed.", nAttributes, pDataItem->getName().latin1() );
        int nReply = QMessageBox::warning( this, "Warning", stringPrompt, QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton );                                                                        
        if ( nReply != QMessageBox::Ok )                                                                                                                                                                        
            return;                                                                                                                                                                                             
    }                                                                                                                                                                                                           
                                                                                                                                                                                                                
    // delete attributes which Use us because they can not exist without a LMDataItem                                                                                                                           
    {
        QList<ADObjectReferenceKey> l = pDataItem->getUsedByKeys(); 
        ADObjectReferenceKey        Key;
        foreach( Key, l )
        {                                                                                                                                                                                                       
            if ( Key.pObject->inherits( "LMAttribute" ) )                                                                                                                                                            
                delete Key.pObject;                                                                                                                                                                                  
        }                                                                                                                                                                                                       
    }                                                                                                                                                                                                           
                                                                                                                                                                                                                
    // delete self                                                                                                                                                                                              
    delete pDataItem;                                                                                                                                                                                           
                                                                                                                                                                                                                
    doTableSet();                                                                                                                                                                                               
}

/*!

*/    
void LMDataItemsWidget::doRowSet( int nRow, AWObject *pmodel )
{
    // let AWDataTable set col 0                                      
    AWDataTable::doRowSet( nRow, pmodel );                            
                                                                      
    // Now we set the remaining cols                                  
    LMDataItem *pDataItem = (LMDataItem*)pmodel;                      
                                                                      
    ptable->item( nRow, 1 )->setText( pDataItem->getName() );                 
    ptable->item( nRow, 2 )->setText( pDataItem->getCode() );                 
    if ( pDataItem->getDomain() )                                     
        ptable->item( nRow, 3 )->setText( pDataItem->getDomain()->getName() );
    else                                                              
        ptable->item( nRow, 3 )->setText( "" );                               
    ptable->item( nRow, 4 )->setText( pDataItem->getDataType() );             
    ptable->item( nRow, 5 )->setText( pDataItem->getLength() );               
    ptable->item( nRow, 6 )->setText( pDataItem->getPrecision1() );           
    ptable->item( nRow, 7 )->setText( pDataItem->getPrecision2() );           
    ptable->item( nRow, 8 )->setText( pDataItem->getScale() );                
}

/*!

*/    
void LMDataItemsWidget::doRowGet( int nRow, AWObject *pmodel )
{
    LMDataItem *pDataItem = (LMDataItem*)pmodel;                     
                                                                     
    pDataItem->setCode( ptable->item( nRow, 2 )->text().simplified() );      
//    pDataItem->setDomain( ptable->item( nRow, 3 )->text().simplified() );  
    pDataItem->setLength( ptable->item( nRow, 5 )->text().simplified() );    
    pDataItem->setPrecision1( ptable->item( nRow, 6 )->text().simplified() );
    pDataItem->setPrecision2( ptable->item( nRow, 7 )->text().simplified() );
    pDataItem->setScale( ptable->item( nRow, 8 )->text().simplified() );     
                                                                     
    // do last                                                       
    pDataItem->setDataType( ptable->item( nRow, 4 )->text().simplified() );  
    pDataItem->setName( ptable->item( nRow, 1 )->text().simplified() );      
}


