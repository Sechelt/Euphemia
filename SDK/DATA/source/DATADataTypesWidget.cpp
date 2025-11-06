#include "LibInfo.h"
#include "DATADataTypesWidget.h"

#include "DATAProfiles.h"
#include "DATAProfile.h"

#include "DATADataType.h"

DATADataTypesWidget::DATADataTypesWidget( DATAProfile *pProfile, QWidget *pParent )
    : QTableWidget( pParent )
{
    this->pProfile = pProfile;
                                                                                                                                        
    QStringList l;
    l << tr("TYPE_NAME") << tr("DATA_TYPE") << tr("COLUMN_SIZE") << tr("LITERAL_PREFIX") << tr("LITERAL_SUFFIX");
    l << tr("CREATE_PARAMS") << tr("NULLABLE") << tr("CASE_SENSITIVE") << tr("SEARCHABLE") << tr("UNSIGNED_ATTRIBUTE");
    l << tr("FIXED_PREC_SCALE") << tr("AUTO_UNIQUE_VALUE") << tr("LOCAL_TYPE_NAME") << tr("MINIMUM_SCALE") << tr("MAXIMUM_SCALE");
    l << tr("SQL_DATA_TYPE") << tr("SQL_DATETIME_SUB") << tr("NUM_PREC_RADIX") << tr("INTERVAL_PRECISION") << tr("Description");

    setColumnCount( 20 );

    setHorizontalHeaderLabels( l );
    setRowCount( 0 );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::NoSelection );    

    // COMPLETE THIS AFTER SWITCH DATA TO ALL QString (so its more readable)
    if ( pProfile )
    {
        QMapIterator<QString,DATADataTypeSpec*> i( pProfile->mapDataTypes );
        int nRow = 0;
        while ( i.hasNext() ) 
        {
            i.next();

            setRowCount( nRow + 1 );
            setItem( nRow, 0, new QTableWidgetItem( i.value()->TYPE_NAME ) );
            setItem( nRow, 1, new QTableWidgetItem( i.value()->DATA_TYPE ) );
            setItem( nRow, 2, new QTableWidgetItem( i.value()->COLUMN_SIZE ) );
            setItem( nRow, 3, new QTableWidgetItem( i.value()->LITERAL_PREFIX ) );
            setItem( nRow, 4, new QTableWidgetItem( i.value()->LITERAL_SUFFIX ) );
            setItem( nRow, 5, new QTableWidgetItem( i.value()->CREATE_PARAMS ) );
            setItem( nRow, 6, new QTableWidgetItem( i.value()->NULLABLE ) );
            setItem( nRow, 7, new QTableWidgetItem( i.value()->CASE_SENSITIVE ) );
            setItem( nRow, 8, new QTableWidgetItem( i.value()->SEARCHABLE ) );
            setItem( nRow, 9, new QTableWidgetItem( i.value()->UNSIGNED_ATTRIBUTE ) );
            setItem( nRow, 10, new QTableWidgetItem( i.value()->FIXED_PREC_SCALE ) );
            setItem( nRow, 11, new QTableWidgetItem( i.value()->AUTO_UNIQUE_VALUE ) );
            setItem( nRow, 12, new QTableWidgetItem( i.value()->LOCAL_TYPE_NAME ) );
            setItem( nRow, 13, new QTableWidgetItem( i.value()->MINIMUM_SCALE ) );
            setItem( nRow, 14, new QTableWidgetItem( i.value()->MAXIMUM_SCALE ) );
            setItem( nRow, 15, new QTableWidgetItem( i.value()->SQL_DATA_TYPE ) );
            setItem( nRow, 16, new QTableWidgetItem( i.value()->SQL_DATETIME_SUB ) );
            setItem( nRow, 17, new QTableWidgetItem( i.value()->NUM_PREC_RADIX ) );
            setItem( nRow, 18, new QTableWidgetItem( i.value()->INTERVAL_PRECISION ) );
            setItem( nRow, 19, new QTableWidgetItem( i.value()->stringDescription ) );
            nRow++;
        }
    }

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotContextMenu(const QPoint &)) );

    doLoadState();
}

DATADataTypesWidget::~DATADataTypesWidget()
{
    doSaveState();
}

void DATADataTypesWidget::slotContextMenu( const QPoint &point )
{
    QMenu *pMenu = new QMenu( this );

    pMenu->addAction( QPixmap( ":W/copy" ), tr("Copy"), this, SLOT(slotCopy()) );

    pMenu->exec( mapToGlobal( point ) );

    delete pMenu;
}

void DATADataTypesWidget::slotCopy()
{
    if ( !pProfile ) return;

    QString s;
    QMapIterator<QString,DATADataTypeSpec*> i( pProfile->mapDataTypes );
    while ( i.hasNext() ) 
    {
        i.next();
        s += "{\n";
        s += "DATADataTypeSpec d;\n";
        s += "d.TYPE_NAME           = \"" + i.value()->TYPE_NAME + "\";\n";
        s += "d.nDATA_TYPE          = \"" + QString::number( i.value()->nDATA_TYPE ) + "\";\n";
        s += "d.DATA_TYPE           = \"" + i.value()->DATA_TYPE + "\";\n";
        s += "d.COLUMN_SIZE         = \"" + i.value()->COLUMN_SIZE + "\";\n";
        s += "d.LITERAL_PREFIX      = \"" + i.value()->LITERAL_PREFIX + "\";\n";
        s += "d.LITERAL_SUFFIX      = \"" + i.value()->LITERAL_SUFFIX + "\";\n";
        s += "d.CREATE_PARAMS       = \"" + i.value()->CREATE_PARAMS + "\";\n";
        s += "d.NULLABLE            = \"" + i.value()->NULLABLE + "\";\n";
        s += "d.CASE_SENSITIVE      = \"" + i.value()->CASE_SENSITIVE + "\";\n";
        s += "d.SEARCHABLE          = \"" + i.value()->SEARCHABLE + "\";\n";
        s += "d.UNSIGNED_ATTRIBUTE  = \"" + i.value()->UNSIGNED_ATTRIBUTE + "\";\n";
        s += "d.FIXED_PREC_SCALE    = \"" + i.value()->FIXED_PREC_SCALE + "\";\n";
        s += "d.AUTO_UNIQUE_VALUE   = \"" + i.value()->AUTO_UNIQUE_VALUE + "\";\n";
        s += "d.LOCAL_TYPE_NAME     = \"" + i.value()->LOCAL_TYPE_NAME + "\";\n";
        s += "d.MINIMUM_SCALE       = \"" + i.value()->MINIMUM_SCALE + "\";\n";
        s += "d.MAXIMUM_SCALE       = \"" + i.value()->MAXIMUM_SCALE + "\";\n";
        s += "d.SQL_DATA_TYPE       = \"" + i.value()->SQL_DATA_TYPE + "\";\n";
        s += "d.SQL_DATETIME_SUB    = \"" + i.value()->SQL_DATETIME_SUB + "\";\n";
        s += "d.NUM_PREC_RADIX      = \"" + i.value()->NUM_PREC_RADIX + "\";\n";
        s += "d.INTERVAL_PRECISION  = \"" + i.value()->INTERVAL_PRECISION + "\";\n";
        s += "d.stringDescription   = \"" + i.value()->stringDescription + "\";\n";
        s += "d.vectorSyntax        = DATADataTypeSpec::getSyntaxVector( d.TYPE_NAME );\n";
        s += "mapDataTypes[d.TYPE_NAME] = d;\n";
        s += "}\n";
    }

    QClipboard *pClipboard = QGuiApplication::clipboard();
    pClipboard->setText( s );
}

void DATADataTypesWidget::doSaveState()
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

void DATADataTypesWidget::doLoadState()
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

