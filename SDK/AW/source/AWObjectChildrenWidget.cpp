#include "LibInfo.h"
#include "AWObjectChildrenWidget.h"

AWObjectChildrenWidget::AWObjectChildrenWidget( AWObject *pObject, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    QStringList listHeader;
    listHeader << "OID" << "Class" << "Name";

    QList<ADObject*> listChildren = pObject->getObjects();

    QTableWidget *pTable = new QTableWidget( listChildren.count(), 3, this );
    pTable->setHorizontalHeaderLabels( listHeader );
    pTable->setVerticalHeader( nullptr );

    ADObject *p;
    int nRow = 0;
    foreach( p, listChildren )
    {
        AWObject*pObj = (AWObject*)p;

        QTableWidgetItem *pItem;    

        pTable->setItem( nRow, 0, pItem = new QTableWidgetItem( QString::number( pObj->getOID() ) ) );
        pItem->setIcon( pObj->getIcon().scaled( 16, 16 ) );
        pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

        pTable->setItem( nRow, 1, pItem = new QTableWidgetItem( pObj->metaObject()->className() ) );
        pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

        pTable->setItem( nRow, 2, pItem = new QTableWidgetItem( pObj->getName() ) );
        pItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

        nRow++;
    }

    pLayout->addWidget( pTable );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}























