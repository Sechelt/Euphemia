#include "LibInfo.h"
#include "AWOutputMessagesWidget.h"

#include "AWObject.h"

AWOutputMessagesWidget::AWOutputMessagesWidget( QWidget *pParent )
    : QTableWidget( pParent )
{
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );

    QStringList l;
    l << tr("Severity") << tr("Message");
    setColumnCount( 2 );
    setHorizontalHeaderLabels( l );

    doLoadState();
}

AWOutputMessagesWidget::~AWOutputMessagesWidget()
{
    doSaveState();
}

void AWOutputMessagesWidget::slotMessage( ADEventGeneral *pEvent )
{
    int nRow = rowCount();

    setRowCount( nRow + 1 );

    QTableWidgetItem *pItem = new QTableWidgetItem( pEvent->stringSeverity );
    if ( pEvent->stringSeverity == "INFO" ) pItem->setIcon( QIcon( ":W/Information16x16" ) );
    else if ( pEvent->stringSeverity == "WARNING" ) pItem->setIcon( QIcon( ":W/Warning16x16" ) );
    else if ( pEvent->stringSeverity == "ERROR" ) pItem->setIcon( QIcon( ":W/Error16x16" ) );

    setItem( nRow, 0, pItem );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setItem( nRow, 1, pItem = new QTableWidgetItem( pEvent->stringMessage ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    scrollToItem( pItem /*, QAbstractItemView::PositionAtCenter */ );

    vectorPaths.append( pEvent->listObjectPath );
}

void AWOutputMessagesWidget::slotClear()
{
    setRowCount( 0 );
    vectorPaths.clear();
}

void AWOutputMessagesWidget::doSaveState()
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

void AWOutputMessagesWidget::doLoadState()
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

