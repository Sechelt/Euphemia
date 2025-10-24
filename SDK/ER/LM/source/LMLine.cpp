#include "LibInfo.h"
#include "LMLine.h"

#include "LMModel.h" 

LMLine::LMLine( LMModel *pParent )
: DLine90Auto( pParent ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // dark purple theme
    font.color = QColor( Qt::black );
    pen.setColor( "#0000ff" );
    brush.setColor( "#9932cc" );
    brush.setStyle( Qt::SolidPattern );

    // we should always be truncated
    bTruncateBegin = true;
    bTruncateEnd = true;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMLine::~LMLine()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}




