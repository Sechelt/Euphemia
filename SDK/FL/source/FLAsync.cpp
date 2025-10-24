#include "LibInfo.h"
#include "FLAsync.h"
    
#include "FLDiagram.h"

FLAsync::FLAsync( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    pixmap = QPixmap( ":FL/Async" );
    resizedPixmap = pixmap.scaled( pDiagram->getCell().width(), pDiagram->getCell().height() );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLAsync::~FLAsync()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap FLAsync::getIcon()
{
    return QPixmap( ":FL/Async" );
}

void FLAsync::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    DBitmap::paint( pPainter, nOption, pWidget );
}


