/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_DRDraw
#define H_DRDraw

#include <LFile.h>
#include <LLayer.h>

#include "DRSelectPolygon.h"
#include "DRSelectRectangle.h"
#include "DRArc.h"
#include "DRChord.h"
#include "DRImage.h"
#include "DRPie.h"
#include "DRLine.h"
#include "DRPolygon.h"
#include "DRPolyline.h"
#include "DRRectangle.h"
#include "DREllipse.h"
#include "DRText.h"
// #include "DRGroup.h"

/*!
 * \brief A layer which allows 2D/Vector drawing.
 * 
 * This layer has most of the logic for drawing within it. The scene just passes the events (mouse events) 
 * to this layer via \sa LFile. 
 * 
 * The \sa signalToolConfig will be emitted when;
 * - tool selected/unselected
 * - an object is selected/unselected
 * 
 * \sa LFile 
 * 
 * \author pharvey (5/2/23)
 */
class DRDraw : public LLayer
{
    Q_OBJECT
public:
    enum Tools
    {
        ToolSelectRectangle,
        ToolSelectPolygon, 
        ToolManipulate,                 /*!< select shape(s) and then cycle through states (manipulate, transform:size, transform:rotate, transform:shear) */
        ToolDrawArc,                    /*!< shape: draw an arc                                      */ 
        ToolDrawChord,                  /*!< shape: draw a chord (an arc that is enclosed and filled)*/
        ToolDrawImage,                  /*!< shape: draw an image                                    */
        ToolDrawPie,                    /*!< shape: draw a pie                                       */
        ToolDrawLine,                   /*!< shape: draw a straight line                             */ 
        ToolDrawRectangle,              /*!< shape: draw an empty rectangle                          */ 
        ToolDrawEllipse,                /*!< shape: draw an empty ellipse                            */ 
        ToolDrawPolygon,                /*!< shape: draw an empty polygon                            */ 
        ToolDrawPolyline,               /*!< shape: draw polyline                                    */ 
        ToolDrawText                    /*!< shape: draw a text                                      */ 
    };

    enum MouseEventStates
    {
        MouseEventStateSelectRectangle,
        MouseEventStateSelectPolygon,
        MouseEventStateManipulate,
        MouseEventStateCreate            /*!< nTool == ToolDraw* and 0 object selected               */
    };

    DRDraw( LFile *pParent, const QString &stringName = QString() );

    void setImage( const QImage & ) override;
    void setTool( int n, bool bCancelDrawing = true );

    QPixmap             getIcon() override;
    LayerTypes          getLayerType() override     { return LLayer::LayerTypeRaster;    }
    TransportTypes      getTransportType() override { return LLayer::TransportTypeImage; }
    QVector<LTool>      getTools() override;
    QImage              getImage() override;
    QImage *            getImageRef() override      { return &image;                }
    QImage              getCopyImage() override;
    QDomDocument        getCopyXML() override;
    int                 getTool() override          { return nTool;                 }
    QWidget *           getLayerPanel( QWidget * ) override     { return nullptr; }
    QWidget *           getToolBar( QWidget * ) override;
    QWidget *           getPanel( QWidget * ) override;
    ADObject *          getObject( const QString &s, ADObject * ) override;
    QString             getToolString();
    QString             getMouseEventStateString();
    QVector<DRObject*>  getSelected() { return vectorSelected; }
    QRectF              getSelectedBoundingRect();

    void doMousePressEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent ) override;

    void doMousePressEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventManipulate( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventCreate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventCreate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventCreate( QGraphicsSceneMouseEvent *pEvent );

    void doDragEnterEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void doDragMoveEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void doDropEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void doContextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent ) override;
    void doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    // save/load
    QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    bool        doLoad( QDomElement *pdomElemLine ) override;
    void doCut() override;
    void doCopy() override;
    void doPaste() override;
    void doPaste( const QImage & ) override;
    void doPaste( QDomElement * ) override;
    void doUndo() override;
    void doRedo() override;
    void doSelectAll() override;
    void doSelectNone() override;
    void doDelete() override;
    void doCommit() override;
    void doCancel() override;
    void doCrop() override;
    void doScale( qreal nX, qreal nY, Qt::AspectRatioMode n = Qt::IgnoreAspectRatio ) override;
    void doFlipX() override;
    void doFlipY() override;
    void doRotate( int nDegrees ) override;
    void doClear();
    void doPushUndo() override;
    void doZoomChanged( int nZoom ) override;
    void doScrollChanged( int nX, int nY ) override;

    bool isDrawing() override;

    bool hasSelection() override;
    bool hasXML() override      { return true;  }
    bool hasImage() override    { return true;  }

    bool canCut() override; 
    bool canCopy() override; 
    bool canPaste() override; 
    bool canUndo() override; 
    bool canRedo() override; 
    bool canSelectAll() override;
    bool canSelectNone() override;
    bool canDelete() override;
    bool canCommit() override;
    bool canCancel() override;
    bool canCrop() override { return canCopy(); } 
    bool canScale() override;
    bool canFlipX() override;
    bool canFlipY() override;
    bool canRotate() override;

protected slots:
    void slotSceneRectChanged( const QRectF &r );
    void slotPreCommit();
    void slotCommitted();

protected:
    void paint( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) override {}; // DRObjects paint themselves, the rest is transparent, and thats enough

    void setSelected( QList<DRObject*>, bool b = true );
    void setSelected( DRObject*, bool b = true );
    void setSelected( DRObject*, DRObject::SelectionStates );

    DRObject *  getObject( const QPointF & );
    DRObject *  getObject( Tools, const QPointF &point = QPointF() );
    DRObject *  getObject( const QImage & );
    QList<DRObject*>getObjects( const QRectF & );
    LHandle *   getHandle( const QPointF & );

    void doSelectAll( const QRectF & );
    void doSelectAll( const QPolygonF & );
    void doSelectNone2();

    // begin - possibly get rid of these
    bool isBackgroundTransparent() { return colorBackground.alpha() == 0; }
    // end

    // state
    Tools               nTool               = ToolManipulate;               /*!< manipulate tool                */
    MouseEventStates    nMouseEventState    = MouseEventStateManipulate;    /*!< ready to manipulate            */
    // state modifiers
    bool            bMoving     = false;    /*!< true if Press was followed by a Move                           */
    bool            bTracking   = false;    /*!< true if we are initializing a polyline based object            */
    bool            bDrawing    = false;    /*!< true if we are initializing a new object                       */

    QImage              image;      
    int                 nMaxUndo    = 10;
    QStack<QImage>      stackUndo;  
    QStack<QImage>      stackRedo;  
    QVector<DRObject*>  vectorSelected;
    //
    QPointF         pointPress;             /*!< point when Press                                               */
    QPointF         pointCurrent;           /*!< store mouse pos in case mouse goes off scene (or even view)    */
    DRObject *      pObject     = nullptr;  /*!< shape being created/manipulated (can be a DRSelect*)           */
    LHandle *       pHandle     = nullptr;  /*!< handle being moved                                             */
};

#endif





