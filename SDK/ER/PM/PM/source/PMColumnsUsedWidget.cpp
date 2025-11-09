#include "LibInfo.h"
#include "PMColumnsUsedWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "PMTableBase.h"
#include "PMColumn.h"

PMColumnsUsedWidget::PMColumnsUsedWidget( PMKey *pKey, QWidget *pParent )
    : AWDataTable( pKey, "PMColumn", pParent, true )
{
    this->pKey  = pKey;           
    pTable = (PMTableBase*)pKey->getParent( "PMTableBase" );
                                                  
    // TABLE                                      
    ptable->setColumnCount( 3 );                      
    QStringList listHeaders;
    listHeaders << tr("ID") << ( "Name" )<< tr("Code");
    ptable->setHorizontalHeaderLabels( listHeaders );
                                                  
    // TOOLBAR                                    
    setNew();
    setDelete();

    // Init                                       
    doTableSet();                                 
}

PMColumnsUsedWidget::~PMColumnsUsedWidget()
{
}

/*!
    slotNew
    
    Create a new entry based upon selecting an existing PMColumn(s).
*/    
void PMColumnsUsedWidget::slotNew()
{
    // get list of data items                                                                   
    QList<ADObject*> listModelsIn = pTable->getObjects( QString(), QString(), "PMColumn" ); 
                                                                                                
    // create dialog                                                                            
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Column..."), "SelectColumn" );
    pdialog->setWindowIcon( QIcon( ":AW/RowNew" ) );
                                                                                                
    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn );    
    pdialog->getTabWidget()->addTab( pmultiselect, "Columns" );                              
                                                                                                
    // popup window                                                                             
    if ( pdialog->exec() != QDialog::Rejected )                                                 
    {                                                                                           
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();
        ADObject *       p;

        foreach( p, listModelsOut )
        {
            PMColumn *pColumn = (PMColumn*)p;
            doRowAppend( pColumn );
            pKey->doAddColumn( pColumn );
        }
    }                                                                                           
                                                                                                
    // clean up                                                                                 
    delete pdialog;                                                                             
}

void PMColumnsUsedWidget::slotDelete()
{
    int nRow = getRowNumStrict();

    if ( nRow < 0 ) return;

    // make edit widget go away
    ptable->setCurrentCell( -1, -1 );

    AWObject *pmodel = getObject( nRow );
    if ( !pmodel ) return;

    pKey->doRemoveColumn( (PMColumn*)pmodel );

    doTableSet();
}

void PMColumnsUsedWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0                      
    AWDataTable::doRowSet( nRow, pObject );            
                                                      
    // Now we set the remaining cols                  
    PMColumn *pColumn = (PMColumn*)pObject;   
                                                      
    ptable->item( nRow, 1 )->setText( pColumn->getName() );
    ptable->item( nRow, 2 )->setText( pColumn->getCode() );
}

void PMColumnsUsedWidget::doRowGet( int nRow, AWObject *pObject )
{
    PMColumn *pColumn = (PMColumn*)pObject;                  
                                                                     
    pColumn->setName( ptable->item( nRow, 1 )->text().simplified() );
    pColumn->setCode( ptable->item( nRow, 2 )->text().simplified() );
}


