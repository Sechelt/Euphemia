#include "AppInfo.h"
#include "EMainWindow.h"

#include <WPaletteColor.h>
#include <WIconLayout.h>
#include <WSizeDialog.h>

#include <PPenToolBar.h>
#include <PBrushToolBar.h>

#include <PFillFlood.h>
#include <PFillGradient.h>

#include "EPreferencesDialog.h"

// max number of recent files in file menu
// - must be > 0
#define PMAX_RECENT_FILES 5

#define PZOOM_MIN 10
#define PZOOM_MAX 300
#define PZOOM_INC 10

EMainWindow::EMainWindow( QWidget *pWidget ) 
    : QMainWindow( pWidget )
{
    // all possible ways to set app icon for a running app - probably all end up in the same place
    QApplication::setWindowIcon( QIcon( ":E/Euphemia" ) );
    if ( windowHandle() ) windowHandle()->setIcon( QIcon( ":E/Euphemia" ) );
    setWindowIcon( QIcon( ":E/Euphemia" ) );

    setWindowTitle( APP_NAME );

    doInitActions();
    doInitMenus();
    doInitToolbar();
    doInitCentralArea();
    doInitStatusBar();
    doInitDockWindows();

    doLoadRecentFiles();

// Qt does not fully support Wayland at this time. This means that moving dock windows around
// will be problematic for those running Wayland (on some platforms - not all). May not want to restore a messed up state.
    if ( g_Context->getGeneral().bRestoreState ) doLoadState();
    connect( g_Context, SIGNAL(signalModified(const PContextGeneral &)), SLOT(slotRefresh(const PContextGeneral &)) );

    QClipboard *clipboard = QGuiApplication::clipboard();
    connect( clipboard, SIGNAL(dataChanged()), SLOT(slotCanvasChangedState()) );
}

EMainWindow::~EMainWindow()
{
}

void EMainWindow::closeEvent( QCloseEvent *pEvent )
{
    int nCount = pTabWidget->count();
    for ( int n = 0; n < nCount; n++ )
    {
        PCanvas *pCanvas = getView( n )->getCanvas();
        if ( pCanvas->isModified() )
        {
            pTabWidget->setCurrentIndex( n );

            QMessageBox::StandardButton nButton = QMessageBox::question( this, tr("Closing..."), tr("Would you like to save changes?") );
            if ( nButton == QMessageBox::Yes )
            {
                if ( !pCanvas->doSave() )
                {
                    pEvent->ignore();
                    return;
                }
            }
        }
    }

    // fini
    doSaveRecentFiles();
    doSaveState();
    pEvent->accept();
}

void EMainWindow::doInitActions()
{
    // FILE
    {
        pActionNew          = new QAction( QIcon( ":W/new" ), tr("New ..."), this );
        pActionNewWithSize  = new QAction( tr("New With Size..."), this );
        pActionNewFromPaste = new QAction( tr("New From Paste..."), this );
        pActionOpen         = new QAction( QIcon( ":W/open" ), tr("Open "), this );
        pActionSave         = new QAction( QIcon( ":W/save" ), tr("Save "), this );
        pActionSaveAs       = new QAction( tr("Save  As..."), this );
        pActionExport       = new QAction( tr("Export ..."), this );
        pActionPrint        = new QAction( QIcon( ":W/print" ), tr("Print ..."), this );
        pActionClose        = new QAction( QIcon( ":W/close" ), tr("Close "), this );         // close editor
        pActionExit         = new QAction( QIcon( ":W/exit" ), tr("E&xit"), this );

        pActionNew->setShortcut( QKeySequence::New );
        pActionOpen->setShortcut( QKeySequence::Open );
        pActionSave->setShortcut( QKeySequence::Save );
        pActionSaveAs->setShortcut( QKeySequence::SaveAs );
        pActionPrint->setShortcut( QKeySequence::Print );
        pActionClose->setShortcut( QKeySequence::Close );

        pActionNew->setEnabled( true ); 
        pActionNewWithSize->setEnabled( true ); 
        pActionNewFromPaste->setEnabled( true ); 
        pActionOpen->setEnabled( true ); 
        pActionSave->setEnabled( false ); 
        pActionSaveAs->setEnabled( false ); 
        pActionExport->setEnabled( false ); 
        pActionPrint->setEnabled( false ); 
        pActionClose->setEnabled( false ); 

        connect( pActionNew, &QAction::triggered, this, &EMainWindow::slotNew );
        connect( pActionNewWithSize, &QAction::triggered, this, &EMainWindow::slotNewWithSize );
        connect( pActionNewFromPaste, &QAction::triggered, this, &EMainWindow::slotNewFromPaste );
        connect( pActionOpen, &QAction::triggered, this, &EMainWindow::slotOpen );
        connect( pActionSave, &QAction::triggered, this, &EMainWindow::slotSave );
        connect( pActionSaveAs, &QAction::triggered, this, &EMainWindow::slotSaveAs );
        connect( pActionExport, &QAction::triggered, this, &EMainWindow::slotExport );
        connect( pActionPrint, &QAction::triggered, this, &EMainWindow::slotPrint );
        connect( pActionClose, &QAction::triggered, this, &EMainWindow::slotClose );
        connect( pActionExit, &QAction::triggered, this, &EMainWindow::slotExit );
    }

    // EDIT
    {
        pActionCut          = new QAction( QIcon( ":W/cut" ), tr("&Cut"), this );
        pActionCopy         = new QAction( QIcon( ":W/copy" ), tr("C&opy"), this );
        pActionPaste        = new QAction( QIcon( ":W/paste" ), tr("&Paste"), this );
        pActionUndo         = new QAction( QIcon( ":W/undo" ), tr("&Undo"), this );
        pActionRedo         = new QAction( QIcon( ":W/redo" ), tr("&Redo"), this );
        pActionUndoLevels   = new QAction( tr("Undo levels..."), this );
        pActionSelectAll    = new QAction( tr("Select &All"), this );
        pActionSelectNone   = new QAction( tr("Select &None"), this );
        pActionAutoCommit   = new QAction( tr("Auto Commit"), this );
        pActionCommit       = new QAction( QIcon( ":E/Commit" ), tr("Commit"), this );
        pActionCancel       = new QAction( QIcon( ":E/Cancel" ), tr("Cancel"), this );
        pActionPreferences  = new QAction( QIcon( ":W/Properties16x16" ), tr("Preferences"), this );

        pActionCut->setShortcut( QKeySequence::Cut );
        pActionCopy->setShortcut( QKeySequence::Copy );
        pActionPaste->setShortcut( QKeySequence::Paste );
        pActionUndo->setShortcut( QKeySequence::Undo );
        pActionRedo->setShortcut( QKeySequence::Redo );
        pActionCancel->setShortcut( QKeySequence::Cancel );

        pActionAutoCommit->setToolTip( tr("auto commit changes to canvas else allow manipulation before commit when possible") );

        pActionAutoCommit->setCheckable( true );
        pActionAutoCommit->setChecked( g_Context->getGeneral().bAutoCommit );

        pActionCut->setEnabled( false );
        pActionCopy->setEnabled( false );
        pActionPaste->setEnabled( false );
        pActionUndo->setEnabled( false );
        pActionRedo->setEnabled( false );
        pActionUndoLevels->setEnabled( true );
        pActionSelectAll->setEnabled( false );
        pActionSelectNone->setEnabled( false );
        pActionAutoCommit->setEnabled( true );
        pActionCommit->setEnabled( false );
        pActionCancel->setEnabled( false );
        pActionPreferences->setEnabled( true );

        connect( pActionCut, &QAction::triggered, this, &EMainWindow::slotCut );
        connect( pActionCopy, &QAction::triggered, this, &EMainWindow::slotCopy );
        connect( pActionPaste, &QAction::triggered, this, &EMainWindow::slotPaste );
        connect( pActionUndo, &QAction::triggered, this, &EMainWindow::slotUndo );
        connect( pActionRedo, &QAction::triggered, this, &EMainWindow::slotRedo );
        connect( pActionUndoLevels, &QAction::triggered, this, &EMainWindow::slotUndoLevels );
        connect( pActionSelectAll, &QAction::triggered, this, &EMainWindow::slotSelectAll );
        connect( pActionSelectNone, &QAction::triggered, this, &EMainWindow::slotSelectNone );
        connect( pActionAutoCommit, &QAction::toggled, this, &EMainWindow::slotAutoCommit );
        connect( pActionCommit, &QAction::triggered, this, &EMainWindow::slotCommit );
        connect( pActionCancel, &QAction::triggered, this, &EMainWindow::slotCancel );
        connect( pActionPreferences, &QAction::triggered, this, &EMainWindow::slotPreferences );
    }

    // VIEW
    {
        pActionZoomIn          = new QAction( QIcon( ":W/ZoomIn48x48" ), tr("Zoom &In"), this );
        pActionZoomOut         = new QAction( QIcon( ":W/ZoomOut48x48" ), tr("Zoom &Out"), this );
        pActionZoomIn->setShortcut( QKeySequence::ZoomIn );
        pActionZoomOut->setShortcut( QKeySequence::ZoomOut );
    }

    // TOOLS
    pActionGroupTools = new QActionGroup( this );
    // Default
    {
        pActionSelectRectangle = new QAction( QIcon( ":E/SelectRectangle" ), tr("Select using rectangle"), this );
        pActionSelectRectangle->setToolTip( tr("select using a rectangle") );
        pActionSelectRectangle->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectRectangle );  
        connect( pActionSelectRectangle, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionSelectEllipse = new QAction( QIcon( ":E/SelectEllipse" ), tr("Select using ellipse"), this );
        pActionSelectEllipse->setToolTip( tr("select using an ellipse") );
        pActionSelectEllipse->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectEllipse );  
        connect( pActionSelectEllipse, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionSelectPolygon = new QAction( QIcon( ":E/SelectPolygon" ), tr("Select using polygon"), this );
        pActionSelectPolygon->setToolTip( tr("select using a polygon") );
        pActionSelectPolygon->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectPolygon );  
        connect( pActionSelectPolygon, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawFreeHand = new QAction( QIcon( ":E/FreeHand" ), tr("Free hand"), this );
        pActionDrawFreeHand->setToolTip( tr("Free Hand") );
        pActionDrawFreeHand->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawFreeHand );  
        connect( pActionDrawFreeHand, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawSpray = new QAction( QIcon( ":E/Spray" ), tr("Spray"), this );
        pActionDrawSpray->setToolTip( tr("Spray") );
        pActionDrawSpray->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawSpray );  
        connect( pActionDrawSpray, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawErase = new QAction( QIcon( ":E/Erase" ), tr("Erase"), this );
        pActionDrawErase->setToolTip( tr("Erase") );
        pActionDrawErase->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawErase );  
        connect( pActionDrawErase, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawText = new QAction( QIcon( ":E/Text" ), tr("Text"), this );
        pActionDrawText->setToolTip( tr("Text") );
        pActionDrawText->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawText );  
        connect( pActionDrawText, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawLine = new QAction( QIcon( ":E/Line" ), tr("Line"), this );
        pActionDrawLine->setToolTip( tr("straight line") );
        pActionDrawLine->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawLine );  
        connect( pActionDrawLine, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawRectangle = new QAction( QIcon( ":E/Rectangle" ), tr("Rectangle"), this );
        pActionDrawRectangle->setToolTip( tr("rectangle") );
        pActionDrawRectangle->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawRectangle );  
        connect( pActionDrawRectangle, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawEllipse = new QAction( QIcon( ":E/Ellipse" ), tr("Ellipse"), this );
        pActionDrawEllipse->setToolTip( tr("ellipse") );
        pActionDrawEllipse->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawEllipse );  
        connect( pActionDrawEllipse, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolygon = new QAction( QIcon( ":E/Polygon" ), tr("Polygon"), this );
        pActionDrawPolygon->setToolTip( tr("polygon") );
        pActionDrawPolygon->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolygon );  
        connect( pActionDrawPolygon, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolyline = new QAction( QIcon( ":E/Polyline" ), tr("Polyline"), this );
        pActionDrawPolyline->setToolTip( tr("polyline") );
        pActionDrawPolyline->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolyline );  
        connect( pActionDrawPolyline, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawRectangleFilled = new QAction( QIcon( ":E/RectangleFilled" ), tr("Rectangle filled"), this );
        pActionDrawRectangleFilled->setToolTip( tr("rectangle filled") );
        pActionDrawRectangleFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawRectangleFilled );  
        connect( pActionDrawRectangleFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawEllipseFilled = new QAction( QIcon( ":E/EllipseFilled" ), tr("Ellipse filled"), this );
        pActionDrawEllipseFilled->setToolTip( tr("ellipse filled") );
        pActionDrawEllipseFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawEllipseFilled );  
        connect( pActionDrawEllipseFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolygonFilled = new QAction( QIcon( ":E/PolygonFilled" ), tr("Polygon filled"), this );
        pActionDrawPolygonFilled->setToolTip( tr("polygon filled") );
        pActionDrawPolygonFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolygonFilled );  
        connect( pActionDrawPolygonFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionFillFlood = new QAction( QIcon( ":E/FillFlood" ), tr("Fill"), this );
        pActionFillFlood->setToolTip( tr("fill") );
        pActionFillFlood->setCheckable( true );
        pActionGroupTools->addAction( pActionFillFlood );  
        connect( pActionFillFlood, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionFillGradient = new QAction( QIcon( ":E/FillGradient" ), tr("Fill with gradient"), this );
        pActionFillGradient->setToolTip( tr("gradient fill") );
        pActionFillGradient->setCheckable( true );
        pActionGroupTools->addAction( pActionFillGradient );  
        connect( pActionFillGradient, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionSelectRectangle->setChecked( true );
    }
    // REGION

    {
        pActionRegionCrop       = new QAction( QIcon( ":E/Crop" ), tr("Crop"), this );
        pActionRegionScale      = new QAction( tr("Scale"), this );
        pActionRegionFlipX      = new QAction( tr("Flip X Axis"), this );
        pActionRegionFlipY      = new QAction( tr("Flip Y Axis"), this );
        pActionRegionRotate     = new QAction( tr("Rotate By.."), this );
        pActionRegionHeuristicMask    = new QAction( tr("Create Heuristic Mask"), this );
        pActionRegionAlphaMask        = new QAction( tr("Create Alpha Mask"), this );

        pActionRegionCrop->setToolTip( tr("crop image to current selection") );
        pActionRegionScale->setToolTip( tr("compress or stretch image to be a new size") );
        pActionRegionFlipX->setToolTip( tr("flip image on the X-axis") );
        pActionRegionFlipY->setToolTip( tr("flip image on the Y-axis") );
        pActionRegionRotate->setToolTip( tr("rotate image") );
        pActionRegionHeuristicMask->setToolTip( tr("Creates a 1-bpp mask from the alpha buffer in this image. Returns a null image if the image's format is RGB32.") );
        pActionRegionAlphaMask->setToolTip( tr("Creates a 1-bpp heuristic mask for this image.") );

        pActionRegionCrop->setEnabled( false );
        pActionRegionScale->setEnabled( false );
        pActionRegionFlipX->setEnabled( false );
        pActionRegionFlipY->setEnabled( false );
        pActionRegionRotate->setEnabled( false );
        pActionRegionHeuristicMask->setEnabled( false );
        pActionRegionAlphaMask->setEnabled( false );

        connect( pActionRegionCrop, &QAction::triggered, this, &EMainWindow::slotCrop );
        connect( pActionRegionScale, &QAction::triggered, this, &EMainWindow::slotScale );
        connect( pActionRegionFlipX, &QAction::triggered, this, &EMainWindow::slotFlipX );
        connect( pActionRegionFlipY, &QAction::triggered, this, &EMainWindow::slotFlipY );
        connect( pActionRegionRotate, &QAction::triggered, this, &EMainWindow::slotRotate );
        connect( pActionRegionHeuristicMask, &QAction::triggered, this, &EMainWindow::slotCreateHeuristicMask );
        connect( pActionRegionAlphaMask, &QAction::triggered, this, &EMainWindow::slotCreateAlphaMask );
    }

    // FILTERS
    {
        pActionFiltersInvert            = new QAction( tr("Invert"), this );
        pActionFiltersGreyScale         = new QAction( tr("Turn into Greyscale"), this );
        pActionFiltersNormalize         = new QAction( tr("Normalize Contrast..."), this );
        pActionFiltersTransformation    = new QAction( tr("Transformation RGB/Gamma..."), this );    
        pActionFiltersFloyd             = new QAction( tr("Floyd-Steinberg Filter..."), this );
        pActionFiltersQuantize          = new QAction( tr("Quantize Colors..."), this );
        pActionFiltersSolarize          = new QAction( tr("Solarize..."), this );
        pActionFiltersSharpen           = new QAction( tr("Sharpen"), this );
        pActionFiltersMerge             = new QAction( tr("Marge..."), this );
        pActionFiltersSmooth            = new QAction( tr("Smooth..."), this );
        pActionFiltersDirectional       = new QAction( tr("Directional Smooth"), this );
        pActionFiltersDespeckle         = new QAction( tr("Despeckle..."), this );
        pActionFiltersEdge              = new QAction( tr("Edge Detect"), this );
        pActionFiltersEmboss            = new QAction( tr("Emboss..."), this );
        pActionFiltersOil               = new QAction( tr("Oil Paint..."), this );
        pActionFiltersAddNoise          = new QAction( tr("Add Noise..."), this );
        pActionFiltersSpread            = new QAction( tr("Spread..."), this );
        pActionFiltersPixelize          = new QAction( tr("Pxelize..."), this );
        pActionFiltersDistortion        = new QAction( tr("Distortion..."), this );
        pActionFiltersPerspective       = new QAction( tr("Perspective..."), this );
        pActionFiltersBlend             = new QAction( tr("Blend"), this );
        pActionFiltersUser              = new QAction( tr("User Defined Filter"), this );
        pActionFiltersRepeat            = new QAction( tr("Repeat Last"), this );
        pActionFiltersUndo              = new QAction( tr("Undo Last"), this );
    }

    // HELP
    {
        pActionAbout            = new QAction( QIcon( ":E/Euphemia" ), tr("&About Euphemia..."), this );
        pActionAboutCBD         = new QAction( QIcon( ":W/CodeByDesign32x32" ), tr("About CodeByDesign..."), this );
        pActionAboutPeterHarvey = new QAction( QIcon( ":E/PeterHarvey" ), tr("About Peter Harvey..."), this );
        pActionAboutQt          = new QAction( QIcon( ":E/Qt" ), tr("About Qt..."), this );
        pActionAboutSlickEdit   = new QAction( QIcon( ":E/SlickEdit" ), tr("About SlickEdit..."), this );
        pActionFeedback         = new QAction( QIcon( ":E/Feedback" ), tr("Feedback..."), this );

        pActionFeedback->setToolTip( tr("report an issue or request a feature") );

        connect( pActionAbout, &QAction::triggered, this, &EMainWindow::slotAbout );
        connect( pActionAboutCBD, &QAction::triggered, this, &EMainWindow::slotAboutCBD );
        connect( pActionAboutPeterHarvey, &QAction::triggered, this, &EMainWindow::slotAboutPeterHarvey );
        connect( pActionAboutQt, &QAction::triggered, this, &EMainWindow::slotAboutQt );
        connect( pActionAboutSlickEdit, &QAction::triggered, this, &EMainWindow::slotAboutSlickEdit );
        connect( pActionFeedback, SIGNAL(triggered()), SLOT(slotFeedback()) );
    }

}

void EMainWindow::doInitMenus()
{
    // FILE
    pMenuFile = menuBar()->addMenu( tr("&File") );
    pMenuFile->addAction( pActionNew );
    pMenuFile->addAction( pActionNewWithSize );
    pMenuFile->addAction( pActionNewFromPaste );
    pMenuFile->addAction( pActionOpen );
    pMenuFile->addAction( pActionSave );
    pMenuFile->addAction( pActionSaveAs );
    pMenuFile->addAction( pActionClose );
    pMenuFile->addSeparator();
    pMenuFile->addAction( pActionExport );
    pMenuFile->addAction( pActionPrint );
    pMenuFile->addSeparator();
    pMenuFile->addAction( pActionExit );
    pMenuFile->addSeparator();

    // recent files - just blank for now
    connect( pMenuFile, SIGNAL(triggered(QAction*)), this, SLOT(slotRecent(QAction*)) );

    // EDIT
    pMenuEdit = menuBar()->addMenu( tr("&Edit") );
    pMenuEdit->addAction( pActionCut );
    pMenuEdit->addAction( pActionCopy );
    pMenuEdit->addAction( pActionPaste );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionUndo );
    pMenuEdit->addAction( pActionRedo );
    pMenuEdit->addAction( pActionUndoLevels );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionSelectAll );
    pMenuEdit->addAction( pActionSelectNone );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionAutoCommit );
    pMenuEdit->addAction( pActionCommit );
    pMenuEdit->addAction( pActionCancel );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionPreferences );

    // VIEW
    pMenuView = menuBar()->addMenu( tr("View") );
    pMenuView->addAction( pActionZoomIn  );
    pMenuView->addAction( pActionZoomOut );
    pMenuView->addSeparator();

    // TOOLS
    pMenuTools = menuBar()->addMenu( tr("Tools") );
    pMenuTools->addAction( pActionSelectRectangle );    
    pMenuTools->addAction( pActionSelectEllipse );      
    pMenuTools->addAction( pActionSelectPolygon );      
    pMenuTools->addAction( pActionDrawFreeHand );       
    pMenuTools->addAction( pActionDrawSpray );          
    pMenuTools->addAction( pActionDrawErase );          
    pMenuTools->addAction( pActionDrawText );           
    pMenuTools->addAction( pActionDrawLine );           
    pMenuTools->addAction( pActionDrawRectangle );      
    pMenuTools->addAction( pActionDrawEllipse );        
    pMenuTools->addAction( pActionDrawPolygon );        
    pMenuTools->addAction( pActionDrawPolyline );       
    pMenuTools->addAction( pActionDrawRectangleFilled );
    pMenuTools->addAction( pActionDrawEllipseFilled );  
    pMenuTools->addAction( pActionDrawPolygonFilled );  
    pMenuTools->addAction( pActionFillFlood );          
    pMenuTools->addAction( pActionFillGradient );       

    // REGION
    pMenuRegion = menuBar()->addMenu( tr("Region") );
    pMenuRegion->addAction( pActionRegionCrop       );
    pMenuRegion->addAction( pActionRegionScale       );
    pMenuRegion->addAction( pActionRegionFlipX      );
    pMenuRegion->addAction( pActionRegionFlipY      );
    pMenuRegion->addAction( pActionRegionRotate     );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionHeuristicMask    );
    pMenuRegion->addAction( pActionRegionAlphaMask        );

    // FILTERS
/*
    pMenuFilters = menuBar()->addMenu( tr("Filters") );     
    pMenuFilters->addAction( pActionFiltersInvert         );
    pMenuFilters->addAction( pActionFiltersGreyScale      );
    pMenuFilters->addAction( pActionFiltersNormalize      );
    pMenuFilters->addAction( pActionFiltersTransformation );
    pMenuFilters->addAction( pActionFiltersFloyd          );
    pMenuFilters->addAction( pActionFiltersQuantize       );
    pMenuFilters->addAction( pActionFiltersSolarize       );
    pMenuFilters->addSeparator();                           
    pMenuFilters->addAction( pActionFiltersSharpen        );
    pMenuFilters->addAction( pActionFiltersMerge          );
    pMenuFilters->addAction( pActionFiltersSmooth         );
    pMenuFilters->addAction( pActionFiltersDirectional    );
    pMenuFilters->addAction( pActionFiltersDespeckle      );
    pMenuFilters->addSeparator();                           
    pMenuFilters->addAction( pActionFiltersEdge           );
    pMenuFilters->addAction( pActionFiltersEmboss         );
    pMenuFilters->addAction( pActionFiltersOil            );
    pMenuFilters->addAction( pActionFiltersAddNoise       );
    pMenuFilters->addAction( pActionFiltersSpread         );
    pMenuFilters->addAction( pActionFiltersPixelize       );
    pMenuFilters->addAction( pActionFiltersDistortion     );
    pMenuFilters->addAction( pActionFiltersPerspective    );
    pMenuFilters->addAction( pActionFiltersBlend          );
    pMenuFilters->addSeparator();                           
    pMenuFilters->addAction( pActionFiltersUser           );
    pMenuFilters->addSeparator();                           
    pMenuFilters->addAction( pActionFiltersRepeat         );
    pMenuFilters->addAction( pActionFiltersUndo           );
*/

    // window
    pMenuWindow = menuBar()->addMenu( tr("&Window") );
    // toggle toolbars
    pMenuToolBars = new QMenu( tr("Tool Bars...") );
    pMenuWindow->addMenu( pMenuToolBars );
    // toggle docks
    pMenuDocks = new QMenu( tr("Dock Windows...") );
    pMenuWindow->addMenu( pMenuDocks );

    // HELP
    menuBar()->addSeparator();
    pMenuHelp = new QMenu( tr("&Help") );
    pMenuHelp->addAction( pActionAbout );
    pMenuHelp->addAction( pActionAboutCBD );
    pMenuHelp->addAction( pActionAboutPeterHarvey );
    pMenuHelp->addAction( pActionAboutQt );
    pMenuHelp->addAction( pActionAboutSlickEdit );
    pMenuHelp->addAction( pActionFeedback );
    pActionHelpMenu = menuBar()->addMenu( pMenuHelp );
}

void EMainWindow::doInitToolbar()
{
    QToolBar *pToolBar;

    pToolBar = addToolBar( tr("File") );
    pToolBar->setObjectName( "File" );
    pToolBar->addAction( pActionNew );
    pToolBar->addAction( pActionOpen );
    pToolBar->addAction( pActionSave );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBar = addToolBar( tr("Edit") );
    pToolBar->setObjectName( "Edit" );
    pToolBar->addAction( pActionCut );
    pToolBar->addAction( pActionCopy );
    pToolBar->addAction( pActionPaste );
    pToolBar->addAction( pActionUndo );
    pToolBar->addAction( pActionRedo );
    pToolBar->addAction( pActionCommit );
    pToolBar->addAction( pActionCancel );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    // pen and brush (fill)
    pToolBar = addToolBar( tr("Pen") );
    pToolBar->setObjectName( "Pen" );
    pToolBar->addWidget( new PPenToolBar( this ) );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBar = addToolBar( tr("Brush") );
    pToolBar->setObjectName( "Brush" );
    pToolBar->addWidget( new PBrushToolBar( this ) );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBarToolConfig = addToolBar( tr("Tool Config") );
    pToolBarToolConfig->setObjectName( "ToolConfig" );
    pMenuToolBars->addAction( pToolBarToolConfig->toggleViewAction() );
}

void EMainWindow::doInitStatusBar()
{
    pMessage= new QLabel( tr("welcome"), statusBar() );
    pMessage->setToolTip( tr("general messages") );
    statusBar()->addWidget( pMessage, 10 );

    pModified  = new QLabel( statusBar() );
    pModified->setToolTip( tr("edit status") );
    pModified->setFixedSize( 16, 16 );
    statusBar()->addPermanentWidget( pModified, 0 );

    pCoord= new WCoordWidget( statusBar() );
    pCoord->setToolTip( tr("pixel coordinate") );
    pCoord->slotCoord( 0.0, 0.0 );
    statusBar()->addPermanentWidget( pCoord, 0 );

    // Zoom (scale) is managed in the view
    // - here we add a tool bar widget which will get connect/disconnected to the view
    pZoom = new WZoomWidget( statusBar() );
    pZoom->setMin( PZOOM_MIN );
    pZoom->setMax( PZOOM_MAX );
    pZoom->setInc( PZOOM_INC );
    statusBar()->addPermanentWidget( pZoom, 0 );
    pZoom->setEnabled( false );
}

void EMainWindow::doInitDockWindows()
{
    doInitDockTools();
    doInitDockScratch(); 
    doInitDockColors();
}

void EMainWindow::doInitDockTools()
{
    // tools
    pDockTools = new QDockWidget( tr("Tools"), this );
    pDockTools->setObjectName( "DockTools" );
    pDockTools->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    // layout the tool buttons on a widget using a flow layout
    QToolButton *pButton;
    QWidget *    pWidgetTools;
    QVBoxLayout *pLayoutTop;
    WIconLayout *pIconLayout;

    pWidgetTools    = new QWidget( pDockTools );
    pLayoutTop      = new QVBoxLayout( pWidgetTools );
    pIconLayout     = new WIconLayout();

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionSelectRectangle );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionSelectEllipse );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionSelectPolygon );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawFreeHand );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawSpray );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawErase );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawText );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawLine );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawRectangle );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawEllipse );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawPolygon );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawPolyline );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawRectangleFilled );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawEllipseFilled );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionDrawPolygonFilled );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionFillFlood );
    pIconLayout->addWidget( pButton );

    pButton = new QToolButton( pWidgetTools );
    pButton->setDefaultAction( pActionFillGradient );
    pIconLayout->addWidget( pButton );

    pLayoutTop->addLayout( pIconLayout );

    pLayoutTop->addStretch( 11 );

    // put the new widget into a scroll area so we get scroll bars as needed
    QScrollArea *pScrollArea = new QScrollArea( pDockTools );
    pScrollArea->setWidgetResizable( true );
    pScrollArea->setWidget( pWidgetTools );

    pDockTools->setWidget( pScrollArea );
    addDockWidget( Qt::LeftDockWidgetArea, pDockTools );

    pMenuDocks->addAction( pDockTools->toggleViewAction() );
}

void EMainWindow::doInitDockColors()
{
    pDockColors = new QDockWidget( tr("Colors - User defined"), this );
    pDockColors->setObjectName( "DockColors" );
    pDockColors->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pPaletteColor = new WPaletteColorWidget( pDockColors );
    pDockColors->setWidget( pPaletteColor );
    addDockWidget( Qt::LeftDockWidgetArea, pDockColors );

    slotPaletteColorWindowTitle();
    connect( g_PaletteColors, SIGNAL(signalModifiedFileName()), SLOT(slotPaletteColorWindowTitle()) );

    pMenuDocks->addAction( pDockColors->toggleViewAction() );
}

void EMainWindow::doInitDockScratch()
{
    QDockWidget *pDockScratch = new QDockWidget( tr("Scratch"), this );
    pDockScratch->setObjectName( "DockScratch" );
    pDockScratch->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pScratchTool = new WScratchTool( pDockScratch );
    pDockScratch->setWidget( pScratchTool );
    addDockWidget( Qt::LeftDockWidgetArea, pDockScratch );

    // enable when;
    // - when canvas has selection (selection only)     
    // - clipboard has image (clipboard image)          
    // - when canvas (all canvas)                       
    //
    // Prompt will decide (or cancel).
    pScratchTool->doEnableAdd(); 
    connect( pScratchTool, SIGNAL(signalAdd()), SLOT(slotScratch()) );
    connect( pScratchTool, SIGNAL(signalPaste(const QImage &)), SLOT(slotScratch(const QImage &)) );

    pMenuDocks->addAction( pDockScratch->toggleViewAction() );
}

void EMainWindow::doInitCentralArea()
{
    pTabWidget = new QTabWidget( this );
    pTabWidget->setObjectName( "CentralWidget" );
    setCentralWidget( pTabWidget ); 

    connect( pTabWidget, SIGNAL(currentChanged(int)), SLOT(slotCanvasFocused(int)) );

    pTabCloseButton = new QToolButton();
    pTabCloseButton->setIcon( QPixmap( ":W/close" ) );
    pTabWidget->setCornerWidget( pTabCloseButton, Qt::TopRightCorner );
    connect( pTabCloseButton, SIGNAL(clicked()), SLOT(slotClose()) );
}

void EMainWindow::doSaveRecentFiles()
{
    QSettings settings;
    settings.beginGroup( "Recent" );
    for ( int n = 0; n < vectorRecentStrings.count(); n++ )
    {
        settings.setValue( QString::number( n ), vectorRecentStrings.at( n ) );
    }
    settings.endGroup();
}

void EMainWindow::doLoadRecentFiles()
{
    QSettings settings;
    settings.beginGroup( "Recent" );
    for ( int n = 0; n < PMAX_RECENT_FILES; n++ )
    {
        QString s = settings.value( QString::number( n ), "" ).toString();
        if ( s.isEmpty() ) break;

        vectorRecentStrings.append( s );
        vectorRecentActions.append( pMenuFile->addAction( QString("%1 %2").arg( n + 1 ).arg( getFileNameUserFriendly( s ) ) ) );

    }
    settings.endGroup();
}

/*!
 * \brief Add the given file name to FileMenu->Recent list. 
 *  
 * \todo When already in list bring to front 
 * \todo Set short cut where index is underlined 
 * \todo Add PMAX_RECENT_FILES to PContextGeneral 
 * 
 * \author pharvey (2/11/23)
 * 
 * \param s      
 */
void EMainWindow::doAddRecentFile( const QString &s )
{
    if ( vectorRecentStrings.contains( s ) )
    {
        // bring to the front
        return;
    }

    // remove actions from menu (order is changing)
    for ( int n = 0; n < vectorRecentActions.count(); n++ )
    {
        pMenuFile->removeAction( vectorRecentActions[n] );
    }
    
    // insert at front
    vectorRecentStrings.insert( 0, s );
    vectorRecentActions.insert( 0, pMenuFile->addAction( QString("1 %2").arg( getFileNameUserFriendly( s ) ) ) );

    // delete oldest as needed
    if ( vectorRecentStrings.count() > PMAX_RECENT_FILES )
    {
        delete vectorRecentActions.last();
        vectorRecentStrings.removeLast();
        vectorRecentActions.removeLast();
    }

    // update all action texts to indicate new index position
    // add back to menu
    for ( int n = 0; n < vectorRecentActions.count(); n++ )
    {
        vectorRecentActions[n]->setText( QString("%1 %2").arg( n + 1 ).arg( getFileNameUserFriendly( vectorRecentStrings[n] ) ) );
        pMenuFile->addAction( vectorRecentActions[n] );
    }
}

void EMainWindow::doSaveState()
{
    QSettings settings;
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "windowState", saveState() );
}

void EMainWindow::doLoadState()
{
    QSettings settings;
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    restoreState( settings.value( "windowState" ).toByteArray() );
}

void EMainWindow::doCreateToolConfig()
{
    // remove current one
    if ( pWidgetToolConfig )
    {
        delete pWidgetToolConfig;
        pWidgetToolConfig = nullptr;
    }

    // add new one
     
    // paste 
    // - this is a special case as it is not a tool but does have a config tool bar
    if ( bPaste )
    {
        pWidgetToolConfig = new PPasteToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
        return;
    }

    // tool
    // - not all tools will have a config tool bar
    switch ( nTool )
    {
        case PCanvas::ToolSelectRectangle:
            break;
        case PCanvas::ToolSelectEllipse:
            break;
        case PCanvas::ToolSelectPolygon:
            break;
        case PCanvas::ToolDrawFreeHand:
            pWidgetToolConfig = new PFreeHandToolBar( pToolBarToolConfig );
            break;
        case PCanvas::ToolDrawSpray:
            pWidgetToolConfig = new PSprayToolBar( pToolBarToolConfig );
            break;
        case PCanvas::ToolDrawErase:
            pWidgetToolConfig = new PEraseToolBar( pToolBarToolConfig );
            break;
        case PCanvas::ToolDrawLine:
            break;
        case PCanvas::ToolDrawRectangle:
            break;
        case PCanvas::ToolDrawEllipse:
            break;
        case PCanvas::ToolDrawPolygon:
            break;
        case PCanvas::ToolDrawPolyline:
            break;
        case PCanvas::ToolDrawRectangleFilled:
            break;
        case PCanvas::ToolDrawEllipseFilled:
            break;
        case PCanvas::ToolDrawPolygonFilled:
            break;
        case PCanvas::ToolDrawText:
            pWidgetToolConfig = new PTextToolBar( pToolBarToolConfig );
            break;
        case PCanvas::ToolFillFlood:
            pWidgetToolConfig = new PFillFloodToolBar( pToolBarToolConfig );
            break;
        case PCanvas::ToolFillGradient:
            pWidgetToolConfig = new PFillGradientToolBar( pToolBarToolConfig );
            break;
    }
    if ( pWidgetToolConfig ) pToolBarToolConfig->addWidget( pWidgetToolConfig );  
}

void EMainWindow::doUpdateTabText()
{
    Q_ASSERT( pView );
    Q_ASSERT( pTabWidget->currentIndex() >= 0 );

    int nIndex = pTabWidget->currentIndex();
    QString stringFileName = pView->getCanvas()->getFileName();
    QString stringTabText;

    if ( stringFileName.isEmpty() )
        stringTabText = tr( "unnamed" );
    else
        stringTabText = getFileBaseName( stringFileName );

    if ( pView->getCanvas()->isModified() ) stringTabText = stringTabText + " *";
    pTabWidget->setTabText( nIndex, stringTabText );
    pTabWidget->setTabToolTip( nIndex, getFilePath( stringFileName ) );
}

PGraphicsView *EMainWindow::getView( int n )
{
    return (PGraphicsView *)pTabWidget->widget( n );
}

/*!
 * \brief Get a file name (including path) in User friendly format. 
 *  
 * The format is [base name] - [path] where a long path is reduced as requested. 
 *  
 * \author pharvey (2/11/23)
 * 
 * \param stringFileName 
 * \param nMaxPath       max path length 0 = no limit otherwise must be >= 10 default:50
 * 
 * \return QString 
 */
QString EMainWindow::getFileNameUserFriendly( const QString &stringFileName, int nMaxPath )
{
    return getFileBaseName( stringFileName ) + " - " + getFilePathShort( stringFileName, nMaxPath );
}

QString EMainWindow::getFileBaseName( const QString &stringFileName )
{
    QFileInfo fileInfo( stringFileName );
    return fileInfo.fileName();
}

/*!
 * \brief Cut left end of path if path too long. 
 *  
 * The path will start with "... " if reduced. 
 * 
 * \author pharvey (2/11/23)
 * 
 * \param stringFileName 
 * \param nMaxPath       
 * 
 * \return QString 
 */
QString EMainWindow::getFilePathShort( const QString &stringFileName, int nMaxPath )
{
    if ( nMaxPath == 0 ) return getFilePath( stringFileName );

    if ( nMaxPath < 10 )
    {
        qWarning( "Path can only be reduced to 10 or more." );
        nMaxPath = 10;
    }

    QString s = getFilePath( stringFileName );
    if ( s.length() <= nMaxPath ) return s;

    return "... " + s.right( nMaxPath - 4 );
}

QString EMainWindow::getFilePath( const QString &stringFileName )
{
    // return QDir::cleanPath( stringFileName );
    return QFileInfo( stringFileName ).absolutePath();
}

/*!
 * \brief New Canvas.
 *  
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool EMainWindow::slotNew()
{
    PGraphicsScene *pScene = new PGraphicsScene( QRectF( 0, 0, 1024, 768 ) );
    PGraphicsView *pView = new PGraphicsView( pScene, pTabWidget );
    PCanvas *pCanvas = pScene->getCanvas();

    pCanvas->setTool( nTool );
    pCanvas->setAutoCommit( pActionAutoCommit->isChecked() );

    pTabWidget->addTab( pView, tr("unnamed") );
    pTabWidget->setCurrentWidget( pView );

    connect( pCanvas, SIGNAL(signalPos(const QPoint &)), pCoord, SLOT(slotCoord(const QPoint &)) );

    return true;
}

bool EMainWindow::slotNewWithSize()
{
    QSize size( 1024, 768 );                                                                       
    {                                                                                              
        bool bOk = false;                                                                          
        size = WSizeDialogInt::getSize( &bOk, size, this );                                          
        if ( !bOk ) return false;                                                                  
    }                                                                                              
                                                                                                   
    slotNew();
    Q_ASSERT( pView );
    pView->getScene()->setSceneRect( QRectF( 0, 0, size.width(), size.height() ) );

    return true;
}

bool EMainWindow::slotNewFromPaste()
{
    QImage image;                                                                                                  
    {                                                                                                              
        const QClipboard *pClipboard = QApplication::clipboard();                                                  
        const QMimeData *pMimeData = pClipboard->mimeData();                                                       
                                                                                                                   
        if ( !pMimeData->hasImage() )                                                                              
        {                                                                                                          
            QMessageBox::information( this, tr("New From Paste"), tr("The clipboard does not contain an image.") );
            return false;                                                                                          
        }                                                                                                          
        image = qvariant_cast<QImage>( pMimeData->imageData() );                                                   
    }                                                                                                              
                                                                                                                   
    image = image.convertToFormat( QImage::Format_ARGB32 );                                                        
                                                                                                                   
    slotNew();
    Q_ASSERT( pView );
    pView->getScene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    pView->getCanvas()->doPaste();
    pView->getCanvas()->doCommit();

    return true;
}

/*!
 * \brief Open a Canvas.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool EMainWindow::slotOpen()
{
    slotNew();
    Q_ASSERT( pView );
    if ( !pView->getCanvas()->doOpen() )                                                                      
    {                                                                                              
        slotClose( false );
        return false;                                                                              
    }                                                                                              

    return true;
}

/*!
 * \brief Save a Canvas.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool EMainWindow::slotSave()
{
    Q_ASSERT( pView );
    return pView->getCanvas()->doSave();
}
  
/*!
 * \brief Save Canvas under a different name.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool EMainWindow::slotSaveAs()
{
    Q_ASSERT( pView );
    return pView->getCanvas()->doSaveAs();
}

bool EMainWindow::slotExport()
{
    return true;
}

bool EMainWindow::slotPrint()
{
    return true;
}

/*!
 * \brief Close Canvas.
 *  
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool EMainWindow::slotClose( bool bPrompt )
{
    Q_ASSERT( pView );

    // copy pointer to view because pView will be processed and then set to null when removed from tab.
    PGraphicsView *p = pView;

    // Get rid of any temp shapes and any handles.
    if ( pView->getCanvas()->isDrawing() ) pView->getCanvas()->doCancel();

    // Give canvas a chance to close gracefully.
    if ( bPrompt && !pView->getCanvas()->doClose() ) return false;

    // this does NOT delete widget
    pTabWidget->removeTab( pTabWidget->currentIndex() );

    // At this point the scene just has a background and a canvas (no temp shapes and no handles).
    // So its ok for the scene to silently delete the background and scene.

    // delete widget
    delete p->getScene(); 
    delete p;

    return true;
}

/*!
 * \brief Open a 'recent' Canvas. 
 *  
 * This will get called for all actions on the menu (even when toolbar was used to invoke) so 
 * such actions as New and the general Open. So do not close any existing Canvas until we 
 * know that it was a 'recent' action that was invoked. 
 * 
 * \author pharvey (8/14/21)
 * 
 * \param p 
 * 
 * \return bool 
 */
bool EMainWindow::slotRecent( QAction *p )     
{
    // x-ref action to file name
    int nIndex = vectorRecentActions.indexOf( p );
    // only carry on if the action was a 'recent' action
    if ( nIndex < 0 ) return false;
    QString s = vectorRecentStrings.at( nIndex );
    if ( s.isEmpty() ) return false;
    // do it
    slotNew();
    Q_ASSERT( pView );
    if ( !pView->getCanvas()->doOpen( s ) )                                                                      
    {                                                                                              
        slotClose( false );
        return false;                                                                              
    }                                                                                              

    return true;
}

void EMainWindow::slotExit()
{
    close();
}

void EMainWindow::slotCut()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doCut();
}

void EMainWindow::slotCopy()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doCopy();
}

void EMainWindow::slotPaste()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doPaste();
}

void EMainWindow::slotUndo()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doUndo();
}

void EMainWindow::slotRedo()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doRedo();
}

void EMainWindow::slotUndoLevels()
{
}

void EMainWindow::slotSelectAll()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doSelectAll();
}

void EMainWindow::slotSelectNone()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doSelectNone();
}

void EMainWindow::slotAutoCommit( bool b )
{
    int nCount = pTabWidget->count();
    for ( int n = 0; n < nCount; n++ )
    {
        getView( n )->getCanvas()->setAutoCommit( b );
    }
}

void EMainWindow::slotCommit()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doCommit();
}

void EMainWindow::slotCancel()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doCancel();
}

void EMainWindow::slotPreferences()
{
    EPreferencesDialog::doPreferences( this );
}

void EMainWindow::slotCrop()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doCrop();
}

void EMainWindow::slotScale()
{
    Q_ASSERT( pView );
    PCanvas *pCanvas = pView->getCanvas();

    QSize size = pCanvas->getImage().size();

    {                                                                                              
        bool bOk = false;                                                                          
        size = WSizeDialogInt::getSize( &bOk, size, this );                                          
        if ( !bOk ) return;                                                                  
    }                                                                                              
                                                                                                   
    pView->getCanvas()->doScale( size.width(), size.height() );
}

void EMainWindow::slotFlipX()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doFlipX();
}

void EMainWindow::slotFlipY()
{
    Q_ASSERT( pView );
    pView->getCanvas()->doFlipY();
}

void EMainWindow::slotRotate()
{
    Q_ASSERT( pView );
    bool bOk;
    int n = QInputDialog::getInt( this, tr("Rotate"), tr("Degrees:"), 90, 1, 359, 1, &bOk );  
    if ( !bOk ) return;
    pView->getCanvas()->doRotate( n );
}

void EMainWindow::slotCreateHeuristicMask()
{
    Q_ASSERT( pView );
    PCanvas *pCanvas = pView->getCanvas();

    QImage image;
    if ( pCanvas->canCopy() )
        image = pCanvas->getCopy().createHeuristicMask();
    else
        image = pCanvas->getImage().createHeuristicMask();

    slotNew();
    Q_ASSERT( pView );
    pView->getCanvas()->setImage( image );
}

void EMainWindow::slotCreateAlphaMask()
{
    Q_ASSERT( pView );
    PCanvas *pCanvas = pView->getCanvas();

    QImage image;
    if ( pCanvas->canCopy() )
        image = pCanvas->getCopy().createAlphaMask();
    else
        image = pCanvas->getImage().createAlphaMask();

    slotNew();
    Q_ASSERT( pView );
    pView->getCanvas()->setImage( image );
}

void EMainWindow::slotAbout()
{
    // this automagically gets app icon
    QMessageBox::about( this, 
                        tr( "About..." ),
                        tr( "<b>" APP_NAME "</b><br><br>" ) +
                        tr( "Description: " APP_DESC "</b><br>" ) +
                        tr( "Version: v" APP_VER "<br>" ) +
                        tr( "Sponsor: " CBD_COMPANY "<br>" ) +
                        tr( "Credits: Peter Harvey<br>" ) +
                        tr( "License: " APP_LICENSE "<br>" ) + 
                        tr( "Copyright: " APP_COPYRIGHT "<br>" ) +
                        tr( "Code: all code is original from CodeByDesign<br>" ) + 
                        tr( "Icons: CodeByDesign, xpaint, and free sources - thanks!<br>" ) 
                        );
}

void EMainWindow::slotAboutCBD()
{
    QMessageBox msg( this );
    msg.setTextFormat( Qt::RichText );
    msg.setWindowTitle( tr( "About " CBD_COMPANY "..." ) );
    msg.setText(    "<TABLE cellpadding=10><TR><TD><img src=':W/CodeByDesign64' width=64 height=64></TD><TD>" 
                    "<A HREF='http://www.codebydesign.com'>" CBD_COMPANY "</A>" + 
                    tr( "<BR><BR>A software company which specializes in tools for; System Architects, Developers, and Consultants.</TD></TR></TABLE>" ) );
    msg.exec();
}

void EMainWindow::slotAboutPeterHarvey()
{
    QMessageBox msg( this );
    msg.setTextFormat( Qt::RichText );
    msg.setWindowTitle( tr( "About Peter Harvey..." ) );
    msg.setText(    "<TABLE cellpadding=10><TR><TD><img src=':E/PeterHarvey' width=64 height=64></TD><TD>"
                    "<A HREF='http://www.peterharvey.org/'>Peter Harvey</A>" + 
                    tr( "<BR><BR>Experienced cross-platform software developer. Slinging code since 1985.</TD></TR></TABLE>" ) );
    msg.exec();
}

void EMainWindow::slotAboutQt()
{
    QMessageBox::aboutQt( this, tr( "About Qt..." ) );
}

void EMainWindow::slotAboutSlickEdit()
{
    QMessageBox msg( this );
    msg.setTextFormat( Qt::RichText );
    msg.setWindowTitle( tr( "About SlickEdit..." ) );
    msg.setText(    "<TABLE cellpadding=10><TR><TD><img src=':E/SlickEdit' width=64 height=64></TD><TD>"
                    "<A HREF='https://www.slickedit.com/'>SlickEdit</A>" + 
                    tr( "<BR><BR>World's Most Powerful Code Editor.</TD></TR></TABLE>" ) );
    msg.exec();
}

/*!
 * \brief Provide feedback. 
 *  
 * Feedback is done via page on www.codebydesign.com web site. 
 * 
 * \author pharvey (7/16/20)
 */
void EMainWindow::slotFeedback()
{
    QDesktopServices::openUrl( QUrl( "https://github.com/Sechelt/Euphemia/issues" ) ); // https://www.codebydesign.com/mantisbt
}

/*!
 * \brief Disconnects from previous current canvas and connects to new current canvas. 
 *  
 * Disocnnects from previous current canvas and then calls slotCanvasChangedState to sync 
 * to a new current canvas. 
 *  
 * \sa slotCanvasChangedState
 * 
 * \author pharvey (1/20/23)
 * 
 * \param nIndex 
 */
void EMainWindow::slotCanvasFocused( int nIndex )
{
    // was a view active? 
    // - disconnect view
    if ( pView  )
    {
        PCanvas *pCanvas = pView->getCanvas();

        if ( pCanvas->isDrawing() ) pCanvas->doCancel();
        disconnect( pZoom, SIGNAL(signalZoom(WZoomWidget::FitTypes,int)), pView, SLOT(slotZoomChanged(WZoomWidget::FitTypes,int)) );
        disconnect( pView, SIGNAL(signalScaleChanged()), pCanvas, SLOT(slotZoomChanged()) );
        disconnect( pCanvas, SIGNAL(signalChangedFileName(const QString &)), this, SLOT(slotChangedFileName(const QString &)) );
        disconnect( pCanvas, SIGNAL(signalChangedState()), this, SLOT(slotCanvasChangedState()) );
        pView = nullptr;
    }

    // no tabs?
    // - then no current canvas so disable a bunch of controls and return
    if ( nIndex < 0 )
    {
        // file
        pActionSave->setEnabled( false ); 
        pActionSaveAs->setEnabled( false ); 
        pActionExport->setEnabled( false ); 
        pActionPrint->setEnabled( false ); 
        pActionClose->setEnabled( false ); 

        // edit
        pActionCut->setEnabled( false ); 
        pActionCopy->setEnabled( false ); 
        pActionPaste->setEnabled( false ); 
        pActionUndo->setEnabled( false ); 
        pActionRedo->setEnabled( false ); 
        pActionSelectAll->setEnabled( false ); 
        pActionSelectNone->setEnabled( false ); 
        pActionCommit->setEnabled( false ); 
        pActionCancel->setEnabled( false ); 

        // view

        // region
        pActionRegionCrop->setEnabled( false ); 
        pActionRegionScale->setEnabled( false ); 
        pActionRegionFlipX->setEnabled( false ); 
        pActionRegionFlipY->setEnabled( false ); 
        pActionRegionRotate->setEnabled( false ); 
        pActionRegionHeuristicMask->setEnabled( false ); 
        pActionRegionAlphaMask->setEnabled( false ); 

        // status bar
        pZoom->setEnabled( false );

        return;
    }

    // new current tab
    // - set view and connect it
    {
        pView = getView( nIndex );
        PCanvas *pCanvas = pView->getCanvas();

        pActionRegionScale->setEnabled( true ); 
        pActionRegionFlipX->setEnabled( true ); 
        pActionRegionFlipY->setEnabled( true ); 
        pActionRegionRotate->setEnabled( true ); 
        pActionRegionHeuristicMask->setEnabled( true ); 
        pActionRegionAlphaMask->setEnabled( true ); 

        // zoom
        connect( pZoom, SIGNAL(signalZoom(WZoomWidget::FitTypes,int)), pView, SLOT(slotZoomChanged(WZoomWidget::FitTypes,int)) );
        connect( pView, SIGNAL(signalScaleChanged()), pCanvas, SLOT(slotZoomChanged()) );
        pZoom->setEnabled( true );
        // canvas
        connect( pCanvas, SIGNAL(signalChangedFileName(const QString &)), this, SLOT(slotChangedFileName(const QString &)) );
        connect( pCanvas, SIGNAL(signalChangedState()), this, SLOT(slotCanvasChangedState()) );
        slotCanvasChangedState();
    }
}

/*!
 * \brief Sync the controls to reflect the state of the current canvas. 
 *  
 * This is connected to PCanvas::signalChangedState but may also be invoked directly 
 * by EMainWindow::slotCanvasFocused. 
 *  
 * \sa slotCanvasFocused 
 * 
 * \author pharvey (1/20/23)
 */
void EMainWindow::slotCanvasChangedState()
{
    // we can be called without a current canvas
    // ie when current selection in palette has changed
    // just ignore
    if ( !pView ) return;

    PCanvas *pCanvas = pView->getCanvas();

    // update tab text (file name with * when modified)
    doUpdateTabText();

    // file
    pActionSave->setEnabled( pCanvas->isModified() ); 
    pActionSaveAs->setEnabled( true ); 
    pActionExport->setEnabled( true ); 
    pActionPrint->setEnabled( true ); 
    pActionClose->setEnabled( true ); 

    // edit
    pActionCut->setEnabled( pCanvas->canCut() ); 
    pActionCopy->setEnabled( pCanvas->canCopy() ); 
    pActionPaste->setEnabled( pCanvas->canPaste() ); 
    pActionUndo->setEnabled( pCanvas->canUndo() ); 
    pActionRedo->setEnabled( pCanvas->canRedo() ); 
    pActionSelectAll->setEnabled( true ); 
    pActionSelectNone->setEnabled( pCanvas->hasSelection() ); 
    pActionCommit->setEnabled( pCanvas->canCommit() ); 
    pActionCancel->setEnabled( pCanvas->canCancel() ); 

    // region
    pActionRegionCrop->setEnabled( pCanvas->canCrop() ); 

    // status bar
    pModified->setPixmap( pCanvas->isModified() ? QPixmap( ":W/Draw16x16" ) : QPixmap() );

    // paste
    // have we changed paste state
    if ( pCanvas->hasPaste() != bPaste )
    {
        bPaste = (!bPaste);
        doCreateToolConfig();
    }
}

void EMainWindow::slotChangedFileName( const QString &s )
{
    // update tab title
    doUpdateTabText();

    if ( s.isEmpty() ) return;      // canvas cleared

    // update recent file list
    doAddRecentFile( s );       // loaded or saved as
}

void EMainWindow::slotScratch()
{
    if ( !pView ) 
    {
        QMessageBox::information( this, tr("Scratch..."), tr("No active canvas.") );
        return;
    }

    if ( !pView->getCanvas()->canCopy() )
    {
        QMessageBox::information( this, tr("Scratch..."), tr("Nothing selected.") );
        return;
    }

    pScratchTool->doAppend( pView->getCanvas()->getCopy() );
}

void EMainWindow::slotScratch( const QImage &image )
{
    if ( !pView ) return;
    pView->getCanvas()->doPaste( image );
}

void EMainWindow::slotToolTriggered()
{
    if ( pActionSelectRectangle->isChecked() )     
    {
        nTool = PCanvas::ToolSelectRectangle;
    }
    else if ( pActionSelectEllipse->isChecked() )       
    {
        nTool = PCanvas::ToolSelectEllipse;
    }
    else if ( pActionSelectPolygon->isChecked() )       
    {
        nTool = PCanvas::ToolSelectPolygon;
    }
    else if ( pActionDrawFreeHand->isChecked() )        
    {
        nTool = PCanvas::ToolDrawFreeHand;
    }
    else if ( pActionDrawSpray->isChecked() )           
    {
        nTool = PCanvas::ToolDrawSpray;
    }
    else if ( pActionDrawErase->isChecked() )           
    {
        nTool = PCanvas::ToolDrawErase;
    }
    else if ( pActionDrawText->isChecked() )           
    {
        nTool = PCanvas::ToolDrawText;
    }
    else if ( pActionDrawLine->isChecked() )            
    {
        nTool = PCanvas::ToolDrawLine;
    }
    else if ( pActionDrawRectangle->isChecked() )       
    {
        nTool = PCanvas::ToolDrawRectangle;
    }
    else if ( pActionDrawEllipse->isChecked() )         
    {
        nTool = PCanvas::ToolDrawEllipse;
    }
    else if ( pActionDrawPolygon->isChecked() )         
    {
        nTool = PCanvas::ToolDrawPolygon;
    }
    else if ( pActionDrawPolyline->isChecked() )         
    {
        nTool = PCanvas::ToolDrawPolyline;
    }
    else if ( pActionDrawRectangleFilled->isChecked() ) 
    {
        nTool = PCanvas::ToolDrawRectangleFilled;
    }
    else if ( pActionDrawEllipseFilled->isChecked() )   
    {
        nTool = PCanvas::ToolDrawEllipseFilled;
    }
    else if ( pActionDrawPolygonFilled->isChecked() )   
    {
        nTool = PCanvas::ToolDrawPolygonFilled;
    }
    else if ( pActionFillFlood->isChecked() )           
    {
        nTool = PCanvas::ToolFillFlood;
    }
    else if ( pActionFillGradient->isChecked() )        
    {
        nTool = PCanvas::ToolFillGradient;
    }
    else
    {
        Q_ASSERT( 1==2 );
    }

    // sometimes adds a tool specific toolbar
    doCreateToolConfig();

    // set for all canvas's
    int nCount = pTabWidget->count();
    for ( int n = 0; n < nCount; n++ )
    {
        getView( n )->getCanvas()->setTool( nTool ); // canvas will cancel any drawing in this call
    }
}

void EMainWindow::slotPaletteColorWindowTitle()
{
    QString stringFileName = g_PaletteColors->getFileName();
    if ( stringFileName.isEmpty() )
    {
        pDockColors->setWindowTitle( tr("Colors") );
    }
    else
    {
        QFileInfo FileInfo( stringFileName );
        pDockColors->setWindowTitle( tr("Colors") + ( g_PaletteColors->isModified() ? "* - " : " - " ) + FileInfo.baseName() );
    }
}

void EMainWindow::slotRefresh( const PContextGeneral &t )
{
    pActionAutoCommit->setChecked( t.bAutoCommit );
}

