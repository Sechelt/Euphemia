#include "LibInfo.h"
#include "AWOutputValidationWidget.h"

#include "AWObject.h"

AWOutputValidationWidget::AWOutputValidationWidget( QWidget *pParent )
    : QTableWidget( pParent )
{
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );

    QStringList l;
    l << tr("Severity") << tr("Message");
    setColumnCount( 2 );
    setHorizontalHeaderLabels( l );

    connect( this, SIGNAL(cellClicked(int,int)), SLOT(slotClicked(int,int)) );
    connect( this, SIGNAL(cellDoubleClicked(int,int)), SLOT(slotDoubleClicked(int,int)) );

    doLoadState();
}

AWOutputValidationWidget::~AWOutputValidationWidget()
{
    doSaveState();
}

void AWOutputValidationWidget::slotMessage( ADEventValidation *pEvent )
{
    ADObjectPathItem PathItem = pEvent->listObjectPath.last();
    int nRow = rowCount();

    setRowCount( nRow + 1 );

    QTableWidgetItem *pItem;
    setItem( nRow, 0, pItem = new QTableWidgetItem( ADValidation::getPixmapSeverity( pEvent->nSeverity ), "" ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setItem( nRow, 1, pItem = new QTableWidgetItem( pEvent->stringMessage ) );
    if ( PathItem.pobject && PathItem.pobject->inherits("AWObject") )
        pItem->setIcon( ((AWObject*)PathItem.pobject)->getIcon() );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    scrollToItem( pItem );

    vectorPaths.append( pEvent->listObjectPath );
}

void AWOutputValidationWidget::slotClear()
{
    setRowCount( 0 );
    vectorPaths.clear();
}

void AWOutputValidationWidget::doSaveState()
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

void AWOutputValidationWidget::doLoadState()
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

void AWOutputValidationWidget::slotClicked( int nRow, int nColumn )
{
    Q_UNUSED(nColumn);
    emit signalGoTo( vectorPaths[nRow] );
}

void AWOutputValidationWidget::slotDoubleClicked( int nRow, int nColumn )
{
    Q_UNUSED(nColumn);
    emit signalGoToView( vectorPaths[nRow] );
}

