#include "LibInfo.h"
#include "PMColumnsWidget.h"

#include <WTabDialog.h>
#include <AWObjectSelectWidget.h>

#include "PMColumn.h"
#include "PMDomain.h"

PMColumnsWidget::PMColumnsWidget( PMTableBase *pTableBase, QWidget *pwidgetParent )
    : AWDataTable( pTableBase, "PMColumn" /* match inherits class */, pwidgetParent )
{
    pModel = (PMModel*)pTableBase->getParent( "PMModel" );
    this->pTableBase = pTableBase;

    // Table
    ptable->setColumnCount( 5 );                                                                                                                                       
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Domain") ) );
    ptable->setHorizontalHeaderItem( 4, new QTableWidgetItem( tr("Type") ) );
                                                                                                                                                                    
    // ToolBar: adjust default
    setNew();
    setEdit();
    setDelete();
    setUp();
    setDown();

    // Load                                                                                                                                                         
    doTableSet();                                                                                                                                                   
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

PMColumnsWidget::~PMColumnsWidget()
{
}

void PMColumnsWidget::slotNew()
{
    PMColumn *pColumn = (PMColumn*)pTableBase->getObject( "Column" ); // use friendly name so it works for derived classes
    if ( !pColumn ) return; // lets not crap out if derived class does not want to create a column

    pColumn->slotObjectDialog( this );
                                                                           
    doRowAppend( pColumn );                                             
}

void PMColumnsWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will 
    //       cause all to be blanked out. So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0                                          
    AWDataTable::doRowSet( nRow, pObject );                                
                                                                          
    // Now we set the remaining cols                                      
    PMColumn *pColumn = (PMColumn*)pObject;                       
                                                                          
    ptable->item( nRow, 1 )->setText( pColumn->getName() );                    
    ptable->item( nRow, 2 )->setText( pColumn->getCode() );                    
    if ( pColumn->getDomain() )                                        
        ptable->item( nRow, 3 )->setText( pColumn->getDomain()->getName() );   
    else                                                                  
        ptable->item( nRow, 3 )->setText( "" );                                   
    ptable->item( nRow, 4 )->setText( pColumn->getDataType() );                

    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

/*!

*/    
void PMColumnsWidget::doRowGet( int nRow, AWObject *pObject )
{
    PMColumn *pColumn = (PMColumn*)pObject;                        
                                                                           
    pColumn->setName( ptable->item( nRow, 1 )->text().simplified() );      
    pColumn->setCode( ptable->item( nRow, 2 )->text().simplified() );      
    // skip domain (3)
    pColumn->setDataType( ptable->item( nRow, 4 )->text().simplified() );  
}

