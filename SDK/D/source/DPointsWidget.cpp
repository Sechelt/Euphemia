#include "LibInfo.h"
#include "DPointsWidget.h"

DPointsWidget::DPointsWidget( DLine *pLine, QWidget *pParent )
    : QWidget( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    QStringList listHeader;
    listHeader << "x" << "y";

    QPolygonF   polygon = pLine->getPolygon();

    QTableWidget *pTable = new QTableWidget( polygon.count(), 2, this );
    pTable->setHorizontalHeaderLabels( listHeader );
    // pTable->setVerticalHeader( nullptr );

    int nRow = 0;
    QPointF point;
    foreach( point, polygon )
    {
        pTable->setItem( nRow, 0, new QTableWidgetItem( QString::number( point.x() ) ) );
        pTable->setItem( nRow, 1, new QTableWidgetItem( QString::number( point.y() ) ) );
        nRow++;
    }

    pLayout->addWidget( pTable );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}























