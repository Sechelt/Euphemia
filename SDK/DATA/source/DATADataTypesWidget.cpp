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

    if ( pProfile )
    {
        int nRow = 0;
        foreach(DATADataTypeSpec * p, pProfile->vectorDataTypes )
        {
            setRowCount( nRow + 1 );
            setItem( nRow, 0, new QTableWidgetItem( p->TYPE_NAME ) );
            setItem( nRow, 1, new QTableWidgetItem( p->DATA_TYPE ) );
            setItem( nRow, 2, new QTableWidgetItem( p->COLUMN_SIZE ) );
            setItem( nRow, 3, new QTableWidgetItem( p->LITERAL_PREFIX ) );
            setItem( nRow, 4, new QTableWidgetItem( p->LITERAL_SUFFIX ) );
            setItem( nRow, 5, new QTableWidgetItem( p->CREATE_PARAMS ) );
            setItem( nRow, 6, new QTableWidgetItem( p->NULLABLE ) );
            setItem( nRow, 7, new QTableWidgetItem( p->CASE_SENSITIVE ) );
            setItem( nRow, 8, new QTableWidgetItem( p->SEARCHABLE ) );
            setItem( nRow, 9, new QTableWidgetItem( p->UNSIGNED_ATTRIBUTE ) );
            setItem( nRow, 10, new QTableWidgetItem( p->FIXED_PREC_SCALE ) );
            setItem( nRow, 11, new QTableWidgetItem( p->AUTO_UNIQUE_VALUE ) );
            setItem( nRow, 12, new QTableWidgetItem( p->LOCAL_TYPE_NAME ) );
            setItem( nRow, 13, new QTableWidgetItem( p->MINIMUM_SCALE ) );
            setItem( nRow, 14, new QTableWidgetItem( p->MAXIMUM_SCALE ) );
            setItem( nRow, 15, new QTableWidgetItem( p->SQL_DATA_TYPE ) );
            setItem( nRow, 16, new QTableWidgetItem( p->SQL_DATETIME_SUB ) );
            setItem( nRow, 17, new QTableWidgetItem( p->NUM_PREC_RADIX ) );
            setItem( nRow, 18, new QTableWidgetItem( p->INTERVAL_PRECISION ) );
            setItem( nRow, 19, new QTableWidgetItem( p->stringDescription ) );
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

    foreach( DATADataTypeSpec *p, pProfile->vectorDataTypes )
    {
        s += "{\n";
        s += "DATADataTypeSpec d;\n";
        s += "d.TYPE_NAME           = \"" + p->TYPE_NAME + "\";\n";
        s += "d.nDATA_TYPE          = \"" + QString::number( p->nDATA_TYPE ) + "\";\n";
        s += "d.DATA_TYPE           = \"" + p->DATA_TYPE + "\";\n";
        s += "d.COLUMN_SIZE         = \"" + p->COLUMN_SIZE + "\";\n";
        s += "d.LITERAL_PREFIX      = \"" + p->LITERAL_PREFIX + "\";\n";
        s += "d.LITERAL_SUFFIX      = \"" + p->LITERAL_SUFFIX + "\";\n";
        s += "d.CREATE_PARAMS       = \"" + p->CREATE_PARAMS + "\";\n";
        s += "d.NULLABLE            = \"" + p->NULLABLE + "\";\n";
        s += "d.CASE_SENSITIVE      = \"" + p->CASE_SENSITIVE + "\";\n";
        s += "d.SEARCHABLE          = \"" + p->SEARCHABLE + "\";\n";
        s += "d.UNSIGNED_ATTRIBUTE  = \"" + p->UNSIGNED_ATTRIBUTE + "\";\n";
        s += "d.FIXED_PREC_SCALE    = \"" + p->FIXED_PREC_SCALE + "\";\n";
        s += "d.AUTO_UNIQUE_VALUE   = \"" + p->AUTO_UNIQUE_VALUE + "\";\n";
        s += "d.LOCAL_TYPE_NAME     = \"" + p->LOCAL_TYPE_NAME + "\";\n";
        s += "d.MINIMUM_SCALE       = \"" + p->MINIMUM_SCALE + "\";\n";
        s += "d.MAXIMUM_SCALE       = \"" + p->MAXIMUM_SCALE + "\";\n";
        s += "d.nSQL_DATA_TYPE      = \"" + QString::number( p->nSQL_DATA_TYPE ) + "\";\n";
        s += "d.SQL_DATA_TYPE       = \"" + p->SQL_DATA_TYPE + "\";\n";
        s += "d.SQL_DATETIME_SUB    = \"" + p->SQL_DATETIME_SUB + "\";\n";
        s += "d.NUM_PREC_RADIX      = \"" + p->NUM_PREC_RADIX + "\";\n";
        s += "d.INTERVAL_PRECISION  = \"" + p->INTERVAL_PRECISION + "\";\n";
        s += "d.stringDescription   = \"" + p->stringDescription + "\";\n";
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

