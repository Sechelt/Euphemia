#include "LibInfo.h"
#include "LMRulesUsedWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "LMRule.h"

LMRulesUsedWidget::LMRulesUsedWidget( AWObject *pObject, QWidget *pParent )
    : AWDataTable( pObject, "LMRule", pParent, true )
{
    this->pModel   = (LMModel*)pObject->getParent( "LMModel" );
    this->pObject  = pObject;

    // Table
    ptable->setColumnCount( 3 );
    QStringList listHeaders;
    listHeaders << tr("ID") << tr("Name") << tr("Type");
    ptable->setHorizontalHeaderLabels( listHeaders );

//    QAction *pActionNewRule = new QAction( QIcon( ":RowNewPlus16x16" ), tr("New Rule"), this );
//    connect( pActionNewRule, SIGNAL(triggered()), this, SLOT(slotNewRule()) );
//    ptoolbar->addAction( pActionNewRule );
    setNew();
//    setEdit();
    setDelete();

    // Load
    doTableSet();
}

LMRulesUsedWidget::~LMRulesUsedWidget()
{
}

/*!
    slotNew
    
    Create a new entry based upon selecting an existing LMRule.
*/    
void LMRulesUsedWidget::slotNew()
{
    // get list of rules
    QList<ADObject*> listModelsIn = pModel->getObjects( QString(), QString(), "LMRule" );

    // create dialog
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Rule..."), "SelectRule" );
    pdialog->setWindowIcon( QIcon( ":W/RowNew16x16" ) );

    AWObjectSelectWidget * pmultiselect = new AWObjectSelectWidget( pdialog, listModelsIn );
    pdialog->getTabWidget()->addTab( pmultiselect, "Rules" );

    // popup window
    if ( pdialog->exec() != QDialog::Rejected )
    {
        // create new for each selected data item
        QList<ADObject*> listModelsOut = pmultiselect->getSelected();
        ADObject *       p;

        foreach( p, listModelsOut )
        {
            LMRule *prule = (LMRule*)p;
            pObject->doConnect( prule );
            doRowAppend( prule );
        }
    }

    // clean up
    delete pdialog;
}

void LMRulesUsedWidget::slotNewRule()
{
    LMRule *    pRule   = (LMRule*)pModel->getObject( "LMRule" );   
    pObject->doConnect( pRule );
    doRowAppend( pRule );                                             
}

void LMRulesUsedWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0
    AWDataTable::doRowSet( nRow, pObject );

    // Now we set the remaining cols
    LMRule *prule = (LMRule*)pObject;

    ptable->item( nRow, 1 )->setText( prule->getName() );
    ptable->item( nRow, 2 )->setText( QString::number( prule->getRuleType() ) );
}

void LMRulesUsedWidget::doRowGet( int nRow, AWObject *pObject )
{
    LMRule *prule = (LMRule*)pObject;

    prule->setName( ptable->item( nRow, 1 )->text().simplified() );
    prule->setRuleType( (LMRule::enumRuleTypes)ptable->item( nRow, 2 )->text().simplified().toInt() );
}


