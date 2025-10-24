#include "LibInfo.h"
#include "DATAFunctionsWidget.h"

#include "DATAProfiles.h"
#include "DATAProfile.h"

DATAFunctionsWidget::DATAFunctionsWidget( DATAProfile *pProfile, QWidget *pParent )
    : QTreeWidget( pParent )
{
    this->pProfile = pProfile;
                                                                                                                                        
    QStringList l;
    l << tr("Function") << tr("Supported") << tr("Compliance");
    setColumnCount( 3 );
    setHeaderLabels( l );
    setAllColumnsShowFocus( true );
    setSelectionMode( QAbstractItemView::SingleSelection );

    if ( pProfile )
    {
        QMapIterator<int,DATAFunction*> i( pProfile->mapFunctions );
        while ( i.hasNext() ) 
        {
            i.next();

            QTreeWidgetItem *pItem = new QTreeWidgetItem( this );

            pItem->setText( 0, i.value()->stringFunction );
            pItem->setText( 1, i.value()->bSupported ? "SQL_TRUE" : "SQL_FALSE" );
            switch ( i.value()->nCompliance )
            {
            case CBD::StandardISO92:
                pItem->setText( 2, "ISO92" );
                break;
            case CBD::StandardODBC:
                pItem->setText( 2, "ODBC" );
                break;
            case CBD::StandardOpenGroup:
                pItem->setText( 2, "OpenGroup" );
                break;
            }
            doUnsupported( i.value(), pItem );
        }
    }

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotContextMenu(const QPoint &)) );

    doLoadState();
}

DATAFunctionsWidget::~DATAFunctionsWidget()
{
    doSaveState();
}

void DATAFunctionsWidget::doUnsupported( DATAFunction *pFunction, QTreeWidgetItem *pItemParent )
{
    // int nRow = 0;
    DATAUnsupported d;
    foreach( d, pFunction->listUnsupported )
    {
        QTreeWidgetItem *pItem = new QTreeWidgetItem( pItemParent );
        pItem->setText( 0, d.stringContext );
    }
}

void DATAFunctionsWidget::slotContextMenu( const QPoint &point )
{
    QMenu *pMenu = new QMenu( this );

    pMenu->addAction( QPixmap( ":W/copy" ), tr("Copy"), this, SLOT(slotCopy()) );

    pMenu->exec( mapToGlobal( point ) );

    delete pMenu;
}

void DATAFunctionsWidget::slotCopy()
{
    if ( !pProfile ) return;

    QString s;
    QMapIterator<int,DATAFunction*> i( pProfile->mapFunctions );
    while ( i.hasNext() ) 
    {
        i.next();

        s += i.value()->stringFunction;
        s += ",";
        s += ( i.value()->bSupported ? "SQL_TRUE" : "SQL_FALSE" );
        switch ( i.value()->nCompliance )
        {
        case CBD::StandardISO92:
            s += ",ISO92";
            break;
        case CBD::StandardODBC:
            s += ",ODBC";
            break;
        case CBD::StandardOpenGroup:
            s += ",OpenGroup";
            break;
        }
        s += "\n";
    }

    QClipboard *pClipboard = QGuiApplication::clipboard();
    pClipboard->setText( s );
}

void DATAFunctionsWidget::doSaveState()
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

void DATAFunctionsWidget::doLoadState()
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


