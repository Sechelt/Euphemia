/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRArrow
#define H_DRArrow

#include <SArrow.h>

#include "DRLine.h"

class DRArrow : public DRLine
{
    Q_OBJECT
public:
    DRArrow( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRArrow();

    virtual QPixmap     getIcon() override;

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    SArrow *pSettingArrow;
    qreal   nArrowSize = 6;

    virtual void doDraw( QPainter * ) override;
    virtual void doDrawArrow( QPainter *pPainter, const QLineF &lineSegment );
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    virtual void slotRefreshArrow();
};

#endif
