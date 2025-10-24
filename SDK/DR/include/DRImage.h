/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_DRImage
#define H_DRImage

#include <SSetting.h>

#include "DRRectangleBase.h"

class DRImage : public DRRectangleBase
{
    Q_OBJECT
public:
    DRImage( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRImage();

    virtual QPixmap     getIcon() override;
    virtual QImage      getImage() override;

    virtual void setImage( const QImage & );

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    SSetting *  pSettingImage;
    QImage      image;

    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    void slotRefreshImage();
};

#endif
