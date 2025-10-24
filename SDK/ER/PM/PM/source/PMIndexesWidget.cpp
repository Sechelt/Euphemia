#include "LibInfo.h"
#include "PMIndexesWidget.h"

#include "PMIndex.h"

PMIndexesWidget::PMIndexesWidget( PMTableBase *pTableBase, QWidget *pwidgetParent )
    : AWDataTable( pTableBase, "PMIndex" /* match inherits class */, pwidgetParent )
{
    this->pTableBase = pTableBase;

    // Table
    ptable->setColumnCount( 3 );                                                                                                                                       
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Unique") ) );
                                                                                                                                                                    
    // ToolBar: adjust default
    setNew();
    setEdit();
    setDelete();

    // Load                                                                                                                                                         
    doTableSet();                                                                                                                                                   
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );

    doLoadState();
}

PMIndexesWidget::~PMIndexesWidget()
{
}

void PMIndexesWidget::slotNew()
{
    PMIndex *pIndex = (PMIndex*)pTableBase->getObject( "Index" ); // use friendly name so it works for derived classes
    if ( !pIndex ) return; // lets not crap out if derived class does not want to create a index

    pIndex->slotObjectDialog( this );

    doRowAppend( pIndex );                                             
}

void PMIndexesWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will 
    //       cause all to be blanked out. So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0                                          
    AWDataTable::doRowSet( nRow, pObject );                                
                                                                              
    // Now we set the remaining cols                                      
    PMIndex *pIndex = (PMIndex*)pObject;                       
                                                                              
    ptable->item( nRow, 1 )->setText( pIndex->getName() );                    
    ptable->item( nRow, 2 )->setText( pIndex->getUnique() ? "*" : "" );                    

    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

void PMIndexesWidget::doRowGet( int nRow, AWObject *pObject )
{
    PMIndex *pIndex = (PMIndex*)pObject;                        
                                                                           
    pIndex->setName( ptable->item( nRow, 1 )->text().simplified() );      
    pIndex->setUnique( ptable->item( nRow, 2 )->text().simplified().isEmpty() ? false : true );      
}

