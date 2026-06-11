/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LGraphicsObject.h"

#include <SSettings.h>

#include "LGraphicsLayer.h"
#include "LObject.h"

LGraphicsObject::LGraphicsObject( LObject *p, LGraphicsLayer *pParent )
    : QGraphicsObject( pParent )
{
    Q_ASSERT( p );
    pObject = p;

    pSettingRender = g_SSettings->getSetting( "LSRender" ); Q_ASSERT( pSettingRender );
    slotRefreshRender();
    connect( pSettingRender, SIGNAL(signalChanged()), SLOT(slotRefreshRender()) );
}

LGraphicsObject::LGraphicsObject( LObject *p, LGraphicsObject *pParent )
    : QGraphicsObject( pParent )
{
    Q_ASSERT( p );
    pObject = p;

    pSettingRender = g_SSettings->getSetting( "LSRender" ); Q_ASSERT( pSettingRender );
    slotRefreshRender();
    connect( pSettingRender, SIGNAL(signalChanged()), SLOT(slotRefreshRender()) );
}

void LGraphicsObject::doToBack() 
{                      
    // get self item                                
    QGraphicsObject *pItemObject = this;

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
        // 
        if ( !pObject->inherits( "LGraphicsObject" ) ) continue;
        // set order
        pItem->setZValue( ++nLast );
    }
}                                                      
                                                       
void LGraphicsObject::doToFront()
{
    // get self item
    QGraphicsObject *pItemObject = this;

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
        //
        if ( !pObject->inherits( "LGraphicsObject" ) ) continue;
        // set order
        pItem->setZValue( ++nLast );
    }

    // set order for self (to be last)
    pItemObject->setZValue( ++nLast );
}

void LGraphicsObject::doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QGraphicsObject *pSelf = this;

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
                // the filter is matched on the data object so lets get the LObject...
                LObject *p = nullptr;
                if ( pObject->inherits( "LGraphicsObject" ) ) p = ((LGraphicsObject*)pObject)->getObject();
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

void LGraphicsObject::doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QGraphicsObject *pSelf = this;

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
                // the filter is matched on the data object so lets get the LObject...
                LObject *p = nullptr;
                if ( pObject->inherits( "LGraphicsObject" ) ) p = ((LGraphicsObject*)pObject)->getObject();
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

void LGraphicsObject::doToBefore( LObject *pObjectSubject )
{
    QGraphicsObject *pItemObjectSubject = pObjectSubject->getGraphics();
    QGraphicsObject *pItemObjectSelf    = this;
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
                // found - so put self before it
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
                if ( !pItemObject->inherits( "LGraphicsObject" ) ) continue;
                pItemObject->setZValue( pItemObject->zValue() + 1 );
            }
        }
    }
}

void LGraphicsObject::doToAfter( LObject *pObjectSubject )
{
    QGraphicsObject *pItemObjectSubject = pObjectSubject->getGraphics();
    QGraphicsObject *pItemObjectSelf    = this;
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
        // we know that *all* of our items are QGraphicsObject based
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
                if ( !pItemObject->inherits( "LGraphicsObject" ) ) continue;
                pItemObject->setZValue( pItemObject->zValue() + 1 );
            }
        }
    }
}

void LGraphicsObject::doReparent( ADObject *pObject )
{
    // null parent means top-level

    QGraphicsItem *pItem = nullptr;
    if ( pObject && pObject->inherits( "LObject" ) ) pItem = ((LObject*)pObject)->getGraphics();

    setParentItem( pItem );
}

void LGraphicsObject::doPrepareGeometryChange()
{
    prepareGeometryChange();
}

QRectF LGraphicsObject::boundingRect() const                                                                
{                                                                                                           
    return pObject->boundingRect();                                                                  
}                                                                                                           
                                                                                                            
void LGraphicsObject::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{                                                                                                           
    pPainter->setRenderHint( QPainter::Antialiasing, nAntialiasing );    
    pPainter->setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );    
    pPainter->setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );    
    pPainter->setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );    
    pObject->paint( pPainter, nOption, pWidget );                                                    
}                                                                                                           
                                                                                                            
QPainterPath LGraphicsObject::shape() const                                                                 
{                                                                                                           
    return pObject->shape();                                                                         
}                                                                                                           

QSizeF LGraphicsObject::sizeHint( Qt::SizeHint nSizeHint, const QSizeF &sizeConstraint ) const
{
    switch ( nSizeHint )
    {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
        case Qt::MaximumSize:
        case Qt::MinimumDescent:
        case Qt::NSizeHints:
            break;
    }
    return boundingRect().size();
}

void LGraphicsObject::slotRefreshRender()
{
    nAntialiasing           = pSettingRender->property( "nAntialiasing" ).toBool();
    nTextAntialiasing       = pSettingRender->property( "nTextAntialiasing" ).toBool();
    nSmoothPixmapTransform  = pSettingRender->property( "nSmoothPixmapTransform" ).toBool();
    nLosslessImageRendering = pSettingRender->property( "nLosslessImageRendering" ).toBool();
    update();
}

