/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAErase
#define H_PAErase

#include <PASErase.h>

#include "PAToolFree.h"

class PAErase : public PAToolFree
{
    Q_OBJECT
public:
    PAErase( PAPaint *pLayer );

    virtual QRect getRect() { return r; }

    void doPress( PMouseEvent *pEvent ) override;
    void doMove( PMouseEvent *pEvent ) override;
    void doRelease( PMouseEvent *pEvent ) override;

protected:
    // settings: refs
    SSetting *pSettingErase;
    // settings: copies - which may get edited here
    PASErase::Shapes nShape;
    QSize   size;
    QImage  image;  
    //
    Qt::MouseButton button  = Qt::NoButton;
    QRect               r;                  /*!< cursor rect                                                            */
    QColor              colorTransparent;   /*!< fully transparent (Qt::transparent is not quite fully transparent)     */
    int                 nWidth;             /*!< width - from current pen config                                        */
    QImage              imageEraser;

    QImage  getEraserImage();
    QSize   getEraserSize();

    void doPaint( QPainter *pPainter, bool bCommit = true );

    void doErase( QPainter *pPainter, const QRect & );
    void doErasePen( QPainter *pPainter, const QRect & );
    void doEraseEllipse( QPainter *pPainter, const QRect & );
    void doEraseRectangle( QPainter *pPainter, const QRect & );
    void doEraseCross( QPainter *pPainter, const QRect & );
    void doEraseImage( QPainter *pPainter, const QRect & );

    void doDrawState( const QPoint & );
    void doIdleState();

protected slots:
    void slotRefreshPen();
    void slotRefreshErase();

private:
    QImage *getMask( QImage *pImage );

    void doErase( const QRect &rect, const QImage &imageMask, QImage *pImageCanvas, const QColor &colorMask );
};

#endif
