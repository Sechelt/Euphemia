#include "LibInfo.h"
#include "AWOutputSearchWidget.h"

AWOutputSearchWidget::AWOutputSearchWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    {
        pLineEdit = new QLineEdit( this );
        pButton = new QPushButton( tr("Find"), this );

        QHBoxLayout *pLayout = new QHBoxLayout();
        pLayout->addWidget( pLineEdit );
        pLayout->addWidget( pButton );

        pName = new QCheckBox( tr("Name") );
        pCode = new QCheckBox( tr("Code") );
        pComment = new QCheckBox( tr("Comment") );
        pDynamicData = new QCheckBox( tr("Dynamic Data") );
        pOther = new QCheckBox( tr("Other") );
        pCaseSensitive = new QCheckBox( tr("Case Sensitive") );

        ADFindCriteria Criteria;
        pName->setChecked( Criteria.bName );
        pCode->setChecked( Criteria.bCode );
        pComment->setChecked( Criteria.bComment );
        pDynamicData->setChecked( Criteria.bDynamicData );
        pOther->setChecked( Criteria.bOther );
        pCaseSensitive->setChecked( Criteria.bCaseSenstive );

        pLayout->addWidget( pName );
        pLayout->addWidget( pCode );
        pLayout->addWidget( pComment );
        pLayout->addWidget( pDynamicData );
        pLayout->addWidget( pOther );
        pLayout->addWidget( pCaseSensitive );

        pLayout->addStretch( 10 );
        
        pLayoutTop->addLayout( pLayout ); 

        connect( pButton, SIGNAL(clicked()), SLOT(slotFind()) );
    }

    // perhaps do a tree to show the entire path?
    pTableWidget = new QTableWidget( this );
    pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pTableWidget->setSelectionMode( QAbstractItemView::SingleSelection );

    QStringList l;
    l << tr("Object") << tr("Match");
    pTableWidget->setColumnCount( 2 );
    pTableWidget->setHorizontalHeaderLabels( l );

    pLayoutTop->addWidget( pTableWidget );

    connect( pTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotClicked(int,int)) );
    connect( pTableWidget, SIGNAL(cellDoubleClicked(int,int)), SLOT(slotDoubleClicked(int,int)) );

    doLoadState();

}

AWOutputSearchWidget::~AWOutputSearchWidget()
{
    doSaveState();
}

void AWOutputSearchWidget::setFocusFind()
{
    pLineEdit->setFocus();
}

void AWOutputSearchWidget::setFocusReplace()
{
    pLineEdit->setFocus();
}

void AWOutputSearchWidget::slotMessage( ADEventSearch *pEvent )
{
    ADObjectPathItem PathItem = pEvent->listObjectPath.last();

    ADObjectPathItem v;
/*
    qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "PATH";
    foreach( v, pEvent->listObjectPath )                                  
    {                                                                     
        switch ( v.nValueType )                                           
        {                                                                 
            case ADObjectPathItem::ValueSpecial:                          
                qDebug() << "Special: " << v.nValue;                      
                break;                                                    
            case ADObjectPathItem::ValuePointer:                          
                qDebug() << "Pointer: " << v.nValue;                      
                break;                                                    
            case ADObjectPathItem::ValueIndex:                            
                qDebug() << "Index: " << v.nValue;                        
                break;                                                    
            case ADObjectPathItem::ValueOID:                              
                qDebug() << "OID: " << v.nValue;                          
                break;                                                    
        }                                                                 
        qDebug() << "Class: " << v.stringClassName;                       
        qDebug() << "Object: " << v.stringObjectName;                     
    }                                                                     
*/

    int nRow = pTableWidget->rowCount();

    pTableWidget->setRowCount( nRow + 1 );

    QTableWidgetItem *pItem;
    pTableWidget->setItem( nRow, 0, pItem = new QTableWidgetItem( pEvent->Pixmap, PathItem.stringObjectName ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    pTableWidget->setItem( nRow, 1, pItem = new QTableWidgetItem( pEvent->stringMatch ) );
    pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    pTableWidget->scrollToItem( pItem );

    vectorPaths.append( pEvent->listObjectPath );
}

void AWOutputSearchWidget::slotClear()
{
    pTableWidget->setRowCount( 0 );
    vectorPaths.clear();
}

void AWOutputSearchWidget::doSaveState()
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

void AWOutputSearchWidget::doLoadState()
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

void AWOutputSearchWidget::slotFind()
{
    if ( pLineEdit->text().isEmpty() ) return;

    ADFindCriteria Criteria( pLineEdit->text() );
    Criteria.bName          = pName->isChecked();
    Criteria.bCode          = pCode->isChecked();
    Criteria.bComment       = pComment->isChecked();
    Criteria.bDynamicData   = pDynamicData->isChecked();
    Criteria.bOther         = pOther->isChecked();
    Criteria.bCaseSenstive  = pCaseSensitive->isChecked();

    emit signalFind( Criteria );
}

void AWOutputSearchWidget::slotClicked( int nRow, int nColumn )
{
    Q_UNUSED(nColumn);
    emit signalGoTo( vectorPaths[nRow] );
}

void AWOutputSearchWidget::slotDoubleClicked( int nRow, int nColumn )
{
    Q_UNUSED(nColumn);
    emit signalGoToView( vectorPaths[nRow] );
}

