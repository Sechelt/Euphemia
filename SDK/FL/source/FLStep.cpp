#include "LibInfo.h"
#include "FLStep.h"
    
#include "FLDiagram.h"

FLStep::FLStep( FLDiagram *pDiagram, const QString &stringName )
    : FLRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

FLStep::~FLStep()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap FLStep::getIcon()
{
    return QPixmap( ":FL/Step" );
}

void FLStep::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    DBitmap::paint( pPainter, nOption, pWidget );
}


