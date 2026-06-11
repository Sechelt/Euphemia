/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAPaint
#define H_PAPaint

#include <LFile.h>
#include <LLayer.h>

#include "PAPasteRectangle.h"
#include "PAPixelSelection.h"
#include "PASelectEllipse.h"
#include "PASelectPolygon.h"
#include "PASelectRectangle.h"
#include "PAArc.h"
#include "PAChord.h"
#include "PAPie.h"
#include "PAFreeHand.h"
#include "PAErase.h"
#include "PASpray.h"
#include "PALine.h"
#include "PAPolygon.h"
#include "PAPolygonFilled.h"
#include "PAPolyline.h"
#include "PARectangle.h"
#include "PARectangleFilled.h"
#include "PAEllipse.h"
#include "PAEllipseFilled.h"
#include "PAText.h"
#include "PAFloodFill.h"
#include "PAGradientFill.h"

/*!
 * \brief A layer which facilitates viewing/editing of a QImage.
 *  
 * This layer has most of the logic for painting within it. The scene just passes the events (mouse events) 
 * to this layer via \sa LFile. 
 *  
 * The; scene, background, canvas and image are all expected to be the same size. The image can change size 
 * (ie after being loaded from a file or by a request to crop). In this case the others are synced to 
 * maintain the same size for all. 
 *  
 * Precision (double vs int)
 *  
 * The scene uses 'double' precision while a QImage is 'int' precision. The painting functionality is 'int' 
 * precision - inheriting the QImage limitation. 
 *  
 * The incoming 'double' coordinates (from mouse events) are converted to 'int' precision for internal use.
 * The outgoing 'int' coordinates (provided by boundingRect()) are converted to 'double'.
 *  
 * Fortunately; QPainter accepts double or int precision. 
 * 
 * The \sa signalToolConfig will be emitted when;
 * - tool selected/unselected
 * 
 * \sa LFile 
 *  
 * \author pharvey (2/10/23)
 */
class PAPaint : public LLayer
{
    Q_OBJECT
public:
    enum Tools
    {
        ToolMagnifierSelection,         /*!< shape: select rectangle, can move but size set elsewhere*/ 
        ToolSelectRectangle,            /*!< shape: select using a rectangle                         */ 
        ToolSelectEllipse,              /*!< shape: select using an ellipse                          */ 
        ToolSelectPolygon,              /*!< shape: select using a polygon                           */ 
        ToolDrawArc,                    /*!< shape: draw an arc                                      */ 
        ToolDrawChord,                  /*!< shape: draw a chord (an arc that is enclosed and filled)*/
        ToolDrawPie,                    /*!< shape: draw a pie                                       */
        ToolDrawFreeHand,               /*!< free: scribble                                          */ 
        ToolDrawSpray,                  /*!< free: emulate a spray can                               */ 
        ToolDrawErase,                  /*!< free: erase to expose background                        */ 
        ToolDrawLine,                   /*!< shape: draw a straight line                             */ 
        ToolDrawRectangle,              /*!< shape: draw an empty rectangle                          */ 
        ToolDrawEllipse,                /*!< shape: draw an empty ellipse                            */ 
        ToolDrawPolygon,                /*!< shape: draw an empty polygon                            */ 
        ToolDrawPolyline,               /*!< shape: draw polyline                                    */ 
        ToolDrawRectangleFilled,        /*!< shape: draw a filled rectangle                          */ 
        ToolDrawEllipseFilled,          /*!< shape: draw a filled ellipse                            */ 
        ToolDrawPolygonFilled,          /*!< shape: draw a filled polygon                            */ 
        ToolDrawText,                   /*!< shape: draw a text                                      */ 
        ToolFillFlood,                  /*!< fill: flood fill with; solid color, pattern or texture  */ 
        ToolFillGradient                /*!< fill: fill with gradient                                */ 
    };

    PAPaint( LFile *pParent, const QString &stringName = QString() );

    void setImage( const QImage & ) override;
    void setTool( int n, bool bCancelDrawing = true );

    QPixmap             getIcon() override;
    LayerTypes          getLayerType() override     { return LLayer::LayerTypeRaster;    }
    TransportTypes      getTransportType() override { return LLayer::TransportTypeImage; }
    QVector<LTool>      getTools() override;
    QImage              getImage() override         { return image;                 }
    QImage *            getImageRef() override      { return &image;                }
    QImage              getCopyImage() override;
    QDomDocument        getCopyXML() override       { return QDomDocument();        }
    int                 getTool() override          { return nTool;                 }
    QWidget *           getLayerPanel( QWidget * ) override     { return nullptr; }
    QWidget *           getToolBar( QWidget * ) override;
    QWidget *           getPanel( QWidget * ) override;

    void doMousePressEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent ) override;
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
    void doPaste( QDomElement * ) override {}
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
    void slotRefreshImage();
    void slotPreCommit();
    void slotCommitted();

protected:
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 ) override;
    void doToolDelete();
    void doToolNew();
    void doToolNew( const QImage & );

    bool isBackgroundTransparent() { return colorBackground.alpha() == 0; }

    //
    SSetting *pSettingImage     = nullptr;
    Qt::ImageConversionFlags flagsImageConversion = Qt::AutoColor;

    Tools           nTool       = ToolDrawLine;
    QImage          image;   
    int             nMaxUndo    = 10;
    QStack<QImage>  stackUndo;  
    QStack<QImage>  stackRedo;  
    PATool *        pTool       = nullptr;                          /*!< active shape (or not)          */
};

#endif


