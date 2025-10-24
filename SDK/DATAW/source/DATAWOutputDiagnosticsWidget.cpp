#include "LibInfo.h"
#include "DATAWOutputDiagnosticsWidget.h"

DATAWOutputDiagnosticsWidget::DATAWOutputDiagnosticsWidget( QWidget *pParent )
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

DATAWOutputDiagnosticsWidget::~DATAWOutputDiagnosticsWidget()
{
    doSaveState();
}

void DATAWOutputDiagnosticsWidget::slotMessage( ADDataEventDiagnostic *pEvent )
{
    QTreeWidgetItem *pItem;

    SQLRETURN nReturn = pEvent->pDiagnostic->getReturncode();
    pItem = new QTreeWidgetItem();

    // try a guess at severity using return code...
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            pItem->setIcon( 0, QIcon( ":W/Information16x16" ) );
            pItem->setText( 0, "SQL_SUCCESS" );
            break;
        case SQL_ERROR:
            pItem->setIcon( 0, QIcon( ":W/Error16x16" ) );
            pItem->setText( 0, "SQL_ERROR" );
            break;
        case SQL_INVALID_HANDLE:
            pItem->setIcon( 0, QIcon( ":W/Error16x16" ) );
            pItem->setText( 0, "SQL_INVALID_HANDLE" );
            break;
        case SQL_NEED_DATA:
            pItem->setIcon( 0, QIcon( ":W/Error16x16" ) );
            pItem->setText( 0, "SQL_NEED_DATA" );
            break;
        case SQL_SUCCESS_WITH_INFO:
            pItem->setIcon( 0, QIcon( ":W/Warning16x16" ) );
            pItem->setText( 0, "SQL_SUCCESS_WITH_INFO" );
            break;
        case SQL_STILL_EXECUTING:
            pItem->setIcon( 0, QIcon( ":W/Warning16x16" ) );
            pItem->setText( 0, "SQL_STILL_EXECUTING" );
            break;
        default:
            pItem->setIcon( 0, QIcon( ":W/Error16x16" ) );
            pItem->setText( 0, QString::number( nReturn ) );
    }

    pItem->setText( 1, QString( "FUNCTION: %1 REQUEST: %2" ).arg( g_Constants->mapFunctionNames.value( pEvent->pDiagnostic->getFunction() ) ).arg( pEvent->pDiagnostic->getRequest() ) );
    //     pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    addTopLevelItem( pItem );

    // RECORDS
    for ( int n = 1; n <= 25; n++ ) 
    {
        DATADiagnosticRecord *p = pEvent->pDiagnostic->getRecord( n );
        if ( !p ) break;
        QTreeWidgetItem *pItemRecord = new QTreeWidgetItem( pItem );
        pItemRecord->setText( 0, p->getSqlstate() );
        pItemRecord->setText( 1, p->getMessageText() );
    }

    pItem->setExpanded( true );

//    scrollTDATAWQuery( pItem /*, QAbstractItemView::PositionAtCenter */ );
    vectorPaths.append( pEvent->listObjectPath );
}

void DATAWOutputDiagnosticsWidget::slotClear()
{
    clear();
    vectorPaths.clear();
}

void DATAWOutputDiagnosticsWidget::doSaveState()
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

void DATAWOutputDiagnosticsWidget::doLoadState()
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

