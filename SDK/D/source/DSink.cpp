#include "LibInfo.h"
#include "DSink.h"

#include "DSource.h"
#include "DRectangle.h"
#include "DLine.h"
#include "DGraphicsProxyItem.h"

DSink::DSink( DRectangle *p, const QString &stringName ) 
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->nType = DSink::SinkTypeStatic;
    setObjectName( stringName ); 
    pRectangle = p; 
    pHandle = nullptr; 

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DSink::DSink( DRectangle *p, const QPointF &pointPercent, const QString &stringName ) 
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->nType = DSink::SinkTypeDynamic;
    setObjectName( stringName ); 
    pRectangle = p; 
    pHandle = nullptr; 
    this->pointPercent = pointPercent;
    setPosRaw(); // to calc/set pointPos 

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DSink::~DSink() 
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // get rid of the DHandle as its parent is the scene so it will not go away with us unless we do this 
    if ( pHandle ) delete pHandle;
    // ensure we are not connected to a source
    DSource *pSource;
    foreach( pSource, listSources )
    {
       pSource->doUnglue();
    }

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
 * \brief move the sink
 *  
 * Ensure that any connected source moves along with us. Do this via DLine so 
 * that DLine can adjust its internals. 
 *  
 * \author pharvey (10/8/19)
 * 
 * \param point 
 */
void DSink::setPos( const QPointF &point ) 
{
    // move ourself
    pointPos = point;
    if ( pHandle ) pHandle->setPos( point );

    // move any attached sources
    // this is redundant when line and connected rectangle are both being moved together - but no harm
    DSource *pSource;
    foreach( pSource, listSources )
    {
        pSource->getLine()->setPos( pSource, point );
    }
}

void DSink::setPosRaw( const QPointF &point ) 
{ 
    // move ourself
    pointPos = point;
    if ( pHandle ) pHandle->setPos( point );
}

void DSink::setPos() 
{
    Q_ASSERT( nType == SinkTypeDynamic );
    QPointF point( pRectangle->boundingRect().width() / pointPercent.x(), 
                   pRectangle->boundingRect().height() / pointPercent.y() );
    setPos( pRectangle->getProxy()->mapToScene( point ) );
}

void DSink::setPosRaw() 
{
    Q_ASSERT( nType == SinkTypeDynamic );
    QPointF point( pRectangle->boundingRect().width() / pointPercent.x(), 
                   pRectangle->boundingRect().height() / pointPercent.y() );
    setPosRaw( pRectangle->getProxy()->mapToScene( point ) );
}

void DSink::setVisible( bool b ) 
{
    if ( b )
    { 
        if ( pHandle ) return;
        pHandle = new DHandle( pRectangle, DHandle::FunctionConnect, DHandle::ConnectTypeSink, pointPos );
        pRectangle->getProxy()->scene()->addItem( pHandle );
    }
    else
    {
        if ( pHandle ) delete pHandle;
        pHandle = nullptr;
    }
}

int DSink::getIndex() 
{ 
    return pRectangle->getSinks().indexOf( this ); 
}

bool DSink::contains( const QPointF &pointScene )
{
    if ( pHandle ) return pHandle->contains(  pHandle->mapFromScene( pointScene ) );

    return QRectF( pointPos.x() - 5, pointPos.y() - 5, 10, 10  ).contains( pointScene );
}


