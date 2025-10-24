/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LPanner
#define H_LPanner

#include <CBD.h>

/*!
 * \brief A view which allows scrolling the main view by dragging a small rectangle (representing its viewport). 
 *  
 * It does this by manipulating the main views scroll bars. 
 * Accounts for scaling in the main view and the panners scaling. 
 *  
 * Also known as; 'birds eye view' or 'mini map'. 
 * 
 * \author pharvey (5/1/23)
 */
class LPanner : public QGraphicsView
{
    Q_OBJECT
public:
    LPanner( QGraphicsScene *pScene, QWidget *pParent );
    ~LPanner();

    QGraphicsView *getView(); // gets the main view

public slots:
    virtual void slotUpdate();

protected:
    bool bPanning;
    QPoint pointLast;

    // events
    void resizeEvent( QResizeEvent *pEvent );
    void mousePressEvent( QMouseEvent *pMouseEvent );
    void mouseMoveEvent( QMouseEvent *pMouseEvent );
    void mouseReleaseEvent( QMouseEvent *pMouseEvent );

    void drawForeground( QPainter *pPainter, const QRectF &rect );
};

#endif


