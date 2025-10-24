#include "LibInfo.h"
#include "LMRulesWidget.h"

#include <AWObjectSelectWidget.h>

#include "LMRule.h"

LMRulesWidget::LMRulesWidget( LMModel *pModel, QWidget *pParent )
    : AWDataTable( pModel, "LMRule", pParent )
{
    // TABLE                                      
    ptable->setColumnCount( 4 );
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Type") ) );

    // ToolBar: adjust default
    setNew();
    setEdit();
    setDelete();

    // Load                                       
    doTableSet();                                 
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

LMRulesWidget::~LMRulesWidget()
{
}

/*!
    slotNew
    
    Create a new entry based upon selecting an existing LMRule.
*/    
void LMRulesWidget::slotNew()
{
    LMModel *pModel  = (LMModel*)pmodelParent;          
    LMRule * pRule   = (LMRule*)pModel->getObject( "LMRule" );

    pRule->slotObjectDialog( this );                    

    doRowAppend( pRule );                             
}

void LMRulesWidget::doRowSet( int nRow, AWObject *pObject )
{
    // let AWDataTable set col 0
    AWDataTable::doRowSet( nRow, pObject );

    // Now we set the remaining cols
    LMRule *prule = (LMRule*)pObject;

    ptable->item( nRow, 1 )->setText( prule->getName() );
    ptable->item( nRow, 2 )->setText( prule->getCode() );
    ptable->item( nRow, 3 )->setText( QString::number( prule->getRuleType() ) );
}

void LMRulesWidget::doRowGet( int nRow, AWObject *pObject )
{
    LMRule *prule = (LMRule*)pObject;

    prule->setName( ptable->item( nRow, 1 )->text().simplified() );
    prule->setCode( ptable->item( nRow, 2 )->text().simplified() );
    prule->setRuleType( (LMRule::enumRuleTypes)ptable->item( nRow, 2 )->text().simplified().toInt() );
}


