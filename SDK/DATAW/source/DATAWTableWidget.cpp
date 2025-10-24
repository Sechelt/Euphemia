// #include <ctype.h>

#include "LibInfo.h"
#include "DATAWTableWidget.h"

#include <AWPalWidget.h>
#include <AWPanWidget.h>

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATADiagnostic.h>

#include "DATAWTableModelApp.h"
#include "DATAWTableModelODBC.h"

DATAWTableWidget::DATAWTableWidget( AWObject *pObject, QWidget *pParent, DATAConnection *p, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable  )
: AWEditorWidget( pObject, pParent )
{
    setObjectName( metaObject()->className() );
    pConnection         = p;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringTable   = stringTable;
            
    // no connection no joy
    if ( !pConnection ) return;
    if ( !pConnection->isConnected() ) return;

    // status bar
    {
        QStatusBar *pstatusbar = statusBar();

        pNavigation = new WDataNavWidget( pstatusbar );
        pstatusbar->addPermanentWidget( pNavigation, 10 );
        connect( pNavigation, SIGNAL(signalFirst()), SLOT(slotRowFirst()) );
        connect( pNavigation, SIGNAL(signalPrevious()), SLOT(slotRowPrevious()) );
        connect( pNavigation, SIGNAL(signalLast()), SLOT(slotRowLast()) );
        connect( pNavigation, SIGNAL(signalNext()), SLOT(slotRowNext()) );
        connect( pNavigation, SIGNAL(signalNew()), SLOT(slotRowNew()) );
        connect( pNavigation, SIGNAL(signalDelete()), SLOT(slotRowDelete()) );

        pRowsAffected   = new QLabel( pstatusbar );
        pstatusbar->addPermanentWidget( pRowsAffected );

        pEdits          = new QLabel( pstatusbar );
        pstatusbar->addPermanentWidget( pEdits );
    }

    //
    pTableView  = new DATAWTableView();
    pTableView->setMouseTracking( false  );

    // We want;
    // * to do positioned updates 
    // * use scrollable cursor
    // * scroll cursor forward and backwards
    // * absolute positioning
    // * single row row-set
    // * have the row-set and result-set be aware of changes in database (particularly our changes)
    //
    // Different cursor solutions;
    // * ODBC driver            - may be server-side or client-side - whatever the driver can manage with a pref to server-side
    // * ODBC driver manager    - client-side
    // * Application            - client-side
    SQLRETURN nReturn;
    QString stringDBMS_NAME = pConnection->getInfo( SQL_DBMS_NAME, &nReturn ).toString();
    QString stringDBMS_VER  = pConnection->getInfo( SQL_DBMS_VER, &nReturn ).toString();
 //   if ( stringDBMS_NAME == "SQL Server" || 
 //        stringDBMS_NAME == "ACCESS" ||
 //        stringDBMS_NAME == "MariaDB" )
 //       pTableView->setModel( pTableModel = new DATAWTableModelODBC( pTableView, pConnection, stringCatalog, stringSchema, stringTable ) );
 //   else
        pTableView->setModel( pTableModel = new DATAWTableModelApp( pTableView, pConnection, stringCatalog, stringSchema, stringTable ) );

    pTableView->setSelectionMode( QAbstractItemView::SingleSelection );
    pTableView->setSelectionBehavior( QAbstractItemView::SelectRows );
    pSelection = pTableView->selectionModel();
    connect( pSelection, SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(slotCurrentSelectionChanged(const QModelIndex &,const QModelIndex &)) );

    pNavigation->setRecords( pTableModel->getRowsData() );
    connect( pTableModel, SIGNAL(signalRowsData(int)), pNavigation, SLOT(slotRecords(int)) );
    connect( pTableModel, SIGNAL(signalRowsAffected(int)), SLOT(slotRowsAffected(int)) );
    connect( pTableModel, SIGNAL(signalEdits(bool)), SLOT(slotEdits(bool)) );
    connect( pTableModel, SIGNAL(signalReadOnly(bool)), SLOT(slotReadOnly(bool)) );

    setCentralWidget( pTableView );

    doLoadState();
}


DATAWTableWidget::~DATAWTableWidget()
{
    doSaveState();
}

AWPalWidget *DATAWTableWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new AWPalWidget( this, nullptr );
    return pPalWidget;
}

AWPanWidget *DATAWTableWidget::getPanWidget()
{
    if ( pPanWidget ) return pPanWidget;
    pPanWidget = new AWPanWidget( this, nullptr );
    return pPanWidget;
}

void DATAWTableWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !stringTable.isEmpty() ) s = s + "/" + stringTable;
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        pTableView->setColumnWidth( n, settings.value( "w", QString::number( pTableView->columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

void DATAWTableWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !stringTable.isEmpty() ) s = s + "/" + stringTable;
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < pTableView->model()->columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", pTableView->columnWidth( n ) );
    }
    settings.endArray();
}

void DATAWTableWidget::slotRowFirst()
{
    pTableView->selectRow( 0 );
}

void DATAWTableWidget::slotRowPrevious()
{
    QModelIndexList listSelected = pSelection->selectedRows();
    if ( listSelected.count() < 1 ) return;
    int nRow = listSelected.first().row();
    pTableView->selectRow( nRow - 1 );
}

void DATAWTableWidget::slotRowLast()
{
    pTableView->selectRow( pTableView->model()->rowCount() - 1 );
}

void DATAWTableWidget::slotRowNext()
{
    QModelIndexList listSelected = pSelection->selectedRows();
    if ( listSelected.count() < 1 ) return;
    int nRow = listSelected.first().row();
    pTableView->selectRow( nRow + 1 );
}

void DATAWTableWidget::slotRowNew()
{
    QModelIndexList listSelected = pSelection->selectedRows();
    if ( listSelected.count() < 1 ) return;
    int nRow = listSelected.first().row();
}

void DATAWTableWidget::slotRowDelete()
{
    QModelIndexList listSelected = pSelection->selectedRows();
    if ( listSelected.count() < 1 ) return;
    int nRow = listSelected.first().row();
    pTableModel->removeRows( nRow, 0 );
}

void DATAWTableWidget::slotCurrentSelectionChanged( const QModelIndex &modelIndexCurrent, const QModelIndex &modelIndexPrevious )
{
    if ( modelIndexCurrent.row() == modelIndexPrevious.row() ) return;

    pTableModel->doEditCommit();

    if ( modelIndexCurrent.isValid() )
        pNavigation->setRecord( modelIndexCurrent.row() + 1 );
    else
        pNavigation->setRecord( 0 );

    // pNavigation->setSelection( modelIndexPrevious.row(), modelIndexPrevious.column(), modelIndexCurrent.row(), modelIndexCurrent.column() );
}

void DATAWTableWidget::slotRowsAffected( int n )
{
    pRowsAffected->setText( QString( tr("Rows Affected: %1").arg( n ) ) );
}

void DATAWTableWidget::slotEdits( bool b )
{
    if ( b )
        pEdits->setPixmap( QPixmap( ":DATAW/Edits" ) );
    else
        pEdits->setPixmap( QPixmap() );
}

void DATAWTableWidget::slotReadOnly( bool b )
{
    if ( b )
        pEdits->setPixmap( QPixmap( ":DATAW/ReadOnly" ) );
    else
        pEdits->setPixmap( QPixmap() );
}


