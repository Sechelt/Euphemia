/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_DRSelectRectangle
#define H_DRSelectRectangle

#include "DRRectangleBase.h"

class DRSelectRectangle : public DRRectangleBase
{
    Q_OBJECT
public:
    DRSelectRectangle( DRDraw *pLayer, const QPointF &pointPos = QPointF() );

    virtual void setDrawing( bool b ) { bDrawing = b; }

    virtual QPixmap     getIcon() override;
    virtual QImage      getImage() override;
    virtual QDomElement getXML( QDomDocument *pdomDoc, QDomElement *pdomElemParent );

    virtual void doMove( LHandle *, const QPointF & ) override;
    virtual void doDrop( LHandle *, const QPointF &point = QPointF() ) override;

    virtual bool isSelector() override { return true; }

protected:
    bool bDrawing = true;
    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;
};

#endif
