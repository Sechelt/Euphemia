/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRImage
#define H_DRImage

#include <DRSImage.h>

#include "DRRectangleBase.h"

/*!
 * \brief An image rendered into the rect using placement.
 * 
 * The underlying image is a QPixmap as it is best for presentation on a screen.
 * The pixmap is saved as a QImage as that is best for IO.
 * 
 * \author pharvey (2026-11-21)
 */
class DRImage : public DRRectangleBase
{
    Q_OBJECT
public:
    DRImage( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRImage();

    virtual QPixmap     getIcon() override;
    virtual QImage      getImage() override;

    virtual void setPixmap( const QPixmap & );

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    DRSImage *          pSettingImage;
    QPixmap             pixmap;
    W::WImagePlacements nPlacement;

    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    void slotRefreshImage();
};

#endif
