#include "LibInfo.h"
#include "FLSync.h"
    
#include "FLDiagram.h"

FLSync::FLSync( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    pixmap = QPixmap( ":FL/Sync" );
    resizedPixmap = pixmap.scaled( pDiagram->getCell().width(), pDiagram->getCell().height() );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLSync::~FLSync()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap FLSync::getIcon()
{
    return QPixmap( ":FL/Sync" );
}

void FLSync::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    DBitmap::paint( pPainter, nOption, pWidget );
}


