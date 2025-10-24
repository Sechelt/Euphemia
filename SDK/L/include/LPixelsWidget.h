/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LPixelsWidget
#define H_LPixelsWidget

#include <SSetting.h>

class LFile;
class LLayer;

class LPixelsWidget : public QWidget
{
    Q_OBJECT
public:
    LPixelsWidget( QWidget *pParent );

    void setLayers( LFile *p );

protected:
    SSetting *  pSettingPen;
    SSetting *  pSettingBackground;
    QPen        pen;
    QBrush      brushTransparency;

    // We want to avoid undo/redo for every pixel change so we group.
    // Everytime we get a slotPixelSelectionMoved we reset this. 
    bool        bPushUndo   = true;

    LFile *     pLayers     = nullptr;
    LLayer *    pLayer      = nullptr;
    QImage *    pImage      = nullptr;
    QPoint      pointTopLeft;
    // set in paintEvent()
    int nXImage;
    int nYImage;
    int nSize       = 16;
    int nXCells;
    int nYCells;

    void mousePressEvent( QMouseEvent *pEvent ) override;
    void mouseMoveEvent( QMouseEvent *pEvent ) override;
    void mouseReleaseEvent( QMouseEvent *pEvent ) override;
    void paintEvent( QPaintEvent *pEvent ) override;
    void wheelEvent( QWheelEvent *pEvent ) override;

    void doCalcMatrix();

    QPoint mapCellToScene( const QPoint & );
    QPoint mapWidgetToCell( const QPoint & );

    void doConnectLayers( LFile * );
    void doDisconnectLayers();
    void doConnectLayer( LLayer * );
    void doDisconnectLayer();

protected slots:
    void slotRefreshPen();
    void slotRefreshBackground();
    //
    void slotSelectionMoved( const QPoint &pointTopLeft );
    void slotRefreshImage();
    void slotChangedCurrent();
};

#endif 

