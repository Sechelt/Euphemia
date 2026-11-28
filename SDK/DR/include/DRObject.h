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
#include <SText.h>
#include <STextContent.h>
#include <LObject.h>
#include <LHandle.h>

class DRDraw;
class DRSObject;

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

    virtual void setSelected( bool b = true ) override;
    virtual void setSelected( SelectionStates );
    virtual void setTransparentHandles( bool b = true, LHandle *pHandleGrabbed = nullptr );
    virtual void setPen( const QPen &pen );
    virtual void setBrush( const QBrush &brush );
    virtual void setTextFormat( const CBD::CBDText &text );
    virtual void setTextDisplayCode( bool bCode );
    virtual void setTextDisplayName( bool bName );
    virtual void setTextDisplayComment( bool bComment );
     
    virtual SelectionStates getSelectionState()     { return nSelectionState;   }
    virtual QImage          getImage() = 0;
    virtual QPen            getPen()                { return pen; }
    virtual QBrush          getBrush()              { return brush; }
    virtual CBD::CBDText    getTextFormat()         { return text; }
    virtual bool            getTextDisplayCode()    { return bDisplayCode; }
    virtual bool            getTextDisplayName()    { return bDisplayName; }
    virtual bool            getTextDisplayComment() { return bDisplayComment; }
    virtual QRectF          getRect() const = 0;
    virtual LHandle *       getGrab( const QPointF &pointPos, bool bSizing = false );
    virtual LHandle *       getHandle( const QPointF &pointPos );
    virtual QString         getSelectionStateString( SelectionStates n );

    virtual void doMoveBy( const QPointF & );
    virtual void doMove( LHandle *, const QPointF & );
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

    virtual bool isSelector() { return false; } /*!< concept of a selector is introduced here               */

signals:
   void signalChanged();                        /*!< echoed to app via DRDraw so app can update menus ect   */ 
   void signalPreCommit();                      /*!< so DRDraw can push to undo stack                       */
   void signalCommitted();                      /*!< so DRDraw can update modified status                   */

protected:
    // settings
    DRSObject * pSettingObject;
    // local setting values
    QPen            pen;        // line (incl. color)
    QBrush          brush;      // fill (incl. color)
    CBD::CBDText    text;       // text format

    bool bDisplayCode = false;
    bool bDisplayName = false;
    bool bDisplayComment = false;

    //
    SelectionStates     nSelectionState = SelectionStateNone;   /*!< this is independent of ADObject::bSelected but SelectionStateNone should be eq to false    */
    DRDraw *            pLayer;                                 /*!< layer we are drawing on (which is a QGraphicsObject)                                       */
    QVector<LHandle*>   vectorHandles;                          /*!< Handles are children of canvas widget and are shown on top of shape due to z-order.        */
    LHandle *           pHandle = nullptr;                      /*!< Handle being moved.                                                                        */
    int                 nHandlesAlloc = 0;                      /*!< number of handles when SelectionStateStandard                              */

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

    virtual void doConnectSettings();
    virtual void doDisconnectSettings();

    virtual void doCreateHandlesStandard() = 0;
    virtual void doCreateHandlesTransform();
    virtual void doSyncHandlesStandard() = 0;
    virtual void doSyncHandlesTransform();

protected slots:
    virtual void slotRefreshPen();
    virtual void slotRefreshBrush();
    virtual void slotRefreshTextFormat();
    virtual void slotRefreshTextContent();
};

/*!
 * \brief Common settings from all based upon DRObject.
 * 
 * This does not contain its own settings - just brings in some useful settings from elsewhere.
 * Another notable thing this provides is a QToolBox with the widgets for the settings used - pre-loaded.
 * 
 * \author pharvey (2026-11-26)
 */
class DRSObject : public SSetting 
{
    Q_OBJECT
public:
    DRSObject();

    SPen *          pPen;
    SBrush *        pBrush;
    SText *         pText;
    STextContent *  pTextContent;

    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getPanelMax( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;   // does nothing
    void doLoad( QDomElement *pdomElem ) override;                          // does nothing

};

#endif
