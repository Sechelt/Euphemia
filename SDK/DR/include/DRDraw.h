/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRDraw
#define H_DRDraw

#include <ADObjectSelectionManager.h>

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
    /*!
     * \brief Supported tools.
     * 
     * A tool is usually selected by the User but ToolManipulate will be selected when ToolSelectRectangle/ToolSelectPolygon results in a selection.
     * 
     * \sa nTool
     */
    enum Tools
    {
        // IMPORTANT: must start at 0 and order important see; getToolBox
        ToolSelectRectangle = 0, 
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

    /*!
     * \brief A tool can be in one of these states.
     * \sa nToolState
     */
    enum ToolStates
    {
        ToolStateIdle,                  /*!< tool selected but not drawing (!pObjectDrawing)                   */
        ToolStateDraw,                  /*!< we are drawing a new pObjectDrawing                               */
        ToolStateDrawManipulate,        /*!< we are manipulating a new pObjectDrawing                          */
        ToolStateManipulate             /*!< manipulating a selected object(s) (!pObjectDrawing)               */
    };

    DRDraw( LFile *pParent, const QString &stringName = QString() );
    virtual ~DRDraw();

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
    QWidget *           getToolBox( QWidget * ) override;
    ADObject *          getObject( const QString &s, ADObject * ) override;
    QString             getToolString( Tools n );
    QString             getToolStateString( ToolStates n );
    QVector<DRObject*>  getSelected();
    QRectF              getSelectedBoundingRect();

    void doMousePressEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent ) override;

    void doMousePressEventIdle( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventIdle( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventIdle( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventDraw( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventDraw( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventDraw( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent );

    void doMousePressEventManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseMoveEventManipulate( QGraphicsSceneMouseEvent *pEvent );
    void doMouseReleaseEventManipulate( QGraphicsSceneMouseEvent *pEvent );

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
    void doCommit( bool bSelectNone = true ) override;
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
    bool isTracking();
    bool isToolDraw()               { return (!isToolSelector()); }
    bool isToolSelector()           { return (isToolSelectorArea() || nTool == ToolManipulate); };
    bool isToolSelectorArea()       { return (nTool == ToolSelectRectangle || nTool == ToolSelectPolygon); }
    bool isToolPolyline()           { return (nTool == ToolSelectPolygon || nTool == ToolDrawPolygon || nTool == ToolDrawPolyline); }
    bool isSelectionToolConfig() override;

    bool hasSelection() override;
    bool hasSelector();
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
    // state - start
    Tools               nTool               = ToolManipulate;               
    ToolStates          nToolState          = ToolStateIdle;                /*!< no pObjectDrawing */

    // settings
    SSetting *          pSettingGeneral;
    bool                bAutoCommit;

    //
    QImage                      image;      
    int                         nMaxUndo    = 10;
    QStack<QImage>              stackUndo;  
    QStack<QImage>              stackRedo;  
    ADObjectSelectionManager *  pSelectionManager;  /*!< use a selection manager to select/unselect objects DO NOT SELECT OBJECTS DIRECTLY  */

    //
    QPointF         pointPress;                     /*!< point when Press                                                                   */
    QPointF         pointCurrent;                   /*!< store mouse pos in case mouse goes off scene (or even view)                        */
    DRObject *      pObjectPress        = nullptr;  /*!< object clicked on (if any)                                                         */
    LHandle *       pHandlePress        = nullptr;  /*!< handle clicked on (if any)                                                         */
    DRObject *      pObjectDrawing      = nullptr;  /*!< shape being; ToolStateDraw or ToolStateDrawManipulate                              */
    LHandle *       pHandleMoving       = nullptr;  /*!< handle being moved can use LHandle::getLayerObject() to get object                 */

    void paint( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) override {}; // DRObjects paint themselves, the rest is transparent, and thats enough

    void setToolState( ToolStates n );
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

    // begin - possibly get rid of these
    bool isBackgroundTransparent() { return colorBackground.alpha() == 0; }
    // end

protected slots:
    virtual void slotRefreshGeneral();
};

#endif





