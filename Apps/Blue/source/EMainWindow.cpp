/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "AppInfo.h"
#include "EMainWindow.h"

#include <WSplash.h>

#include "EFile.h"
#include "LEditorWidget.h"

// max number of recent files in file menu
// - must be > 0
#define PMAX_RECENT_FILES 5

#define PZOOM_MIN 10
#define PZOOM_MAX 6000
#define PZOOM_PAGE 10
#define PZOOM_INC 1

EMainWindow::EMainWindow( QWidget *pWidget ) 
    : QMainWindow( pWidget )
{
    // All possible ways to set app icon for a running app - probably all end up in the same place.
    // 
    // NOTE: Some desktops require the app be registered in the system to get a proper icon.
    //       In this case; these are a noop.
    //  
    // QApplication::setWindowIcon( QIcon( ":E/Paint" ) );
    // if ( windowHandle() ) windowHandle()->setIcon( QIcon( ":E/Paint" ) );
    setWindowIcon( QIcon( ":E/Paint" ) );

    //
    g_Root = pWorkspace = new EWorkspace( "Workspace" );
    pWorkspace->setEditorWidgetParent( this );
    connect( pWorkspace, SIGNAL(signalEditorOpened(AWObject*,AWEditorWidget*)), this, SLOT(slotEditorOpened(AWObject*,AWEditorWidget*)) );
    connect( pWorkspace, SIGNAL(signalEditorClosing(AWObject*,AWEditorWidget*)), this, SLOT(slotEditorClosing(AWObject*,AWEditorWidget*)) );

    //
    doInitActions();
    doInitMenus();
    doInitToolbar();
    doInitCentralArea();
    doInitStatusBar();
    doInitDockWindows();

    doLoadRecentFiles();

    // NOTE
    // Qt does not fully support Wayland proper at this time. One could run XWayland instead.
    // This means that moving dock windows around in Wayland proper will be problematic.
    // May not want to restore a messed up state.
    if ( QSettings().value( "RestoreState", true ).toInt() ) doLoadState();

    show();

    // system
    QClipboard *clipboard = QGuiApplication::clipboard();
    connect( clipboard, SIGNAL(dataChanged()), SLOT(slotChangedClipboard()) );

    doUpdateWindowTitle();

    if ( QSettings().value( "Splash", true ).toInt() ) doInitSplash();
}

EMainWindow::~EMainWindow()
{
    g_Root = nullptr;
    delete pWorkspace;
    pWorkspace = nullptr;
    doSaveState();
}

void EMainWindow::closeEvent( QCloseEvent *pEvent )
{
    // reset state of current layer
    if ( pLayer )
    {
        if ( pLayer->canCancel() ) pLayer->doCancel();
        else if ( pLayer->canCommit() ) pLayer->doCommit();
    }

    // save/discard/cancel
    AWObject::SaveResults n = pWorkspace->doCloseChildren();
    if ( n == AWObject::SaveCancel ) 
    {
        pEvent->ignore();
        return;
    }
    // discard
    pWorkspace->doClear();                                                                                                                                          

    // fini
    doSaveRecentFiles();
    doSaveState();
    pEvent->accept();
}

void EMainWindow::doInitActions()
{
    // FILE
    {
        pActionNew          = new QAction( QIcon( ":W/new" ), tr("New..."), this );
        pActionNewWithSize  = new QAction( tr("New With Size..."), this );
        pActionNewFromPaste = new QAction( tr("New From Paste..."), this );
        pActionOpen         = new QAction( QIcon( ":W/open" ), tr("Open "), this );
        pActionSave         = new QAction( QIcon( ":W/save" ), tr("Save "), this );
        pActionSaveAs       = new QAction( tr("Save  As..."), this );
        pActionExport       = new QAction( tr("Export ..."), this );
        pActionImport       = new QAction( tr("Import ..."), this );
        pActionPrint        = new QAction( QIcon( ":W/print" ), tr("Print..."), this );
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
        pActionImport->setEnabled( false ); 
        pActionPrint->setEnabled( false ); 
        pActionClose->setEnabled( false ); 

        connect( pActionNew, &QAction::triggered, this, &EMainWindow::slotNew );
        connect( pActionNewWithSize, &QAction::triggered, this, &EMainWindow::slotNewWithSize );
        connect( pActionNewFromPaste, &QAction::triggered, this, &EMainWindow::slotNewFromPaste );
        connect( pActionOpen, &QAction::triggered, this, &EMainWindow::slotOpen );
        connect( pActionSave, &QAction::triggered, this, &EMainWindow::slotSave );
        connect( pActionSaveAs, &QAction::triggered, this, &EMainWindow::slotSaveAs );
        connect( pActionExport, &QAction::triggered, this, &EMainWindow::slotExport );
        connect( pActionImport, &QAction::triggered, this, &EMainWindow::slotImport );
        connect( pActionPrint, &QAction::triggered, this, &EMainWindow::slotPrint );
        connect( pActionClose, &QAction::triggered, this, &EMainWindow::slotClose );
        connect( pActionExit, &QAction::triggered, this, &EMainWindow::slotExit );
    }

    // EDIT
    {
        pActionCut          = new QAction( QIcon( ":W/cut" ), tr("&Cut"), this );
        pActionCopy         = new QAction( QIcon( ":W/copy" ), tr("C&opy"), this );
        pActionPaste        = new QAction( QIcon( ":W/paste" ), tr("&Paste"), this );
        pActionDelete       = new QAction( QIcon( ":W/delete" ), tr("&Delete"), this );
        pActionUndo         = new QAction( QIcon( ":W/undo" ), tr("&Undo"), this );
        pActionRedo         = new QAction( QIcon( ":W/redo" ), tr("&Redo"), this );
        pActionUndoLevels   = new QAction( tr("Undo levels..."), this );
        pActionSelectAll    = new QAction( tr("Select &All"), this );
        pActionSelectNone   = new QAction( tr("Select &None"), this );
        pActionBackground   = new QAction( tr("Background..."), this );
        pActionAutoCommit   = new QAction( tr("Auto Commit"), this );
        pActionCommit       = new QAction( QIcon( ":E/Commit" ), tr("Commit"), this );
        pActionCancel       = new QAction( QIcon( ":E/Cancel" ), tr("Cancel"), this );
        pActionPreferences  = new QAction( QIcon( ":W/Properties16x16" ), tr("Preferences"), this );

        pActionCut->setShortcut( QKeySequence::Cut );
        pActionCopy->setShortcut( QKeySequence::Copy );
        pActionPaste->setShortcut( QKeySequence::Paste );
        pActionDelete->setShortcut( QKeySequence::Delete );
        pActionUndo->setShortcut( QKeySequence::Undo );
        pActionRedo->setShortcut( QKeySequence::Redo );
        pActionCancel->setShortcut( QKeySequence::Cancel );

        pActionAutoCommit->setToolTip( tr("auto commit changes to canvas else allow manipulation before commit when possible") );
        pActionAutoCommit->setCheckable( true );

        pActionAutoCommit->setChecked( g_SSettings->getSetting( "LSGeneral" )->property( "bAutoCommit" ).toBool() );

        pActionCut->setEnabled( false );
        pActionCopy->setEnabled( false );
        pActionPaste->setEnabled( false );
        pActionDelete->setEnabled( false );
        pActionUndo->setEnabled( false );
        pActionRedo->setEnabled( false );
        pActionUndoLevels->setEnabled( true );
        pActionSelectAll->setEnabled( false );
        pActionSelectNone->setEnabled( false );
        pActionBackground->setEnabled( true );
        pActionAutoCommit->setEnabled( true );
        pActionCommit->setEnabled( false );
        pActionCancel->setEnabled( false );
        pActionPreferences->setEnabled( true );

        connect( pActionCut, &QAction::triggered, this, &EMainWindow::slotCut );
        connect( pActionCopy, &QAction::triggered, this, &EMainWindow::slotCopy );
        connect( pActionPaste, &QAction::triggered, this, &EMainWindow::slotPaste );
        connect( pActionDelete, &QAction::triggered, this, &EMainWindow::slotDelete );
        connect( pActionUndo, &QAction::triggered, this, &EMainWindow::slotUndo );
        connect( pActionRedo, &QAction::triggered, this, &EMainWindow::slotRedo );
        connect( pActionUndoLevels, &QAction::triggered, this, &EMainWindow::slotUndoLevels );
        connect( pActionSelectAll, &QAction::triggered, this, &EMainWindow::slotSelectAll );
        connect( pActionSelectNone, &QAction::triggered, this, &EMainWindow::slotSelectNone );
        connect( pActionBackground, &QAction::triggered, this, &EMainWindow::slotBackground );
        connect( pActionAutoCommit, &QAction::toggled, this, &EMainWindow::slotAutoCommit );
        connect( pActionCommit, &QAction::triggered, this, &EMainWindow::slotCommit );
        connect( pActionCancel, &QAction::triggered, this, &EMainWindow::slotCancel );
        connect( pActionPreferences, &QAction::triggered, this, &EMainWindow::slotPreferences );
    }

    // VIEW
    {
        pActionZoomIn          = new QAction( QIcon( ":W/ZoomIn48x48" ), tr("Zoom &In"), this );
        pActionZoomOut         = new QAction( QIcon( ":W/ZoomOut48x48" ), tr("Zoom &Out"), this );
        pActionZoomIn->setShortcut( QKeySequence::ZoomIn ); // this is actually translates into Ctrl+Shft++ for some reason
        // pActionZoomIn->setShortcut( QString( "Ctrl++" ) );
        pActionZoomOut->setShortcut( QKeySequence::ZoomOut );

        pActionPage         = new QAction( tr("Page"), this );
        pActionGrid         = new QAction( tr("Grid"), this );
        pActionRuler        = new QAction( tr("Ruler"), this );

        pActionPage->setCheckable( true );
        pActionGrid->setCheckable( true );
        pActionRuler->setCheckable( true );

        pActionPage->setChecked( g_SSettings->getSetting( "LSGeneral" )->property( "bPage" ).toBool() );
        pActionGrid->setChecked( g_SSettings->getSetting( "LSGeneral" )->property( "bGrid" ).toBool() );
        pActionRuler->setChecked( g_SSettings->getSetting( "LSGeneral" )->property( "bRuler" ).toBool() );

        pActionZoomIn->setEnabled( false ); 
        pActionZoomOut->setEnabled( false ); 
        pActionPage->setEnabled( true );
        pActionGrid->setEnabled( true );
        pActionRuler->setEnabled( true );

        connect( pActionZoomIn, &QAction::triggered, this, &EMainWindow::slotZoomIn );
        connect( pActionZoomOut, &QAction::triggered, this, &EMainWindow::slotZoomOut );
        connect( pActionPage, &QAction::toggled, this, &EMainWindow::slotPage );
        connect( pActionGrid, &QAction::toggled, this, &EMainWindow::slotGrid );
        connect( pActionRuler, &QAction::toggled, this, &EMainWindow::slotRuler );
    }

    // CONTEXT
    {
        pActionRenderContext       = new QAction( tr("Render..."), this );
        pActionPageContext         = new QAction( tr("Page..."), this );
        pActionGridContext         = new QAction( tr("Grid..."), this );
        pActionRulerContext        = new QAction( tr("Ruler..."), this );
        pActionPointerContext      = new QAction( tr("Pointer..."), this );

        pActionRenderContext->setEnabled( true );
        pActionPageContext->setEnabled( true );
        pActionGridContext->setEnabled( true );
        pActionRulerContext->setEnabled( true );
        pActionPointerContext->setEnabled( true );

        connect( pActionRenderContext, &QAction::triggered, this, &EMainWindow::slotRenderContext );
        connect( pActionPageContext, &QAction::triggered, this, &EMainWindow::slotPageContext );
        connect( pActionGridContext, &QAction::triggered, this, &EMainWindow::slotGridContext );
        connect( pActionRulerContext, &QAction::triggered, this, &EMainWindow::slotRulerContext );
        connect( pActionPointerContext, &QAction::triggered, this, &EMainWindow::slotPointerContext );
    }

    // REGION

    {
        pActionRegionCrop       = new QAction( QIcon( ":E/Crop" ), tr("Crop"), this );
        pActionRegionResize     = new QAction( tr("Resize..."), this );
        pActionRegionScale      = new QAction( tr("Scale..."), this );
        pActionRegionFlipX      = new QAction( tr("Flip X Axis"), this );
        pActionRegionFlipY      = new QAction( tr("Flip Y Axis"), this );
        pActionRegionRotate     = new QAction( tr("Rotate By..."), this );
        pActionRegionHeuristicMask    = new QAction( tr("Create Heuristic Mask"), this );
        pActionRegionAlphaMask        = new QAction( tr("Create Alpha Mask"), this );

        pActionRegionCrop->setToolTip( tr("crop image to current selection") );
        pActionRegionResize->setToolTip( tr("resize image") );
        pActionRegionScale->setToolTip( tr("compress or stretch image to be a new size") );
        pActionRegionFlipX->setToolTip( tr("flip image on the X-axis") );
        pActionRegionFlipY->setToolTip( tr("flip image on the Y-axis") );
        pActionRegionRotate->setToolTip( tr("rotate image") );
        pActionRegionHeuristicMask->setToolTip( tr("Creates a 1-bpp mask from the alpha buffer in this image. Returns a null image if the image's format is RGB32.") );
        pActionRegionAlphaMask->setToolTip( tr("Creates a 1-bpp heuristic mask for this image.") );

        pActionRegionCrop->setEnabled( false );
        pActionRegionResize->setEnabled( false );
        pActionRegionScale->setEnabled( false );
        pActionRegionFlipX->setEnabled( false );
        pActionRegionFlipY->setEnabled( false );
        pActionRegionRotate->setEnabled( false );
        pActionRegionHeuristicMask->setEnabled( false );
        pActionRegionAlphaMask->setEnabled( false );

        connect( pActionRegionCrop, &QAction::triggered, this, &EMainWindow::slotCrop );
        connect( pActionRegionResize, &QAction::triggered, this, &EMainWindow::slotResize );
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
        pActionFiltersPixelize          = new QAction( tr("Pixelize..."), this );
        pActionFiltersDistortion        = new QAction( tr("Distortion..."), this );
        pActionFiltersPerspective       = new QAction( tr("Perspective..."), this );
        pActionFiltersBlend             = new QAction( tr("Blend"), this );
        pActionFiltersUser              = new QAction( tr("User Defined Filter"), this );
        pActionFiltersRepeat            = new QAction( tr("Repeat Last"), this );
        pActionFiltersUndo              = new QAction( tr("Undo Last"), this );
    }

    // HELP
    {
        pActionAbout            = new QAction( QIcon( ":E/Paint" ), tr("&About..."), this );
        pActionAboutCBD         = new QAction( QIcon( ":W/CodeByDesign32x32" ), tr("About CodeByDesign..."), this );
        pActionFeedback         = new QAction( QIcon( ":E/Feedback" ), tr("Feedback..."), this );

        pActionFeedback->setToolTip( tr("report an issue or request a feature") );

        connect( pActionAbout, &QAction::triggered, this, &EMainWindow::slotAbout );
        connect( pActionAboutCBD, &QAction::triggered, this, &EMainWindow::slotAboutCBD );
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
    pMenuFile->addAction( pActionImport );
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
    pMenuEdit->addAction( pActionDelete );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionUndo );
    pMenuEdit->addAction( pActionRedo );
    pMenuEdit->addAction( pActionUndoLevels );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionSelectAll );
    pMenuEdit->addAction( pActionSelectNone );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionBackground );
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
    pMenuView->addAction( pActionPage );
    pMenuView->addAction( pActionGrid );
    pMenuView->addAction( pActionRuler );

    // CONTEXT
    pMenuContext = menuBar()->addMenu( tr("Context") );
    pMenuContext->addAction( pActionRenderContext );
    pMenuContext->addAction( pActionPageContext );
    pMenuContext->addAction( pActionGridContext );
    pMenuContext->addAction( pActionRulerContext );
    pMenuContext->addAction( pActionPointerContext );
    
    // REGION
    pMenuRegion = menuBar()->addMenu( tr("Region") );
    pMenuRegion->addAction( pActionRegionCrop       );
    pMenuRegion->addAction( pActionRegionResize      );
    pMenuRegion->addAction( pActionRegionScale      );
    pMenuRegion->addAction( pActionRegionFlipX      );
    pMenuRegion->addAction( pActionRegionFlipY      );
    pMenuRegion->addAction( pActionRegionRotate     );
//    pMenuRegion->addSeparator();
//    pMenuRegion->addAction( pActionRegionHeuristicMask    );
//    pMenuRegion->addAction( pActionRegionAlphaMask        );

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
    pToolBar->addAction( pActionDelete );
    pToolBar->addAction( pActionUndo );
    pToolBar->addAction( pActionRedo );
    pToolBar->addAction( pActionCommit );
    pToolBar->addAction( pActionCancel );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    // pen and brush (fill)
    pToolBar = addToolBar( tr("Pen") );
    pToolBar->setObjectName( "Pen" );
    pToolBar->addWidget( g_SSettings->getSetting( "SPen" )->getToolBar( this ) );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBar = addToolBar( tr("Arrow") );
    pToolBar->setObjectName( "Arrow" );
    pToolBar->addWidget( g_SSettings->getSetting( "SArrow" )->getToolBar( this ) );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBar = addToolBar( tr("Brush") );
    pToolBar->setObjectName( "Brush" );
    pToolBar->addWidget( g_SSettings->getSetting( "SBrush" )->getToolBar( this ) );
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    pToolBarHolder = addToolBar( tr("Tool Config") );
    pToolBarHolder->setObjectName( "ToolConfig" );
    pMenuToolBars->addAction( pToolBarHolder->toggleViewAction() );

    // grid
    pToolBar = addToolBar( tr("Grid") );
    pToolBar->setObjectName( "Grid" );
    pToolBar->addWidget( g_SSettings->getSetting( "LSGrid" )->getToolBar( this ) );
    pToolBar->hide();
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    // render
    pToolBar = addToolBar( tr("Render") );
    pToolBar->setObjectName( "Render" );
    pToolBar->addWidget( g_SSettings->getSetting( "LSRender" )->getToolBar( this ) );
    pToolBar->hide();
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );

    // image
    pToolBar = addToolBar( tr("Image") );
    pToolBar->setObjectName( "Image" );
    pToolBar->addWidget( g_SSettings->getSetting( "LSImage" )->getToolBar( this ) );
    pToolBar->hide();
    pMenuToolBars->addAction( pToolBar->toggleViewAction() );
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

    statusBar()->addPermanentWidget( new QLabel( tr("Pixel:"), statusBar() ), 0);
    pPosPixel= new WCoordWidget( statusBar() );
    pPosPixel->setToolTip( tr("pixel coordinate") );
    pPosPixel->slotCoord( 0.0, 0.0 );
    statusBar()->addPermanentWidget( pPosPixel, 0);

    statusBar()->addPermanentWidget( new QLabel( tr("Cell:"), statusBar() ), 0);
    pPosCell= new WCoordWidget( statusBar() );
    pPosCell->setToolTip( tr("cell coordinate") );
    pPosCell->slotCoord( 0, 0 );
    statusBar()->addPermanentWidget( pPosCell, 0);

    // Zoom (scale) is managed in the view
    // - here we add a tool bar widget which will get connect/disconnected to the view
    pZoom = new WZoomWidget( statusBar() );
    pZoom->setVisibleFit( true );
    pZoom->setMin( PZOOM_MIN );
    pZoom->setMax( PZOOM_MAX );
    pZoom->setPage( PZOOM_PAGE );
    pZoom->setInc( PZOOM_INC );
    pZoom->setZoom( 100 );

    statusBar()->addPermanentWidget( pZoom, 10 );
    pZoom->setEnabled( false );
}

void EMainWindow::doInitDockWindows()
{
    doInitDockTools();
    doInitDockLayers(); 
    doInitDockScratch(); 
    doInitDockColors();
    doInitDockMagnifier();
    doInitDockPanner();
}

void EMainWindow::doInitDockTools()
{
    // tools
    pDockTools = new QDockWidget( tr("Tools"), this );
    pDockTools->setObjectName( "DockTools" );
    pDockTools->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    pToolsWidget = new LToolsWidget( pDockTools );

    pDockTools->setWidget( pToolsWidget );
    addDockWidget( Qt::LeftDockWidgetArea, pDockTools );

    pMenuDocks->addAction( pDockTools->toggleViewAction() );
}

void EMainWindow::doInitDockLayers()
{
    QDockWidget *pDockLayers = new QDockWidget( tr("Layers"), this );
    pDockLayers->setObjectName( "DockLayers" );
    pDockLayers->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pLayersWidget = new LLayersWidget( pDockLayers );
    pDockLayers->setWidget( pLayersWidget );
    addDockWidget( Qt::LeftDockWidgetArea, pDockLayers );

    pMenuDocks->addAction( pDockLayers->toggleViewAction() );
}

void EMainWindow::doInitDockScratch()
{
    QDockWidget *pDockScratch = new QDockWidget( tr("Scratch"), this );
    pDockScratch->setObjectName( "DockScratch" );
    pDockScratch->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pScratchWidget = new LScratchWidget( pDockScratch );
    pDockScratch->setWidget( pScratchWidget );
    addDockWidget( Qt::LeftDockWidgetArea, pDockScratch );

    // enable when;
    // - when canvas has selection (selection only)     
    // - clipboard has image (clipboard image)          
    // - when canvas (all canvas)                       
    //
    // Prompt will decide (or cancel).
    pScratchWidget->doEnableAdd(); 
    connect( pScratchWidget, SIGNAL(signalAdd()), SLOT(slotScratch()) );
    connect( pScratchWidget, SIGNAL(signalPaste(const QImage &)), SLOT(slotScratch(const QImage &)) );

    pMenuDocks->addAction( pDockScratch->toggleViewAction() );
}

void EMainWindow::doInitDockColors()
{
    pDockColors = new QDockWidget( tr("Colors - User defined"), this );
    pDockColors->setObjectName( "DockColors" );
    pDockColors->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    LColorsWidget *p = new LColorsWidget( pDockColors );
    pDockColors->setWidget( p );

    addDockWidget( Qt::LeftDockWidgetArea, pDockColors );

    slotPaletteColorWindowTitle();
    connect( g_SColors, SIGNAL(signalModifiedFileName()), SLOT(slotPaletteColorWindowTitle()) );

    pMenuDocks->addAction( pDockColors->toggleViewAction() );
}

void EMainWindow::doInitDockMagnifier()
{
    pDockPixels = new QDockWidget( tr("Pixels"), this );
    pDockPixels->setWindowFlags( Qt::WindowStaysOnTopHint ); // this is supposed to be set by default (lost during state restore)
    pDockPixels->setObjectName( "DockPixels" );
    pDockPixels->setAllowedAreas( Qt::AllDockWidgetAreas );

    pPixelWidget = new LPixelsWidget( pDockPixels );
    pDockPixels->setWidget( pPixelWidget );

    addDockWidget( Qt::RightDockWidgetArea, pDockPixels );

    pMenuDocks->addAction( pDockPixels->toggleViewAction() );
}

void EMainWindow::doInitDockPanner()
{
    pDockPanner = new QDockWidget( tr("Panner"), this );
    pDockPanner->setObjectName( "DockPanner" );
    pDockPanner->setAllowedAreas( Qt::AllDockWidgetAreas );

    // contents;
    // - second view on canvas or null

    addDockWidget( Qt::RightDockWidgetArea, pDockPanner );

    pMenuDocks->addAction( pDockPanner->toggleViewAction() );
}

void EMainWindow::doInitSplash()
{
    WSplash splash( this, QPixmap( ":E/Splash" ), true, true, 10 );
    splash.exec();
}

void EMainWindow::doInitCentralArea()
{
    pCentralWidget = new QTabWidget( this );
    pCentralWidget->setObjectName( "CentralWidget" );
    setCentralWidget( pCentralWidget ); 
    connect( pCentralWidget, SIGNAL(currentChanged(int)), SLOT(slotEditorFocused(int)) );

    pTabCloseButton = new QToolButton();
    pTabCloseButton->setIcon( QPixmap( ":W/close" ) );
    pCentralWidget->setCornerWidget( pTabCloseButton, Qt::TopRightCorner );

    // close the file (if we can)
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
    // remove actions from menu (order may be changing)
    for ( int n = 0; n < vectorRecentActions.count(); n++ )
    {
        pMenuFile->removeAction( vectorRecentActions[n] );
    }

    // if its already in the recents - remove it
    int n = vectorRecentStrings.indexOf( s );
    if ( n >= 0 )
    {
        vectorRecentStrings.remove( n );
        vectorRecentActions.remove( n );
    }

    // add it to top of list
    vectorRecentStrings.prepend( s );
    vectorRecentActions.prepend( new QAction( QString("1 %2").arg( getFileNameUserFriendly( s ) ) ) );

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
        pMenuFile->addAction( vectorRecentActions[n] ); // because we played it safe
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

void EMainWindow::doConnectEditor( AWEditorWidget *p )
{
    Q_ASSERT( !pEditorWidget );
    Q_ASSERT( !pLayers );
    Q_ASSERT( !pLayer );

    //
    pEditorWidget = p;
    pLayers = (EFile*)pEditorWidget->getObject();
    LGraphicsView *pView = ((LEditorWidget*)pEditorWidget)->getView();

    // file
    pActionSave->setEnabled( pLayers->isModified() ); 
    pActionSaveAs->setEnabled( true ); 
    pActionExport->setEnabled( true ); 
    pActionImport->setEnabled( true ); 
    pActionPrint->setEnabled( true ); 
    pActionClose->setEnabled( true ); 

    // status bar
    pModified->setPixmap( pLayers->isModified() ? QPixmap( ":W/Draw16x16" ) : QPixmap() );

    // update tab text (file name with * when modified)
    doUpdateWindowTitle();
    doUpdateTabText();

    pActionZoomIn->setEnabled( true ); 
    pActionZoomOut->setEnabled( true ); 

    pActionRegionResize->setEnabled( true ); 
    pActionRegionScale->setEnabled( true ); 
    pActionRegionFlipX->setEnabled( true ); 
    pActionRegionFlipY->setEnabled( true ); 
    pActionRegionRotate->setEnabled( true ); 
    pActionRegionHeuristicMask->setEnabled( true ); 
    pActionRegionAlphaMask->setEnabled( true ); 

    // docks
    pDockPanner->setWidget( new LPanner( pLayers->getGraphics(), pDockPanner ) );
    pPixelWidget->setLayers( pLayers );
    pToolsWidget->setLayers( pLayers );
    pLayersWidget->setLayers( pLayers );

    // zoom
    // - the view holds the current zoom/scale for the canvas 
    // - requests to change zoom go to view and then view notifies (if change) via signalZoomChanged
    pZoom->slotRefresh( pView->getZoomFit(), pView->getZoom() );
    connect( pZoom, SIGNAL(signalZoom(WZoomWidget::FitTypes,int)), pView, SLOT(slotZoomChanged(WZoomWidget::FitTypes,int)) );
    connect( pView, SIGNAL(signalZoomChanged(WZoomWidget::FitTypes,int)), pZoom, SLOT(slotRefresh(WZoomWidget::FitTypes,int)) );
    pZoom->setEnabled( true );

    // graphics view (via layers)
    connect( pLayers, SIGNAL(signalPos(const QPointF &)), pPosPixel, SLOT(slotCoord(const QPointF &)) );
    connect( pLayers, SIGNAL(signalPos(const QPoint &)), pPosCell, SLOT(slotCoord(const QPoint &)) );

    // layers
    connect( pLayers, SIGNAL(signalDeleted(ADObject *)), this, SLOT(slotDeletedLayer(ADObject *)) );
    connect( pLayers, SIGNAL(signalChangedFileName(const QString &)), this, SLOT(slotChangedFileName(const QString &)) );
    connect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrentLayer()) );
    connect( pLayers, SIGNAL(signalModified()), this, SLOT(slotChangedLayers()) );


    //
    if ( pLayers->getLayerCurrent() ) doConnectLayer();
}

void EMainWindow::doDisconnectEditor()
{
    Q_ASSERT( pEditorWidget );
    Q_ASSERT( pLayers );

    //
    if ( pLayer ) doDisconnectLayer();

    LGraphicsView *pView = ((LEditorWidget*)pEditorWidget)->getView();

    if ( pDockPanner->widget() ) delete pDockPanner->widget();
    pPixelWidget->setLayers( nullptr );
    pToolsWidget->setLayers( nullptr );
    pLayersWidget->setLayers( nullptr );

    disconnect( pLayers, SIGNAL(signalPos(const QPointF &)), pPosPixel, SLOT(slotCoord(const QPointF &)) );
    disconnect( pLayers, SIGNAL(signalPos(const QPoint &)), pPosCell, SLOT(slotCoord(const QPoint &)) );
    disconnect( pLayers, SIGNAL(signalDeleted(ADObject *)), this, SLOT(slotDeletedLayer(ADObject *)) );
    disconnect( pLayers, SIGNAL(signalChangedFileName(const QString &)), this, SLOT(slotChangedFileName(const QString &)) );
    disconnect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrentLayer()) );
    disconnect( pLayers, SIGNAL(signalModified()), this, SLOT(slotChangedLayers()) );

    disconnect( pZoom, SIGNAL(signalZoom(WZoomWidget::FitTypes,int)), pView, SLOT(slotZoomChanged(WZoomWidget::FitTypes,int)) );
    disconnect( pView, SIGNAL(signalZoomChanged(WZoomWidget::FitTypes,int)), pZoom, SLOT(slotRefresh(WZoomWidget::FitTypes,int)) );

    // file
    pActionSave->setEnabled( false ); 
    pActionSaveAs->setEnabled( false ); 
    pActionExport->setEnabled( false ); 
    pActionImport->setEnabled( false ); 
    pActionPrint->setEnabled( false ); 
    pActionClose->setEnabled( false ); 

    // status bar
    pModified->setPixmap( QPixmap() );
    // update tab text (file name with * when modified)
    doUpdateWindowTitle();
    doUpdateTabText();

    // view
    pActionZoomIn->setEnabled( false ); 
    pActionZoomOut->setEnabled( false ); 

    // region
    pActionRegionResize->setEnabled( false ); 
    pActionRegionScale->setEnabled( false ); 
    pActionRegionFlipX->setEnabled( false ); 
    pActionRegionFlipY->setEnabled( false ); 
    pActionRegionRotate->setEnabled( false ); 
    pActionRegionHeuristicMask->setEnabled( false ); 
    pActionRegionAlphaMask->setEnabled( false ); 

    // status bar
    pZoom->setEnabled( false );

    // fini
    pEditorWidget = nullptr;
    pLayers = nullptr;
}

void EMainWindow::doConnectLayer()
{
//    Q_ASSERT( !pLayer );
    pLayer      = pLayers->getLayerCurrent();
    Q_ASSERT( pLayer );

    pActionCut->setEnabled( pLayer->canCut() ); 
    pActionCopy->setEnabled( pLayer->canCopy() ); 
    pActionPaste->setEnabled( pLayer->canPaste() ); 
    pActionDelete->setEnabled( pLayer->canDelete() ); 
    pActionUndo->setEnabled( pLayer->canUndo() ); 
    pActionRedo->setEnabled( pLayer->canRedo() ); 
    pActionSelectAll->setEnabled( true ); 
    pActionSelectNone->setEnabled( pLayer->hasSelection() ); 
    pActionCommit->setEnabled( pLayer->canCommit() ); 
    pActionCancel->setEnabled( pLayer->canCancel() ); 
    pActionRegionCrop->setEnabled( pLayer->canCrop() ); 

    doConnectTool();

    connect( pLayer, SIGNAL(signalModified()), this, SLOT(slotChangedLayer()) );
    connect( pLayer, SIGNAL(signalChanged()), this, SLOT(slotChangedLayer()) );
    connect( pLayer, SIGNAL(signalChangedToolState()), this, SLOT(slotChangedLayerToolState()) );
    connect( pLayer, SIGNAL(signalChangedTool()), this, SLOT(slotChangedTool()) );
}

void EMainWindow::doDisconnectLayer()
{
    Q_ASSERT( pLayer );

    // catch case where the current layer was deleted
    if ( pLayers->hasLayer( pLayer ) && pLayer->isDrawing() ) pLayer->doCancel();

    pActionCut->setEnabled( false ); 
    pActionCopy->setEnabled( false ); 
    pActionPaste->setEnabled( false ); 
    pActionDelete->setEnabled( false ); 
    pActionUndo->setEnabled( false ); 
    pActionRedo->setEnabled( false ); 
    pActionSelectAll->setEnabled( false ); 
    pActionSelectNone->setEnabled( false ); 
    pActionCommit->setEnabled( false ); 
    pActionCancel->setEnabled( false ); 
    pActionRegionCrop->setEnabled( false ); 

    disconnect( pLayer, SIGNAL(signalModified()), this, SLOT(slotChangedLayer()) );
    disconnect( pLayer, SIGNAL(signalChanged()), this, SLOT(slotChangedLayer()) );
    disconnect( pLayer, SIGNAL(signalChangedToolState()), this, SLOT(slotChangedLayerToolState()) );
    disconnect( pLayer, SIGNAL(signalChangedTool()), this, SLOT(slotChangedTool()) );
    
    doDisconnectTool();

    pLayer = nullptr;
}

void EMainWindow::doConnectTool()
{
    Q_ASSERT( pLayer );
    Q_ASSERT( !pToolConfigWidget );

    pToolConfigWidget = pLayer->getToolBar( pToolBarHolder );         
    // not all tools have a config widget
    if ( pToolConfigWidget ) pToolBarHolder->addWidget( pToolConfigWidget );     
}

void EMainWindow::doDisconnectTool()
{
    Q_ASSERT( pLayer );

    // not all tools have a config widget
    if ( !pToolConfigWidget ) return;

    delete pToolConfigWidget;                                                    
    pToolConfigWidget = nullptr;                                                 
}

void EMainWindow::doUpdateWindowTitle()
{
    if ( !pEditorWidget )
    {
        setWindowTitle( APP_NAME " - " APP_VER " - " APP_DESC );
        return;
    }

    // leave file off title and just rely on tab title for file name
    //
    // AWFile *p = (AWFile*)pEditorWidget->getObject();
    // QString stringFileName = p->getFileName().isEmpty() ? tr("unknown") : p->getFileName();
    // setWindowTitle( APP_NAME " - " + stringFileName + (p->isModified() ? "*" : "") );

    setWindowTitle( APP_NAME " - " APP_VER " - " APP_DESC );
}

void EMainWindow::doUpdateTabText()
{
    Q_ASSERT( pEditorWidget );
    Q_ASSERT( pCentralWidget->currentIndex() >= 0 );

    EFile *pFile = (EFile*)pEditorWidget->getObject();

    int nIndex = pCentralWidget->currentIndex();
    QString stringFileName = pFile->getFileName();
    QString stringTabText;

    if ( stringFileName.isEmpty() )
        stringTabText = tr( "unnamed" );
    else
        stringTabText = getFileBaseName( stringFileName );

    if ( pFile->isModified() ) stringTabText = stringTabText + " *";
    pCentralWidget->setTabText( nIndex, stringTabText );
    pCentralWidget->setTabToolTip( nIndex, getFilePath( stringFileName ) );
}

LGraphicsView *EMainWindow::getView( int n )
{
    return (LGraphicsView *)pCentralWidget->widget( n );
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

bool EMainWindow::slotNew()
{
    // create the 'model' and create its graphics (a scene)
    EFile *pFile = (EFile*)pWorkspace->getObject( "EFile" );
    Q_ASSERT( pFile );
    pFile->setModified( false );
    // open a 'view'
    pFile->slotOpenEditor();

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
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    pFile->doResize( size.width(), size.height() );

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
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    pFile->doResize( image.size().width(), image.size().height() );
    pLayer->doPaste();
    pLayer->doCommit();

    return true;
}

bool EMainWindow::slotOpen()
{
    // create the 'model' and create its graphics (a scene)
    EFile *pFile = (EFile*)pWorkspace->getObject( "EFile" );
    Q_ASSERT( pFile );
        
    if ( !pFile->slotOpen() )
    {
        pWorkspace->slotDelete( pFile );
        return false;
    }
    pFile->slotOpenEditor();

    doAddRecentFile( pFile->getFileName() );

    return true;
}

bool EMainWindow::slotSave()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    AWObject::SaveResults n = pFile->slotSave();
    if ( n == AWObject::SaveError ) return false;
    return true;
}
  
bool EMainWindow::slotSaveAs()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    AWObject::SaveResults n = pFile->slotSaveAs();
    if ( n == AWObject::SaveError ) return false;
    return true;
}

bool EMainWindow::slotExport()
{
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    if ( !pFile ) return false;
    return pFile->doExport();
}

bool EMainWindow::slotImport()
{
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    if ( !pFile ) return false;
    return pFile->doImport();
}

bool EMainWindow::slotPrint()
{
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    if ( !pFile ) return false;
    return pFile->doPrint();
}

bool EMainWindow::slotClose()
{
    Q_ASSERT(pEditorWidget);
    int n = pCentralWidget->indexOf( pEditorWidget );
    Q_ASSERT( n >= 0 );

    EFile *pFile = (EFile*)pEditorWidget->getObject();                
    AWFile::SaveResults nSaved = pFile->slotClose();

    switch ( nSaved )
    {
        case AWFile::SaveSuccess:
        case AWFile::SaveLoseChanges:
            break;
        case AWFile::SaveCancel:
        case AWFile::SaveError:
            return false;
    }

    if  ( pFile->isEditor() ) pFile->slotCloseEditor();
    pWorkspace->slotDelete( pFile );

    return true;
}

bool EMainWindow::slotRecent( QAction *p )     
{
    int nIndex = vectorRecentActions.indexOf( p );
    if ( nIndex < 0 ) return false;

    QString s = vectorRecentStrings.at( nIndex );
    if ( s.isEmpty() ) return false;

    // create the 'model' and create its graphics (a scene)
    EFile *pFile = (EFile*)pWorkspace->getObject( "EFile" );
    Q_ASSERT( pFile );

    if ( !pFile->doOpen( s ) ) 
    {
        pWorkspace->slotDelete( pFile );
        return false;
    }
    pFile->slotOpenEditor();

    doAddRecentFile( pFile->getFileName() );

    return true;
}

void EMainWindow::slotExit()
{
    close();
}

void EMainWindow::slotCut()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doCut();
}

void EMainWindow::slotCopy()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doCopy();
}

void EMainWindow::slotPaste()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doPaste();
}

void EMainWindow::slotDelete()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doDelete();
}

void EMainWindow::slotUndo()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doUndo();
}

void EMainWindow::slotRedo()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doRedo();
}

void EMainWindow::slotUndoLevels()
{
}

void EMainWindow::slotSelectAll()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doSelectAll();
}

void EMainWindow::slotSelectNone()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doSelectNone();
}

void EMainWindow::slotBackground()
{
    SDialog dialog( this, "LSBackground", "Background" );
    dialog.exec();
}

void EMainWindow::slotAutoCommit( bool b )
{
    g_SSettings->getSetting( "LSGeneral" )->setProperty( "bAutoCommit", b );
}

void EMainWindow::slotPage( bool b )
{
    g_SSettings->getSetting( "LSGeneral" )->setProperty( "bPage", b );
}

void EMainWindow::slotGrid( bool b )
{
    g_SSettings->getSetting( "LSGeneral" )->setProperty( "bGrid", b );
}

void EMainWindow::slotRuler( bool b )
{
    g_SSettings->getSetting( "LSGeneral" )->setProperty( "bRuler", b );
}

void EMainWindow::slotRenderContext()
{
    SDialog dialog( this, "LSRender", "Render" );
    dialog.exec();
}

void EMainWindow::slotPageContext()
{
    SDialog dialog( this, "LSPage", "Page" );
    dialog.exec();
}

void EMainWindow::slotGridContext()
{
    SDialog dialog( this, "LSGrid", "Grid" );
    dialog.exec();
}

void EMainWindow::slotRulerContext()
{
    SDialog dialog( this, "SRuler", "Ruler" );
    dialog.exec();
}

void EMainWindow::slotPointerContext()
{
    SDialog dialog( this, "LSPointer", "Pointer" );
    dialog.exec();
}

void EMainWindow::slotCommit()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doCommit();
}

void EMainWindow::slotCancel()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doCancel();
}

void EMainWindow::slotPreferences()
{
    EPreferencesDialog::doPreferences( this );
}

void EMainWindow::slotZoomIn()
{
    Q_ASSERT( pEditorWidget );
    LGraphicsView *pView = ((LEditorWidget*)pEditorWidget)->getView();

    int nZoom = pView->getZoom();
    if ( nZoom <= 290 ) nZoom += 10;
    pView->slotZoomChanged( WZoomWidget::FitIgnore, nZoom );
}

void EMainWindow::slotZoomOut()
{
    Q_ASSERT( pEditorWidget );
    LGraphicsView *pView = ((LEditorWidget*)pEditorWidget)->getView();

    int nZoom = pView->getZoom();
    if ( nZoom >= 20 ) nZoom -= 10;
    pView->slotZoomChanged( WZoomWidget::FitIgnore, nZoom );
}

void EMainWindow::slotCrop()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    pFile->doCrop();
}

void EMainWindow::slotResize()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    QSize size = pFile->getGraphics()->sceneRect().size().toSize();

    {                                                                                              
        bool bOk = false;                                                                          
        size = WSizeDialogInt::getSize( &bOk, size, this );                                          
        if ( !bOk ) return;                                                                  
    }                                                                                              
                                                                                                   
    pFile->doResize( size.width(), size.height() );
}

void EMainWindow::slotScale()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    QSize size = pFile->getGraphics()->sceneRect().size().toSize();

    {                                                                                              
        bool bOk = false;                                                                          
        size = WSizeDialogInt::getSize( &bOk, size, this, true );                                          
        if ( !bOk ) return;                                                                  
    }                                                                                              
                                                                                                   
    pFile->doScale( size.width(), size.height() );
}

void EMainWindow::slotFlipX()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doFlipX();
}

void EMainWindow::slotFlipY()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doFlipY();
}

void EMainWindow::slotRotate()
{
    Q_ASSERT( pEditorWidget );
    EFile *pFile = (EFile*)pEditorWidget->getObject();
    bool bOk;
    int n = QInputDialog::getInt( this, tr("Rotate"), tr("Degrees:"), 90, 1, 359, 1, &bOk );  
    if ( !bOk ) return;
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( pLayer ) pLayer->doRotate( n );
}

void EMainWindow::slotCreateHeuristicMask()
{
/*
    Q_ASSERT( pView );                                    
    PRaster *pCanvas = pView->getLayers();                
                                                          
    QImage image;                                         
    if ( pCanvas->canCopy() )                             
        image = pCanvas->getCopy().createHeuristicMask(); 
    else                                                  
        image = pCanvas->getImage().createHeuristicMask();
                                                          
    slotNew();                                            
    Q_ASSERT( pView );                                    
    pView->getLayers()->setImage( image );                
*/
}

void EMainWindow::slotCreateAlphaMask()
{
/*
    Q_ASSERT( pView );                                
    PRaster *pCanvas = pView->getLayers();            
                                                      
    QImage image;                                     
    if ( pCanvas->canCopy() )                         
        image = pCanvas->getCopy().createAlphaMask(); 
    else                                              
        image = pCanvas->getImage().createAlphaMask();
                                                      
    slotNew();                                        
    Q_ASSERT( pView );                                
    pView->getLayers()->setImage( image );            
*/
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
                        tr( "License: " CBD_LICENSE "<br>" ) + 
                        tr( "Copyright: " CBD_COPYRIGHT "<br>" ) +
                        tr( "<b>another original application from CodeByDesign</b><br>" ) );
}

void EMainWindow::slotAboutCBD()
{
    QMessageBox msg( this );
    msg.setTextFormat( Qt::RichText );
    msg.setWindowTitle( tr( "About " CBD_COMPANY "..." ) );
    msg.setText(    "<TABLE cellpadding=10><TR><TD><img src=':W/CodeByDesign64' width=64 height=64></TD><TD>" 
                    "<A HREF='http://www.codebydesign.com'>" CBD_COMPANY "</A>" + 
                    tr( "<BR><BR>A software company which specializes in tools for creative people.</TD></TR></TABLE>" ) );
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
    QDesktopServices::openUrl( QUrl( "https://www.codebydesign.com/mantisbt" ) ); 
}

void EMainWindow::slotChangedClipboard()
{
    if ( !pLayer ) return;
    pActionPaste->setEnabled( pLayer->canPaste() );
}

void EMainWindow::slotScratch()
{
    if ( !pEditorWidget ) 
    {
        QMessageBox::information( this, tr("Scratch..."), tr("No active canvas.") );
        return;
    }

    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();
    if ( !pLayer->canCopy() )
    {
        QMessageBox::information( this, tr("Scratch..."), tr("Nothing selected.") );
        return;
    }

    // just raster/bitmap supported at the moment but will add xml for vector
    pScratchWidget->doAppend( pLayer->getCopyImage() );
}

void EMainWindow::slotScratch( const QImage &image )
{
    if ( !pEditorWidget ) 
    {
        QMessageBox::information( this, tr("Scratch..."), tr("No active editor.") );
        return;
    }

    EFile *pFile = (EFile*)pEditorWidget->getObject();
    LLayer *pLayer = pFile->getLayerCurrent();

    if ( !pLayer ) 
    {
        QMessageBox::information( this, tr("Scratch..."), tr("No active layer.") );
        return;
    }

    pLayer->doPaste( image );
}


/*!
 * \brief Catch when an object has created/opened an editor. 
 *  
 * This is connected to g_Root (\sa DWorkspace::signalEditorOpened). 
 *  
 * Add to the central tab widget and add some dock window widgets. 
 *  
 * Other controls (menu,toolbar etc) are connected/disconnected based upon focus (\sa slotEditorFocused). 
 *  
 * \author pharvey (12/29/20)
 * 
 * \param pObject 
 * \param pEditorWidget 
 */
void EMainWindow::slotEditorOpened( AWObject *pObject, AWEditorWidget *pEditorWidget )
{
    int nTab = pCentralWidget->addTab( pEditorWidget, pObject->getName() );

//    if ( pEditorWidget->inherits( "LEditorWidget" ) )
//    { 
//        // diagrams have a zoom widget/dock
//        DDiagramEditorWidget *p = (DDiagramEditorWidget*)pEditorWidget;
//        pWidgetStackZoom->addWidget( (QWidget*)p->getZoomWidget() );
//    }

    // this will trigger slotEditorFocused
    pCentralWidget->setCurrentIndex( nTab );

//    doUpdateWindowTitle();
//    doUpdateTabText();

//    connect( pEditorWidget, SIGNAL(signalModified(AWEditorWidget*)), this, SLOT(slotModified(AWEditorWidget*)) );
//    connect( pEditorWidget, SIGNAL(signalGoTo(AWEditorWidget*)), this, SLOT(slotGoTo(AWEditorWidget*)) );
}

void EMainWindow::slotCloseEditor()
{
    if ( !pEditorWidget ) return;
    pEditorWidget->getObject()->slotCloseEditor(); // this triggers signalEditorClosing but gives LFile a chance to respond
}

/*!
 * \brief Focus has changed. 
 *  
 * Disconnect View that lost focus (if any) and then connect the one that got focus (if any). 
 * Sync App state. 
 *  
 * Will further sync App state with layer state by calling slotLayerChanged as needed.
 *  
 * \sa slotLayerChanged
 * 
 * \author pharvey (1/20/23)
 * 
 * \param n 
 */
void EMainWindow::slotEditorFocused( int n )
{
    // slotEditorOpened has already happened
    if ( pEditorWidget == (AWEditorWidget*)pCentralWidget->currentWidget() ) return;
    if ( pEditorWidget ) doDisconnectEditor();
    if ( n < 0 ) return;

    if ( (AWEditorWidget*)pCentralWidget->currentWidget() ) doConnectEditor( (AWEditorWidget*)pCentralWidget->currentWidget() );
}

void EMainWindow::slotEditorClosing( AWObject *pObject, AWEditorWidget *pEditorWidget )
{
    Q_ASSERT(pObject);
    Q_ASSERT(pEditorWidget);

    int n = pCentralWidget->indexOf( pEditorWidget );
    Q_ASSERT( n >= 0 );

    // The following is normally done in slotEditorFocused but in this case it would fail
    // as the objects in question will be deleted by then. So we do it here.
    if ( pEditorWidget == this->pEditorWidget )
    { 
        if ( pEditorWidget ) doDisconnectEditor();
    }

    // Just remove these - they will get deleted when the editor is deleted.
//    pWidgetStackClasses->removeWidget( pWidgetStackClasses->widget( n ) );
//    pWidgetStackPanners->removeWidget( pWidgetStackPanners->widget( n ) );  

//    if ( pEditorWidget->inherits( "LEditorWidget" ) )
//    { 
//        pWidgetStackZoom->removeWidget( pWidgetStackZoom->widget( n ) );
//    }
}

/*!
 * \brief Respond to pLayers SaveAs.
 * 
 * NOT called during Open due to order of events. \sa slotOpen.
 * 
 * \author pharvey (1/21/25)
 * 
 * \param s      
 */
void EMainWindow::slotChangedFileName( const QString &s )
{
    // update tab title
    doUpdateWindowTitle();
    doUpdateTabText();

    if ( s.isEmpty() ) return;      // canvas cleared

    // update recent file list
    doAddRecentFile( s );       // loaded or saved as
}

void EMainWindow::slotChangedLayers()
{
    Q_ASSERT( pLayers );

    doUpdateWindowTitle();
    doUpdateTabText();

    // file
    pActionSave->setEnabled( pLayers->isModified() ); 
    pActionSaveAs->setEnabled( true ); 
    pActionExport->setEnabled( true ); 
    pActionPrint->setEnabled( true ); 
    pActionClose->setEnabled( true ); 

    // status bar
    pModified->setPixmap( pLayers->isModified() ? QPixmap( ":W/Draw16x16" ) : QPixmap() );
}

void EMainWindow::slotChangedLayer() // layer modified state
{
    Q_ASSERT( pLayer );

    // edit
    pActionCut->setEnabled( pLayer->canCut() ); 
    pActionCopy->setEnabled( pLayer->canCopy() ); 
    pActionPaste->setEnabled( pLayer->canPaste() ); 
    pActionDelete->setEnabled( pLayer->canDelete() ); 
    pActionUndo->setEnabled( pLayer->canUndo() ); 
    pActionRedo->setEnabled( pLayer->canRedo() ); 
    pActionSelectAll->setEnabled( true ); 
    pActionSelectNone->setEnabled( pLayer->hasSelection() );

    // region
    pActionRegionCrop->setEnabled( pLayer->canCrop() ); 
}

void EMainWindow::slotChangedCurrentLayer()
{
    if ( pLayer ) doDisconnectLayer();
    if ( pLayers->getLayerCurrent() ) doConnectLayer();
}

void EMainWindow::slotDeletedLayer( ADObject *p )
{
    if ( !pLayer ) return;
    if ( p != pLayer ) return;
    doDisconnectLayer();
}

/*!
 * \brief Respond to pLayer changing tool.
 * 
 * \author pharvey (1/21/25)
 */
void EMainWindow::slotChangedTool()
{
    Q_ASSERT( pLayer );
    if ( pToolConfigWidget ) doDisconnectTool();
    if ( pLayer->getTool() >= 0 ) doConnectTool();
}

void EMainWindow::slotChangedLayerToolState()
{
    pActionCommit->setEnabled( pLayer->canCommit() );
    pActionCancel->setEnabled( pLayer->canCancel() ); 

    // in case tool is a selector
    pActionCut->setEnabled( pLayer->canCut() ); 
    pActionCopy->setEnabled( pLayer->canCopy() ); 
    pActionPaste->setEnabled( pLayer->canPaste() ); 
    pActionDelete->setEnabled( pLayer->canDelete() ); 
}

void EMainWindow::slotPaletteColorWindowTitle()
{
    QString stringFileName = g_SColors->getFileName();
    if ( stringFileName.isEmpty() )
    {
        pDockColors->setWindowTitle( tr("Colors") );
    }
    else
    {
        QFileInfo FileInfo( stringFileName );
        pDockColors->setWindowTitle( tr("Colors") + ( g_SColors->isModified() ? "* - " : " - " ) + FileInfo.baseName() );
    }
}




