/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRObject
#define H_DRObject

#include "DR.h"

#include <CBDPersist.h>
#include <SPen.h>
#include <SBrush.h>
#include <SFont.h>
#include <LSGeneral.h>
#include <LObject.h>
#include <LHandle.h>

class DRDraw;

class DRObject : public LObject
{
    Q_OBJECT
public:
    // dictates what, if any, handles are available
    enum SelectionStates
    {
        SelectionStateNone,         /*!< not selected                                               */
        SelectionStateMinimal,      /*!< multi-object selection: - no DRObject handles              */
        SelectionStateStandard,     /*!< manipulation: size, move, plus class dependent             */
        SelectionStateScale,        /*!< transform: scale                                           */
        SelectionStateShear,        /*!< transform: shear                                           */
        SelectionStateRotate,       /*!< transform: rotate                                          */
        SelectionStateGradient      /*!< gradient fill: on/off/adjust                               */
    };

    DRObject( DRDraw *pLayer, const QPointF &pointPos = QPointF(), const QString &stringName = QString() ); /*!< IF pointPos.isNull() THEN assume we are going to be calling doLoad ELSE being setup for initial sizing */
    virtual ~DRObject();

    virtual QRectF boundingRect() const override;
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 ) override;

    virtual void setSelected( SelectionStates );
    virtual void setSelected( bool b = true );
    virtual void setPen( const QPen &pen );
    virtual void setBrush( const QBrush &brush );
    virtual void setFont( const CBD::CBDText &font );
     
    virtual SelectionStates getSelectionState()     { return nSelectionState;   }
    virtual QImage          getImage() = 0;
    virtual QPen            getPen()            { return pen; }
    virtual QBrush          getBrush()          { return brush; }
    virtual CBD::CBDText    getFont()           { return font; }
    virtual QRectF          getRect() const = 0;
    virtual LHandle *       getGrab( const QPointF &pointPos, bool bSizing = false );
    virtual LHandle *       getHandle( const QPointF &pointPos );
    virtual QString         getSelectionStateString();

    virtual void doMoveBy( const QPointF & ) = 0;
    virtual void doMove( LHandle *, const QPointF & ) = 0;
    virtual void doDrop( LHandle *, const QPointF &point = QPointF() ) = 0;
    virtual void doSelectionStateCycle();
    virtual void doCut() {}
    virtual void doCopy();
    virtual bool doScale( int, int, Qt::AspectRatioMode )   { return false; }
    virtual bool doFlipX()          { return false; }
    virtual bool doFlipY()          { return false; }
    virtual bool doRotate( int )    { return false; }
    virtual void doZoom();
    virtual void doScrolled();
    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

    virtual bool isSelected();
    virtual bool isSelector() { return false; }

signals:
   void signalChanged();                /*!< echoed to app via DRDraw so app can update menus ect   */ 
   void signalPreCommit();              /*!< so DRDraw can push to undo stack                       */
   void signalCommitted();              /*!< so DRDraw can update modified status                   */

protected:
    // some common settings
    SPen *      pSettingPen;
    SBrush *    pSettingBrush;
    SFont *     pSettingFont;
    LSGeneral * pSettingGeneral;
    QPen            pen;        // line (incl. color)
    QBrush          brush;      // fill (incl. color)
    CBD::CBDText    font;       // font
    bool            bAutoCommit;                /*!< mouse release automatically unselects  */

    //
    SelectionStates     nSelectionState = SelectionStateNone;
    DRDraw *            pLayer;                     /*!< layer we are drawing on (which is a QGraphicsObject)                                   */
    QVector<LHandle*>   vectorHandles;              /*!< Handles are children of canvas widget and are shown on top of shape due to z-order.    */
    LHandle *           pHandle = nullptr;          /*!< Handle being moved.                                                                    */

    // transform handle positions relative to center
    // - they need to be realtive so we can restore even if the rect has moved or changed size
    QPointF pointRotateDelta;
    QPointF pointShearHDelta;
    QPointF pointShearVDelta;
    bool    bInitTransform      = false;   // is transform init
    // transform values
    qreal nAngle    = 0;          // rotation
    qreal nShearH   = 0;          // shear horizontal
    qreal nShearV   = 0;          // shear vertical


    virtual void doDraw( QPainter *pPainter ) = 0;

    virtual LHandle *  getHandleUnder( LHandle *pHandle, LHandle::Type n );
    virtual LHandle *  getHandleNext( LHandle *pHandle, LHandle::Type n );
    virtual LHandle *  getHandlePrev( LHandle *pHandle, LHandle::Type n );

    virtual void doCreateHandles();
    virtual void doDeleteHandles();
    virtual void doSyncHandles();                   /*!< usually called after view zoom/scale change    */
    virtual void doSyncHandleTypes();
    virtual void doShowHandles( bool b = true );
    virtual void doConnectSettings();
    virtual void doDisconnectSettings();

protected slots:
    virtual void slotRefreshPen();
    virtual void slotRefreshBrush();
    virtual void slotRefreshFont();
    virtual void slotRefreshGeneral();
};

#endif
