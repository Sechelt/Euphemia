#include "AppInfo.h"
#include "EMainWindow.h"

#include <WPaletteColor.h>
#include <WIconLayout.h>
#include <WSizeDialog.h>

#include <PFillFlood.h>

#define PMAX_RECENT_FILES 5

EMainWindow::EMainWindow( QWidget *pWidget ) 
    : QMainWindow( pWidget )
{
    vectorRecentStrings.resize( PMAX_RECENT_FILES );
    vectorRecentActions.resize( PMAX_RECENT_FILES );

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
    doLoadState();

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
        PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea*)pTabWidget->widget( n );
        PCanvas *pCanvas = pLayersScrollArea->getLayers()->getCanvas();

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

        pActionCut->setShortcut( QKeySequence::Cut );
        pActionCopy->setShortcut( QKeySequence::Copy );
        pActionPaste->setShortcut( QKeySequence::Paste );
        pActionUndo->setShortcut( QKeySequence::Undo );
        pActionRedo->setShortcut( QKeySequence::Redo );
        pActionCancel->setShortcut( QKeySequence::Cancel );

        pActionAutoCommit->setToolTip( tr("auto commit changes to canvas else allow manipulation before commit when possible") );

        pActionAutoCommit->setCheckable( true );
        pActionAutoCommit->setChecked( true );

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
    }

    // VIEW
    {
        pActionZoomIn          = new QAction( QIcon( ":W/ZoomIn48x48" ), tr("Zoom &In"), this );
        pActionZoomOut         = new QAction( QIcon( ":W/ZoomOut48x48" ), tr("Zoom &Out"), this );
        pActionZoomIn->setShortcut( QKeySequence::ZoomIn );
        pActionZoomOut->setShortcut( QKeySequence::ZoomOut );

        // view -> tool bars
        pActionToolFile     = new QAction( tr("File"), this );
        pActionToolEdit     = new QAction( tr("Edit"), this );
        pActionToolPen      = new QAction( tr("Pen"), this );
        pActionToolBrush    = new QAction( tr("Brush"), this );
        pActionToolFont     = new QAction( tr("Font"), this );

        pActionToolFile->setCheckable( true );
        pActionToolEdit->setCheckable( true );
        pActionToolPen->setCheckable( true );
        pActionToolBrush->setCheckable( true );
        pActionToolFont->setCheckable( true );

        pActionToolFile->setChecked( true );
        pActionToolEdit->setChecked( true );
        pActionToolPen->setChecked( true );
        pActionToolBrush->setChecked( true );
        pActionToolFont->setChecked( true );

        // view -> docks
        pActionDockTools        = new QAction( tr("Tools"), this );
        pActionDockPaletteColor = new QAction( tr("Palette Color"), this );
        pActionDockScratch      = new QAction( tr("Scratch Pad"), this );

        pActionDockTools->setCheckable( true );
        pActionDockPaletteColor->setCheckable( true );
        pActionDockScratch->setCheckable( true );

        pActionDockTools->setChecked( true );
        pActionDockPaletteColor->setChecked( true );
        pActionDockScratch->setChecked( true );
    }

    // TOOLS
    pActionGroupTools = new QActionGroup( this );
    // Default
    {
        pActionSelectRectangle = new QAction( QIcon( ":E/SelectRectangle" ), tr(""), this );
        pActionSelectRectangle->setToolTip( tr("select using a rectangle") );
        pActionSelectRectangle->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectRectangle );  
        connect( pActionSelectRectangle, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionSelectEllipse = new QAction( QIcon( ":E/SelectEllipse" ), tr(""), this );
        pActionSelectEllipse->setToolTip( tr("select using an ellipse") );
        pActionSelectEllipse->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectEllipse );  
        connect( pActionSelectEllipse, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionSelectPolygon = new QAction( QIcon( ":E/SelectPolygon" ), tr(""), this );
        pActionSelectPolygon->setToolTip( tr("select using a polygon") );
        pActionSelectPolygon->setCheckable( true );
        pActionGroupTools->addAction( pActionSelectPolygon );  
        connect( pActionSelectPolygon, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawFreeHand = new QAction( QIcon( ":E/FreeHand" ), tr(""), this );
        pActionDrawFreeHand->setToolTip( tr("Free Hand") );
        pActionDrawFreeHand->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawFreeHand );  
        connect( pActionDrawFreeHand, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawSpray = new QAction( QIcon( ":E/Spray" ), tr(""), this );
        pActionDrawSpray->setToolTip( tr("Spray") );
        pActionDrawSpray->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawSpray );  
        connect( pActionDrawSpray, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawErase = new QAction( QIcon( ":E/Erase" ), tr(""), this );
        pActionDrawErase->setToolTip( tr("Erase") );
        pActionDrawErase->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawErase );  
        connect( pActionDrawErase, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawText = new QAction( QIcon( ":E/Text" ), tr(""), this );
        pActionDrawText->setToolTip( tr("Text") );
        pActionDrawText->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawText );  
        connect( pActionDrawText, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawLine = new QAction( QIcon( ":E/Line" ), tr(""), this );
        pActionDrawLine->setToolTip( tr("straight line") );
        pActionDrawLine->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawLine );  
        connect( pActionDrawLine, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawRectangle = new QAction( QIcon( ":E/Rectangle" ), tr(""), this );
        pActionDrawRectangle->setToolTip( tr("rectangle") );
        pActionDrawRectangle->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawRectangle );  
        connect( pActionDrawRectangle, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawEllipse = new QAction( QIcon( ":E/Ellipse" ), tr(""), this );
        pActionDrawEllipse->setToolTip( tr("ellipse") );
        pActionDrawEllipse->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawEllipse );  
        connect( pActionDrawEllipse, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolygon = new QAction( QIcon( ":E/Polygon" ), tr(""), this );
        pActionDrawPolygon->setToolTip( tr("polygon") );
        pActionDrawPolygon->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolygon );  
        connect( pActionDrawPolygon, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolyline = new QAction( QIcon( ":E/Polyline" ), tr(""), this );
        pActionDrawPolyline->setToolTip( tr("polyline") );
        pActionDrawPolyline->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolyline );  
        connect( pActionDrawPolyline, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawRectangleFilled = new QAction( QIcon( ":E/RectangleFilled" ), tr(""), this );
        pActionDrawRectangleFilled->setToolTip( tr("rectangle filled") );
        pActionDrawRectangleFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawRectangleFilled );  
        connect( pActionDrawRectangleFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawEllipseFilled = new QAction( QIcon( ":E/EllipseFilled" ), tr(""), this );
        pActionDrawEllipseFilled->setToolTip( tr("ellipse filled") );
        pActionDrawEllipseFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawEllipseFilled );  
        connect( pActionDrawEllipseFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionDrawPolygonFilled = new QAction( QIcon( ":E/PolygonFilled" ), tr(""), this );
        pActionDrawPolygonFilled->setToolTip( tr("polygon filled") );
        pActionDrawPolygonFilled->setCheckable( true );
        pActionGroupTools->addAction( pActionDrawPolygonFilled );  
        connect( pActionDrawPolygonFilled, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionFillFlood = new QAction( QIcon( ":E/FillFlood" ), tr(""), this );
        pActionFillFlood->setToolTip( tr("flood fill") );
        pActionFillFlood->setCheckable( true );
        pActionGroupTools->addAction( pActionFillFlood );  
        connect( pActionFillFlood, SIGNAL(triggered()), SLOT(slotToolTriggered()) );

        pActionFillGradient = new QAction( QIcon( ":E/FillGradient" ), tr(""), this );
        pActionFillGradient->setToolTip( tr("gradient fill") );
        pActionFillGradient->setCheckable( true );
        pActionGroupTools->addAction( pActionFillGradient );  
        connect( pActionFillGradient, SIGNAL(triggered()), SLOT(slotToolTriggered()) );
    }
    // REGION

    {
        pActionRegionFlipX      = new  QAction( tr("Flip X Axis"), this );
        pActionRegionFlipY      = new  QAction( tr("Flip Y Axis"), this );
        pActionRegionRotate     = new  QAction( tr("Rotate By.."), this );
        pActionRegionLinear     = new  QAction( tr("Linear Transformation..."), this );
        pActionRegionReset      = new  QAction( tr("Reset"), this );
        pActionRegionExpand     = new  QAction( tr("Expand"), this );
        pActionRegionDownscale  = new  QAction( tr("Downscale..."), this );
        pActionRegionEdit       = new  QAction( tr("Edit Region"), this );
        pActionRegionCrop       = new  QAction( tr("Crop"), this );
        pActionRegionCreateMask = new  QAction( tr("Create mask of Region"), this );
        pActionRegionMerge      = new  QAction( tr("Merge Region"), this );
        pActionRegionAutoCrop   = new  QAction( tr("Auto Crop..."), this );
        pActionRegionComplement = new  QAction( tr("Complement Region"), this );
        pActionRegionDelimit    = new  QAction( tr("Delimit Region"), this );
        pActionRegionOCR        = new  QAction( tr("OCR"), this );
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
    pMenuFile->addAction( pActionPrint );
    pMenuFile->addSeparator();
    pMenuFile->addAction( pActionExit );
    pMenuFile->addSeparator();

    // recent files - just blank for now
    for ( int n = 0; n < PMAX_RECENT_FILES; n++ )
    {
        vectorRecentStrings[n] = "";
        vectorRecentActions[n] = pMenuFile->addAction( QString("%1 -").arg( n + 1 ) );
    }
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

    // VIEW
    pMenuView = menuBar()->addMenu( tr("View") );
    pMenuView->addAction( pActionZoomIn  );
    pMenuView->addAction( pActionZoomOut );
    pMenuView->addSeparator();

    pMenuToolBars = new QMenu( tr("Tool Bars...") );
    pMenuToolBars->addAction( pActionToolFile  );
    pMenuToolBars->addAction( pActionToolEdit  );
    pMenuToolBars->addAction( pActionToolPen   );
    pMenuToolBars->addAction( pActionToolBrush );
    pMenuToolBars->addAction( pActionToolFont  );
    pMenuView->addMenu( pMenuToolBars );

    pMenuDocks = new QMenu( tr("Dock Windows...") );
    pMenuDocks->addAction( pActionDockTools        );
    pMenuDocks->addAction( pActionDockPaletteColor   );
    pMenuDocks->addAction( pActionDockScratch      );
    pMenuView->addMenu( pMenuDocks );

    // TOOLS
    pMenuTools = menuBar()->addMenu( tr("Tools") );

    // REGION
    pMenuRegion = menuBar()->addMenu( tr("Region") );
    pMenuRegion->addAction( pActionRegionFlipX      );
    pMenuRegion->addAction( pActionRegionFlipY      );
    pMenuRegion->addAction( pActionRegionRotate     );
    pMenuRegion->addAction( pActionRegionLinear     );
    pMenuRegion->addAction( pActionRegionReset      );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionExpand     );
    pMenuRegion->addAction( pActionRegionDownscale  );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionEdit       );
    pMenuRegion->addAction( pActionRegionCrop       );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionCreateMask );
    pMenuRegion->addAction( pActionRegionMerge      );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionAutoCrop   );
    pMenuRegion->addAction( pActionRegionComplement );
    pMenuRegion->addAction( pActionRegionDelimit    );
    pMenuRegion->addSeparator();
    pMenuRegion->addAction( pActionRegionOCR        );

    // FILTERS
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

    pToolBar = addToolBar( tr("Edit") );
    pToolBar->setObjectName( "Edit" );
    pToolBar->addAction( pActionCut );
    pToolBar->addAction( pActionCopy );
    pToolBar->addAction( pActionPaste );
    pToolBar->addAction( pActionUndo );
    pToolBar->addAction( pActionRedo );
    pToolBar->addAction( pActionCommit );
    pToolBar->addAction( pActionCancel );

    pToolBarToolConfig = addToolBar( tr("Tool Config") );
    pToolBarToolConfig->setObjectName( "ToolConfig" );
}

void EMainWindow::doInitStatusBar()
{
    pMessage= new QLabel( tr("welcome"), statusBar() );
    pMessage->setToolTip( tr("general messages") );
    statusBar()->addWidget( pMessage, 10 );

    pModified  = new QLabel( statusBar() );
    pModified->setToolTip( tr("edit status") );
    statusBar()->addPermanentWidget( pModified, 0 );

    pCoord= new WCoordWidget( statusBar() );
    pCoord->setToolTip( tr("pixel coordinate") );
    pCoord->slotCoord( 0.0, 0.0 );
    statusBar()->addPermanentWidget( pCoord, 0 );

    pZoom = new WZoomWidget( statusBar() );
    statusBar()->addPermanentWidget( pZoom, 0 );
    pZoom->setEnabled( false );
    connect( pZoom, SIGNAL(signalZoom(int,int)), this, SLOT(slotZoom(int,int)) );
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
    QDockWidget *pDock = new QDockWidget( tr("Tools"), this );
    pDock->setObjectName( "DockTools" );
    pDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    // layout the tool buttons on a widget using a flow layout
    QToolButton *pButton;
    QWidget *    pWidgetTools;
    QVBoxLayout *pLayoutTop;
    WIconLayout *pIconLayout;

    pWidgetTools    = new QWidget( pDock );
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
    QScrollArea *pScrollArea = new QScrollArea( pDock );
    pScrollArea->setWidgetResizable( true );
    pScrollArea->setWidget( pWidgetTools );

    pDock->setWidget( pScrollArea );
    addDockWidget( Qt::LeftDockWidgetArea, pDock );
}

void EMainWindow::doInitDockColors()
{
    pDockColors = new QDockWidget( tr("Colors - User defined"), this );
    pDockColors->setObjectName( "DockColors" );
    pDockColors->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pPaletteColor = new WPaletteColorWidget( pDockColors );
    pDockColors->setWidget( pPaletteColor );
    addDockWidget( Qt::LeftDockWidgetArea, pDockColors );
    // viewMenu->addAction(dock->toggleViewAction());

    slotPaletteColorWindowTitle();
    connect( g_PaletteColors, SIGNAL(signalModifiedFileName()), SLOT(slotPaletteColorWindowTitle()) );
}

void EMainWindow::doInitDockScratch()
{
    QDockWidget *pDock = new QDockWidget( tr("Scratch"), this );
    pDock->setObjectName( "DockScratch" );
    pDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pScratchTool = new WScratchTool( pDock );
    pDock->setWidget( pScratchTool );
    addDockWidget( Qt::LeftDockWidgetArea, pDock );
    // viewMenu->addAction(dock->toggleViewAction());

    // enable when;
    // - when canvas has selection (selection only)     
    // - clipboard has image (clipboard image)          
    // - when canvas (all canvas)                       
    //
    // Prompt will decide (or cancel).
    pScratchTool->doEnableAdd(); 
    connect( pScratchTool, SIGNAL(signalAdd()), SLOT(slotScratch()) );
    connect( pScratchTool, SIGNAL(signalPaste(const QImage &)), SLOT(slotScratch(const QImage &)) );
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
    for ( int n = 0; n < PMAX_RECENT_FILES; n++ )
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
        vectorRecentStrings[ n ] = s;
        vectorRecentActions[ n ]->setText( QString( "%1 - %2" ).arg( n + 1 ).arg( s ) );
    }
    settings.endGroup();
}

void EMainWindow::doUpdateRecentFiles( const QString &stringFileName )
{
    if ( vectorRecentStrings.contains( stringFileName ) ) return;

    for ( int n = PMAX_RECENT_FILES - 1; n > 0; n-- )
    {
        vectorRecentStrings[n] = vectorRecentStrings[n - 1];
    }
    vectorRecentStrings[0] = stringFileName;

    for ( int n = 0; n < PMAX_RECENT_FILES; n++ )
    {
        vectorRecentActions[n]->setText( QString( "%1 - %2" ).arg( n + 1 ).arg( vectorRecentStrings[n] ) );
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

PLayers *EMainWindow::getLayersCurrent()
{
    PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea*)pTabWidget->currentWidget(); 

    // DO NOT call here if there is no current widget
    // - most actions will be disabled when no current widget so this is just to make sure
    Q_ASSERT( pLayersScrollArea );

    return pLayersScrollArea->getLayers();
}

PCanvas *EMainWindow::getCanvasCurrent()
{
    // There will always be a current canvas as layers has 1-n.
    return getLayersCurrent()->getCanvas();
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
    PLayersScrollArea *pScrollArea = new PLayersScrollArea( pTabWidget );
    pScrollArea->setBackgroundRole( QPalette::Dark );

    PCanvas *pCanvas = pScrollArea->getLayers()->getCanvas();
    pCanvas->setTool( nTool );
    pCanvas->setAutoCommit( pActionAutoCommit->isChecked() );

    pTabWidget->addTab( pScrollArea, tr("unnamed") );
    pTabWidget->setCurrentWidget( pScrollArea );

    connect( pCanvas, SIGNAL(signalPos(const QPoint &)), pCoord, SLOT(slotCoord(const QPoint &)) );

    return true;
}

bool EMainWindow::slotNewWithSize()
{
    QSize size( 1024, 768 );                                                                       
    {                                                                                              
        bool bOk = false;                                                                          
        size = WSizeDialogI::getSize( &bOk, size, this );                                          
        if ( !bOk ) return false;                                                                  
    }                                                                                              
                                                                                                   
    slotNew();
    getLayersCurrent()->resize( size );

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
    PLayers *pLayer = getLayersCurrent();
    pLayer->resize( image.size() );
    pLayer->getCanvas()->doPaste();
    pLayer->getCanvas()->doCommit();

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
    PLayers *pLayers = getLayersCurrent();
    PCanvas *pCanvas = pLayers->getCanvas();

    if ( !pCanvas->doOpen() )                                                                      
    {                                                                                              
        slotClose( false );
        return false;                                                                              
    }                                                                                              
                                                                                                   
    pLayers->resize( pCanvas->size() );

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
    return getCanvasCurrent()->doSave();
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
    return getCanvasCurrent()->doSaveAs();
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
    PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea*)pTabWidget->currentWidget(); 
    int nIndex = pTabWidget->currentIndex(); 
    Q_ASSERT( nIndex >= 0 );

    if ( bPrompt && !getCanvasCurrent()->doClose() ) return false;

    pTabWidget->removeTab( nIndex );
    delete pLayersScrollArea; // canvas deleted by canvasview
    pCanvas = nullptr;

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

    return true;
}

void EMainWindow::slotExit()
{
//    if ( pWorkspace && !slotCloseWorkspace() ) return;
    close();
}

void EMainWindow::slotCut()
{
    getCanvasCurrent()->doCut();
}

void EMainWindow::slotCopy()
{
    getCanvasCurrent()->doCopy();
}

void EMainWindow::slotPaste()
{
    getCanvasCurrent()->doPaste();
}

void EMainWindow::slotUndo()
{
    getCanvasCurrent()->doUndo();
}

void EMainWindow::slotRedo()
{
    getCanvasCurrent()->doRedo();
}

void EMainWindow::slotUndoLevels()
{
}

void EMainWindow::slotSelectAll()
{
}

void EMainWindow::slotSelectNone()
{
}

void EMainWindow::slotAutoCommit( bool b )
{
    int nCount = pTabWidget->count();
    for ( int n = 0; n < nCount; n++ )
    {
        PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea*)pTabWidget->widget( n );
        PCanvas *pCanvas = pLayersScrollArea->getLayers()->getCanvas();
        pCanvas->setAutoCommit( b );
    }
}

void EMainWindow::slotCommit()
{
    getCanvasCurrent()->doCommit();
}

void EMainWindow::slotCancel()
{
    getCanvasCurrent()->doCancel();
}

void EMainWindow::slotAbout()
{
    QMessageBox::about( this, 
                        tr( "About..." ),
                        tr( "<b>" APP_NAME "</b><br><br>" ) +
                        tr( "Description: " APP_DESC "</b><br>" ) +
                        tr( "Version: v" APP_VER "<br>" ) +
                        tr( "Credits: Peter Harvey<br>" ) +
                        tr( "License: " CBD_COPYRIGHT "<br>" ) +
                        tr( "Web: https://www.codebydesign.com/<br>" )
                      );
}

void EMainWindow::slotAboutCBD()
{
    // seems app icon gets in there automagically
    QMessageBox::about( this, 
                        tr( "About " CBD_COMPANY "..." ),
                        tr( "<b>Sponsor of QPaint development.<br>" ) + 
                        tr( "Description: " CBD_COMPANY " is a software company which specializes in tools for; System Architects, Developers, and Consultants.<br><br>" ) +
                        tr( "Web: http://www.codebydesign.com<br>" )
                      );
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
    PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea *)pTabWidget->widget( nIndex );

    // cancel any drawing that may have been happening
    if ( pCanvas && pCanvas->isDrawing() ) pCanvas->doCancel();
    // no canvas - so disable a bunch of controls
    if ( nIndex < 0 )
    {
        if ( pCanvas  )
        {
            disconnect( pCanvas, SIGNAL(signalChangedState()), this, SLOT(slotCanvasChangedState()) );
            pCanvas = nullptr;
        }

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
        pActionCommit->setEnabled( false ); 
        pActionCancel->setEnabled( false ); 

        // view

        // status bar
        pZoom->setEnabled( false );

        return;
    }

    // canvas - so sync
    if ( pCanvas ) disconnect( pCanvas, SIGNAL(signalChangedState()), this, SLOT(slotCanvasChangedState()) );
    pCanvas = pLayersScrollArea->getLayers()->getCanvas();
    connect( pCanvas, SIGNAL(signalChangedState()), this, SLOT(slotCanvasChangedState()) );
    slotCanvasChangedState();
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
    if ( !pCanvas ) return;

    // update tab text (file name with * when modified)
    {
        QString stringTabText = pCanvas->getFileName();
        if ( stringTabText.isEmpty() ) stringTabText = tr( "unnamed" );
        if ( pCanvas->isModified() ) stringTabText += " *";
        pTabWidget->setTabText( pTabWidget->indexOf( pCanvas ), stringTabText );
    }

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
    pActionCommit->setEnabled( pCanvas->canCommit() ); 
    pActionCancel->setEnabled( pCanvas->canCancel() ); 

    // status bar
    pZoom->setEnabled( true );
    pZoom->setFit( pCanvas->getFit() );
    pZoom->setZoom( pCanvas->getZoom() );
}

void EMainWindow::slotScratch()
{
    // clipboard
    QImage image;                                                                                                  
    {                                                                                                              
        const QClipboard *pClipboard = QApplication::clipboard();                                                  
        const QMimeData *pMimeData = pClipboard->mimeData();                                                       

        if ( !pMimeData->hasImage() )                                                                              
        {                                                                                                          
            QMessageBox::information( this, tr("New From Paste"), tr("The clipboard does not contain an image.") );
            return;                                                                                          
        }                                                                                                          
        image = qvariant_cast<QImage>( pMimeData->imageData() );                                                   
    }                                                                                                              

    image = image.convertToFormat( QImage::Format_ARGB32 );                                                        

    pScratchTool->doAppend( image );
}

void EMainWindow::slotScratch( const QImage &image )
{
    if ( !pTabWidget->currentWidget() ) return;
    PCanvas *pCanvas = getCanvasCurrent();
    pCanvas->doPaste( image );
}

void EMainWindow::slotToolTriggered()
{
    // cancel any drawing that may have been happening
    if ( pCanvas && pCanvas->isDrawing() )
        pCanvas->doCancel();

    // remove any tool config widget from the tool bar
    if ( pWidgetToolConfig )
    {
        delete pWidgetToolConfig;
        pWidgetToolConfig = nullptr;
    }

    // update our nTool
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
        pWidgetToolConfig = new PFreeHandToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawSpray->isChecked() )           
    {
        nTool = PCanvas::ToolDrawSpray;
        pWidgetToolConfig = new PSprayToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawErase->isChecked() )           
    {
        nTool = PCanvas::ToolDrawErase;
    }
    else if ( pActionDrawText->isChecked() )           
    {
        nTool = PCanvas::ToolDrawText;
        pWidgetToolConfig = new PTextToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawLine->isChecked() )            
    {
        nTool = PCanvas::ToolDrawLine;
        pWidgetToolConfig = new PLineToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawRectangle->isChecked() )       
    {
        nTool = PCanvas::ToolDrawRectangle;
        pWidgetToolConfig = new PRectangleToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawEllipse->isChecked() )         
    {
        nTool = PCanvas::ToolDrawEllipse;
        pWidgetToolConfig = new PEllipseToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawPolygon->isChecked() )         
    {
        nTool = PCanvas::ToolDrawPolygon;
        pWidgetToolConfig = new PPolygonToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawPolyline->isChecked() )         
    {
        nTool = PCanvas::ToolDrawPolyline;
        pWidgetToolConfig = new PPolylineToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawRectangleFilled->isChecked() ) 
    {
        nTool = PCanvas::ToolDrawRectangleFilled;
        pWidgetToolConfig = new PRectangleFilledToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawEllipseFilled->isChecked() )   
    {
        nTool = PCanvas::ToolDrawEllipseFilled;
        pWidgetToolConfig = new PEllipseFilledToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionDrawPolygonFilled->isChecked() )   
    {
        nTool = PCanvas::ToolDrawPolygonFilled;
        pWidgetToolConfig = new PPolygonFilledToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionFillFlood->isChecked() )           
    {
        nTool = PCanvas::ToolFillFlood;
        pWidgetToolConfig = new PFillFloodToolBar( pToolBarToolConfig );
        pToolBarToolConfig->addWidget( pWidgetToolConfig );  
    }
    else if ( pActionFillGradient->isChecked() )        
    {
        nTool = PCanvas::ToolFillGradient;
    }
    else
    {
        Q_ASSERT( 1==2 );
    }

    // set for all canvas's
    int nCount = pTabWidget->count();
    for ( int n = 0; n < nCount; n++ )
    {
        PLayersScrollArea *pLayersScrollArea = (PLayersScrollArea*)pTabWidget->widget( n );
        PCanvas *pCanvas = pLayersScrollArea->getLayers()->getCanvas();
        pCanvas->setTool( nTool );
    }
}

void EMainWindow::slotZoom( int nFit, int nZoom )
{
    getCanvasCurrent()->setZoom( (WZoomWidget::FitTypes)nFit, nZoom );
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


