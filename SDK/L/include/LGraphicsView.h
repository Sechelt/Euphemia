/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LGraphicsView
#define H_LGraphicsView

#include <SSetting.h>
#include <WZoomWidget.h>

class LFile;

/*!
 * \brief A graphics view for LGraphicsScene. 
 *  
 * re. scale
 *  
 * We always scale the same on X & Y axis so we only use a single qreal.
 * Only use slotZoomChanged to set scale. 
 * Use getScale to get the single qreal.
 *  
 * re. zoom
 *  
 * This can work with a WZoomWidget. The WZoomWidget can be used to set the zoom 
 * by connecting to slotZoomChanged(). This will call setScale. 
 *  
 * getZoom() and getZoomFit() can be used to sync WZoomWidget when this 
 * view has become the active view.
 *  
 * \author pharvey (2/8/23)
 */
class LGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    LGraphicsView( LFile *pFile, QWidget *pParent = nullptr );

    LFile *                 getLayers() { return pFile; }
    QWidget *               getViewportTop();
    WZoomWidget::FitTypes   getZoomFit()    { return nZoomFit;  }
    int                     getZoom()       { return nZoom;     }

    QPoint                  getTopLeft();       // of scene mapped to view
    QPoint                  getBottomRight();   // of scene mapped to view
    QPointF                 getScale();

    qreal                   getOffsetH();
    qreal                   getScaleH();
    qreal                   getLengthH();
    qreal                   getOffsetV();
    qreal                   getScaleV();
    qreal                   getLengthV();

signals:
    void signalZoomChanged( WZoomWidget::FitTypes nFit, int nZoom );
    void signalChangedH( qreal nOffset, qreal nScale, qreal nLength );
    void signalChangedV( qreal nOffset, qreal nScale, qreal nLength );
    void signalChanged( const QPoint &pointTopLeft, const QPoint &pointBottomRight, const QPointF &pointScale );

public slots:
    void slotZoomChanged( WZoomWidget::FitTypes nFit, int nZoom );
    void slotSceneRectChanged( const QRectF & );
 
protected slots:
    void slotRefreshRender();
    void slotRefreshPointer();

protected:
    // settings: refs
    SSetting *              pSettingRender;
    SSetting *              pSettingPointer;
    //
    WZoomWidget::FitTypes   nZoomFit    = WZoomWidget::FitIgnore;
    int                     nZoom       = 100; // % inc/dec by 10
    LFile *                 pFile       = nullptr;

    void setScale( qreal n );

    void resizeEvent( QResizeEvent * ) override;
    void scrollContentsBy(int dx, int dy) override;

    // The following is probably best done with QGraphicsScene::drawForeground.
    // Leave comment to remind us.
    // Implementing this will stop QGraphicsScene::drawForeground from being called.
    // void drawForeground( QPainter *pPainter, const QRectF &r ) override;
};

#endif


