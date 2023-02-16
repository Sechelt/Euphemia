#ifndef H_EMainWindow
#define H_EMainWindow

// docks
#include <WPaletteColorWidget.h>
#include <WScratchTool.h>
// status bar
#include <WCoordWidget.h>
#include <WZoomWidget.h>

#include <PGraphicsView.h>
#include <PCanvas.h>

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
    QDockWidget *          pDockColors;
    WPaletteColorWidget *  pPaletteColor;
    WScratchTool *         pScratchTool;

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
    // view -> tool bars
    QMenu *pMenuToolBars;
    QAction *pActionToolFile;
    QAction *pActionToolEdit;
    QAction *pActionToolPen;
    QAction *pActionToolBrush;
    QAction *pActionToolFont;
    // view -> docks
    QMenu *pMenuDocks;
    QAction *pActionDockTools;
    QAction *pActionDockPaletteColor;
    QAction *pActionDockScratch;
    // tools
    QMenu *pMenuTools;
    QMenu *pMenuSelections;
    QMenu *pMenuFreeHand;
    QMenu *pMenuShapes;
    QMenu *pMenuFills;

    QAction *pActionToolsFatBitEditor;
    QAction *pActionMagnifier;

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

    // region
    QMenu *pMenuRegion;
    QAction *pActionRegionFlipX;
    QAction *pActionRegionFlipY;
    QAction *pActionRegionRotate;
    QAction *pActionRegionLinear;
    QAction *pActionRegionReset;
    QAction *pActionRegionExpand;
    QAction *pActionRegionDownscale;
    QAction *pActionRegionEdit;
    QAction *pActionRegionCrop;
    QAction *pActionRegionCreateMask;
    QAction *pActionRegionMerge;
    QAction *pActionRegionAutoCrop;
    QAction *pActionRegionComplement;
    QAction *pActionRegionDelimit;
    QAction *pActionRegionOCR;
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
    void doInitDockColors();
    void doInitDockScratch();

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



