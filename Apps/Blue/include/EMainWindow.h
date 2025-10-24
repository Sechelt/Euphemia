/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_EMainWindow
#define H_EMainWindow

#include <SColors.h>
#include <SColorsWidget.h>

#include <WCoordWidget.h>
#include <WZoomWidget.h>
#include <WIconLayout.h>
#include <WSizeDialog.h>

#include <LGraphicsView.h>
#include <LPanner.h>
#include <LLayersWidget.h>
#include <LColorsWidget.h>
#include <LPixelsWidget.h>
#include <LScratchWidget.h>
#include <LToolsWidget.h>

#include "EWorkspace.h"
#include "EPreferencesDialog.h"

class EMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    EMainWindow( QWidget *pWidget = nullptr );
    ~EMainWindow();

protected:
    void closeEvent( QCloseEvent *pEvent );

private:
    // central widget fills main client area
    QToolButton *pTabCloseButton;
    QTabWidget *pCentralWidget; 

    // workspace
    // - this is the g_Root object 
    // - there will always be 1
    // - automatically created and deleted
    // - used to create EFile objects
    EWorkspace * pWorkspace;

    // current editor
    // - has focus 
    // - can have selected objects
    // - some controls are connected to this
    // - determined in slotEditorFocused
    AWEditorWidget *pEditorWidget   = nullptr;  // IF this changes then pLayers, pLayer and nTool must be updated
    LFile *         pLayers         = nullptr;  // IF this changes then pLayer and nTool must be updated
    LLayer *        pLayer          = nullptr;  // IF this changes then nTool must be updated

    // tool bar
    QToolBar *  pToolBarHolder      = nullptr;  /*!< container for pToolConfigWidget                                */
    QWidget *   pToolConfigWidget   = nullptr;  /*!< current tool bar widget - for current tool in layer (if any)   */

    // docks
    QDockWidget *           pDockTools;
    LToolsWidget *          pToolsWidget;

    QDockWidget *           pDockScratch;
    LScratchWidget *        pScratchWidget;

    QDockWidget *           pDockLayers;
    LLayersWidget *         pLayersWidget;

    QDockWidget *           pDockColors;

    QDockWidget *           pDockPixels;
    LPixelsWidget *         pPixelWidget;

    QDockWidget *           pDockPanner;                    /*!< has a second LGraphicsView on canvas for panning large images  */

    // status bar
    QLabel *        pMessage;
    QLabel *        pModified;
    WCoordWidget *  pPosCell;
    WCoordWidget *  pPosPixel;
    WZoomWidget *   pZoom;

    // menu
    // file
    QMenu *pMenuFile;
    QAction *pActionNew; 
    QAction *pActionNewWithSize; 
    QAction *pActionNewFromPaste; 
    QAction *pActionOpen;
    QAction *pActionSave;
    QAction *pActionSaveAs;
    QAction *pActionExport;
    QAction *pActionImport; // import to new layer(s)
    QAction *pActionPrint;
    QAction *pActionClose;
    QVector<QString>    vectorRecentStrings;
    QVector<QAction*>   vectorRecentActions;
    QAction *pActionExit;       
    // edit
    QMenu *pMenuEdit;
    QAction *pActionCut; 
    QAction *pActionCopy; 
    QAction *pActionPaste; 
    QAction *pActionDelete; 
    QAction *pActionUndo;
    QAction *pActionRedo;
    QAction *pActionUndoLevels;
    QAction *pActionSelectAll;
    QAction *pActionSelectNone;
    QAction *pActionBackground;
    QAction *pActionAutoCommit;
    QAction *pActionPage;
    QAction *pActionGrid;
    QAction *pActionRuler;
    QAction *pActionCommit;
    QAction *pActionCancel;
    QAction *pActionPreferences;
    // view
    QMenu *pMenuView;
    QAction *pActionZoomIn;
    QAction *pActionZoomOut;

    // context
    QMenu *pMenuContext;
    QAction *pActionRenderContext;
    QAction *pActionPageContext;
    QAction *pActionGridContext;
    QAction *pActionRulerContext;
    QAction *pActionPointerContext;

    // QAction *pActionToolsFatBitEditor;
    // QAction *pActionMagnifier;

    // region
    QMenu *pMenuRegion;
    QAction *pActionRegionCrop;
    QAction *pActionRegionResize;
    QAction *pActionRegionScale;
    QAction *pActionRegionFlipX;
    QAction *pActionRegionFlipY;
    QAction *pActionRegionRotate;
    QAction *pActionRegionHeuristicMask;
    QAction *pActionRegionAlphaMask;
    // filters
    QMenu *pMenuFilters;
    QAction *pActionFiltersInvert;    
    QAction *pActionFiltersGreyScale;    
    QAction *pActionFiltersNormalize;    
    QAction *pActionFiltersTransformation;    
    QAction *pActionFiltersFloyd;    
    QAction *pActionFiltersQuantize;    
    QAction *pActionFiltersSolarize;    
    QAction *pActionFiltersSharpen;    
    QAction *pActionFiltersMerge;    
    QAction *pActionFiltersSmooth;    
    QAction *pActionFiltersDirectional;    
    QAction *pActionFiltersDespeckle;    
    QAction *pActionFiltersEdge;    
    QAction *pActionFiltersEmboss;    
    QAction *pActionFiltersOil;    
    QAction *pActionFiltersAddNoise;    
    QAction *pActionFiltersSpread;    
    QAction *pActionFiltersPixelize;    
    QAction *pActionFiltersDistortion;    
    QAction *pActionFiltersPerspective;    
    QAction *pActionFiltersBlend;    
    QAction *pActionFiltersUser;    
    QAction *pActionFiltersRepeat;    
    QAction *pActionFiltersUndo;    

    // window
    QMenu *pMenuWindow;
    // tool bars
    QMenu *pMenuToolBars;
    // docks
    QMenu *pMenuDocks;
    
    // help
    QMenu *pMenuHelp;
    QAction *pActionAbout;      
    QAction *pActionAboutCBD;
    QAction *pActionFeedback;

    QAction *pActionHelpMenu;

    // setters

    // getters

    // init
    void doInitActions();
    void doInitMenus();
    void doInitCentralArea();
    void doInitToolbar();
    void doInitStatusBar();
    void doInitDockWindows();
    void doInitDockTools();
    void doInitDockLayers();
    void doInitDockScratch();
    void doInitDockColors();
    void doInitDockMagnifier();
    void doInitDockPanner();
    void doInitSplash();

    // recent file names
    void doSaveRecentFiles();
    void doLoadRecentFiles();
    void doAddRecentFile( const QString &s );

    // state
    void doSaveState();
    void doLoadState();

    //
    void doConnectEditor( AWEditorWidget * );
    void doDisconnectEditor();
    void doConnectLayer();
    void doDisconnectLayer();
    void doConnectTool();
    void doDisconnectTool();

    void doUpdateWindowTitle();
    void doUpdateTabText();

    // get view from tab widget
    LGraphicsView *getView( int n );

    // file names
    QString getFileNameUserFriendly( const QString &stringFileName, int nMaxPath = 50 );
    QString getFileBaseName( const QString &stringFileName );
    QString getFilePathShort( const QString &stringFileName, int nMaxPath );
    QString getFilePath( const QString &stringFileName );

private slots:
    // catch signals from app controls
    bool slotNew();
    // bool slotNewDraw();
    // bool slotNewPaint();
    bool slotNewWithSize();             
    bool slotNewFromPaste();             
    bool slotOpen();            
    bool slotSave();            
    bool slotSaveAs();          
    bool slotExport();          
    bool slotImport();          
    bool slotPrint();          
    bool slotClose();  
    bool slotRecent( QAction *p );  
    void slotExit();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDelete();
    void slotUndo();
    void slotRedo();
    void slotUndoLevels();
    void slotSelectAll();
    void slotSelectNone();
    void slotBackground();
    void slotAutoCommit( bool );
    void slotPage( bool );
    void slotGrid( bool );
    void slotRuler( bool );
    void slotRenderContext();
    void slotPageContext();
    void slotGridContext();
    void slotRulerContext();
    void slotPointerContext();
    void slotCommit();
    void slotCancel();
    void slotPreferences();
    void slotZoomIn();
    void slotZoomOut();
    void slotCrop();
    void slotResize();
    void slotScale();
    void slotFlipX();
    void slotFlipY();
    void slotRotate();
    void slotCreateHeuristicMask();
    void slotCreateAlphaMask();
    void slotAbout();
    void slotAboutCBD();
    void slotFeedback();

    // system
    void slotChangedClipboard();                        // <<- QClipBoard::dataChanged()

    // editor
    void slotEditorOpened( AWObject *, AWEditorWidget * );
    void slotEditorClosing( AWObject *, AWEditorWidget * );
    void slotCloseEditor();
    void slotEditorFocused( int nIndex );

    // from pLayers
    void slotChangedFileName( const QString & );    // <<- pLayers->signalChangedFileName()
    void slotChangedLayers();                       // <<- pLayers->signalModified()
    void slotChangedCurrentLayer();                 // <<- pLayers->signalChangedCurrent()
    void slotDeletedLayer( ADObject * );            // <<- pLayers->signalDeleted( ADObject *pLayer )

    // from pLayer
    void slotChangedLayer();                        // <<- pLayer->signalModified()
    void slotChangedTool();                         // so we can adjust tool config toolbar
    void slotChangedLayerToolState();               // so we can enable/disable commit/cancel etc

    // catch from scratch
    void slotScratch();
    void slotScratch( const QImage & );

    // update dock window titles
    void slotPaletteColorWindowTitle();
};

#endif




