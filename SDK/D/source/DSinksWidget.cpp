#include "LibInfo.h"
#include "DSinksWidget.h"

#include "DSink.h"

DSinksWidget::DSinksWidget( DRectangle *pRectangle, QWidget *pParent )
    : QWidget( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    QStringList listHeader;
    listHeader << "Name" << "Type" << "Source Count";

    QVector<DSink*> vectorSinks = pRectangle->getSinks();

    QTableWidget *pTable = new QTableWidget( vectorSinks.count(), 3, this );
    pTable->setHorizontalHeaderLabels( listHeader );
    // pTable->setVerticalHeader( nullptr );

    DSink *pSink;
    int nRow = 0;
    foreach( pSink, vectorSinks )
    {
        pTable->setItem( nRow, 0, new QTableWidgetItem( pSink->objectName() ) );
        pTable->setItem( nRow, 1, new QTableWidgetItem( ( pSink->getType() == DSink::SinkTypeStatic ? tr("Static") : tr("Dynamic") ) ) );
        pTable->setItem( nRow, 2, new QTableWidgetItem( QString::number( pSink->getSources().count() ) ) );
        nRow++;
    }

    pLayout->addWidget( pTable );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}























