/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 *
 * This file is part of Euphemia.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H_EMainWindow
#define H_EMainWindow

#include <WPaletteColorWidget.h>
#include <WScratchTool.h>
#include <WCoordWidget.h>
#include <WZoomWidget.h>

#include <PGraphicsView.h>
#include <PCanvas.h>
#include <PMagnifierWidget.h>

class EMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    EMainWindow( QWidget *pWidget = nullptr );
    ~EMainWindow();

protected:
    void closeEvent( QCloseEvent *pEvent );

private:
    // current view or null
    PGraphicsView * pView   = nullptr;
    PCanvas::Tools  nTool   = PCanvas::ToolSelectRectangle;

    // tool bar
    QToolBar *  pToolBarToolConfig  = nullptr;              /*!< container for pWidgetToolConfig                                */
    QWidget *   pWidgetToolConfig   = nullptr;              /*!< tool config widget (if any)                                    */
    bool        bPaste              = false;                /*!< true if we are in paste mode - pWidgetToolConfig is for paste  */

    // main widgets
    QTabWidget *    pTabWidget; 
    QToolButton *   pTabCloseButton;

    // docks
    QDockWidget *           pDockTools;

    QDockWidget *           pDockScratch;
    WScratchTool *          pScratchTool;

    QDockWidget *           pDockColors;
    WPaletteColorWidget *   pPaletteColor;

    QDockWidget *           pDockMagnifier;
    PMagnifierWidget *      pMagnifier;

    QDockWidget *           pDockPanner;                    /*!< has a second PGraphicsView on canvas for panning large images  */

    // status bar
    QLabel *        pMessage;
    QLabel *        pModified;
    WCoordWidget *  pCoord;
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
    QAction *pActionUndo;
    QAction *pActionRedo;
    QAction *pActionUndoLevels;
    QAction *pActionSelectAll;
    QAction *pActionSelectNone;
    QAction *pActionAutoCommit;
    QAction *pActionCommit;
    QAction *pActionCancel;
    QAction *pActionPreferences;
    // view
    QMenu *pMenuView;
    QAction *pActionZoomIn;
    QAction *pActionZoomOut;
    // tools
    QMenu *pMenuTools;

    // QAction *pActionToolsFatBitEditor;
    // QAction *pActionMagnifier;

    // tools
    QActionGroup *pActionGroupTools;
    QAction *pActionSelectRectangle;
    QAction *pActionSelectEllipse;
    QAction *pActionSelectPolygon;
    QAction *pActionDrawFreeHand;
    QAction *pActionDrawSpray;
    QAction *pActionDrawErase;
    QAction *pActionDrawText;
    QAction *pActionDrawLine;
    QAction *pActionDrawRectangle;
    QAction *pActionDrawEllipse;
    QAction *pActionDrawPolygon;
    QAction *pActionDrawPolyline;
    QAction *pActionDrawRectangleFilled;
    QAction *pActionDrawEllipseFilled;
    QAction *pActionDrawPolygonFilled;
    QAction *pActionFillFlood;
    QAction *pActionFillGradient;
    QAction *pActionMagnifierSelection;

    // region
    QMenu *pMenuRegion;
    QAction *pActionRegionCrop;
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
    QAction *pActionAboutPeterHarvey;
    QAction *pActionAboutQt;
    QAction *pActionAboutSlickEdit;
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
    void doInitDockScratch();
    void doInitDockColors();
    void doInitDockMagnifier();
    void doInitDockPanner();

    // recent file names
    void doSaveRecentFiles();
    void doLoadRecentFiles();
    void doAddRecentFile( const QString &s );

    // state
    void doSaveState();
    void doLoadState();

    //
    void doCreateToolConfig();
    void doUpdateTabText();

    // get view from tab widget
    PGraphicsView *     getView( int n );

    // file names
    QString getFileNameUserFriendly( const QString &stringFileName, int nMaxPath = 50 );
    QString getFileBaseName( const QString &stringFileName );
    QString getFilePathShort( const QString &stringFileName, int nMaxPath );
    QString getFilePath( const QString &stringFileName );

private slots:
    // workspace
    bool slotNew();             
    bool slotNewWithSize();             
    bool slotNewFromPaste();             
    bool slotOpen();            
    bool slotSave();            
    bool slotSaveAs();          
    bool slotExport();          
    bool slotPrint();          
    bool slotClose( bool bPrompt = true );  
    bool slotRecent( QAction *p );  
    
    // for app
    void slotExit();

    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotUndo();
    void slotRedo();
    void slotUndoLevels();
    void slotSelectAll();
    void slotSelectNone();
    void slotAutoCommit( bool );
    void slotCommit();
    void slotCancel();
    void slotPreferences();

    void slotZoomIn();
    void slotZoomOut();

    void slotCrop();
    void slotScale();
    void slotFlipX();
    void slotFlipY();
    void slotRotate();
    void slotCreateHeuristicMask();
    void slotCreateAlphaMask();

    void slotAbout();
    void slotAboutCBD();
    void slotAboutPeterHarvey();
    void slotAboutQt();
    void slotAboutSlickEdit();
    void slotFeedback();

    void slotCanvasFocused( int nIndex );
    void slotCanvasChangedState();
    void slotChangedFileName( const QString & );

    void slotScratch();
    void slotScratch( const QImage & );

    // these are applied to all canvas'
    void slotToolTriggered();

    // update dock window titles
    void slotPaletteColorWindowTitle();

    void slotRefresh( const PContextGeneral & );
};

#endif



