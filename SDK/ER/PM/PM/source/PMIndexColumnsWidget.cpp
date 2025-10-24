#include "LibInfo.h"
#include "PMIndexColumnsWidget.h"

#include "PMTableBase.h"
#include "PMIndexColumn.h"

PMIndexColumnsWidget::PMIndexColumnsWidget( PMIndex *pIndex, QWidget *pwidgetParent )
    : AWDataTable( pIndex, "PMIndexColumn" /* match inherits class */, pwidgetParent )
{
    this->pIndex = pIndex;

    // Table
    ptable->setColumnCount( 3 );                                                                                                                                       
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Order") ) );
                                                                                                                                                                    
    // ToolBar: adjust default
    setNew();
    setEdit();
    setDelete();

    // Load                                                                                                                                                         
    doTableSet();                                                                                                                                                   
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

PMIndexColumnsWidget::~PMIndexColumnsWidget()
{
}

void PMIndexColumnsWidget::slotNew()
{
    PMIndexColumn *pColumn = (PMIndexColumn*)pIndex->getObject( "Column" ); // use friendly name so it works for derived classes
    if ( !pColumn ) return; // lets not crap out if derived class does not want to create a column

//    pColumn->slotObjectDialog( this );
                                                                           
    doRowAppend( pColumn );                                             
}

void PMIndexColumnsWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will 
    //       cause all to be blanked out. So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0                                          
    AWDataTable::doRowSet( nRow, pObject );                                
                                                                          
    // Now we set the remaining cols                                      
    PMIndexColumn *pIndexColumn = (PMIndexColumn*)pObject;                       
                                                                          
    QComboBox *pColumn = new QComboBox( this );
    {
        PMTableBase *pTable = (PMTableBase*)pIndex->getParent( "PMTableBase" );
        QList<ADObject*> l = pTable->getObjects( QString(), QString(), "PMColumn" );
        ADObject *p;
        foreach( p, l )
        {
            pColumn->addItem( p->getName() );
        }
    }
    pColumn->addItem( "" );
    pColumn->setCurrentText( pIndexColumn->getName() );
    ptable->setCellWidget( nRow, 1, pColumn );
    connect( pColumn, SIGNAL(currentTextChanged(const QString&)), pIndexColumn, SLOT(slotName(const QString &)) );

    QComboBox *pOrder = new QComboBox( this );
    pOrder->addItem( "ASC" );
    pOrder->addItem( "DESC" );
    pOrder->addItem( "" );
    pOrder->setCurrentText( pIndexColumn->getOrder() );
    ptable->setCellWidget( nRow, 2, pOrder );
    connect( pOrder, SIGNAL(currentTextChanged(const QString&)), pIndexColumn, SLOT(slotOrder(const QString &)) );

    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

void PMIndexColumnsWidget::doRowGet( int nRow, AWObject *pObject )
{
    Q_UNUSED(nRow);
    Q_UNUSED(pObject);
}

