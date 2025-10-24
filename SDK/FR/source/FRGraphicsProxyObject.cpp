#include "LibInfo.h"
#include "FRGraphicsProxyObject.h"

//
//
//
FRGraphicsProxyObject::FRGraphicsProxyObject( FRObject *p )
: DGraphicsProxyItem( p )
{
}

void FRGraphicsProxyObject::setWidget( QWidget *p )
{
    ((FRGraphicsObject *)pGraphicsObject)->setWidget( p );
}

void FRGraphicsProxyObject::setGeometry( const QRectF rect )
{
    ((FRGraphicsObject *)pGraphicsObject)->setGeometry( rect );
}

void FRGraphicsProxyObject::resize( const QSizeF size )
{
    ((FRGraphicsObject*)pGraphicsObject)->resize( size );
}

QWidget *FRGraphicsProxyObject::widget()
{
    return ((FRGraphicsObject*)pGraphicsObject)->widget();
}

QRectF FRGraphicsProxyObject::geometry()
{
    return ((FRGraphicsObject*)pGraphicsObject)->geometry();
}

QSizeF FRGraphicsProxyObject::size()
{
    return ((FRGraphicsObject*)pGraphicsObject)->size();
}

void FRGraphicsProxyObject::doCreateGraphicsObject( QGraphicsItem *pItemParent )
{
    Q_ASSERT( !pGraphicsObject );
    pGraphicsObject = new FRGraphicsObject( this, pItemParent );
}

void FRGraphicsProxyObject::doPrepareGeometryChange()
{
    ((FRGraphicsObject*)pGraphicsObject)->doPrepareGeometryChange();
}

//
//
//
FRGraphicsObject::FRGraphicsObject( FRGraphicsProxyObject *p, QGraphicsItem *pItemParent )
: QGraphicsProxyWidget( pItemParent )
{
    Q_ASSERT( p );
    this->p = p;
}

void FRGraphicsObject::doPrepareGeometryChange()
{
    prepareGeometryChange();
}


