/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPasteRectangle
#define H_PAPasteRectangle

#include <SSetting.h>

#include "PARectangle.h"

class PAPasteRectangle : public PARectangle
{
    Q_OBJECT
public:
    PAPasteRectangle( PAPaint *pLayer );
    PAPasteRectangle( PAPaint *pLayer, const QImage & );

    virtual void setImage( const QImage & );

    virtual void doPress( PMouseEvent *pEvent ) override;
    virtual void doCommit() override;
    virtual bool doScale( qreal nX, qreal nY, Qt::AspectRatioMode nRatio ) override;
    virtual bool doFlipX() override;
    virtual bool doFlipY() override;
    virtual bool doRotate( int nDegrees ) override;

    virtual bool canCommit() override;
    virtual bool canCancel() override;

protected:
    // settings
    SSetting *pSettingPaste;
    QPainter::CompositionMode nCompositionMode = QPainter::CompositionMode_SourceOver;
    bool bStamp = false;

    //
    QImage image;

    virtual void doPaint( QPainter *, bool bCommit = true ) override;
    virtual void doDrawState( const QPoint &point ) override;

protected slots:
    void slotRefreshPaste();
};

#endif
