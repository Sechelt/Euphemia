#include "LibInfo.h"
#include "PMRulesUsedWidget.h"

#include <WTabDialog.h>

#include <AWObjectSelectWidget.h>

#include "PMRule.h"

PMRulesUsedWidget::PMRulesUsedWidget( AWObject *pObject, QWidget *pParent )
    : AWDataTable( pObject, "PMRule", pParent, true )
{
    this->pModel   = (PMModel*)pObject->getParent( "PMModel" );
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
    setEdit();
    setDelete();

    // Load
    doTableSet();
}

PMRulesUsedWidget::~PMRulesUsedWidget()
{
}

/*!
    slotNew
    
    Create a new entry based upon selecting an existing PMRule.
*/    
void PMRulesUsedWidget::slotNew()
{
    // get list of rules
    QList<ADObject*> listModelsIn = pModel->getObjects( QString(), QString(), "PMRule" );

    // create dialog
    WTabDialog * pdialog = new WTabDialog( this, tr("Select Rule..."), "SelectRule" );
    pdialog->setWindowIcon( QIcon( ":AW/RowNew" ) );

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
            PMRule *prule = (PMRule*)p;
            pObject->doConnect( prule );
            doRowAppend( prule );
        }
    }

    // clean up
    delete pdialog;
}

void PMRulesUsedWidget::slotNewRule()
{
    PMRule *    pRule   = (PMRule*)pModel->getObject( "PMRule" );   
    pObject->doConnect( pRule );
    doRowAppend( pRule );                                             
}

void PMRulesUsedWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0
    AWDataTable::doRowSet( nRow, pObject );

    // Now we set the remaining cols
    PMRule *pRule = (PMRule*)pObject;

    ptable->item( nRow, 1 )->setText( pRule->getName() );
    ptable->item( nRow, 2 )->setText( QString::number( pRule->getRuleType() ) );
}

void PMRulesUsedWidget::doRowGet( int nRow, AWObject *pObject )
{
    PMRule *pRule = (PMRule*)pObject;

    pRule->setName( ptable->item( nRow, 1 )->text().simplified() );
    pRule->setRuleType( (PMRule::enumRuleTypes)ptable->item( nRow, 2 )->text().simplified().toInt() );
}


