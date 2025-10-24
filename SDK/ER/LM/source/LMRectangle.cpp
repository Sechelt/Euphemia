#include "LibInfo.h"
#include "LMRectangle.h"

#include "LMModel.h" 

LMRectangle::LMRectangle( LMModel *pParent )
: DRectangle( pParent ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // nTextAlign will only be used in the entity/inherit caption and we want the default to be center
    nTextHAlign = Qt::AlignCenter;

    // dark purple theme
    font.color = QColor( Qt::white );
    pen.setColor( "#0000ff" );
    brush.setColor( "#9932cc" );
    brush.setStyle( Qt::SolidPattern );

    // plain theme
    // pen.setColor( Qt::black );
    // font.color = QColor( Qt::black );
    // brush.setColor( Qt::white );
    // brush.setStyle( Qt::SolidPattern );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMRectangle::~LMRectangle()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}




