/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LObject.h"

#include <SSettings.h>

#include "LGraphicsScene.h"
#include "LGraphicsObject.h"
#include "LLayer.h"

LObject::LObject( LLayer *pParent, const QString &stringName )
    : AWObject( pParent, stringName )
{
    pSettingRender = g_SSettings->getSetting( "LSRender" ); Q_ASSERT( pSettingRender );
    slotRefreshRender();
    connect( pSettingRender, SIGNAL(signalChanged()), SLOT(slotRefreshRender()) );
}

LObject::~LObject()
{
    if ( pGraphics ) doGraphicsDelete();
}

QRectF LObject::boundingRect() const
{
    return shape().boundingRect();
}

LGraphicsObject *LObject::getGraphics() 
{ 
    return pGraphics; 
}

LGraphicsScene *LObject::getScene()
{
    if ( !getGraphics() ) return nullptr;
    return (LGraphicsScene*)getGraphics()->scene();
}

LGraphicsView *LObject::getView()
{
    if ( !getScene() ) return nullptr;
    if ( !getScene()->views().count() ) return nullptr;
    return (LGraphicsView *)getScene()->views().first();
}


/*!
 * \brief Creates a graphics object. 
 *  
 * This creates a LGraphicsObject by default.
 * Could, if needed, override to return something derived from LGraphicsObject.
 * ie to introduce QWidget to the scene (can not use QGraphicsWidget because it is not based upon LGraphicsObject).
 * 
 * \author pharvey (4/7/23)
 * 
 * \param pParent 
 */
void LObject::doGraphicsCreate( LGraphicsLayer *pParent )
{
    Q_ASSERT( pParent );
    Q_ASSERT( !pGraphics );

    pGraphics = new LGraphicsObject( this, pParent ); // not a top-level so it will auto-magically be added to scene

    QList<LObject*> l = findChildren<LObject*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LObject *pObject, l )
    {
        pObject->doGraphicsCreate( pGraphics );
    }
}

QDomElement LObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );

    return domElemThis;
}

bool LObject::doLoad( QDomElement *pdomElemThis )                              
{ 
    AWObject::doLoad( pdomElemThis );
    return true;
}                                                                               

void LObject::doGraphicsCreate( LGraphicsObject *pParent )
{
    Q_ASSERT( pParent );
    Q_ASSERT( !pGraphics );

    pGraphics = new LGraphicsObject( this, pParent ); // not a top-level so it will auto-magically be added to scene

    QList<LObject*> l = findChildren<LObject*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LObject *pObject, l )
    {
        pObject->doGraphicsCreate( pGraphics );
    }
}

void LObject::doGraphicsDelete()
{
    Q_ASSERT( pGraphics );
    QList<LObject*> l = findChildren<LObject*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LObject *pObject, l )
    {
        pObject->doGraphicsDelete();
    }
    delete pGraphics; // will be auto-magically removed from scene
    pGraphics = nullptr;
}

void LObject::slotRefreshRender()
{
    nAntialiasing           = pSettingRender->property( "nAntialiasing" ).toBool();
    nTextAntialiasing       = pSettingRender->property( "nTextAntialiasing" ).toBool();
    nSmoothPixmapTransform  = pSettingRender->property( "nSmoothPixmapTransform" ).toBool();
    nLosslessImageRendering = pSettingRender->property( "nLosslessImageRendering" ).toBool();
    if ( pGraphics ) pGraphics->update();
}

