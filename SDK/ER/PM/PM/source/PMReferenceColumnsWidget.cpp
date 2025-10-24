#include "LibInfo.h"
#include "PMReferenceColumnsWidget.h"

#include "PMTableBase.h"
#include "PMColumn.h"
#include "PMReferenceColumn.h"

//
// PMReferenceColumnWidget
//
PMReferenceColumnWidget::PMReferenceColumnWidget( PMReferenceColumn *pReferenceColumn, PMTableBase *pChildTable, QWidget *pwidgetParent )
: QComboBox( pwidgetParent )
{
    this->pReferenceColumn = pReferenceColumn;

    if ( pChildTable )
    { 
        QList<PMColumn *> listChildColumns = pChildTable->getColumns();

        PMColumn *pChildColumn;
        foreach( pChildColumn, listChildColumns )
        {
            addItem( pChildColumn->getName(), qlonglong(pChildColumn) );
        }
        addItem( "" );

        PMColumn *pForeignKeyColumn = pReferenceColumn->getChildColumn();
        if ( pForeignKeyColumn ) setCurrentIndex( findData( QVariant( qlonglong(pForeignKeyColumn) ) ) );
        else setCurrentText( "" );

        connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );
    }
}

void PMReferenceColumnWidget::slotChanged( int nIndex )
{
    PMColumn *pColumn = nullptr;

    QVariant v = itemData( nIndex );
    if ( !v.isNull() ) pColumn = (PMColumn*)v.toLongLong();

    pReferenceColumn->setChildColumn( pColumn );
}

//
// PMReferenceColumnsWidget
//
PMReferenceColumnsWidget::PMReferenceColumnsWidget( PMReference *pReference, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pReference = pReference;

    PMKey *pKey = getKey();
    QVBoxLayout *pLayout = new QVBoxLayout( this );

    // columns
    {
        doInit();
        pLayout->addWidget( pTableWidget );
    }

    if ( pKey ) 
        doLoad();
    else
        pTableWidget->setDisabled( true );

    doLoadState();
}

PMReferenceColumnsWidget::~PMReferenceColumnsWidget()
{
    doSaveState();
}

void PMReferenceColumnsWidget::setKey( PMKey *pKey )
{
    doReInit();
    pReference->setKey( pKey );
    doLoad();
}

void PMReferenceColumnsWidget::doInit()
{
    pTableWidget = new QTableWidget( this );
    pTableWidget->setToolTip( tr("PK/AK Column and corresponding| FK Column") );
    pTableWidget->setColumnCount( 2 );           
    pTableWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( QIcon( ":PM/PrimaryKey" ), tr( "Primary/Alternate Key" ) ) );
    pTableWidget->setHorizontalHeaderItem( 1, new QTableWidgetItem( QIcon( ":PM/ForeignKey" ), tr( "Foreign Key" ) ) );
}

/*!
 * \brief Init (or re-init) columns table. Load if data available.
 *  
 * Logic elsewhere, hopefully, ensures that we are in sync with any pk/ak we may be associated with. 
 *  
 * \sa PMReference 
 *  
 * \author pharvey (5/22/20)
 * 
 * \param p 
 */
void PMReferenceColumnsWidget::doLoad()
{
    PMKey *      pKey           = pReference->getKey();
    PMTableBase *pTableParent   = pReference->getParentTableObject();
    PMTableBase *pTableChild    = pReference->getChildTableObject();

    if ( pTableParent && !pTableParent->getName().isEmpty() ) 
        pTableWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( QIcon( ":PM/PrimaryKey" ), pTableParent->getName() ) );

    if ( pTableChild && !pTableChild->getName().isEmpty() )
        pTableWidget->setHorizontalHeaderItem( 1, new QTableWidgetItem( QIcon( ":PM/ForeignKey" ), pTableChild->getName() ) );

    if ( !pKey ) return;

    Q_ASSERT( pTableParent );

    // provide a combobox for each column in the key
    int              nRows  = 0;
    int              nRow   = -1;
    QList<PMColumn*> l      = pKey->getColumns();
    PMColumn *       pColumn;

    foreach( pColumn, l )
    {
        nRows++; nRow++;
        // get PMReferenceColumn (create one if we must)
        PMReferenceColumn *pReferenceColumn = pReference->getReferenceColumn( pColumn, true );
        //
        pTableWidget->setRowCount( nRows );
        // parent - show primary/alternate key column
        pTableWidget->setItem( nRow, 0, new QTableWidgetItem( pColumn->getName() ) );
        // child - select foreign key column
        pTableWidget->setCellWidget( nRow, 1, new PMReferenceColumnWidget( pReferenceColumn, pTableChild, this ) );
    }
}

void PMReferenceColumnsWidget::doReInit()
{
    // table has taken ownership of widgets
    // do clear table of widgets and rows etc
    pTableWidget->setRowCount( 0 );
    pTableWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( QIcon( ":PM/PrimaryKey" ), tr( "Primary/Alternate Key" ) ) );
    pTableWidget->setHorizontalHeaderItem( 1, new QTableWidgetItem( QIcon( ":PM/ForeignKey" ), tr( "Foreign Key" ) ) );
}

void PMReferenceColumnsWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < pTableWidget->columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", pTableWidget->columnWidth( n ) );
    }
    settings.endArray();
}

void PMReferenceColumnsWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        pTableWidget->setColumnWidth( n, settings.value( "w", QString::number( pTableWidget->columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}


