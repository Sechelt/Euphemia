/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRRectangle
#define H_DRRectangle

#include <SSetting.h>

#include "DRRectangleBase.h"

class DRRectangle : public DRRectangleBase
{
    Q_OBJECT
public:
    DRRectangle( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRRectangle();

    virtual QPixmap     getIcon() override;
    virtual QRectF      getRect() const override { return r.normalized(); }
    virtual QImage      getImage() override;

    virtual void        doMove( LHandle *, const QPointF & ) override;
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    Qt::SizeMode    nSizeMode   = Qt::AbsoluteSize;
    qreal           nXRadius    = 0;
    qreal           nYRadius    = 0;

    qreal   getXRadius( const QPointF &point );
    qreal   getYRadius( const QPointF &point );
    qreal   getXRadiusValidated( qreal nXRadius );
    qreal   getYRadiusValidated( qreal nXRadius );
    QPointF getXRadiusPoint( qreal nXRadius );
    QPointF getYRadiusPoint( qreal nYRadius );

    virtual void doDraw( QPainter * ) override;

    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;


protected slots:
    virtual void slotRefreshRectangle() override;
};

#endif
