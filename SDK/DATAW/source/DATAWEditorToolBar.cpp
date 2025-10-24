#include "LibInfo.h"
#include "DATAWEditorToolBar.h"

#include "DATAWConnection.h"

//
// DATAWEditorToolBar
//
DATAWEditorToolBar::DATAWEditorToolBar( DATAWConnection *p, QWidget *pParent )
: QToolBar( pParent )
{
    setObjectName( tr("SQL Editor") );
    setWindowTitle( tr("SQL Editor") );

    // \todo default values should come from Preferences

    pConnection = p;
    if ( pConnection )
    {
        connect( pConnection, SIGNAL(signalConnected()), this, SLOT(slotChangedConnected()) ); 
        connect( pConnection, SIGNAL(signalDisconnected()), this, SLOT(slotChangedConnected()) ); 
    }
    hasSelection = false;

    pSubmit = addAction( QIcon(":DATAW/Submit"), tr("Submit") );
    pSubmit->setToolTip( tr("submit all text to the data source\nmust be connected to data source") );
    connect( pSubmit, SIGNAL(triggered()), SIGNAL(signalSubmit()) );

    pSubmitSelected = addAction( QIcon(":DATAW/SubmitSelected"), tr("Submit Selected") );
    pSubmitSelected->setToolTip( tr("submit selected text to the data source\nmust be connected to data source") );
    connect( pSubmitSelected, SIGNAL(triggered()), SIGNAL(signalSubmitSelected()) );

    pTables = addAction( QIcon(":DATAW/Table"), tr("Tables") );
    pTables->setToolTip( tr("show tables\nmust be connected to data source") );
    connect( pTables, SIGNAL(triggered()), SIGNAL(signalTables()) );

    pTableAttr = new QComboBox( this );
    pTableAttr->addItem( "" ); // blank needed because we can not select current so make current blank and User knows to select something else
    pTableAttr->addItem( QIcon(":DATAW/Column"), tr("Columns") );
    pTableAttr->addItem( QIcon(":DATAW/PrimaryKey"), tr("PrimaryKey") );
    pTableAttr->addItem( QIcon(":DATAW/ForeignKey"), tr("ForeignKeys") );
    pTableAttr->addItem( QIcon(":DATAW/Index"), tr("Indexs") );
    pTableAttr->addItem( QIcon(":DATAW/ID"), tr("Best Row ID") );
    pTableAttr->addItem( QIcon(":DATAW/ID"), tr("Row Version") );
    pTableAttr->setToolTip( tr("show table attributes for selected table\nmust be connected to data source") );
    addWidget( pTableAttr );
    connect( pTableAttr, SIGNAL(currentTextChanged(const QString &)), SIGNAL(signalTableAttr(const QString &)) );

    addSeparator();

    pResultFormat = new QComboBox( this );
    pResultFormat->setToolTip( tr("result format") );
    pResultFormat->addItem( tr("HTML Table"), (int)DATAResultSetWidget::ResultFormatHTMLGrid );
    pResultFormat->addItem( tr("Text Table"), (int)DATAResultSetWidget::ResultFormatTextGrid );
    pResultFormat->addItem( tr("Text Delimited"), (int)DATAResultSetWidget::ResultFormatTextDelimited );
    pResultFormat->addItem( tr("GUI Table"), (int)DATAResultSetWidget::ResultFormatGUIGrid );
    addWidget( pResultFormat );
    pResultFormat->setCurrentText( tr("GUI Table") );
    connect( pResultFormat, SIGNAL(currentTextChanged(const QString &)), SLOT(slotResultFormat(const QString &)) );

    pLimit = new QSpinBox( this );
    pLimit->setToolTip( tr("limit number of records/rows/tuples to return (-1 = unlimited)") );
    addWidget( pLimit );
    pLimit->setMinimum( -1 );
    pLimit->setValue( 5000 );
    connect( pLimit, SIGNAL(valueChanged(int)), SIGNAL(signalLimit(int)) );

    pHistoryClear = addAction( QIcon(":DATAW/Clear"), tr("Clear History") );
    pHistoryClear->setToolTip( tr("clear history") );
    connect( pHistoryClear, SIGNAL(triggered()), SIGNAL(signalHistoryClear()) );

    pHistoryMax = new QSpinBox( this );
    pHistoryMax->setToolTip( tr("maximum history entries") );
    addWidget( pHistoryMax );
    pHistoryMax->setMinimum( 0 );
    pHistoryMax->setMaximum( 100 );
    pHistoryMax->setValue( 10 );
    connect( pHistoryMax, SIGNAL(valueChanged(int)), SIGNAL(signalHistoryMax(int)) );

    pBatched = addAction( QIcon(":DATAW/BatchOff"), tr("Batch") );
    pBatched->setCheckable( true );
    pBatched->setToolTip(tr("submit multiple SQL statements to the server as one request"));
    connect( pBatched, SIGNAL(toggled(bool)), SLOT(slotBatched(bool)) );

    pExport = addAction( QIcon(":DATAW/Export"), tr("Export") );
    pExport->setToolTip( tr("export the result set") );
    connect( pExport, SIGNAL(triggered()), SIGNAL(signalExport()) );

    addSeparator();

    pFirstRowColumns = addAction( QIcon(":DATAW/ColumnNamesOff"), tr("Column Names") );
    pFirstRowColumns->setCheckable( true );
    pFirstRowColumns->setToolTip(tr("delimited: make 1st result row column names"));
    pFirstRowColumns->setChecked( false );
    connect( pFirstRowColumns, SIGNAL(toggled(bool)), SLOT(slotFirstRowColumnNames(bool)) );

    pDelimitChar = new QLineEdit( this );
    pDelimitChar->setToolTip( tr("delimit: delimiter for result columns") );
    pDelimitChar->setMaximumWidth( 50 );
    addWidget( pDelimitChar );
    pDelimitChar->setText( "|" );
    connect( pDelimitChar, SIGNAL(textChanged(const QString &)), this, SIGNAL(signalDelimitChar(const QString &)) );

    doSyncControls();
}

DATAWEditorToolBar::~DATAWEditorToolBar()
{
}

void DATAWEditorToolBar::setResultFormat( DATAResultSetWidget::ResultFormatTypes n )
{
    pResultFormat->setCurrentIndex( pResultFormat->findData( (int)n ) );
}

void DATAWEditorToolBar::setLimit( int n )
{
    pLimit->setValue( n );
}

void DATAWEditorToolBar::setBatched( bool b )
{
    pBatched->setChecked( b );
    if ( b )
    {
        pBatched->setToolTip(tr("click to submit multiple SQL statements one at a time"));
        pBatched->setIcon( QIcon(":DATAW/BatchOn") );
    }
    else
    {
        pBatched->setToolTip(tr("click to submit multiple SQL statements to the server as one request"));
        pBatched->setIcon( QIcon(":DATAW/BatchOff") );
    }
}

void DATAWEditorToolBar::setFirstRowColumnNames( bool b )
{
    pFirstRowColumns->setChecked( b );
    if ( b )
    {
        pFirstRowColumns->setToolTip(tr("click to hide column names\nNOTE: Applies to delimited results."));
        pFirstRowColumns->setIcon( QIcon(":DATAW/ColumnNamesOn") );
    }
    else
    {
        pFirstRowColumns->setToolTip(tr("click to show column names\nNOTE: Applies to delimited results."));
        pFirstRowColumns->setIcon( QIcon(":DATAW/ColumnNamesOff") );
    }
}

void DATAWEditorToolBar::setDelimitChar( const QString &s )
{
    pDelimitChar->setText( s );
}

void DATAWEditorToolBar::setHistoryMax( int n )
{
    pHistoryMax->setValue( n );
}

bool DATAWEditorToolBar::isBatched()
{
    return pBatched->isChecked();
}

DATAResultSetWidget::ResultFormatTypes DATAWEditorToolBar::getResultFormat()
{
    return (DATAResultSetWidget::ResultFormatTypes)pResultFormat->currentData().toInt();
}

int DATAWEditorToolBar::getLimit()
{
    return pLimit->value();
}

bool DATAWEditorToolBar::getFirstRowColumnNames()
{
    return pFirstRowColumns->isChecked();
}

QString DATAWEditorToolBar::getDelimitChar()
{
    return pDelimitChar->text();
}

void DATAWEditorToolBar::slotSelection( bool b )
{
    hasSelection = b;
    doSyncControls();
}

void DATAWEditorToolBar::slotChangedConnected()
{
    doSyncControls();
}

void DATAWEditorToolBar::slotBatched( bool b )
{
    if ( b )
    {
        pBatched->setToolTip(tr("click to submit multiple SQL statements one at a time"));
        pBatched->setIcon( QIcon(":DATAW/BatchOn") );
    }
    else
    {
        pBatched->setToolTip(tr("click to submit multiple SQL statements to the server as one request"));
        pBatched->setIcon( QIcon(":DATAW/BatchOff") );
    }
    emit signalBatched( b );
}

void DATAWEditorToolBar::slotFirstRowColumnNames( bool b )
{
    if ( b )
    {
        pFirstRowColumns->setToolTip(tr("click to hide column names\nNOTE: Applies to delimited results."));
        pFirstRowColumns->setIcon( QIcon(":DATAW/ColumnNamesOn") );
    }
    else
    {
        pFirstRowColumns->setToolTip(tr("click to show column names\nNOTE: Applies to delimited results."));
        pFirstRowColumns->setIcon( QIcon(":DATAW/ColumnNamesOff") );
    }
    emit signalFirstRowColumnNames( b );
}

void DATAWEditorToolBar::slotResultFormat( const QString &s )
{
    if ( s == tr("HTML Table") ) emit signalResultFormat( DATAResultSetWidget::ResultFormatHTMLGrid );
    else if ( s == tr("Text Table") )  emit signalResultFormat( DATAResultSetWidget::ResultFormatTextGrid );         
    else if ( s == tr("Text Delimited") )  emit signalResultFormat( DATAResultSetWidget::ResultFormatTextDelimited );
    else if ( s == tr("GUI Table") )  emit signalResultFormat( DATAResultSetWidget::ResultFormatGUIGrid );           
}

bool DATAWEditorToolBar::isConnected() 
{ 
    return pConnection ? pConnection->isConnected() : false; 
}

void DATAWEditorToolBar::doSyncControls()
{
    pSubmit->setEnabled( isConnected() );
    pTables->setEnabled( isConnected() );
    pSubmitSelected->setEnabled( isConnected() && hasSelection );
    pTableAttr->setEnabled( isConnected() && hasSelection );
}


