#include "LibInfo.h"
#include "AWObjectSelectWidget.h"

AWObjectSelectWidget::AWObjectSelectWidget( QWidget *pwidgetParent, const QList<ADObject*> &l, bool bMultiSelect )
    : QTableWidget( pwidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    listObjects = l;

    setColumnCount( 4 );
    setRowCount( l.count() );

    QStringList listStrings;
    listStrings << tr("OID") << tr("Name") << tr("Code") << tr("Comment");
    setHorizontalHeaderLabels( listStrings );

    ADObject *pObject;
    int nRow = 0;
    foreach( pObject, l )
    {
        QTableWidgetItem *pItem = new QTableWidgetItem( QString::number( pObject->getOID() ) );
        if ( pObject->inherits( "AWObject" ) ) pItem->setIcon(  ((AWObject*)pObject)->getIcon() );
        setItem( nRow, 0, pItem );
        setItem( nRow, 1, new QTableWidgetItem( pObject->getName() ) );
        setItem( nRow, 2, new QTableWidgetItem( pObject->getCode() ) );
        setItem( nRow, 3, new QTableWidgetItem( pObject->getComment() ) );

        nRow++;
    }

    setSelectionBehavior( QAbstractItemView::SelectRows );
    if ( bMultiSelect )
        setSelectionMode( QAbstractItemView::ExtendedSelection );    
    else
        setSelectionMode( QAbstractItemView::SingleSelection );    

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

AWObjectSelectWidget::~AWObjectSelectWidget()
{
}

void AWObjectSelectWidget::setSelected( ADObject *pObject ) 
{
    if ( !pObject ) return;

    for ( int nRow = 0; nRow < rowCount(); nRow++ )
    {
        if ( item( nRow, 0 )->text().toInt() == pObject->getOID() )
        {
            setRangeSelected( QTableWidgetSelectionRange( nRow, 0, nRow, columnCount() - 1 ), true );
            break;
        }
    }
}

void AWObjectSelectWidget::setSelected( const QList<ADObject*> &l ) 
{
    ADObject *pObject;
    foreach( pObject, l )
    {
        Q_UNUSED(pObject);

        // \todo
//        setRangeSelected(const QTableWidgetSelectionRange &range, bool select)
        if ( this->selectionMode() == QAbstractItemView::SingleSelection ) break;
    }
}

QList<ADObject*> AWObjectSelectWidget::getSelected() 
{
    QList<ADObject*> listSelected;

    QList<QTableWidgetSelectionRange> listRanges = selectedRanges();
    QTableWidgetSelectionRange Range;
    foreach( Range, listRanges )
    {
        for ( int nRow = Range.topRow(); nRow <= Range.bottomRow(); nRow++ )
        {
           listSelected.append( listObjects.at( nRow ) );
        }
    }

    return listSelected;
}


