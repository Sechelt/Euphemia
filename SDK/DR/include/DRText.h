/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRText
#define H_DRText

#include <SSetting.h>

#include "DRRectangleBase.h"

class DRText : public DRRectangleBase
{
    Q_OBJECT
public:
    DRText( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap     getIcon() override;

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    SSetting *pSettingText;
    QString stringText;
    Qt::AlignmentFlag nHAlign = Qt::AlignHCenter;
    Qt::AlignmentFlag nVAlign = Qt::AlignVCenter;

    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    void slotRefreshText();
};

#endif
