#include "LibInfo.h"
#include "AWDynamicDataWidget.h"
        
AWDynamicDataWidget::AWDynamicDataWidget( AWObject *pObject, QWidget *pwidgetParent )
    : QMainWindow( pwidgetParent )
{
    this->pObject = pObject;         

    doInit();
}

AWDynamicDataWidget::~AWDynamicDataWidget()
{
}

void AWDynamicDataWidget::slotNew()
{
    // must be first as setItem() will trigger cellChanged() event
    vectorNames.append( QString() );
    vectorValues.append( QString() );

    int nRow = ptable->rowCount();
    ptable->setRowCount( nRow + 1 );
    ptable->setItem( nRow, 0, new QTableWidgetItem() );
    ptable->setItem( nRow, 1, new QTableWidgetItem() );

    Q_ASSERT( vectorNames.count() == ptable->rowCount() );
}

void AWDynamicDataWidget::slotDelete()
{
    int nRow = ptable->currentRow();
    if ( nRow < 0 ) return;

    QString stringName = ptable->item( nRow, 0 )->text();
    pObject->setProperty( stringName.toLatin1(), QVariant() );

    // make edit widget go away
    ptable->setCurrentCell( -1, -1 );

    ptable->removeRow( nRow );

    vectorNames.remove( nRow );
    vectorValues.remove( nRow );

    Q_ASSERT( vectorNames.count() == ptable->rowCount() );
}

void AWDynamicDataWidget::slotValueChanged( int nRow, int nCol )
{
    Q_ASSERT( vectorNames.count() == ptable->rowCount() );
    Q_ASSERT( vectorNames.count() > nRow );

    // name requires some special considerations
    if ( nCol == 0 )
    {
        QString stringName = ptable->item( nRow, 0 )->text();
        if ( !stringName.isEmpty() && vectorNames.contains( stringName ) )
        {
            pObject->doMessageBox( "ERROR", tr("Dynamic Data"), tr("Name already exists. Reverting change.") );
            ptable->item( nRow, 0 )->setText( vectorNames.at( nRow ) );
            return; 
        }
        // delete old name
        pObject->setProperty( vectorNames.at( nRow ).toLatin1(), QVariant() );
        // add new name (with existing value)
        pObject->setProperty( stringName.toLatin1(), vectorValues[nRow] );
        vectorNames[nRow] = stringName;
        pObject->setModified();
        return;
    }

    // no real validation needed on value
    QString stringName  = ptable->item( nRow, 0 )->text();
    QString stringValue = ptable->item( nRow, 1 )->text();
    pObject->setProperty( stringName.toLatin1(), stringValue );
    vectorValues[nRow] = stringValue;
    pObject->setModified();
}


void AWDynamicDataWidget::doInit()
{
    // TABLE
    ptable = new QTableWidget( 1, 2, this );
    // ptable->hideColumn( 0 );
    ptable->setHorizontalHeaderItem( 0, new QTableWidgetItem( tr("Name") ) );
    ptable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Value") ) );
    ptable->setSelectionBehavior( QAbstractItemView::SelectRows );
    ptable->setSelectionMode( QAbstractItemView::SingleSelection );    
    ptable->setRowCount( 0 );
    setCentralWidget( ptable );

    // TOOLBAR
    pActionNew = new QAction( QIcon( ":W/RowNew16x16" ), tr("New"), this );
    pActionDelete = new QAction( QIcon( ":W/RowDelete16x16" ), tr("Delete"), this );

    connect( pActionNew, SIGNAL(triggered()), this, SLOT(slotNew()) );
    connect( pActionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()) ); 

    ptoolbar = addToolBar( tr("") );
    ptoolbar->addAction( pActionNew );
    ptoolbar->addAction( pActionDelete );

    doLoad();

    // after load to avoid calls to validate/accept changes
    connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

void AWDynamicDataWidget::doLoad()
{
    QList<QByteArray> l = pObject->dynamicPropertyNames();
    QByteArray a;
    ptable->setRowCount( l.count() );
    int nRow = 0;
    foreach( a, l )
    {
        // update vectors before table
        vectorNames.append( QString( a ) );
        vectorValues.append( pObject->property( a ).toString() );
        //
        ptable->setItem( nRow, 0, new QTableWidgetItem( vectorNames.last() ) );
        ptable->setItem( nRow, 1, new QTableWidgetItem( vectorValues.last() ) );
        nRow++;
    }
    Q_ASSERT( vectorNames.count() == ptable->rowCount() );
}


