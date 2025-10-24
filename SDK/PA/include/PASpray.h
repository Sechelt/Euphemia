/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PASpray
#define H_PASpray

#include <SSetting.h>

#include "PAToolFree.h"

class PASpray : public PAToolFree
{
    Q_OBJECT
public:
    PASpray( PAPaint *pLayer );

    virtual QRect getRect() { return r; }

    virtual void doPress( PMouseEvent *pEvent ) override;
    virtual void doMove( PMouseEvent *pEvent ) override;
    virtual void doRelease( PMouseEvent *pEvent ) override;

protected:
    SSetting *      pSettingSpray;
    int             nRadius = 10;
    int             nPoints = 5;

    Qt::MouseButton button  = Qt::NoButton;
    QRect           r;      /*!< cursor rect                                                            */

    void doPaint( QPainter *pPainter, bool bCommit = true );

    void doSpray( const QPoint & );

    void doDrawState( const QPoint & );
    void doIdleState();

protected slots:
    void slotRefreshSpray();

private:
    double getRandom();
};

#endif
