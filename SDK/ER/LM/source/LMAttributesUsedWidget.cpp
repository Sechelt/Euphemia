#include "LibInfo.h"
#include "LMAttributesUsedWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "LMEntityBase.h"

LMAttributesUsedWidget::LMAttributesUsedWidget( LMIdentifier *pIdentifier, QWidget *pParent )
    : AWDataTable( pIdentifier, "LMAttribute", pParent, true )
{
    this->pIdentifier  = pIdentifier;           
    pEntity = (LMEntityBase*)pIdentifier->getParent( "LMEntityBase" );
                                                  
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

LMAttributesUsedWidget::~LMAttributesUsedWidget()
{
}

/*!
    slotNew
    
    Create a new entry based upon selecting an existing LMAttribute(s).
*/    
void LMAttributesUsedWidget::slotNew()
{
    // get list of data items                                                                   
    QList<ADObject*> listModelsIn = pEntity->getObjects( QString(), QString(), "LMAttribute" ); 
                                                                                                
    // create dialog                                                                            
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Attribute..."), "SelectAttribute" );
    pdialog->setWindowIcon( QIcon( ":W/RowNew16x16" ) );
                                                                                                
    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn );    
    pdialog->getTabWidget()->addTab( pmultiselect, "Attributes" );                              
                                                                                                
    // popup window                                                                             
    if ( pdialog->exec() != QDialog::Rejected )                                                 
    {                                                                                           
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();
        ADObject *       p;

        foreach( p, listModelsOut )
        {
            LMAttribute *pAttribute = (LMAttribute*)p;
            pIdentifier->doConnect( pAttribute );
            doRowAppend( pAttribute );
        }
    }                                                                                           
                                                                                                
    // clean up                                                                                 
    delete pdialog;                                                                             
}

void LMAttributesUsedWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0                      
    AWDataTable::doRowSet( nRow, pObject );            
                                                      
    // Now we set the remaining cols                  
    LMAttribute *pAttribute = (LMAttribute*)pObject;   
                                                      
    ptable->item( nRow, 1 )->setText( pAttribute->getName() );
    ptable->item( nRow, 2 )->setText( pAttribute->getCode() );
}

void LMAttributesUsedWidget::doRowGet( int nRow, AWObject *pObject )
{
    LMAttribute *pAttribute = (LMAttribute*)pObject;                  
                                                                     
    pAttribute->setName( ptable->item( nRow, 1 )->text().simplified() );
    pAttribute->setCode( ptable->item( nRow, 2 )->text().simplified() );
}


