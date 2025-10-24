#include "LibInfo.h"
#include "DATAOutputDiagnosticsWidget.h"

DATAOutputDiagnosticsWidget::DATAOutputDiagnosticsWidget( QWidget *pParent )
    : QTreeWidget( pParent )
{
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );

    QStringList l;
    l << tr("Severity") << tr("Message");
    setColumnCount( 2 );
    setHeaderLabels( l );

    doLoadState();
}

DATAOutputDiagnosticsWidget::~DATAOutputDiagnosticsWidget()
{
    doSaveState();
}

void DATAOutputDiagnosticsWidget::slotMessage( DATAMessage *pMessage )
{
    QTreeWidgetItem *pItem = new QTreeWidgetItem();

    switch ( pMessage->getType() )
    {
        case DATAMessage::Success:
            pItem->setIcon( 0, QIcon( ":DATA/Success" ) );
            pItem->setText( 0, "SUCCESS" );
            break;
        case DATAMessage::Error:
            pItem->setIcon( 0, QIcon( ":DATA/Error" ) );
            pItem->setText( 0, "ERROR" );
            break;
        case DATAMessage::Warning:
            pItem->setIcon( 0, QIcon( ":DATA/Warning" ) );
            pItem->setText( 0, "WARNING" );
            break;
        case DATAMessage::Info:
            pItem->setIcon( 0, QIcon( ":DATA/Information" ) );
            pItem->setText( 0, "INFO" );
            break;
    }

    pItem->setText( 1, QString( "%1 %2 (%3)" ).arg( pMessage->getRequest() ).arg( pMessage->getText() ).arg( pMessage->getNumeric() ) );
//    pItem->setText( 1, QString( "Request: %1 Text: %2 Numeric: %3" ).arg( pMessage->getRequest() ).arg( pMessage->getText() ).arg( pMessage->getNumeric() ) );
    addTopLevelItem( pItem );

    scrollToItem( pItem /*, QAbstractItemView::PositionAtCenter */ );
}

void DATAOutputDiagnosticsWidget::slotDiagnostic( DATADiagnostic *pDiagnostic )
{
    QTreeWidgetItem *pItem;

    SQLRETURN nReturn = pDiagnostic->getReturncode();
    pItem = new QTreeWidgetItem();

    // try a guess at severity using return code...
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            pItem->setIcon( 0, QIcon( ":DATA/Success" ) );
            pItem->setText( 0, "SQL_SUCCESS" );
            break;
        case SQL_ERROR:
            pItem->setIcon( 0, QIcon( ":DATA/Error" ) );
            pItem->setText( 0, "SQL_ERROR" );
            break;
        case SQL_INVALID_HANDLE:
            pItem->setIcon( 0, QIcon( ":DATA/Error" ) );
            pItem->setText( 0, "SQL_INVALID_HANDLE" );
            break;
        case SQL_NEED_DATA:
            pItem->setIcon( 0, QIcon( ":DATA/Error" ) );
            pItem->setText( 0, "SQL_NEED_DATA" );
            break;
        case SQL_SUCCESS_WITH_INFO:
            pItem->setIcon( 0, QIcon( ":DATA/Information" ) );
            pItem->setText( 0, "SQL_SUCCESS_WITH_INFO" );
            break;
        case SQL_STILL_EXECUTING:
            pItem->setIcon( 0, QIcon( ":DATA/Warning" ) );
            pItem->setText( 0, "SQL_STILL_EXECUTING" );
            break;
        default:
            pItem->setIcon( 0, QIcon( ":DATA/Error" ) );
            pItem->setText( 0, QString::number( nReturn ) );
    }

    pItem->setText( 1, QString( "FUNCTION: %1 REQUEST: %2" ).arg( g_Constants->mapFunctionNames.value( pDiagnostic->getFunction() ) ).arg( pDiagnostic->getRequest() ) );
    addTopLevelItem( pItem );

    SQLINTEGER nRecords = pDiagnostic->getNumber();

    // RECORDS
    for ( SQLINTEGER n = 1; n <= nRecords; n++ ) 
    {
        DATADiagnosticRecord *p = pDiagnostic->getRecord( n );
        if ( !p ) break;
        QTreeWidgetItem *pItemRecord = new QTreeWidgetItem( pItem );
        pItemRecord->setText( 0, p->getSqlstate() );
        pItemRecord->setText( 1, p->getMessageText() );
    }

/*
    if ( !pItem->childCount() )                                                                                     
    {                                                                                                               
        if ( pDiagnostic->getRequest().left( 11 ) == "SQLGetInfo" )                                                 
        {                                                                                                           
            QTreeWidgetItem *pItemRecord = new QTreeWidgetItem( pItem );                                            
            pItemRecord->setText( 0, "HYC00" );                                                                     
            pItemRecord->setText( 1, tr( "No state provided so HYC00 assumed. Request not supported by driver." ) );
        }                                                                                                           
    }                                                                                                               
*/

    pItem->setExpanded( true );

    scrollToItem( pItem /*, QAbstractItemView::PositionAtCenter */ );
}

void DATAOutputDiagnosticsWidget::slotClear()
{
    clear();
}

void DATAOutputDiagnosticsWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", columnWidth( n ) );
    }
    settings.endArray();
}

void DATAOutputDiagnosticsWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        setColumnWidth( n, settings.value( "w", QString::number( columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

