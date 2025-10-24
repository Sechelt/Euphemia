#include "LibInfo.h"
#include "DATAOutputMessagesWidget.h"

DATAOutputMessagesWidget::DATAOutputMessagesWidget( QWidget *pParent )
    : QTableWidget( pParent )
{
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );

    QStringList l;
    l << tr("Severity") << tr("Code") << tr("Request") << tr("Message");
    setColumnCount( 4 );
    setHorizontalHeaderLabels( l );

    doLoadState();
}

DATAOutputMessagesWidget::~DATAOutputMessagesWidget()
{
    doSaveState();
}

void DATAOutputMessagesWidget::slotMessage( DATAMessage *pMessage )
{
    if ( pMessage->isNull() ) return;

    int nRow = rowCount();

    setRowCount( nRow + 1 );

    QTableWidgetItem *pItem = new QTableWidgetItem( pMessage->getTypeText() );
    
    switch ( pMessage->getType() )
    {
        case DATAMessage::Success:
            pItem->setIcon( QIcon( ":DATA/Success" ) );
            break;
        case DATAMessage::Info:
            pItem->setIcon( QIcon( ":DATA/Information" ) );
            break;
        case DATAMessage::Warning:
            pItem->setIcon( QIcon( ":DATA/Warning" ) );
            break;
        case DATAMessage::Error:
            pItem->setIcon( QIcon( ":DATA/Error" ) );
            break;
    }

    setItem( nRow, 0, pItem );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setItem( nRow, 1, pItem = new QTableWidgetItem( pMessage->getNumeric() ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setItem( nRow, 2, pItem = new QTableWidgetItem( pMessage->getRequest() ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setItem( nRow, 3, pItem = new QTableWidgetItem( pMessage->getText() ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    scrollToItem( pItem /*, QAbstractItemView::PositionAtCenter */ );
}

void DATAOutputMessagesWidget::slotClear()
{
    setRowCount( 0 );
}

void DATAOutputMessagesWidget::doSaveState()
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

void DATAOutputMessagesWidget::doLoadState()
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

