#include "LibInfo.h"
#include "AWObjectsWidget.h"

AWObjectsWidget::AWObjectsWidget( AWObject *pObject, QWidget *pWidgetParent, const QString &stringClass )
    : AWDataTable( pObject, pWidgetParent, stringClass /* match class */ )
{
    this->pObject       = pObject;
    this->stringClass   = stringClass;

    // TABLE
    ptable->setColumnCount( 4 );
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Code") ) );
    ptable->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Comment") ) );

    // TOOLBAR

    // Init
    doTableSet();
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

AWObjectsWidget::~AWObjectsWidget()
{
}

void AWObjectsWidget::slotNew()
{
    AWObject *p = (AWObject*)pObject->getObject( stringClass );
    if ( !p ) return;
    doRowAppend( p );
}

void AWObjectsWidget::doRowSet( int nRow, AWObject *pObject )
{
    // \note This will trigger cellChanged for each col. and, because of the order of events, will cause all to be blanked out.
    //       So temp disconnect.
    disconnect( ptable, SIGNAL(cellChanged(int,int)), this, SLOT(slotValueChanged(int,int)) );

    // let AWDataTable set col 0
    AWDataTable::doRowSet( nRow, pObject );

    ptable->item( nRow, 1 )->setIcon( pObject->getIcon() );
    ptable->item( nRow, 1 )->setText( pObject->getName() );
    ptable->item( nRow, 2 )->setText( pObject->getCode() );
    ptable->item( nRow, 3 )->setText( pObject->getComment() );

    // reconnect...
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

void AWObjectsWidget::doRowGet( int nRow, AWObject *pObject )
{
    // pobject will ignore if no change so set them without bothering to check here...
    pObject->setName( ptable->item( nRow, 1 )->text().simplified() );
    pObject->setCode( ptable->item( nRow, 2 )->text().simplified() );
    pObject->setComment( ptable->item( nRow, 3 )->text().simplified() );
}

