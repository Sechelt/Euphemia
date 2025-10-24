#include "LibInfo.h"
#include "PMLine.h"

#include "PMModel.h" 

PMLine::PMLine( PMModel *pParent )
: DLinePoly( pParent ), PMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // slate green theme
    font.color = QColor( Qt::black );
    pen.setColor( Qt::black );
    brush.setColor( "#637877" );
    brush.setStyle( Qt::SolidPattern );

    // we should always be truncated with an arrow at the end
    bTruncateBegin = true;
    bTruncateEnd = true;
    nStyleBegin = W::WLineEndNone;
    nStyleEnd = W::WLineEndArrow;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMLine::~PMLine()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QDomElement PMLine::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DLine::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );

    return domElemThis;
}

bool PMLine::doLoad( QDomElement *pdomElemThis )
{
    DLine::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    return true;
}




