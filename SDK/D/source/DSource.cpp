#include "LibInfo.h"
#include "DSource.h"

#if QT_VERSION < 0x060000
    #include <QSound> 
#endif

#include "DSink.h"
#include "DLine.h"
#include "DGraphicsProxyItem.h"

DSource::DSource( DLine *p, CBD::EOLTypes n ) 
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    switch ( n )
    {
        case CBD::EOLBegin:
            setObjectName( "Begin" );
            break;
        case CBD::EOLEnd:
            setObjectName( "End" );
            break;
        case CBD::EOLNeither:
            setObjectName( "Neither" );
            break;
    }

    pLine = p;
    nSourceType = n;
    pHandle = nullptr; 
    pSink = nullptr; 

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DSource::~DSource() 
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // get rid of the DHandle as its parent is the scene so it will not go away with us unless we do this 
    if ( pHandle ) delete pHandle;
    // ensure we are not connected to a sink
    if ( pSink )
    {
        pSink->doUnglue( this ); 
    }

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void DSource::setPos( const QPointF &point ) 
{ 
    pointPos = point;
    if ( pHandle ) pHandle->setPos( point );
}

void DSource::setVisible( bool b ) 
{
    if ( b )
    { 
        if ( pHandle ) return;
        pHandle = new DHandle( pLine, DHandle::FunctionConnect, DHandle::ConnectTypeSource, pointPos );
        pHandle->setConnected( isGlued() );
        pLine->getProxy()->scene()->addItem( pHandle );     
        // pLine->getProxy()->stackBefore( pHandle );
    }
    else
    {
       if ( pHandle ) delete pHandle;
       pHandle = nullptr;
    }
}

DRectangle *DSource::getRectangle()
{
    if ( !pSink ) return nullptr;
    return pSink->getRectangle();
}

int DSource::getIndex() 
{ 
    return pLine->getSources().indexOf( this ); 
}

void DSource::doGlue( DSink *p ) 
{ 
    pSink = p; 
    if ( pHandle ) pHandle->setConnected( isGlued() ); 
#if QT_VERSION < 0x060000
    if ( 0 ) QSound::play(":/sounds/click");
#endif
}

void DSource::doUnglue() 
{ 
    pSink = nullptr; 
    if ( pHandle ) pHandle->setConnected( isGlued() ); 
#if QT_VERSION < 0x060000
    if ( 0 ) QSound::play(":/sounds/pop");
#endif
}

