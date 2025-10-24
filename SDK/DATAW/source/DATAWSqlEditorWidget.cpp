#include "LibInfo.h"
#include "DATAWSqlEditorWidget.h"

#include <WFindTextDialog.h>

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATASqlWidget.h>

#include <AWPalWidget.h>
#include <AWPanWidget.h>

#include "DATAWMeta.h"
#include "DATAWEditorToolBar.h"
#include "DATAWConnection.h"
#include "DATAWSql.h"

//
// DATAWSqlWidget
//
DATAWSqlWidget::DATAWSqlWidget( QWidget *pParent, const QStringList &listCompletionStrings )
: QWidget( pParent )
{
    setObjectName( metaObject()->className() );

    nHistoryMax     = 10; // should be same as default in tool bar
    nHistoryCurrent = 0;
    listHistory << ""; // place holder for current/last/editable

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        pSlider = new QSlider( Qt::Vertical, this );
        pSlider->setTickPosition( QSlider::TicksRight );
        pSlider->setTickInterval( 1 );
        pSlider->setMinimum( 0 );
        pSlider->setMaximum( 0 );
        pSlider->setEnabled( false );
        connect( pSlider, SIGNAL(valueChanged(int)), SLOT(slotHistoryGoTo(int)) );
        pLayout->addWidget( pSlider );

        pSqlWidget = new DATASqlWidget( this );
        new WSQL92Highlighter( pSqlWidget->document() );

/*
        // code completion
        // !!! THIS DOES NOT WORK (yet) !!!
        QCompleter *pCompleter = new QCompleter( pSqlWidget );

        // This loads string list from a resource text file (which is read like any text file).
        // The contents of the file would likley be a simple list of special words.
        // We do not do this here but it may be a useful way for the caller to pass a string list. 
        //    pCompleter->setModel( modelFromFile( ":/resources/wordlist.txt" ) );

        // Here we just pass a string list using QStringListModel because it is simple.
        // ie all of the object names, column names, etc in a DBMS.
        // This does not account for context so that could be a future improvement.
        pCompleter->setModelSorting( QCompleter::CaseInsensitivelySortedModel );
        pCompleter->setCaseSensitivity( Qt::CaseInsensitive );
        pCompleter->setWrapAround( false );
        pCompleter->setCompletionMode( QCompleter::PopupCompletion ); // default 
        pCompleter->setMaxVisibleItems( 5 );

        QStringListModel *pModel = new QStringListModel( listCompletionStrings, pCompleter );
        QColumnView *pView = new QColumnView( pSqlWidget );
        pView->setModel( pModel );

        pCompleter->setModel( pModel );
        pCompleter->setPopup( pView );
//        pCompleter->setCompletionMode( QCompleter::InlineCompletion );      
//        pSqlWidget->setCompleter( pCompleter );
*/
        pLayout->addWidget( pSqlWidget );

        pLayoutTop->addLayout( pLayout );
    }

    doInitStatusBar();
    pLayoutTop->addWidget( pStatusBar );

    connect( pSqlWidget, SIGNAL(signalChangedCut(bool)), SIGNAL(signalChangedCut(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedCopy(bool)), SIGNAL(signalChangedCopy(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedPaste(bool)), SIGNAL(signalChangedPaste(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedUndo(bool)), SIGNAL(signalChangedUndo(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedRedo(bool)), SIGNAL(signalChangedRedo(bool)) );
    connect( pSqlWidget, SIGNAL(signalSelection(bool)), SIGNAL(signalSelection(bool)) );
    connect( pSqlWidget, SIGNAL(signalCursorPosition(int,int)), SLOT(slotCursorPosition(int,int)) );
    connect( pSqlWidget, SIGNAL(signalModified(bool)), SLOT(slotModified(bool)) );
}

DATAWSqlWidget::~DATAWSqlWidget()
{
}

void DATAWSqlWidget::setModified( bool b )   
{ 
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
//    if ( b ) pLabelModified->setPixmap( QPixmap( ":DATAW/Modified" ) );
//    else pLabelModified->setPixmap( QPixmap() );


    // pSqlWidget->setModified( b ); 
}

bool DATAWSqlWidget::doFindFirst( const QString &stringFind, 
                                        bool bExp, 
                                        bool bCaseSensitive, 
                                        bool bWholeWord, 
                                        bool bWrap, 
                                        bool bForward, 
                                        int nLine, 
                                        int nIndex, 
                                        bool bShow, 
                                        bool bPosixExp, 
                                        bool bCxx11Exp )
{
    this->stringFind = stringFind; 
    return pSqlWidget->doFind( stringFind );
}

bool DATAWSqlWidget::doFindFirstInSelection( const QString &stringFind, 
                                                   bool bExp, 
                                                   bool bCaseSensitive, 
                                                   bool bWholeWord, 
                                                   bool bForward, 
                                                   bool bShow, 
                                                   bool bPosixExp, 
                                                   bool bCxx11Exp )
{
    this->stringFind = stringFind; 
    return pSqlWidget->doFind( stringFind );
}

void DATAWSqlWidget::doFindNext()
{
    pSqlWidget->doFind( stringFind );
}

void DATAWSqlWidget::doReplace( const QString &stringNew )
{
    pSqlWidget->doReplace( stringNew );
}

void DATAWSqlWidget::doReplaceAll( const QString &stringNew )
{
    pSqlWidget->doReplaceAll( stringNew );
}

void DATAWSqlWidget::doCancelFind()
{
}

void DATAWSqlWidget::doHistoryPush( const QString &s )
{
    int nLast = listHistory.count() - 1;

    if ( nLast >= nHistoryMax )
    { 
        if ( nHistoryMax < 1 ) return; // we are not maintaining history
        listHistory.removeAt( 0 );
        nLast--;
    }

    listHistory.insert( nLast, s );
    nHistoryCurrent = nLast + 1;
    pSlider->setMaximum( nHistoryCurrent );
    pSlider->setValue( nHistoryCurrent );

    pSlider->setEnabled( true );

    if ( nLast == 0 ) emit signalHasHistory( true ); 
}

void DATAWSqlWidget::doHistoryClear()
{
    int nLast = listHistory.count() - 1;

    if ( nHistoryCurrent == nLast )
    {
        listHistory.clear();
        listHistory << "";
    }
    else
    {
        QString s = listHistory.last();
        listHistory.clear();
        listHistory << "";
        pSqlWidget->setText( s );  
    }

    pSqlWidget->setReadOnly( false );
    
    nHistoryCurrent = 0;    
    pSlider->setMinimum( 0 );
    pSlider->setMaximum( 0 );
    // pSlider->setValue( 1 );
    pSlider->setEnabled( false );    

    emit signalHasHistory( false ); 
}

void DATAWSqlWidget::slotCut()
{
    doCut();
}

void DATAWSqlWidget::slotCopy()
{
    doCopy();
}

void DATAWSqlWidget::slotPaste()
{
    doPaste();
}

void DATAWSqlWidget::slotDelete()
{
    doDelete();
}

void DATAWSqlWidget::slotSelectAll()
{
    doSelectAll();
}

void DATAWSqlWidget::slotSelectNone()
{
    doSelectNone();
}

void DATAWSqlWidget::slotUndo()
{
    doUndo();
}

void DATAWSqlWidget::slotRedo()
{
    doRedo();
}

void DATAWSqlWidget::doInitStatusBar()
{
    pStatusBar = new QStatusBar( this );
    QLabel *pLabel;                                     

    pLabel = new QLabel( tr( "Line:" ), pStatusBar );         
    pLabelLine = new QLabel( pStatusBar );                    
    pLabelLine->setMinimumWidth( 50 );                  
    pLabelLine->setToolTip( tr("current line") );
    pStatusBar->addPermanentWidget( pLabel );                       
    pStatusBar->addPermanentWidget( pLabelLine );                   

    pLabel = new QLabel( tr( "Column:" ), pStatusBar );       
    pLabelColumn = new QLabel( pStatusBar );                  
    pLabelColumn->setMinimumWidth( 50 );                
    pLabelColumn->setToolTip( tr("current column") );
    pStatusBar->addPermanentWidget( pLabel );                       
    pStatusBar->addPermanentWidget( pLabelColumn );                 

    pLabelModified = new QLabel( pStatusBar );                 
    pLabelModified->setMinimumWidth( 20 );               
    pLabelModified->setToolTip( tr("modification status") );
    pStatusBar->addPermanentWidget( pLabelModified );           
}

void DATAWSqlWidget::slotHistoryGoTo( int nRequested )
{
    if ( nRequested == nHistoryCurrent ) return;

    int nLast = listHistory.count() - 1;
    if ( nHistoryCurrent == nLast ) listHistory.replace( nHistoryCurrent, pSqlWidget->getText() );

    nHistoryCurrent = nRequested;
    pSqlWidget->setText( listHistory.at( nHistoryCurrent ) );

    if ( nHistoryCurrent == nLast ) pSqlWidget->setReadOnly( false );
    else pSqlWidget->setReadOnly( true );;
}

void DATAWSqlWidget::slotCursorPosition( int nLine, int nColumn )
{
    pLabelLine->setText( QString::number( nLine ) );
    pLabelColumn->setText( QString::number( nColumn ) );
}

void DATAWSqlWidget::slotModified( bool b )
{
    if ( b ) pLabelModified->setPixmap( QPixmap( ":DATAW/Modified" ) );
    else pLabelModified->setPixmap( QPixmap() );
    emit signalModified( b );
}

//
// DATAWResultWidget
//
DATAWResultWidget::DATAWResultWidget( QWidget *pParent )
: QMainWindow( pParent )
{
    setObjectName( metaObject()->className() );

    pWidgetCentral = new QWidget( this );

    QVBoxLayout *pLayout = new QVBoxLayout( pWidgetCentral );

    pResultSetWidget = new DATAResultSetWidget( pWidgetCentral );
    pLayout->addWidget( pResultSetWidget );

    doInitStatusBar();
    // pLayout->addWidget( pStatusBar );

    setCentralWidget( pWidgetCentral );
}

DATAWResultWidget::~DATAWResultWidget()
{
}

void DATAWResultWidget::doResult( DATAStatement *pStatement )
{
    doClear(); // in case we are reused
    pResultSetWidget->doResult( pStatement );
    pLabelRows->setText( QString::number( pResultSetWidget->getRows() ) );  
    pLabelRowsAffected->setText( QString::number( pStatement->getRowCount() ) );    
    pLabelSeconds->setText( QString::number( pStatement->getElapsedSeconds() ) );   
}

void DATAWResultWidget::doInitStatusBar()
{
    pStatusBar = statusBar();
    // pStatusBar = new QStatusBar( pWidgetCentral );
    QLabel *pLabel;                                     

    pLabel = new QLabel( tr( "Rows:" ), pStatusBar );         
    pLabelRows = new QLabel( pStatusBar );                    
    pLabelRows->setMinimumWidth( 50 );                  
    pLabelRows->setToolTip( tr("number of rows in the result set") );
    pStatusBar->addPermanentWidget( pLabel );                       
    pStatusBar->addPermanentWidget( pLabelRows );                   

    pLabel = new QLabel( tr( "Rows Affected:" ), pStatusBar );
    pLabelRowsAffected = new QLabel( pStatusBar );            
    pLabelRowsAffected->setMinimumWidth( 50 );          
    pLabelRowsAffected->setToolTip( tr("number of rows affected by request") );
    pStatusBar->addPermanentWidget( pLabel );                       
    pStatusBar->addPermanentWidget( pLabelRowsAffected );           

    pLabel = new QLabel( tr( "Seconds:" ), pStatusBar );      
    pLabelSeconds = new QLabel( pStatusBar );                 
    pLabelSeconds->setMinimumWidth( 50 );               
    pLabelSeconds->setToolTip( tr("number of seconds to complete request") );
    pStatusBar->addPermanentWidget( pLabel );                       
    pStatusBar->addPermanentWidget( pLabelSeconds );           
}

void DATAWResultWidget::doClear()
{
    pLabelRows->clear();
    pLabelRowsAffected->clear();
    pLabelSeconds->clear();
}

//
// DATAWSqlEditorWidget - main widget here
//
DATAWSqlEditorWidget::DATAWSqlEditorWidget( DATAWSql *pSql, QWidget *pParent )
: AWEditorWidget( pSql, pParent )
{
    Q_ASSERT( pSql );
    setObjectName( metaObject()->className() );

    pConnection = pSql->getConnection();
    Q_ASSERT( pConnection );

    // these get saved/loaded with the SQL
    stringDelimitChar = '|';
    bBatched = false;            
    nResultFormat = DATAResultSetWidget::ResultFormatGUIGrid;       
    bFirstRowColumnNames = true;
    nHistoryMax = 5;

    pFindTextDialog = new WFindTextDialog( this );
    connect( pFindTextDialog, SIGNAL(signalFindFirst(const QString &, bool, bool, bool)), SLOT(slotFindFirst(const QString &, bool, bool, bool)) );
    connect( pFindTextDialog, SIGNAL(signalFindNext()), SLOT(slotFindNext()) );
    connect( pFindTextDialog, SIGNAL(signalFindPrevious()), SLOT(slotFindPrevious()) );
    connect( pFindTextDialog, SIGNAL(signalReplace( const QString & )), SLOT(slotReplace( const QString & )) );
    connect( pFindTextDialog, SIGNAL(signalReplaceAll( const QString & )), SLOT(slotReplaceAll( const QString & )) );
    connect( pFindTextDialog, SIGNAL(signalFindTextChanged()), SLOT(slotFindTextChanged()) );
    connect( pFindTextDialog, SIGNAL(finished(int)), SLOT(slotFindFinished(int)) );

    // central widget
    pSplitter = new QSplitter( Qt::Vertical, this );

    // Editor widget with completion strings etc.
    // We can get completion strings from; DBMS (if connected) or ERD (if exists).
    // Here we get them from the DBMS only.
    if ( pSql->getConnection()->isConnected() )
    {
        DATAWMeta meta;
        if ( !meta.doLoad( pSql->getConnection()->getConnection() ) )
            printf( "[%s][%s][%d] WARNING: Had issues with loading meta info for table.\n", __FILE__, __FUNCTION__, __LINE__ );
        QStringList listStrings = meta.getCompletionStrings(); 

        // remove empty values
        // remove dups
        QStringList listClean;
        QList<QString>::const_iterator constIt;
        for (constIt = listStrings.cbegin(); constIt != listStrings.cend(); ++constIt) 
        {
            QString s = *constIt;
            if ( s.isEmpty() ) continue;
            if ( listClean.contains( s ) ) continue;
            listClean << s;
        }
        pSqlWidget = new DATAWSqlWidget( pSplitter, listClean );
    }
    else
        pSqlWidget = new DATAWSqlWidget( pSplitter, QStringList() );

    pResultsTabWidget   = new QTabWidget( pSplitter );
    pSplitter->addWidget( pSqlWidget );
    pSplitter->addWidget( pResultsTabWidget );

    setCentralWidget( pSplitter );

    connect( pSqlWidget, SIGNAL(signalChangedCut(bool)), SLOT(slotChangedCut(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedCopy(bool)), SLOT(slotChangedCopy(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedPaste(bool)), SLOT(slotChangedPaste(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedUndo(bool)), SLOT(slotChangedUndo(bool)) );
    connect( pSqlWidget, SIGNAL(signalChangedRedo(bool)), SLOT(slotChangedRedo(bool)) );
    connect( pSqlWidget, SIGNAL(signalSelection(bool)), SLOT(slotChangedSelection(bool)) );

    connect( pSqlWidget, SIGNAL(signalModified(bool)), SIGNAL(signalModified(bool)) );
    connect( pSqlWidget, SIGNAL(signalCursorPosition(int,int)), SIGNAL(signalCursorPosition(int,int)) );
    connect( pSqlWidget, SIGNAL(signalHasHistory(bool)), SIGNAL(signalHasHistory(bool)) );

    // init tool bar AFTER widgets created
    doInitToolBar();

    //
    doLoadState();
}

DATAWSqlEditorWidget::~DATAWSqlEditorWidget()
{
    doSaveState();
}

AWPalWidget *DATAWSqlEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new AWPalWidget( this, nullptr );
    return pPalWidget;
}

AWPanWidget *DATAWSqlEditorWidget::getPanWidget()
{
    if ( pPanWidget ) return pPanWidget;
    pPanWidget = new AWPanWidget( this, nullptr );
    return pPanWidget;
}

void DATAWSqlEditorWidget::doSubmit()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "Submit" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "Submit" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }
    doSubmit( getSQL() );
}

void DATAWSqlEditorWidget::doSubmitSelected()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "Submit Selected" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "Submit Selected" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }
    doSubmit( getSQLSelected() );
}

void DATAWSqlEditorWidget::doTables()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "Tables" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "Tables" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";

    doClear();

    DATAStatement *pStatement = pConnection->getConnection()->getTables( stringCatalog, stringSchema, "TABLE" );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( "" );                                                
}

void DATAWSqlEditorWidget::doColumns()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "Columns" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "Columns" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";
    QString stringTable     = getSQLSelected();

    doClear();

    if ( stringTable.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "Columns" ), tr( "No table selected. Highlight/select a table in the SQL." ) );
        return;
    }

    DATAStatement *pStatement = pConnection->getConnection()->getColumns( stringCatalog, stringSchema, stringTable, "%" );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( stringTable );                                                
}

void DATAWSqlEditorWidget::doPrimaryKey()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "PrimaryKeys" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "PrimaryKeys" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";
    QString stringTable     = getSQLSelected();

    doClear();

    if ( stringTable.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "PrimaryKeys" ), tr( "No table selected. Highlight/select a table in the SQL." ) );
        return;
    }

    DATAStatement *pStatement = pConnection->getConnection()->getPrimaryKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( stringTable );                                                
}

void DATAWSqlEditorWidget::doForeignKeys()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "ForeignKeys" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "ForeignKeys" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";
    QString stringTable     = getSQLSelected();

    doClear();

    if ( stringTable.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "ForeignKeys" ), tr( "No table selected. Highlight/select a table in the SQL." ) );
        return;
    }

    DATAStatement *pStatement = pConnection->getConnection()->getForeignKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( stringTable );                                                
}

void DATAWSqlEditorWidget::doIndexs()
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "Indexs" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "Indexs" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";
    QString stringTable     = getSQLSelected();

    doClear();

    if ( stringTable.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "Indexs" ), tr( "No table selected. Highlight/select a table in the SQL." ) );
        return;
    }

    DATAStatement *pStatement = pConnection->getConnection()->getIndexs( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( stringTable );                                                
}

void DATAWSqlEditorWidget::doSpecialColumns( SQLSMALLINT n )
{
    if ( !pConnection )
    {
        QMessageBox::warning( this, tr( "SpecialColumns" ), tr( "No connection. Select a connection." ) );
        return;
    }
    if ( !pConnection->isConnected() ) 
    {
        QMessageBox::warning( this, tr( "SpecialColumns" ), tr( "Disconnected. The selected connection is not in a connected state." ) );
        return;
    }

    // we could get these from the Profile but the current could have been changed via sql so request latest
    QString stringCatalog   = pConnection->getConnection()->getAttr( SQL_ATTR_CURRENT_CATALOG ).toString();
    QString stringSchema    = "%";
    QString stringTable     = getSQLSelected();

    doClear();

    if ( stringTable.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "SpecialColumns" ), tr( "No table selected. Highlight/select a table in the SQL." ) );
        return;
    }

    DATAStatement *pStatement = pConnection->getConnection()->getSpecialColumns( stringCatalog, stringSchema, stringTable, n );
    if ( !pStatement ) return;                                                      

    DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
    pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
    pResultWidget->getResultSetWidget()->setLimit( nLimit );
    pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
    pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
    pResultsTabWidget->addTab( pResultWidget, "1" );
    pResultWidget->doResult( pStatement );                             

    delete pStatement;                                                              
    pSqlWidget->doHistoryPush( stringTable );                                                
}

bool DATAWSqlEditorWidget::doFindFirst( const QString &stringFind, 
                                        bool bExp, 
                                        bool bCaseSensitive, 
                                        bool bWholeWord, 
                                        bool bWrap, 
                                        bool bForward, 
                                        int nLine, 
                                        int nIndex, 
                                        bool bShow, 
                                        bool bPosixExp, 
                                        bool bCxx11Exp )
{
    return pSqlWidget->doFindFirst( stringFind, bExp, bCaseSensitive, bWholeWord, bWrap, bForward, nLine, nIndex, bShow, bPosixExp, bCxx11Exp );
}

bool DATAWSqlEditorWidget::doFindFirstInSelection( const QString &stringFind, 
                                                   bool bExp, 
                                                   bool bCaseSensitive, 
                                                   bool bWholeWord, 
                                                   bool bForward, 
                                                   bool bShow, 
                                                   bool bPosixExp, 
                                                   bool bCxx11Exp )
{
    return pSqlWidget->doFindFirstInSelection( stringFind, bExp, bCaseSensitive, bWholeWord, bForward, bShow, bPosixExp, bCxx11Exp );
}

void DATAWSqlEditorWidget::doFindNext()
{
    pSqlWidget->doFindNext();
}

void DATAWSqlEditorWidget::doReplace( const QString &stringNew )
{
    pSqlWidget->doReplace( stringNew );
}

void DATAWSqlEditorWidget::doReplaceAll( const QString &stringNew )
{
    pSqlWidget->doReplaceAll( stringNew );
}

void DATAWSqlEditorWidget::doCancelFind()
{
    if ( !pFindTextDialog->isVisible() ) return;
    pFindTextDialog->doClear();
    pFindTextDialog->hide(); // does NOT trigger QDialog::finished
}

bool DATAWSqlEditorWidget::isFind()       
{ 
    return pFindTextDialog->isVisible(); 
}

void DATAWSqlEditorWidget::slotChangedCut( bool b )
{
    pCut->setEnabled( b );
    emit signalChangedCut( b );
}

void DATAWSqlEditorWidget::slotChangedCopy( bool b )
{
    pCopy->setEnabled( b );
    emit signalChangedCopy( b );
}

void DATAWSqlEditorWidget::slotChangedPaste( bool b )
{
    pPaste->setEnabled( b );
    emit signalChangedPaste( b );
}

void DATAWSqlEditorWidget::slotChangedUndo( bool b )
{
    pUndo->setEnabled( b );
    emit signalChangedUndo( b );
}

void DATAWSqlEditorWidget::slotChangedRedo( bool b )
{
    pRedo->setEnabled( b );
    emit signalChangedRedo( b );
}

void DATAWSqlEditorWidget::slotChangedSelection( bool b )
{
    pDelete->setEnabled( b );
    emit signalSelection( b );
}

void DATAWSqlEditorWidget::slotSubmit()
{
    DATAWSql *p = (DATAWSql*)pObject;
    p->doPreSubmit();
    doSubmit();
}

void DATAWSqlEditorWidget::slotSubmitSelected()
{
    DATAWSql *p = (DATAWSql*)pObject;
    p->doPreSubmit();
    doSubmitSelected();
}

void DATAWSqlEditorWidget::slotTables()
{
    DATAWSql *p = (DATAWSql*)pObject;
    p->doPreSubmit();
    doTables();
}

void DATAWSqlEditorWidget::slotTableAttr( const QString &s )
{
    DATAWSql *p = (DATAWSql*)pObject;
    p->doPreSubmit();

    if ( s.isEmpty() ) return;

    if ( s == tr("Columns") )
    {
        doColumns();
    }
    else if ( s == tr("PrimaryKey") )
    {
        doPrimaryKey();
    }
    else if ( s == tr("ForeignKeys") )
    {
        doForeignKeys();
    }
    else if ( s == tr("Indexs") )
    {
        doIndexs();
    }
    else if ( s == tr("Best Row ID") )
    {
        doSpecialColumns( SQL_BEST_ROWID );
    }
    else if ( s == tr("Row Version") )
    {
        doSpecialColumns( SQL_ROWVER );
    }
    else
        printf( "[%s][%s][%d] Unhandled request %s\n", __FILE__, __FUNCTION__, __LINE__, s.toUtf8().constData() );
}

void DATAWSqlEditorWidget::slotBatched( bool b )
{
    setBatched( b );
}

void DATAWSqlEditorWidget::slotHistoryClear()
{
    pSqlWidget->doHistoryClear();
}

void DATAWSqlEditorWidget::slotHistoryMax( int n )
{
    pSqlWidget->setHistoryMax( n );
}

// export results
void DATAWSqlEditorWidget::slotExport()
{
    DATAWResultWidget *p1 = (DATAWResultWidget*)(pResultsTabWidget->currentWidget());
    if ( !p1 )
    {
        qWarning( "[PAH][%s][%s][%d] No result-set.\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    DATAResultSetWidget *p2 = p1->getResultSetWidget();
    if ( !p2 )
    {
        qWarning( "[PAH][%s][%s][%d] No result-set.\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    QString stringFileName = QFileDialog::getSaveFileName( this, tr( "Export..." ) );
    if ( stringFileName.isEmpty() ) return;                                          
            
    p2->doSave( stringFileName );
}

void DATAWSqlEditorWidget::slotResultFormat( DATAResultSetWidget::ResultFormatTypes n )
{
    setResultFormat( n );
}

void DATAWSqlEditorWidget::slotLimit( int n )
{
    setLimit( n );
}

void DATAWSqlEditorWidget::slotFirstRowColumnNames( bool b )
{
    setFirstRowColumnNames( b );
}

void DATAWSqlEditorWidget::slotDelimitChar( const QString &s )
{
    setDelimitChar( s );
}

void DATAWSqlEditorWidget::slotCut()
{
    doCut();
}

void DATAWSqlEditorWidget::slotCopy()
{
    doCopy();
}

void DATAWSqlEditorWidget::slotPaste()
{
    doPaste();
}

void DATAWSqlEditorWidget::slotDelete()
{
    doDelete();
}

void DATAWSqlEditorWidget::slotSelectAll()
{
    doSelectAll();
}

void DATAWSqlEditorWidget::slotSelectNone()
{
    doSelectNone();
}

void DATAWSqlEditorWidget::slotUndo()
{
    doUndo();
}

void DATAWSqlEditorWidget::slotRedo()
{
    doRedo();
}

void DATAWSqlEditorWidget::slotFind()
{
    pFindTextDialog->show();
}

QStringList DATAWSqlEditorWidget::getSQLStatements( const QString &stringSQL )
{
    QStringList l;

    QString stringStatement = "";
    int nChars              = stringSQL.length();
    int bInSingleQuote      = false;
    int bInDoubleQuote      = false;
    int bInComment          = false;
    int bInLineComment      = false;    // starts with '#' or "--" as first char(s) in line and ends with EOL

    for ( int nChar = 0; nChar < nChars; nChar++ )
    {
        if ( bInSingleQuote )
        {
            // looking for "'"
            if ( stringSQL[nChar] == '\'' )
            {
                if ( !stringStatement.isEmpty() && stringStatement.right( 1 ) != '\\' ) bInSingleQuote = false;
            }
            stringStatement += stringSQL[nChar];
            continue;
        }
        else if ( bInDoubleQuote )
        { 
            // looking for '"'
            if ( stringSQL[nChar] == '"' )
            {
                if ( !stringStatement.isEmpty() && stringStatement.right( 1 ) != '\\' ) bInDoubleQuote = false;
            }
            stringStatement += stringSQL[nChar];
            continue;
        }
        else if ( bInComment )
        { 
            // looking for "*/"
            if ( stringSQL[nChar] == '*' && ((nChar+1 < nChars) && stringSQL[nChar+1] == '/' ) ) 
            {
                bInComment = false;
                stringStatement += stringSQL[nChar];
                nChar++;
                stringStatement += stringSQL[nChar];
            }
            else stringStatement += stringSQL[nChar];
            continue;
        }
        else if ( bInLineComment )
        { 
            // looking for EOL
            if ( stringSQL[nChar] == '\n' ) bInLineComment = false;
            stringStatement += stringSQL[nChar];
            continue;
        }

        // is first char of line?
        if ( stringStatement.isEmpty() || stringStatement.right( 1 ) == '\n' )
        { 
            // is "--" line-comment?
            if (stringSQL[nChar] == '-' && ((nChar+1 < nChars) && stringSQL[nChar+1] == '-' ) )
            {
                stringStatement += stringSQL[nChar];
                nChar++;
                stringStatement += stringSQL[nChar];
                bInLineComment = true;
                continue;
            }
            // is '#' line-comment?
            if ( stringSQL[nChar] == '#' )
            {
                stringStatement += stringSQL[nChar];
                bInLineComment = true;
                continue;
            }
        }

        // going into "/*" comment?
        if ( stringSQL[nChar] == '/' && ((nChar+1 < nChars) && stringSQL[nChar+1] == '*' ) )
        {
            stringStatement += stringSQL[nChar];
            nChar++;
            stringStatement += stringSQL[nChar];
            bInComment = true;
            continue;
        }

        // going into single-quote string?
        if ( stringSQL[nChar] == '\'' )
        { 
            stringStatement += stringSQL[nChar];
            bInSingleQuote = true;
            continue;
        }

        // going into double-quote string?
        if ( stringSQL[nChar] == '"' )
        { 
            stringStatement += stringSQL[nChar];
            bInDoubleQuote = true;
            continue;
        }

        // end of SQL statement?
        if ( stringSQL[nChar] == ';' )
        {
            l << stringStatement;
            stringStatement = "";
            continue;
        }

        // no special condition - just add the char
        stringStatement += stringSQL[nChar];
    }

    if ( !stringStatement.trimmed().isEmpty() ) l << stringStatement;

    return l;
}

void DATAWSqlEditorWidget::doInitToolBar()
{
    pToolBarEditor = new DATAWEditorToolBar( pConnection, nullptr );
    addToolBar( pToolBarEditor );

    pToolBarEditor->setResultFormat( nResultFormat );
    pToolBarEditor->setLimit( nLimit );
    pToolBarEditor->setBatched( bBatched );
    pToolBarEditor->setFirstRowColumnNames( bFirstRowColumnNames );
    pToolBarEditor->setDelimitChar( stringDelimitChar );
    pToolBarEditor->setHistoryMax( nHistoryMax );
    pSqlWidget->setHistoryMax( nHistoryMax );

    // allow controls to request change in document
    connect( pToolBarEditor, SIGNAL(signalSubmit()),                        this, SLOT(slotSubmit()) );
    connect( pToolBarEditor, SIGNAL(signalSubmitSelected()),                this, SLOT(slotSubmitSelected()) );
    connect( pToolBarEditor, SIGNAL(signalTables()),                        this, SLOT(slotTables()) );
    connect( pToolBarEditor, SIGNAL(signalTableAttr(const QString &)),      this, SLOT(slotTableAttr(const QString &)) );
    connect( pToolBarEditor, SIGNAL(signalBatched(bool)),                   this, SLOT(slotBatched(bool)) );
    connect( pToolBarEditor, SIGNAL(signalHistoryClear()),                  this, SLOT(slotHistoryClear()) );
    connect( pToolBarEditor, SIGNAL(signalHistoryMax(int)),                 this, SLOT(slotHistoryMax(int)) );
    connect( pToolBarEditor, SIGNAL(signalExport()),                        this, SLOT(slotExport()) );
    connect( pToolBarEditor, SIGNAL(signalResultFormat(DATAResultSetWidget::ResultFormatTypes)), this, SLOT(slotResultFormat(DATAResultSetWidget::ResultFormatTypes)) );
    connect( pToolBarEditor, SIGNAL(signalLimit(int)),                      this, SLOT(slotLimit(int)) );
    connect( pToolBarEditor, SIGNAL(signalFirstRowColumnNames(bool)),       this, SLOT(slotFirstRowColumnNames(bool)) );
    connect( pToolBarEditor, SIGNAL(signalDelimitChar(const QString &)),    this, SLOT(slotDelimitChar(const QString &)) );

    connect( this, SIGNAL(signalSelection(bool)),           pToolBarEditor,     SLOT(slotSelection(bool)) );

    // add edit toolbar
    QToolBar *pToolBar = addToolBar( tr("Edit") );

    pCut = pToolBar->addAction( QIcon(":DATAW/Cut"), tr("Cut") );
    pCut->setToolTip( tr("cut") );
    pCut->setEnabled( pSqlWidget->canCut() );
    connect( pCut, SIGNAL(triggered()), SLOT(slotCut()) );

    pCopy = pToolBar->addAction( QIcon(":DATAW/Copy"), tr("Copy") );
    pCopy->setToolTip( tr("copy") );
    pCopy->setEnabled( pSqlWidget->canCopy() );
    connect( pCopy, SIGNAL(triggered()), SLOT(slotCopy()) );

    pPaste = pToolBar->addAction( QIcon(":DATAW/Paste"), tr("Paste") );
    pPaste->setToolTip( tr("paste") );
    pPaste->setEnabled( pSqlWidget->canPaste() );
    connect( pPaste, SIGNAL(triggered()), SLOT(slotPaste()) );

    pDelete = pToolBar->addAction( QIcon(":DATAW/Delete"), tr("Delete") );
    pDelete->setToolTip( tr("delete") );
    pDelete->setEnabled( pSqlWidget->hasSelection() /* pSqlWidget->canDelete() */ );
    connect( pDelete, SIGNAL(triggered()), SLOT(slotDelete()) );

    pSelectAll = pToolBar->addAction( QIcon(":DATAW/SelectAll"), tr("Select All") );
    pSelectAll->setToolTip( tr("select all") );
    connect( pSelectAll, SIGNAL(triggered()), SLOT(slotSelectAll()) );

    pSelectNone = pToolBar->addAction( QIcon(":DATAW/SelectNone"), tr("Select None") );
    pSelectNone->setToolTip( tr("select none") );
    connect( pSelectNone, SIGNAL(triggered()), SLOT(slotSelectNone()) );

    pUndo = pToolBar->addAction( QIcon(":DATAW/Undo"), tr("Undo") );
    pUndo->setToolTip( tr("undo") );
    // always enabled as QsciScintilla does not let us know if state changed 
    // pUndo->setEnabled( pSqlWidget->canUndo() );
    connect( pUndo, SIGNAL(triggered()), SLOT(slotUndo()) );

    pRedo = pToolBar->addAction( QIcon(":DATAW/Redo"), tr("Redo") );
    pRedo->setToolTip( tr("redo") );
    // always enabled as QsciScintilla does not let us know if state changed 
    // pRedo->setEnabled( pSqlWidget->canRedo() );
    connect( pRedo, SIGNAL(triggered()), SLOT(slotRedo()) );

    pFind = pToolBar->addAction( QIcon(":DATAW/Find"), tr("Find") );
    pFind->setToolTip( tr("find") );
    connect( pFind, SIGNAL(triggered()), SLOT(slotFind()) );
}

void DATAWSqlEditorWidget::doSubmit( const QString &stringIn )
{
    QString stringSQL = stringIn;
    // Switch unicode nulls '\u0000' to space. This happens when text from QPlainTextEdit::toPlainText. 
    stringSQL.replace( '\u0000', ' ' ); 
    // Switch unicode paragraph '\u2029' to new line. This happens when text from QTextCursor::selectedText.
    // Note: We avoid this by calling QTextCursor::selection().toPlainText.
    // stringSQL.replace(  QChar::ParagraphSeparator, '\n' ); 

    doClear();

    if ( stringSQL.trimmed().isEmpty() ) 
    {
        QMessageBox::warning( this, tr( "Submit" ), tr( "No SQL. Enter one or more SQL statements." ) );
        return;
    }

    if ( bBatched )                                                  
    {                                                                                   
        DATAStatement *pStatement = pConnection->getConnection()->getExecute( stringSQL );               
        if ( !pStatement ) return;                                                      

        DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
        pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
        pResultWidget->getResultSetWidget()->setLimit( nLimit );
        pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
        pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
        pResultsTabWidget->addTab( pResultWidget, "1" );
        pResultWidget->doResult( pStatement );                             
                                                                                        
        delete pStatement;                                                              
        pSqlWidget->doHistoryPush( stringSQL );                                                
    }                                                                                   
    else
    {                                                                                   
        uint    nRows           = 0;                                                    
        SQLLEN  nRowsAffected   = 0;                                                    
        double  nSeconds        = 0;                                                    
        QString stringTried;                                                            
        QStringList l = getSQLStatements( stringSQL );                                  
        QString s;
        int nResult = 1;                                                                      
        foreach( s, l )                                                                 
        {                                                                               
            stringTried += s;                                                           
            DATAStatement *pStatement = pConnection->getConnection()->getExecute( s );                   
            if ( !pStatement ) break;                                                   

            DATAWResultWidget *pResultWidget = new DATAWResultWidget( pResultsTabWidget );
            pResultWidget->getResultSetWidget()->setResultFormat( nResultFormat );
            pResultWidget->getResultSetWidget()->setLimit( nLimit );
            pResultWidget->getResultSetWidget()->setFirstRowColumnNames( bFirstRowColumnNames );
            pResultWidget->getResultSetWidget()->setDelimitChar( stringDelimitChar );
            pResultsTabWidget->addTab( pResultWidget, QString::number( nResult ) );
            pResultWidget->doResult( pStatement );                             

            nRows += pResultWidget->getResultSetWidget()->getRows();                               
            nRowsAffected += pStatement->getRowCount();                                 
            nSeconds += pStatement->getElapsedSeconds();                                

            delete pStatement;
            nResult++;                                                          
        }                                                                               
        pSqlWidget->doHistoryPush( stringTried );                                              
    }                                                                                   
}

void DATAWSqlEditorWidget::doClear()
{
    while ( pResultsTabWidget->count() )
    {
        delete pResultsTabWidget->widget( 0 );
    }
}

void DATAWSqlEditorWidget::doSaveState()
{
    QSettings settings;
    QString stringClass = metaObject()->className();
    settings.setValue( stringClass + "/SplitterSizes", pSplitter->saveState() );
}

void DATAWSqlEditorWidget::doLoadState()
{
    QSettings settings;
    QString stringClass = metaObject()->className();
    pSplitter->restoreState( settings.value( stringClass + "/SplitterSizes").toByteArray() );
}

void DATAWSqlEditorWidget::slotFindFirst( const QString &s, bool bExp, bool bCaseSensitive, bool bWholeWord )
{
    bool bWrap = true;

    doFindFirst( s, bExp, bCaseSensitive, bWholeWord, bWrap );
}

void DATAWSqlEditorWidget::slotFindNext()
{
    doFindNext();
}

void DATAWSqlEditorWidget::slotFindPrevious()
{
}

void DATAWSqlEditorWidget::slotReplace( const QString &s )
{
    doReplace( s );
}

void DATAWSqlEditorWidget::slotReplaceAll( const QString &s )
{
    doReplaceAll( s );
}

void DATAWSqlEditorWidget::slotFindTextChanged()
{
    // leave dialog alone but cancel find in editor
    if ( pFindTextDialog->isFinding() ) pSqlWidget->doCancelFind();
}

void DATAWSqlEditorWidget::slotFindFinished( int )
{
    if ( pFindTextDialog->isFinding() ) pSqlWidget->doCancelFind();
    pFindTextDialog->doClear();
}


