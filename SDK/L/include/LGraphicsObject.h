/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LGraphicsObject
#define H_LGraphicsObject

#include <ADObject.h>
#include <SSetting.h>

class LGraphicsLayer;
class LObject;

/*!
 * \brief A QGraphicsObject which reperesents a \sa LObject based object.
 *  
 * \author pharvey (4/6/23)
 */
class LGraphicsObject : public QGraphicsObject, public QGraphicsLayoutItem
{
    Q_OBJECT
public:
    LGraphicsObject( LObject *p, LGraphicsLayer *pParent = nullptr );
    LGraphicsObject( LObject *p, LGraphicsObject *pParent = nullptr );

    virtual LObject *           getObject() { return pObject; }
    // virtual QGraphicsObject *   getItem();

    virtual void doToBack();
    virtual void doToFront();
    virtual void doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBefore( LObject *pObject );
    virtual void doToAfter( LObject *pObject );
    virtual void doReparent( ADObject *pObject );
    virtual void doPrepareGeometryChange();

    // QGraphicsObject
    virtual QRectF boundingRect() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
    virtual QPainterPath shape() const;

    // QGraphicsLayoutItem
    virtual QSizeF sizeHint( Qt::SizeHint nSizeHint, const QSizeF &sizeConstraint = QSizeF() ) const;

protected slots:
    virtual void slotRefreshRender();

protected:
    LObject *   pObject = nullptr;
    // Rendering
    SSetting *pSettingRender;
    int nAntialiasing           = 1;
    int nTextAntialiasing       = 1;
    int nSmoothPixmapTransform  = 1;
    int nLosslessImageRendering = 1;
};

#endif
