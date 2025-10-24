/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PAFloodFill
#define H_PAFloodFill

#include "PAToolFree.h"

class PAFloodFill : public PAToolFree
{
    Q_OBJECT
public:
    PAFloodFill( PAPaint *pLayer );

    virtual QRect getRect() { return r; }

    void doPress( PMouseEvent *pEvent ) override;
    void doMove( PMouseEvent *pEvent ) override;
    void doRelease( PMouseEvent *pEvent ) override;

protected:
    QRect       r;                  /*!< cursor rect                                                            */
    QImage *    pImage = nullptr;

    void doPaint( QPainter *pPainter, bool bCommit = true ) override;

    void doDrawState( const QPoint & ) override {}
    void doIdleState() override {} 

private:
    void doFillFlood( const QPoint &pointSeed );    
    void doFillFloodColor( const QPoint &pointSeed );    
    void doFillFloodTiled( const QPoint &pointSeed );
    void doFillGradient( const QPoint &pointSeed );    
};

#endif
