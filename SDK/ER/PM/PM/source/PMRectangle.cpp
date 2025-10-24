#include "LibInfo.h"
#include "PMRectangle.h"

#include "PMModel.h" 

PMRectangle::PMRectangle( PMModel *pParent )
: DRectangle( pParent ), PMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // nTextAlign will only be used in the table/view caption and we want the default to be center
    nTextHAlign = Qt::AlignCenter;

    // slate green theme
    font.color = QColor( Qt::black );
    pen.setColor( Qt::black );
    brush.setColor( "#637877" );
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

PMRectangle::~PMRectangle()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QDomElement PMRectangle::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangle::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );

    return domElemThis;
}

bool PMRectangle::doLoad( QDomElement *pdomElemThis )
{
    DRectangle::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    return true;
}



