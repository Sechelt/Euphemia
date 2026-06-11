/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAFreeHand
#define H_PAFreeHand

#include <PASFreeHand.h>

#include "PAToolFree.h"

class PAFreeHand : public PAToolFree
{
    Q_OBJECT
public:
    PAFreeHand( PAPaint *pLayer );

    virtual QRect getRect() { return r; }

    void doPress( PMouseEvent *pEvent ) override;
    void doMove( PMouseEvent *pEvent ) override;
    void doRelease( PMouseEvent *pEvent ) override;

protected:
    SSetting *          pSettingFreeHand;
    PASFreeHand::Shapes   nShape;
    QSize               size;
    QImage              image;
    bool                bLineTo;

    Qt::MouseButton button  = Qt::NoButton;
    QPoint          pointLast;
    QRect           r;              /*!< cursor rect                                                            */
    QImage          imagePen;

    QImage  getPenImage();
    QSize   getPenSize();

    void doPaint( QPainter *pPainter, bool bCommit = true );

    void doDraw( QPainter *pPainter, const QRect & );
    void doDrawPen( QPainter *pPainter, const QRect & );
    void doDrawEllipse( QPainter *pPainter, const QRect & );
    void doDrawRectangle( QPainter *pPainter, const QRect & );
    void doDrawCross( QPainter *pPainter, const QRect & );
    void doDrawImage( QPainter *pPainter, const QRect & );

    void doDrawState( const QPoint & );
    void doIdleState();

protected slots:
    void slotRefreshPen();
    void slotRefreshBrush();
    void slotRefreshFreeHand();
};

#endif
