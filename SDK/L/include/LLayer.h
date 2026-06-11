/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LLayer
#define H_LLayer

#include <AWObject.h>

#include "L.h"
#include "LSGrid.h"

class LGraphicsScene;
class LGraphicsLayer;
class LFile;

/*!
 * \brief Describes a tool. This can be used, by the app, to create QActionGroup for tools. 
 *  
 * A list of LTool's can be used to create a QActionGroup for seleting a tool. 
 * A tool is mutually exclusive group of tools (only one can be active at any one time). 
 * A common feature of a tool is that a mouse click on the canvas initiates the use of the tool. 
 *  
 * Each LLayer can have 0-n tools. 
 * The nID is used when calling LLayer::setTool(). 
 * The nID will probably correspond to an enum in the class derived from LLayer. 
 *  
 * \author pharvey (3/10/23)
 */
class LTool
{
public:
    LTool( int nID, const QString &stringText, const QPixmap &pixmap, const QString &stringToolTip )
    {
        this->nID               = nID;
        this->stringText        = stringText;
        this->pixmap            = pixmap;
        this->stringToolTip     = stringToolTip;
    }

    int     nID;
    QString stringText;
    QPixmap pixmap;
    QString stringToolTip;
};

/*!
 * \brief Layer interface.
 * 
 * \author pharvey (3/18/23) 
 */
class LLayer : public AWObject
{
    Q_OBJECT
public:
    enum FormatTypes
    {
        FormatTypeImage,        /*!< file format is an image file - so 1 layer and must be raster           */
        FormatTypeXML,          /*!< file format is an XML doc - so 1-n layers - any mix of raster/vector   */
        FormatTypeNone          /*!< file format not determined                                             */
    };

    enum TransportTypes
    {
        TransportTypeImage,     /*!< cut/copy/paste is a QImage (raster)   */
        TransportTypeXML,       /*!< cut/copy/paste is an XML doc (vector) */
        TransportTypeNone
    };

    enum LayerTypes
    {
        LayerTypeTransparency,  /*!< represents transparency (always 1 and must be back-most layer)         */
        LayerTypeRaster,        /*!< 'painting' on an image which is the only object on the scene           */
        LayerTypeVector,        /*!< 'drawing' line and shape objects on the scene                          */
        LayerTypeOther,
        LayerTypeNone
    };

    //
    LLayer( LFile *pParent, const QString &stringName = QString() );
    ~LLayer();

    virtual QMenu *getContextMenu( QWidget * );

    // Graphics...
    virtual LGraphicsLayer *getGraphics() { return pGraphics; }
    virtual void doGraphicsCreate( LGraphicsScene *pScene );
    virtual void doGraphicsDelete();
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget );

    // Main event handlers...
    // - called by \sa LFile
    virtual void doMousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent ) = 0;
    virtual void doMouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent ) = 0;
    virtual void doMouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent ) = 0;
    virtual void doDragEnterEvent( QGraphicsSceneDragDropEvent *pEvent ) = 0;
    virtual void doDragMoveEvent( QGraphicsSceneDragDropEvent *pEvent ) = 0;
    virtual void doDropEvent( QGraphicsSceneDragDropEvent *pEvent ) = 0;
    virtual void doContextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent ) = 0;
    virtual void doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent ) = 0;
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElemLine ) override;

    // Layer...
    virtual void setModified( bool b = true );
    virtual void setImage( const QImage & ) = 0;
    virtual void setTool( int n, bool bCancelDrawing = true ) = 0;
    virtual void setBackground( const QColor &c );
    virtual void setVisible( bool b = true );

    virtual LayerTypes          getLayerType() = 0;
    virtual QString             getName()           { return objectName();      }
    virtual TransportTypes      getTransportType() = 0;     // internal format - used for scratch and cut/copy/paste
    virtual QVector<LTool>      getTools() = 0;             // for app to create QAction's for tools
    virtual bool                getModified()   { return bModified; }
    virtual QImage              getImage() = 0;
    virtual QImage *            getImageRef() { return nullptr; }
    virtual QImage              getCopyImage() = 0; // for scratch
    virtual QDomDocument        getCopyXML() = 0;   // for scratch
    virtual int                 getTool() = 0;
    virtual QColor              getBackground() { return colorBackground; }
    virtual QWidget *           getLayerPanel( QWidget *pParent ) = 0;              /*!< return layers panel widget (could be nullptr)              */
    virtual QWidget *           getToolBar( QWidget *pParent ) = 0;                 /*!< IF tool THEN return its tool bar widget (could be nullptr) */
    virtual QWidget *           getPanel( QWidget *pParent ) = 0;                   /*!< IF tool THEN return its panel widget (could be nullptr)    */

    virtual void doCut() = 0;
    virtual void doCopy() = 0;
    virtual void doPaste() = 0;
    virtual void doPaste( const QImage & ) = 0;     // for scratch
    virtual void doPaste( QDomElement * ) = 0;      // for scratch
    virtual void doUndo() = 0;
    virtual void doRedo() = 0;
    virtual void doSelectAll() = 0;
    virtual void doSelectNone() = 0;
    virtual void doDelete() = 0;
    virtual void doCommit() = 0;
    virtual void doCancel() = 0;
    virtual void doCrop() = 0;
    virtual void doScale( qreal nX, qreal nY, Qt::AspectRatioMode n = Qt::IgnoreAspectRatio ) = 0;
    virtual void doFlipX() = 0;
    virtual void doFlipY() = 0;
    virtual void doRotate( int nDegrees ) = 0;
    virtual void doToBack();
    virtual void doToFront();
    virtual void doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBefore( ADObject *pObject );
    virtual void doToAfter( ADObject *pObject );
    virtual void doClear();
    virtual void doPushUndo() = 0;
    virtual void doZoomChanged( int nZoom );
    virtual void doScrollChanged( int nX, int nY );

    virtual bool isModified()       { return bModified; }
    virtual bool isDrawing()        { return false; } 
    virtual bool isVisible()        { return bVisible; }

    virtual bool hasSelection()     { return false; }
    virtual bool hasXML()           { return false; }
    virtual bool hasImage()         { return false; }

    virtual bool canCut()           { return false; } 
    virtual bool canCopy()          { return false; } 
    virtual bool canPaste()         { return false; } 
    virtual bool canUndo()          { return false; } 
    virtual bool canRedo()          { return false; } 
    virtual bool canSelectAll()     { return false; } 
    virtual bool canSelectNone()    { return false; } 
    virtual bool canDelete()        { return false; } 
    virtual bool canCommit()        { return false; } 
    virtual bool canCancel()        { return false; } 
    virtual bool canCrop()          { return false; } 
    virtual bool canScale()         { return false; }
    virtual bool canFlipX()         { return false; }
    virtual bool canFlipY()         { return false; }
    virtual bool canRotate()        { return false; }

signals:
    void signalChanged();                                           /*!< compliments signalModified for cases such as selection changed */
    void signalChangedTool();
    void signalChangedToolState();
    void signalPixelSelectionMoved( const QPoint &pointTopLeft );
    void signalPixelRefreshImage();
    void signalToolConfig( int nTool );                             /*!< layer emits this to let the main window create/delete a tool config (ie in tool bar). Logic for this varies depending upon the layer class. */

protected slots:
    virtual void slotRefreshGrid();

protected:
    LGraphicsLayer *    pGraphics       = nullptr;                  /*!< our presentation/control on a QGraphicsScene   */
    bool                bVisible        = true;
    QColor              colorBackground = QColor( 0, 0, 0, 0 );     /*!< more transparent than Qt::transparent          */

    // grid
    LSGrid *        pSettingGrid    = nullptr;
    L::GridTypes    nGridType       = L::GridTypeStandard;
    L::GridSnaps    nGridSnap       = L::GridSnapNone;
    QSize           sizeGridCell    = QSize( 64, 64 );
};

#endif
