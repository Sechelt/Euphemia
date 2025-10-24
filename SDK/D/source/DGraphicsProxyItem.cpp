#include "LibInfo.h"
#include "DGraphicsProxyItem.h"

//
//
//
DGraphicsProxyItem::DGraphicsProxyItem( DObject *p )
{
    Q_ASSERT( p );

    pObject                 = p;
    pGraphicsObject         = nullptr;     
}

DGraphicsProxyItem::~DGraphicsProxyItem()
{
    if ( pObject ) pObject->setProxy( nullptr );
    if ( pGraphicsObject ) delete pGraphicsObject;
}

void DGraphicsProxyItem::setScene( QGraphicsScene *pScene )
{
    if ( pScene ) return pScene->addItem( pGraphicsObject );
    pScene->removeItem( pGraphicsObject );
}

void DGraphicsProxyItem::setVisible( bool b )
{
    pGraphicsObject->setVisible( b );
}

void DGraphicsProxyItem::setPos( const QPointF &point )
{
    pGraphicsObject->setPos( point );
}

void DGraphicsProxyItem::setTransform( const QTransform &t, bool b )
{
    pGraphicsObject->setTransform( t, b );
}

QPointF DGraphicsProxyItem::pos()
{
    return pGraphicsObject->pos();
}

QGraphicsScene *DGraphicsProxyItem::scene()
{
    return pGraphicsObject->scene();
}

DObject *DGraphicsProxyItem::getObject() 
{ 
    return pObject; 
}

QGraphicsObject *DGraphicsProxyItem::getItem()
{
    return pGraphicsObject;
}

QPointF DGraphicsProxyItem::mapToParent( const QPointF &point )
{
    return pGraphicsObject->mapToParent( point );
}

QPointF DGraphicsProxyItem::mapFromParent( const QPointF &point )
{
    return pGraphicsObject->mapFromParent( point );
}

QPolygonF DGraphicsProxyItem::mapToScene( const QRectF &rect )
{
    return pGraphicsObject->mapToScene( rect );
}

QPointF DGraphicsProxyItem::mapToScene( const QPointF &point )
{
    return pGraphicsObject->mapToScene( point );
}

QPolygonF DGraphicsProxyItem::mapToScene( const QPolygonF &polygon )
{
    return pGraphicsObject->mapToScene( polygon );
}

QPointF DGraphicsProxyItem::mapFromScene( const QPointF &point )
{
    return pGraphicsObject->mapFromScene( point );
}

QPolygonF DGraphicsProxyItem::mapFromScene( const QRectF &rect )
{
    return pGraphicsObject->mapFromScene( rect );
}

QPolygonF DGraphicsProxyItem::mapFromScene( const QPolygonF &polygon )
{
    return pGraphicsObject->mapFromScene( polygon );
}

QRectF DGraphicsProxyItem::mapRectToScene( const QRectF &rect )
{
    return pGraphicsObject->mapRectToScene( rect );
}

QTransform DGraphicsProxyItem::transform()
{
    return pGraphicsObject->transform();
}

void DGraphicsProxyItem::doToBack() 
{                      
    // get self item                                
    QGraphicsObject *pItemObject = pGraphicsObject;
    if ( !pItemObject ) return;

    // get a child list
    QList<QGraphicsItem *> l;

    QGraphicsItem *pItem = pItemObject->parentItem();
    if ( pItem ) l = pItem->childItems();
    else l = scene()->items( Qt::AscendingOrder );

    // set order for self (to be first)
    pItemObject->setZValue( 1 );

    // adjust others
    qreal nLast = 1;
    foreach( pItem, l )
    {
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pObject = (QGraphicsObject*)pItem;
        // we already handled self
        if ( pObject == pItemObject ) continue;
        // all DObject's are either a DGraphicsObject or a FRGraphicsObject
        // \todo do something other than using constant "FRGraphicsObject" because we are not supposed to know about this here
        if ( !pObject->inherits( "DGraphicsObject" ) && !pObject->inherits( "FRGraphicsObject" ) ) continue;
        // set order
        pItem->setZValue( ++nLast );
    }
}                                                      
                                                       
void DGraphicsProxyItem::doToFront()
{
    // get self item
    QGraphicsObject *pItemObject = pGraphicsObject;
    if ( !pItemObject ) return;

    // get child list
    QList<QGraphicsItem *> l;

    QGraphicsItem *pItem = pItemObject->parentItem();
    if ( pItem ) l = pItem->childItems();
    else l = scene()->items( Qt::AscendingOrder );

    // adjust others
    qreal nLast = 0;
    foreach( pItem, l )
    {
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pObject = (QGraphicsObject*)pItem;
        // we will handle self later
        if ( pObject == pItemObject ) continue;
        // all DObject's are either a DGraphicsObject or a FRGraphicsObject
        // \todo do something other than using constant "FRGraphicsObject" because we are not supposed to know about this here
        if ( !pObject->inherits( "DGraphicsObject" ) && !pObject->inherits( "FRGraphicsObject" ) ) continue;
        // set order
        pItem->setZValue( ++nLast );
    }

    // set order for self (to be last)
    pItemObject->setZValue( ++nLast );
}

void DGraphicsProxyItem::doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QGraphicsObject *pSelf = pGraphicsObject;
    if ( !pSelf ) return;

    QList<QGraphicsItem *> listChildren;

    QGraphicsItem *pItem = pSelf->parentItem();
    if ( pItem ) listChildren = pItem->childItems();
    else listChildren = scene()->items( Qt::AscendingOrder );

    if ( listChildren.size() < 2 ) return;

    // scan in reverse
    // scan to find self then
    // scan to find filter match then
    // set zorder for self
    // scan forward inc zorder values
    const int nStateFindSelf = 0;
    const int nStateFindFilter = 1;
    const int nStateAdjustOrder = 2;
    int nState = nStateFindSelf; 
    QGraphicsItem *pItemMatched = nullptr;

    QListIterator<QGraphicsItem*> i( listChildren );
    i.toBack();
    while ( i.hasPrevious() )
    {
        pItem = i.previous();
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pObject = (QGraphicsObject*)pItem;

        switch ( nState )
        {
            case nStateFindSelf:
            {
                if ( pObject == pSelf ) nState = nStateFindFilter;
                break;
            }
            case nStateFindFilter:
            {
                // the filter is matched on the data object (DObject) so lets get the DObject...
                DObject *p = nullptr;
                if ( pObject->inherits( "DGraphicsObject" ) ) p = ((DGraphicsObject*)pObject)->getProxy()->getObject();
                // else if ( pObject->inherits( "FRGraphicsObject" ) )  p = ((FRGraphicsObject*)pObject)->getProxy()->getObject();
                if ( !p ) break;

                // does the object match?
                if ( !stringName.isNull() && p->getName() != stringName ) break;
                if ( !stringClassName.isNull() && p->metaObject()->className() != stringClassName ) break;
                if ( !stringInheritsClass.isNull() && !p->inherits( stringInheritsClass.toLatin1().constData() ) ) break;

                // yes - object matchs...
                // this is the previous object that matchs so no need to scan further
                pItemMatched = pItem;
                nState = nStateAdjustOrder;
                break;
            }
        }

        // no need to scan further
        if ( nState == nStateAdjustOrder ) break;
    }

    // no match so change nothing
    if ( !pItemMatched ) return;

    // insert self
    qreal nZValue = pItemMatched->zValue();
    pSelf->setZValue( nZValue );

    // inc everything after 
    while ( i.hasNext() )
    {
        pItem = i.next();
        pItem->setZValue( nZValue++ );
    }
}

void DGraphicsProxyItem::doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QGraphicsObject *pSelf = pGraphicsObject;
    if ( !pSelf ) return;

    QList<QGraphicsItem *> listChildren;

    QGraphicsItem *pItem = pSelf->parentItem();
    if ( pItem ) listChildren = pItem->childItems();
    else listChildren = scene()->items( Qt::AscendingOrder );

    if ( listChildren.size() < 2 ) return;

    // scan to find self then
    // scan to find filter match then
    // set zorder for self
    // scan forward inc zorder values
    const int nStateFindSelf = 0;
    const int nStateFindFilter = 1;
    const int nStateAdjustOrder = 2;
    int nState = nStateFindSelf; 

    foreach( pItem, listChildren )
    {
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pObject = (QGraphicsObject*)pItem;

        switch ( nState )
        {
            case nStateFindSelf:
            {
                if ( pObject == pSelf ) nState = nStateFindFilter;
                break;
            }
            case nStateFindFilter:
            {
                // the filter is matched on the data object (DObject) so lets get the DObject...
                DObject *p = nullptr;
                if ( pObject->inherits( "DGraphicsObject" ) ) p = ((DGraphicsObject*)pObject)->getProxy()->getObject();
                // else if ( pObject->inherits( "FRGraphicsObject" ) )  p = ((FRGraphicsObject*)pObject)->getProxy()->getObject();
                if ( !p ) break;

                // does the object match?
                if ( !stringName.isNull() && p->getName() != stringName ) break;
                if ( !stringClassName.isNull() && p->metaObject()->className() != stringClassName ) break;
                if ( !stringInheritsClass.isNull() && !p->inherits( stringInheritsClass.toLatin1().constData() ) ) break;

                // yes - object matchs...
                pSelf->setZValue( pObject->zValue() );
                pObject->setZValue( pObject->zValue() + 1 );
                nState = nStateAdjustOrder;
                break;
            }
            case nStateAdjustOrder:
            default:
            {
                pObject->setZValue( pObject->zValue() + 1 );
            }
        }
    }
}

void DGraphicsProxyItem::doToBefore( DObject *pObjectSubject )
{
    QGraphicsObject *pItemObjectSubject = pObjectSubject->getProxy()->getItem();
    QGraphicsObject *pItemObjectSelf    = pGraphicsObject;
    if ( !pItemObjectSelf ) return;

    // get child list
    QList<QGraphicsItem *> listChildren;

    QGraphicsItem *pItem = pItemObjectSelf->parentItem();
    if ( pItem ) listChildren = pItem->childItems();
    else listChildren = scene()->items( Qt::AscendingOrder );

    if ( listChildren.size() < 2 ) return;

    // scan child list
    const int nStateFindObject = 0;
    const int nStateAdjustOrder = 2;
    int nState = nStateFindObject; 

    foreach( pItem, listChildren )
    {
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pItemObject = (QGraphicsObject*)pItem;

        switch ( nState )
        {
            case nStateFindObject:
            {
                // found self - so there is no need to call this method
                if ( pItemObject == pItemObjectSelf ) return;
                // found pItemObjectSubject - so put self before it
                if ( pItemObject == pItemObjectSubject )
                {
                    pItemObjectSelf->setZValue( pItemObject->zValue() );
                    pItemObject->setZValue( pItemObject->zValue() + 1 );
                    nState = nStateAdjustOrder;
                }
                break;
            }
            case nStateAdjustOrder:
            default:
            {
                // skip self
                if ( pItemObject == pItemObjectSelf ) break;
                // inc others
                // \todo do something other than using constant "FRGraphicsObject" because we are not supposed to know about this here
                if ( !pItemObject->inherits( "DGraphicsObject" ) && !pItemObject->inherits( "FRGraphicsObject" ) ) continue;
                pItemObject->setZValue( pItemObject->zValue() + 1 );
            }
        }
    }
}

void DGraphicsProxyItem::doToAfter( DObject *pObjectSubject )
{
    QGraphicsObject *pItemObjectSubject = pObjectSubject->getProxy()->getItem();
    QGraphicsObject *pItemObjectSelf    = pGraphicsObject;
    if ( !pItemObjectSelf ) return;

    // get child list
    QList<QGraphicsItem *> listChildren;

    QGraphicsItem *pItem = pItemObjectSelf->parentItem();
    if ( pItem ) listChildren = pItem->childItems();
    else listChildren = scene()->items( Qt::AscendingOrder );

    if ( listChildren.size() < 2 ) return;

    // scan child list
    const int nStateFindSelf = 0;
    const int nStateFindObject = 1;
    const int nStateAdjustOrder = 2;
    int nState = nStateFindSelf; 

    foreach( pItem, listChildren )
    {
        // we know that *all* of our diagram items are QGraphicsObject based
        QGraphicsObject *pItemObject = (QGraphicsObject*)pItem;

        switch ( nState )
        {
            case nStateFindSelf:
            {
                // found pObject - so there is no need to call this method
                if ( pItemObject == pItemObjectSubject ) return;
                // found self - now scan for pItemObjectSubject  
                if ( pItemObject == pItemObjectSelf ) nState = nStateFindObject;
                break;
            }
            case nStateFindObject:
            {
                if ( pItemObject != pItemObjectSubject ) break;
                // found pObject - make self come after pObject
                pItemObjectSelf->setZValue( pItemObject->zValue() + 1 );
                nState = nStateAdjustOrder;
                break;
            }
            case nStateAdjustOrder:
            default:
            {
                // inc others 
                // \todo do something other than using constant "FRGraphicsObject" because we are not supposed to know about this here
                if ( !pItemObject->inherits( "DGraphicsObject" ) && !pItemObject->inherits( "FRGraphicsObject" ) ) continue;
                pItemObject->setZValue( pItemObject->zValue() + 1 );
            }
        }
    }
}

void DGraphicsProxyItem::update()
{
    pGraphicsObject->update();
}

void DGraphicsProxyItem::doReparent( ADObject *pObject )
{
    // null parent means top-level

    QGraphicsItem *pItem = nullptr;
    if ( pObject && pObject->inherits( "DObject" ) ) pItem = ((DObject*)pObject)->getProxy()->getItem();

    pGraphicsObject->setParentItem( pItem );
}

QRectF DGraphicsProxyItem::boundingRect() const
{
    return pGraphicsObject->boundingRect();
}

void DGraphicsProxyItem::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    pGraphicsObject->paint( pPainter, nOption, pWidget );
}

QPainterPath DGraphicsProxyItem::shape() const
{
    return pGraphicsObject->shape();
}

//
//
//
DGraphicsProxyObject::DGraphicsProxyObject( DObject *p )
: DGraphicsProxyItem( p )
{
}

void DGraphicsProxyObject::doCreateGraphicsObject( QGraphicsItem *pItemParent )
{
    Q_ASSERT( !pGraphicsObject );
    pGraphicsObject = new DGraphicsObject( this, pItemParent );
}

void DGraphicsProxyObject::doPrepareGeometryChange()
{
    ((DGraphicsObject*)pGraphicsObject)->doPrepareGeometryChange();
}

//
//
//
DGraphicsObject::DGraphicsObject( DGraphicsProxyItem *p, QGraphicsItem *pItemParent )
: QGraphicsObject( pItemParent )
{
    Q_ASSERT( p );
    this->p = p;
}

void DGraphicsObject::doPrepareGeometryChange()
{
    prepareGeometryChange();
}

QRectF DGraphicsObject::boundingRect() const                                                                
{                                                                                                           
    return p->getObject()->boundingRect();                                                                  
}                                                                                                           
                                                                                                            
void DGraphicsObject::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{                                                                                                           
    pPainter->setRenderHints( QPainter::Antialiasing );                                                     
    p->getObject()->paint( pPainter, nOption, pWidget );                                                    
}                                                                                                           
                                                                                                            
QPainterPath DGraphicsObject::shape() const                                                                 
{                                                                                                           
    return p->getObject()->shape();                                                                         
}                                                                                                           


