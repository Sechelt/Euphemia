/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_LObject
#define H_LObject

#include "L.h"

#include <AWObject.h>
#include <SSetting.h>

class LGraphicsLayer;
class LGraphicsObject;
class LGraphicsScene;
class LGraphicsView;
class LLayer;

/*!
 * \brief Base class for objects which can be a child of \sa LLayer.
 *  
 * These objects use a \sa LGraphicsObject, when needed, to represent themselves 
 * on a \sa LGraphicsLayer.
 *  
 * We will call LGraphicsObject::doPrepareGeometryChange() when we are changing 
 * our shape info. 
 *  
 * LGraphicsObject will call our methods to; get the bounding rect, get the shape, 
 * and/or to have us paint ourself. 
 *  
 * The LLayer will call us to create our LGraphicsObject when being initialized 
 * for editing - *after* a \sa LGraphicsScene has been created.
 *  
 * The LLayer will call us to delete our LGraphicsObject *before* the LGraphicsScene 
 * is destroyed. Failing to do this will certianly result in bad things happening 
 * even if not observable.
 *  
 * \author pharvey (4/6/23)
 */
class LObject : public AWObject
{
    Q_OBJECT
public:
    LObject( LLayer *pParent, const QString &stringName = QString() );
    ~LObject();

    // Graphics...
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const = 0;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget ) = 0;
//    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos ) = 0;

    virtual LGraphicsObject *   getGraphics();
    virtual LGraphicsScene *    getScene();
    virtual LGraphicsView *     getView();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    virtual void doGraphicsCreate( LGraphicsLayer *pParent );
    virtual void doGraphicsCreate( LGraphicsObject *pParent );
    virtual void doGraphicsDelete();

protected slots:
    virtual void slotRefreshRender();

protected:
    LGraphicsObject *pGraphics = nullptr;
    // Rendering
    SSetting *pSettingRender;
    int nAntialiasing           = 1;
    int nTextAntialiasing       = 1;
    int nSmoothPixmapTransform  = 1;
    int nLosslessImageRendering = 1;
};

#endif
