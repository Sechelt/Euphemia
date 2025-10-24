#include "LibInfo.h"
#include "PMKeysWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "PMTableNormal.h"
#include "PMColumn.h"
#include "PMKey.h"

PMKeysWidget::PMKeysWidget( PMTableNormal *pTable, QWidget *pParent )
    : AWDataTable( pTable, "PMKey", pParent )
{
    this->pTable = pTable;

    ptable->setColumnCount( 4 );                      
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Primary") ) );
                                                      
    // TOOLBAR                                    
    setNew();
    setEdit();
    setDelete();

    // Load                                       
    doTableSet();                                 
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

PMKeysWidget::~PMKeysWidget()
{
}

/*!
    slotNew
    
    Create a new PMKey(s) based upon selecting an existing PMColumn(s).
*/    
void PMKeysWidget::slotNew()
{
/*
    // get list of attributes                                                                     
    QList<ADObject*> listModelsIn = pTable->getObjects( QString(), QString(), "PMColumn" );       
                                                                                                  
    // create dialog                                                                              
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Column..."), "SelectColumn" );        
    pdialog->setWindowIcon( QIcon( ":W/RowNew16x16" ) );                                          
                                                                                                  
    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn, true );
    pdialog->getTabWidget()->addTab( pmultiselect, "Columns" );                                   
                                                                                                  
    // popup window                                                                               
    if ( pdialog->exec() != QDialog::Rejected )                                                   
    {                                                                                             
        PMKey *pKey = (PMKey*)pTable->getObject( "Key" );                                         
        doRowAppend( pKey );                                                                      
                                                                                                  
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();                             
        ADObject *p;                                                                              
        foreach( p, listModelsOut )                                                               
        {                                                                                         
            PMColumn *pColumn = (PMColumn*)p;                                                     
            pKey->doAddColumn( pColumn );                                                         
        }                                                                                         
        pKey->slotObjectDialog( this );                                                           
    }                                                                                             
                                                                                                  
    // clean up                                                                                   
    delete pdialog;                                                                               
*/

    PMKey *pKey = (PMKey*)pTable->getObject( "Key" );
    if ( !pKey ) return;                             
    pKey->slotObjectDialog( this );                  
    doRowAppend( pKey );                             
}

/*!
    doRowSet
    
*/    
void PMKeysWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will 
    //       cause all to be blanked out. So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0                                        
    AWDataTable::doRowSet( nRow, pObject );                              
                                                                        
    // Now we set the remaining cols                                    
    PMKey *pKey = (PMKey*)pObject;                  
                                                                        
    ptable->item( nRow, 1 )->setText( pKey->getName() );
    ptable->item( nRow, 2 )->setText( pKey->getCode() );
    ptable->item( nRow, 3 )->setText( (pKey->isPrimary() ? "*" : "" ) );

    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

/*!
    doRowGet
    
*/    
void PMKeysWidget::doRowGet( int nRow, AWObject *pObject )
{
    PMKey *pKey = (PMKey*)pObject;                
                                                                      
    pKey->setName( ptable->item( nRow, 1 )->text().simplified() );
    pKey->setCode( ptable->item( nRow, 2 )->text().simplified() );
    pKey->setPrimary( ptable->item( nRow, 3 )->text().simplified() == "" ? 0 : 1 );
}


