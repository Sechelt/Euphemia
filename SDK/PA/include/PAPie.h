/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPie
#define H_PAPie

#include "PARectangle.h"

/*!
 * \brief Draw and Arc. 
 *  
 * This acts like PAEllipse. It is sized and moved by sizing and moving a virtual rectangle. 
 *  
 * This has two additional handles; Start and End angle.
 * The arc drawn is the part of the ellipse that exists between Start and End angles. 
 *  
 * The arc is always drawn clockwise. That is to say; the span is always positive. 
 * 
 * \author pharvey (2/28/23)
 */
class PAPie : public PARectangle
{
    Q_OBJECT
public:
    PAPie( PAPaint *pLayer );

    virtual void doMove( PMouseEvent *pEvent ) override; 
    virtual void doRelease( PMouseEvent *pEvent ) override;

protected:
    // keeping track of points here simplifies some operations
    QPoint pointStart;      /*!< last handle pos for nStartAngle                                            */
    QPoint pointStop;       /*!< last handle pos for nSpan                                                  */
    int nStartAngle;        /*!< start angle in degrees. 0 = east                                           */
    int nEndAngle;          /*!< end angle in degrees. 180 = west                                           */

    virtual void doPaint( QPainter *, bool bCommit = true );
    virtual void doDrawState( const QPoint & ) override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
    virtual void doMoveHandle( const QPoint &pointPos );
};


#endif
