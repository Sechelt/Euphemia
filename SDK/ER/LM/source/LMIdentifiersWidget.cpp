#include "LibInfo.h"
#include "LMIdentifiersWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "LMIdentifier.h"

LMIdentifiersWidget::LMIdentifiersWidget( LMEntityNormal *pEntity, QWidget *pParent )
    : AWDataTable( pEntity, "LMIdentifier", pParent )
{
    this->pEntity = pEntity;

    ptable->setColumnCount( 4 );                      
    QStringList listHeaders;
    listHeaders << tr("ID") << tr("Name") << tr("Code") << tr("Primary");
    ptable->setHorizontalHeaderLabels( listHeaders );
                                                  
    // TOOLBAR                                    
    setNew();
    setEdit();
    setDelete();

    // Load                                       
    doTableSet();                                 
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

LMIdentifiersWidget::~LMIdentifiersWidget()
{
}

/*!
    slotNew
    
    Create a new LMIdentifier(s) based upon selecting an existing LMAttribute(s).
*/    
void LMIdentifiersWidget::slotNew()
{
    // get list of attributes
    QList<ADObject*> listModelsIn = pEntity->getObjects( QString(), QString(), "LMAttribute" );

    // create dialog
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Attribute..."), "SelectAttribute" );
    pdialog->setWindowIcon( QIcon( ":W/RowNew16x16" ) );

    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn, true );
    pdialog->getTabWidget()->addTab( pmultiselect, "Attributes" );

    // popup window
    if ( pdialog->exec() != QDialog::Rejected )
    {
        // create new for each selected data item
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();

        if ( listModelsOut.count() > 0 )
        {
            LMIdentifier *pIdentifier = new LMIdentifier( pEntity );
            doRowAppend( pIdentifier );

            ADObject *p;

            foreach( p, listModelsOut )
            {
                LMAttribute *pAttribute = (LMAttribute*)p;
                pIdentifier->doConnect( pAttribute );
            }
        }
    }

    // clean up
    delete pdialog;
}

/*!
    doRowSet
    
*/    
void LMIdentifiersWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0                                        
    AWDataTable::doRowSet( nRow, pObject );                              
                                                                        
    // Now we set the remaining cols                                    
    LMIdentifier *pIdentifier = (LMIdentifier*)pObject;                  
                                                                        
    ptable->item( nRow, 1 )->setText( pIdentifier->getName() );
    ptable->item( nRow, 2 )->setText( pIdentifier->getCode() );
    ptable->item( nRow, 3 )->setText( pIdentifier->isPrimary() ? "*" : "" );
}

/*!
    doRowGet
    
*/    
void LMIdentifiersWidget::doRowGet( int nRow, AWObject *pObject )
{
    LMIdentifier *pIdentifier = (LMIdentifier*)pObject;                
                                                                      
    pIdentifier->setName( ptable->item( nRow, 1 )->text().simplified() );
    pIdentifier->setCode( ptable->item( nRow, 2 )->text().simplified() );
    pIdentifier->setPrimary( ptable->item( nRow, 3 )->text().simplified() == "" ? 0 : 1 );
}


