/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LFile
#define H_LFile

#include <AWFile.h>

#include "LLayer.h"
#include "LGraphicsScene.h"
#include "LGraphicsLine.h"
#include "LCrossHairLine.h"

#include "LSGrid.h"

/*!
 * \brief A document with support for multiple layers where a layer can be, for example, paint, drawing, diagram etc.
 *  
 * This will manage (create/delete/reorder) layers and the rest is pass-through to the current layer. 
 *  
 * How do we know what layer classes we can support? 
 *  
 * This could be done by loading plugin/factory or by having it determined at compile time. 
 * At the moment - it is done at compile time. \sa EFile::getObject
 *
 * The app will create a class based off of LFile. This will know which LLayer based objects are supported. 
 * It will provide \sa ADMeta to provide info for the UI. 
 * It will provide a version of getObject() which will create the LLayer based objects.
 *  
 * \author pharvey (4/5/23)
 */
class LFile : public AWFile
{
    Q_OBJECT
public:
    LFile( ADObject *pParent, const QString &stringName = QString() );
    virtual ~LFile();

    // Graphics...
    virtual LGraphicsScene *getGraphics() { return pGraphics; }
    virtual void doGraphicsCreate( QObject *pParent );
    virtual void doGraphicsDelete();

    virtual void doMousePressEvent( QGraphicsSceneMouseEvent *pEvent );
    virtual void doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent );
    virtual void doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent );
    virtual void doDragEnterEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void doDragMoveEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void doDropEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void doContextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );
    virtual void doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent );
    virtual void doDrawBackground( QPainter *painter, const QRectF &rect );
    virtual void doDrawForeground( QPainter *painter, const QRectF &rect );

    // Managing layers...
    virtual bool    setLayerCurrentIndex( int );

    virtual int     getLayerCurrentIndex();
    virtual LLayer *getLayerCurrent();
    virtual int     getLayerCount();
    virtual QString getLayerClass( int nIndex = -1 );           /*!< can be used to lookup in meta                  */
    virtual QMenu * getLayerContextMenu( QWidget * );

    virtual bool    doLayerNew( const QString &stringClass );   /*!< appends a new layer and makes current          */
    virtual bool    doLayerDelete( int nIndex = -1 );           /*!< delets layer                                   */
    virtual bool    doLayerToPrev();                            /*!< current layer: move to back one position       */
    virtual bool    doLayerToNext();                            /*!< current layer: move to forward one position    */

    virtual bool    hasLayer( LLayer * );
    virtual bool    hasLayerCurrent();
    virtual bool    isLayerVisible( int nIndex = -1 );

    // AWFile
    // just noop these
    void doFindLoaded( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria );
    void doValidateLoaded( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    // Current layer...
    virtual void setImage( const QImage & );

    virtual LLayer::LayerTypes      getLayerType() = 0;
    virtual QString                 getName()           { return objectName();      }
    virtual LLayer::TransportTypes  getTransportType() = 0;     // internal format - used for scratch and cut/copy/paste
    virtual QSize                   getCell() { return sizeGridCell; }
    virtual QVector<LGraphicsLine*> getGuidesX() { return vectorGuidesX; }
    virtual QVector<LGraphicsLine*> getGuidesY() { return vectorGuidesY; }

    virtual void doCrop();
    virtual void doResize( qreal nX, qreal nY );
    virtual void doResize( const QSizeF & );
    virtual void doScale( qreal nX, qreal nY, Qt::AspectRatioMode n = Qt::IgnoreAspectRatio );
    virtual void doClear() override;                                    

public slots:
    virtual bool slotOpen() override; // to avoid close

    virtual void slotPos( const QPointF &pointPixel );
    virtual void slotZoomChanged( int nZoom );
    virtual void slotScrollChanged( int nX, int nY );
    virtual void slotGuideNewRequest( Qt::Orientation nLineOrientation, qreal nOnScene );
    virtual void slotGuideDeleteRequest( Qt::Orientation nLineOrientation, int nIndex );
    virtual void slotGuideMoveRequest( Qt::Orientation nLineOrientation, int nIndex, qreal nOnScene );

signals:
    void signalPos( const QPointF &pointPixel );
    void signalPos( const QPoint &pointCell );
    void signalChangedCurrent();                    // current layer changed (not the layer itself) (can be nullptr)
    void signalChangedOrder();                      // the order of the layers has changed
    void signalChangedCount();                      // a layer HAS been created or deleted (can intercept in g_Root if object needed)
    // void signalPixelRefreshImage();

protected:
    QRectF          rect        = QRectF( 0.0, 0.0, 1000.0, 1000.0 );   /*!< the size we want for the scene         */
    int             nCurrent    = -1;                                   /*!< current layer -1 means none            */
    LLayer *        pLayer      = nullptr;                              /*!< current layer nullptr means none       */
    LGraphicsScene *pGraphics   = nullptr;                              /*!< current scene nullptr means none       */

    // general: ruler, etc
    SSetting *  pSettingGeneral   = nullptr;
    bool        bDrawPage         = false;
    bool        bGrid             = true;                               /*!< true to show a grid                    */

    // background
    SSetting *  pSettingBackground= nullptr;
    QBrush      brushTransparency;                                      /*!< transparency representation            */

    // pointer
    SSetting *  pSettingPointer   = nullptr;
    bool        bCrossHairs       = true;                               /*!< true to show cross hairs               */

    // page
    SSetting *  pSettingPage    = nullptr;
    QSizeF      sizePage        = QSizeF( 32, 32 );
    QSize       sizePages       = QSize( 1, 1 );
    QSize       sizeMargins     = QSize( 0, 0 );
    bool        bDrawMargins    = false;

    // grid: 
    SSetting *      pSettingGrid= nullptr;    
    L::GridTypes    nGridType   = L::GridTypeStandard; 
    QSize           sizeGridCell= QSize( 64, 64 );                      /*!< grid cell size (never let it be < 1)   */
    int             nGridSizeMin= 4;

    LCrossHairLine *pCrossH    = nullptr;
    LCrossHairLine *pCrossV    = nullptr;

    // Guides are typically created/deleted by a ruler but they can exist without a ruler and are saved/loaded with the diagram.
    // So the diagram is the 'model' and the ruler (if any) is simply a 'controller' that can come and go.
    // Guides can be a horizontal line or a vertical line. 
    // Orientation can also be determined by checking line coordinates. For example; IF x1==x2 THEN line is vertical.
    // Guides that find themselves off of the scene due to a scene resize... are deleted.
    QVector<LGraphicsLine*> vectorGuidesX; 
    QVector<LGraphicsLine*> vectorGuidesY; 

    LLayer *    getLayer( int nIndex );
    ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;

    virtual void doCreateCrossHairs();
    virtual void doDeleteCrossHairs();
    virtual void doSyncGraphicsOrder();
/*
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemModel );                               
    virtual void doEnsurePages();
*/
    virtual void doPostLoad();

    virtual bool hasRuler();
    virtual bool hasCrossHairs() { return pCrossH; }

protected slots:
    virtual void slotRefreshGeneral();
    virtual void slotRefreshBackground();
    virtual void slotRefreshPage();
    virtual void slotRefreshGrid();
    virtual void slotRefreshPointer();
};

#endif


