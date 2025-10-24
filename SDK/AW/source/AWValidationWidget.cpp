#include "LibInfo.h"
#include "AWValidationWidget.h"

//
//
//
AWValidationItem::AWValidationItem( const QVariant &v, AWValidationItem *p )
{
    variantColumn0  = v;
    pParent         = p;
}

AWValidationItem::~AWValidationItem()
{
    qDeleteAll( listChildren );
}

void AWValidationItem::appendChild( AWValidationItem *item )
{
    listChildren.append( item );
}

AWValidationItem* AWValidationItem::child( int row )
{
    return listChildren.value( row );
}

int AWValidationItem::childCount() const
{
    return listChildren.count();
}

int AWValidationItem::columnCount() const
{
    return 2;
}

QVariant AWValidationItem::data( int column ) const
{
    switch ( column )
    { 
        case 0:
            return variantColumn0;
    }
    return QVariant();
}

AWValidationItem* AWValidationItem::parentItem()
{
    return pParent;
}

int AWValidationItem::row() const
{
    if ( pParent ) return pParent->listChildren.indexOf( const_cast<AWValidationItem *>( this ) );

    return 0;
}
 
//
//
//
AWValidationItemRoot::AWValidationItemRoot()
: AWValidationItem(  QObject::tr("Check") )
{
}

QVariant AWValidationItemRoot::data( int column ) const
{
    switch ( column )
    {
        case 0:
            return QObject::tr( "Check" );
        case 1:
            return QObject::tr( "Severity" );
    }
    return QVariant();
}

//
//
//
AWValidationItemSeverity::AWValidationItemSeverity( ADValidationDatabase *pData, const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringCheck, AWValidationItem *pParent )
: AWValidationItem(  stringCheck, pParent )
{
    this->pData         = pData;
    this->stringClass   = stringClass;
    this->nStrictness   = nStrictness;
    this->stringCheck   = stringCheck;
    this->pParent       = pParent;
}

QVariant AWValidationItemSeverity::data( int column ) const
{
    switch ( column )
    { 
        case 0:
            return stringCheck;
        case 1:
            return ADValidation::getSeverity( (*pData)[stringClass][nStrictness][stringCheck] );
    }
    return QVariant();
}

void AWValidationItemSeverity::setData( int n ) 
{
    (*pData)[stringClass][nStrictness][stringCheck] = (ADValidation::Severity)n;
}

//
//
//
AWValidationModel::AWValidationModel( ADValidationDatabase *pData, QObject *parent )
    : QAbstractItemModel( parent )
{
    pRoot = new AWValidationItemRoot();
    setupModelData( pData, pRoot );
}

AWValidationModel::~AWValidationModel()
{
    delete pRoot;
}

QModelIndex AWValidationModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( !hasIndex( row, column, parent ) ) return QModelIndex();

    AWValidationItem *parentItem;

    if ( !parent.isValid() ) parentItem = pRoot;
    else parentItem = static_cast<AWValidationItem *>( parent.internalPointer() );

    AWValidationItem *childItem = parentItem->child( row );
    if ( childItem ) return createIndex( row, column, childItem );
    else return QModelIndex();
}

QModelIndex AWValidationModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) return QModelIndex();

    AWValidationItem *childItem = static_cast<AWValidationItem *>( index.internalPointer() );
    AWValidationItem *parentItem = childItem->parentItem();

    if ( parentItem == pRoot ) return QModelIndex();

    return createIndex( parentItem->row(), 0, parentItem );
}

int AWValidationModel::rowCount( const QModelIndex &parent ) const
{
    AWValidationItem *parentItem;
    if ( parent.column() > 0 ) return 0;

    if ( !parent.isValid() ) parentItem = pRoot;
    else parentItem = static_cast<AWValidationItem *>( parent.internalPointer() );

    return parentItem->childCount();
}


int AWValidationModel::columnCount( const QModelIndex &parent ) const
{
    if ( parent.isValid() ) return static_cast<AWValidationItem *>( parent.internalPointer() )->columnCount();
    else return pRoot->columnCount();
}


QVariant AWValidationModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() ) return QVariant();

    if ( role != Qt::DisplayRole ) return QVariant();

    AWValidationItem *item = static_cast<AWValidationItem *>( index.internalPointer() );

    return item->data( index.column() );
}

Qt::ItemFlags AWValidationModel::flags( const QModelIndex &index ) const
{
// #if QT_VERSION < 0x060000
//    if ( !index.isValid() ) return 0;
// #else
    if ( !index.isValid() ) return (Qt::ItemFlag)0;
// #endif

    if ( index.column() == 1 ) return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable;

    return QAbstractItemModel::flags( index );
}

QVariant AWValidationModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) return pRoot->data( section );

    return QVariant();
}

void AWValidationModel::setupModelData( ADValidationDatabase *pData, AWValidationItem *pRoot ) 
{
    QList<QString> listClasses = pData->keys();
    QString stringClass;
    foreach( stringClass, listClasses )
    {
        AWValidationItem *pClass = new AWValidationItem( stringClass, pRoot );
        pRoot->appendChild( pClass );

        QList<ADValidation::Strictness> listStrictness = (*pData)[stringClass].keys();
        ADValidation::Strictness nStrictness;
        foreach( nStrictness, listStrictness )
        {
            AWValidationItem *pStrictness = new AWValidationItem( ADValidation::getStrictness( nStrictness ), pClass );
            pClass->appendChild( pStrictness );

            QList<QString> listChecks = (*pData)[stringClass][nStrictness].keys();
            QString stringCheck;
            foreach( stringCheck, listChecks )
            {
                pStrictness->appendChild( new AWValidationItemSeverity( pData, stringClass, nStrictness, stringCheck, pStrictness ) );
            }
        }
    }
}

bool AWValidationModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( index.isValid() && index.column() == 1 && role == Qt::EditRole )
    {
        AWValidationItemSeverity *p = static_cast<AWValidationItemSeverity *>( index.internalPointer() );
        p->setData( value.toInt() );

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

//
//
//
AWValidationSeverityDelegate::AWValidationSeverityDelegate( QObject *parent )
    : QStyledItemDelegate( parent )
{
}

QWidget* AWValidationSeverityDelegate::createEditor( QWidget *parent, 
                                        const QStyleOptionViewItem &/* option */,
                                        const QModelIndex &/* index */ ) const
{
    return ADValidation::getEditorSeverity( parent );
}

void AWValidationSeverityDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
    int value = index.model()->data( index, Qt::EditRole ).toInt();

    QComboBox *pComboBox = static_cast<QComboBox *>( editor );
    pComboBox->setCurrentIndex( value );
}

void AWValidationSeverityDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    QComboBox *pComboBox = static_cast<QComboBox *>( editor );
    model->setData( index, pComboBox->currentIndex(), Qt::EditRole );
}

void AWValidationSeverityDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */ ) const
{
    editor->setGeometry( option.rect );
}
 
//
//
//
AWValidationWidget::AWValidationWidget( ADObject *pObject, ADValidationDatabase *pData, ADValidation::Strictness nStrictness, QWidget *pWidgetParent )
: QWidget( pWidgetParent )
{
    this->pObject   = pObject;
    this->pData     = pData;
    pStrictness     = ADValidation::getEditorStrictness( this, nStrictness );
    pModel          = new AWValidationModel( pData, this );
    pView           = new QTreeView( this );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    {
        QHBoxLayout *pLayout01 = new QHBoxLayout();
        pLayout01->addWidget( new QLabel( tr("Desired Strictness During Validation:"), this ) );
        pLayout01->addWidget( pStrictness );
        pLayout01->addStretch( 10 );
        pLayout->addLayout( pLayout01 );
    }
    pLayout->addWidget( pView );
    pView->setModel( pModel );

    pDelegate = new AWValidationSeverityDelegate( this );
    pView->setItemDelegate( pDelegate );

    connect( pStrictness, SIGNAL(currentTextChanged(const QString&)), SLOT(slotStrictness(const QString&)) );
    connect( pModel, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(slotChanged(const QModelIndex &,const QModelIndex &)) );

    doLoadState();
}

AWValidationWidget::~AWValidationWidget()
{
    doSaveState();
}

void AWValidationWidget::slotChanged( const QModelIndex &, const QModelIndex & )
{
    pObject->setModified();
}

void AWValidationWidget::slotStrictness( const QString &s )
{
    emit signalStrictness( ADValidation::getStrictness( s ) );
}

void AWValidationWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < 2 /* pView->columnCount() */; n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", pView->columnWidth( n ) );
    }
    settings.endArray();
}

void AWValidationWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        pView->setColumnWidth( n, settings.value( "w", QString::number( pView->columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

