/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_DRArc
#define H_DRArc

#include "DRRectangleBase.h"

/*!
 * \brief Draw and Arc. 
 *  
 * This acts like DREllipse. It is sized and moved by sizing and moving a virtual rectangle. 
 *  
 * This has two additional handles; Start and End angle.
 * The arc drawn is the part of the ellipse that exists between Start and End angles. 
 *  
 * The arc is always drawn clockwise. That is to say; the span is always positive. 
 * 
 * \author pharvey (2/28/23)
 */
class DRArc : public DRRectangleBase
{
    Q_OBJECT
public:
    DRArc( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap     getIcon() override;

    virtual void        doMove( LHandle *, const QPointF & ) override;
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    // keeping track of points here simplifies some operations
    QPointF pointStart;     /*!< last handle pos for nStartAngle                                            */
    QPointF pointStop;      /*!< last handle pos for nSpan                                                  */
    int nStartAngle;        /*!< start angle in degrees. 0 = east                                           */
    int nEndAngle;          /*!< end angle in degrees. 180 = west                                           */

    virtual void doDraw( QPainter * ) override;
    virtual void doCreateHandles() override;
    virtual void doSyncHandles() override;
};


#endif
