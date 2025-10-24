/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_PATool
#define H_PATool

#include "PA.h"

#include <SSetting.h>
#include <LHandle.h>

class LGraphicsLayer;
class LGraphicsScene;
class PAPaint;

class PATool : public QGraphicsObject
{
    Q_OBJECT
public:
    enum States
    {
        StateIdle,
        StateDraw,
        StateManipulate
    };

    PATool( PAPaint *pLayer );
    virtual ~PATool();

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 ) override; /*!< redirects to doPaint unless idle*/
    QRectF boundingRect() const override;                       /*!< all PATool based objects are same size as scene - keeping it simple    */

    virtual States          getState()   { return nState;    }
    virtual QImage          getCopy()    { return QImage();  }
    virtual QRect           getRect() = 0;                               /*!< the rect within boundingRect that we are actually using                    */

    virtual void doDoubleClick( PMouseEvent *pEvent ) = 0;      /*!< mouse press from canvas                                                    */
    virtual void doPress( PMouseEvent *pEvent ) = 0;            /*!< mouse press from pLayer                                                    */
    virtual void doMove( PMouseEvent *pEvent ) = 0;             /*!< mouse move from canvas                                                     */
    virtual void doRelease( PMouseEvent *pEvent ) = 0;          /*!< mouse release from canvas                                                  */
    virtual void doCommit() = 0;                                /*!< paint shape on QImage                                                      */
    virtual void doCancel();                                    /*!< reset state to StateIdle                                                   */
    virtual void doCut() {}
    virtual void doCopy();
    virtual void doDelete() {}
    virtual bool doScale( qreal, qreal, Qt::AspectRatioMode )   { return false; }
    virtual bool doFlipX()          { return false; }
    virtual bool doFlipY()          { return false; }
    virtual bool doRotate( int )    { return false; }
    virtual void doZoomChanged( int nZoom );
    virtual void doScrollChanged( int nX, int nY );

    virtual bool    canCommit() { return nState == StateManipulate; }
    virtual bool    canCancel() { return nState != StateIdle; }
    virtual bool    canCut()    { return false; }
    virtual bool    canCopy()   { return nState == StateManipulate; }           /*!< draw/paste shape (copy from doPaint) select shape (copy from canvas) */
    virtual bool    canDelete() { return false; }

    virtual bool    isSelector() { return false; }

signals:
   void signalChanged();        /*!< changed state: echoed to app via canvas so app can update menus ect   */ 
   void signalPreCommit();      /*!< so canvas can push to undo stack                       */
   void signalCommitted();      /*!< so canvas can update modified status                   */

protected:
    // pen
    SSetting *          pSettingPen;
    QPen                pen;
    // brush
    SSetting *          pSettingBrush;
    QBrush              brush;
    // font
    SSetting *          pSettingFont;
    QFont               font;
    // general
    SSetting *          pSettingGeneral;
    bool                bAutoCommit;
    // Rendering
    SSetting *pSettingRender;
    int nAntialiasing           = 1;
    int nTextAntialiasing       = 1;
    int nSmoothPixmapTransform  = 1;
    int nLosslessImageRendering = 1;

    //
    States              nState = StateIdle;
    PAPaint *            pLayer;                     /*!< layer we are drawing on                                                                */
    LGraphicsLayer *    pLayerGraphics;             /*!< QGraphicsObject for layer                                                              */
    LGraphicsScene *    pLayerScene;                /*!< QGraphicsScene for layer                                                               */
    QGraphicsView *     pLayerView;                 /*!< view with focus - handles belong here - in pLayer but we use it a lot so its here      */
    QVector<LHandle*>   vectorHandles;              /*!< Handles are children of canvas widget and are shown on top of shape due to z-order.    */
    LHandle *           pHandle = nullptr;          /*!< Handle being moved.                                                                    */

    virtual void doPaint( QPainter *pPainter, bool bCommit = true ) = 0;

    virtual LHandle *  getHandle( const QPoint &pointPos );
    virtual LHandle *  getHandleUnder( LHandle *pHandle, LHandle::Type n );
    virtual LHandle *  getHandleNext( LHandle *pHandle, LHandle::Type n );
    virtual LHandle *  getHandlePrev( LHandle *pHandle, LHandle::Type n );
    virtual QImage     getMask() { return QImage();  }
    virtual QImage     getTrimmed( const QRect &r, QImage &image, const QImage &imageMask, bool bErase = false );

    // state changes
    virtual void doDrawState( const QPoint & ) = 0;
    virtual void doManipulateState() = 0;
    virtual void doIdleState() = 0;

    virtual void doCreateHandles() = 0;
    virtual void doDeleteHandles();
    virtual void doSyncHandles() = 0;                   /*!< usually called after view zoom/scale change    */
    virtual void doShowHandles( bool b = true );

protected slots:
    virtual void slotRefreshPen();
    virtual void slotRefreshBrush();
    virtual void slotRefreshFont();
    virtual void slotRefreshGeneral();
    virtual void slotRefreshRender();
};

#endif
