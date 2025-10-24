#include "LibInfo.h"
#include "DSourcesWidget.h"

#include "DSource.h"

DSourcesWidget::DSourcesWidget( DLine *pLine, QWidget *pParent )
    : QWidget( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    QStringList listHeader;
    listHeader << "Name" << "EOL" << "Glued";

    QVector<DSource*> vectorSources = pLine->getSources();

    QTableWidget *pTable = new QTableWidget( vectorSources.count(), 3, this );
    pTable->setHorizontalHeaderLabels( listHeader );
    // pTable->setVerticalHeader( nullptr );

    DSource *pSource;
    int nRow = 0;
    foreach( pSource, vectorSources )
    {
        pTable->setItem( nRow, 0, new QTableWidgetItem( pSource->objectName() ) );
        pTable->setItem( nRow, 1, new QTableWidgetItem( pSource->isBegin() ? "Begin" : "End" ) );
        pTable->setItem( nRow, 2, new QTableWidgetItem( QString::number( pSource->isGlued() ) ) );
        nRow++;
    }

    pLayout->addWidget( pTable );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}























