#include "LibInfo.h"
#include "DDiagramEditorWidget.h"

#include "DGraphicsSceneProxy.h"
#include "DGraphicsView.h"
#include "DDiagram.h"
#include "DRectangle.h"
#include "DGraphicsItemLine.h"
#include "DDiagramPalWidget.h"
#include "DDiagramPanWidget.h"

#include "AWScratchWidget.h"

DDiagramEditorWidget::DDiagramEditorWidget( DDiagram *pDiagram, QWidget *pParent )
    : AWEditorWidget( pDiagram, pParent )
{
    this->pDiagram      = pDiagram;

    pRulerCorner    = nullptr;
    pRulerH         = nullptr;
    pRulerV         = nullptr;
    pLayoutHTop     = nullptr;
    pLayoutRulerLeft= nullptr;
    pLayoutVTop     = nullptr;
    pZoom           = nullptr;

    doInitCentralArea();
    doInitActions();
    doInitMenus();
    doInitToolbar();
    doInitStatusBar();

    // this will configure central area layout
    setRuler( pDiagram->isRuler() );

    connect( pDiagram, SIGNAL(signalChangedSelection(bool)), SLOT(slotSelectionChanged(bool)) );
    connect( QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), SLOT(slotClipboardChanged(QClipboard::Mode)) );
}

DDiagramEditorWidget::~DDiagramEditorWidget()
{
    if ( pPalWidget )
    { 
        delete pPalWidget;
        pPalWidget = nullptr;
    }

    if ( pPanWidget )
    { 
        delete pPanWidget;
        pPanWidget = nullptr;
    }

    if ( pZoom )
    { 
        delete pZoom;
        pZoom = nullptr;
    }
}

void DDiagramEditorWidget::setRuler( bool b )
{
    // clear
    if ( pRulerCorner )
    {
        delete pRulerCorner;
        delete pRulerH;
        delete pRulerV;
        pRulerCorner    = nullptr;
        pRulerH         = nullptr;
        pRulerV         = nullptr;

        delete pLayoutRulerLeft;
        delete pLayoutVTop;
        delete pLayoutHTop;
        pLayoutHTop         = nullptr;
        pLayoutRulerLeft    = nullptr;
        pLayoutVTop         = nullptr;
    }

    if ( pLayoutVTop )
    { 
        delete pLayoutVTop;
        pLayoutVTop         = nullptr;
    }

    // add rulers?
    if ( b )
    {
printf( "[PAH][%s][%s][%d] Cell W %d H %d\n", __FILE__, __FUNCTION__, __LINE__, getDiagram()->getCell().width(), getDiagram()->getCell().height() );

        pLayoutHTop         = new QHBoxLayout();
        pLayoutRulerLeft    = new QVBoxLayout();
        pLayoutVTop         = new QVBoxLayout();

        pRulerCorner = new WRulerCornerWidget( this );
        pLayoutRulerLeft->addWidget( pRulerCorner );

        pRulerV = new WRulerWidget( this, Qt::Vertical, getDiagram()->getCell().height() / 2 );
        pRulerV->slotUpdate( pView->getOffsetV(), pView->getScaleV(), pView->getLengthV() );
        connect( pView, SIGNAL(signalChangedV(qreal,qreal,qreal)), pRulerV, SLOT(slotUpdate(qreal,qreal,qreal)) );

        connect( pRulerV, SIGNAL(signalGuideNewRequest(Qt::Orientation,qreal)), pDiagram, SLOT(slotGuideNewRequest(Qt::Orientation,qreal)) );
        connect( pRulerV, SIGNAL(signalGuideDeleteRequest(Qt::Orientation,int)), pDiagram, SLOT(slotGuideDeleteRequest(Qt::Orientation,int)) );
        connect( pRulerV, SIGNAL(signalGuideMoveRequest(Qt::Orientation,int,qreal)), pDiagram, SLOT(slotGuideMoveRequest(Qt::Orientation,int,qreal)) );

        pLayoutRulerLeft->addWidget( pRulerV );
        pLayoutHTop->addLayout( pLayoutRulerLeft );

        pRulerH = new WRulerWidget( this, Qt::Horizontal, getDiagram()->getCell().width() / 2 );
        pRulerH->slotUpdate( pView->getOffsetH(), pView->getScaleH(), pView->getLengthH() );
        connect( pView, SIGNAL(signalChangedH(qreal,qreal,qreal)), pRulerH, SLOT(slotUpdate(qreal,qreal,qreal)) );

        connect( pRulerH, SIGNAL(signalGuideNewRequest(Qt::Orientation,qreal)), pDiagram, SLOT(slotGuideNewRequest(Qt::Orientation,qreal)) );
        connect( pRulerH, SIGNAL(signalGuideDeleteRequest(Qt::Orientation,int)), pDiagram, SLOT(slotGuideDeleteRequest(Qt::Orientation,int)) );
        connect( pRulerH, SIGNAL(signalGuideMoveRequest(Qt::Orientation,int,qreal)), pDiagram, SLOT(slotGuideMoveRequest(Qt::Orientation,int,qreal)) );

        pLayoutVTop->addWidget( pRulerH );
        pLayoutVTop->addWidget( pView );
        pLayoutHTop->addLayout( pLayoutVTop );

        pLayoutCentral->addLayout( pLayoutHTop );

        // add any guides
        {
            DGraphicsItemLine *pItem;
            QVector<DGraphicsItemLine*> v = pDiagram->getGuidesX();
            foreach( pItem, v )
            {
                pRulerH->doGuideNew( pItem->getLine().x1() );
            }

            v = pDiagram->getGuidesY();
            foreach( pItem, v )
            {
                pRulerV->doGuideNew( pItem->getLine().y1() );
            }
        }

        return; 
    }

    // no rulers
    pLayoutVTop = new QVBoxLayout();
    pLayoutVTop->addWidget( pView );
    pLayoutCentral->addLayout( pLayoutVTop );
}

void DDiagramEditorWidget::doGoTo( DObject *p )
{
    pView->centerOn( p->getProxy()->getItem() );
}

DDiagram *DDiagramEditorWidget::getDiagram() 
{ 
    return pDiagram; 
}

DGraphicsSceneProxy *DDiagramEditorWidget::getScene() 
{ 
    return pDiagram->getScene(); 
}

AWPalWidget *DDiagramEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new DDiagramPalWidget( this );
    return pPalWidget;
}

AWPanWidget *DDiagramEditorWidget::getPanWidget()
{
    if ( pPanWidget ) return pPanWidget;
    DDiagramPanWidget *pDiagramPanWidget;
    pPanWidget = pDiagramPanWidget = new DDiagramPanWidget( this, pDiagram->getScene() );;
    DPanner *pPanner = pDiagramPanWidget->getPanner();
    connect( pView->horizontalScrollBar(), SIGNAL(valueChanged(int)), pPanner, SLOT(slotUpdate()) );
    connect( pView->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), pPanner, SLOT(slotUpdate()) );
    connect( pView->verticalScrollBar(), SIGNAL(valueChanged(int)), pPanner, SLOT(slotUpdate()) );
    connect( pView->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), pPanner, SLOT(slotUpdate()) );
    return pPanWidget;
}

WRulerWidget *DDiagramEditorWidget::getRuler( Qt::Orientation n )
{
    if ( n == Qt::Horizontal ) return pRulerH;
    return pRulerV;
}

void DDiagramEditorWidget::doInitCentralArea()
{
    setCentralWidget( new QWidget( this ) );
    pLayoutCentral = new QVBoxLayout( centralWidget() );

    pView = new DGraphicsView( pDiagram->getScene(), this );
    pView->setRenderHint( QPainter::Antialiasing, true );
    pView->setMouseTracking( true );
}

void DDiagramEditorWidget::doInitActions()
{
    // edit
    pActionCut          = new QAction( QIcon( ":W/cut" ), tr("&Cut"), this );
    pActionCopy         = new QAction( QIcon( ":W/copy" ), tr("C&opy"), this );
    pActionPaste        = new QAction( QIcon( ":W/paste" ), tr("&Paste"), this );
    pActionUndo         = new QAction( QIcon( ":W/Undo48x48" ), tr("&Undo"), this );
    pActionRedo         = new QAction( QIcon( ":W/Redo48x48" ), tr("&Redo"), this );
    pActionSelectAll    = new QAction( QIcon( ":W/SelectAll16x16" ), tr("Select All"), this );
    pActionSelectNone   = new QAction( QIcon( ":W/SelectNone16x16" ), tr("Select None"), this );
    pActionGroup        = new QAction( QIcon( ":W/Group" ), tr("Group"), this );
    pActionUngroup      = new QAction( QIcon( ":W/Ungroup" ), tr("Ungroup"), this );
    pActionToFront      = new QAction( QIcon( ":W/ToFront48x48" ), tr("To Front"), this );
    pActionToBack       = new QAction( QIcon( ":W/ToBack48x48" ), tr("To Back"), this );
    pActionOptimizeSize = new QAction( QIcon( ":D/OptimizeSize" ), tr("Optimal Size"), this );
    pActionSize16       = new QAction( tr("16"), this );
    pActionSize32       = new QAction( tr("32"), this );
    pActionSize48       = new QAction( tr("48"), this );
    pActionSize64       = new QAction( tr("64"), this );
    pActionSize96       = new QAction( tr("96"), this );
    pActionSize128      = new QAction( tr("128"), this );
    pActionResetTransformation      = new QAction( tr("Reset Transformation"), this );
    pActionSnapGrid     = new QAction( tr("Snap To Grid"), this );
    pActionDelete       = new QAction( QIcon( ":W/remove" ), tr("&Delete"), this );

    pActionCut->setShortcut( QKeySequence::Cut );
    pActionCopy->setShortcut( QKeySequence::Copy );
    pActionPaste->setShortcut( QKeySequence::Paste );
    pActionSelectAll->setShortcut( QKeySequence::SelectAll );
    pActionSelectNone->setShortcut( QKeySequence::Deselect );
    pActionUndo->setShortcut( QKeySequence::Undo );
    pActionRedo->setShortcut( QKeySequence::Redo );
    pActionDelete->setShortcut( QKeySequence::Delete );

    pActionSelectAll->setToolTip( tr( "select all" ) );
    pActionSelectNone->setToolTip( tr( "select none" ) );
    pActionToFront->setToolTip( tr( "move shape to front" ) );
    pActionToBack->setToolTip( tr( "move shape to back" ) );
    pActionOptimizeSize->setToolTip( tr( "optimize shape size for contents\nNote: some shapes can not be resized" ) );
    pActionResetTransformation->setToolTip( tr( "undo rotate/shear" ) );

    pActionCut->setEnabled( false );
    pActionCopy->setEnabled( false );
    pActionPaste->setEnabled( false );
    pActionUndo->setEnabled( false );
    pActionRedo->setEnabled( false );
    pActionSelectAll->setEnabled( true );
    pActionSelectNone->setEnabled( true );
    pActionGroup->setEnabled( false );
    pActionUngroup->setEnabled( false ); 
    pActionToFront->setEnabled( false );
    pActionToBack->setEnabled( false ); 
    pActionOptimizeSize->setEnabled( false ); 
    pActionSize16->setEnabled( false );       
    pActionSize32->setEnabled( false );       
    pActionSize48->setEnabled( false );       
    pActionSize64->setEnabled( false );       
    pActionSize96->setEnabled( false );       
    pActionSize128->setEnabled( false );      
    pActionResetTransformation->setEnabled( false );      
    pActionSnapGrid->setEnabled( false );
    pActionDelete->setEnabled( false );

    pActionSnapGrid->setCheckable( true );

    pActionSnapGrid->setChecked( false );

    // view
    pActionPageBreaks = new QAction( tr("Page Breaks"), this );
    pActionGrid = new QAction( tr("Grid"), this );
    pActionRuler = new QAction( tr("Ruler"), this );
    pActionCrossHairs = new QAction( tr("Cross Hairs"), this );

    pActionPageBreaks->setCheckable( true );
    pActionGrid->setCheckable( true );
    pActionRuler->setCheckable( true );
    pActionCrossHairs->setCheckable( true );


    pActionPageBreaks->setChecked( false );
    pActionGrid->setChecked( false );
    pActionRuler->setChecked( false );
    pActionCrossHairs->setChecked( false );

    // settings
    pActionSettingsGrid = new QAction( tr("Grid..."), this );
    pActionSettingsRuler = new QAction( tr("Ruler..."), this );
}

void DDiagramEditorWidget::doInitMenus()
{
    pMenuEdit = menuBar()->addMenu( tr("&Edit") );
    pMenuEdit->addAction( pActionCut );
    pMenuEdit->addAction( pActionCopy );
    pMenuEdit->addAction( pActionPaste );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionUndo );
    pMenuEdit->addAction( pActionRedo );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionSelectAll );
    pMenuEdit->addAction( pActionSelectNone );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionGroup );
    pMenuEdit->addAction( pActionUngroup );
    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionToFront );
    pMenuEdit->addAction( pActionToBack );
    pMenuEdit->addSeparator();

    QMenu *pMenu = new QMenu( tr("Shape...") );
    pMenu->addAction( pActionSnapGrid );
    pMenuEdit->addSeparator();
    pMenu->addAction( pActionOptimizeSize ); 
    pMenu->addAction( pActionSize16 );       
    pMenu->addAction( pActionSize32 );       
    pMenu->addAction( pActionSize48 );       
    pMenu->addAction( pActionSize64 );       
    pMenu->addAction( pActionSize96 );       
    pMenu->addAction( pActionSize128 );      
    pMenuEdit->addSeparator();
    pMenu->addAction( pActionResetTransformation );      
    pMenuEdit->addMenu( pMenu );
    pMenuEdit->addSeparator();

    pMenuEdit->addSeparator();
    pMenuEdit->addAction( pActionDelete );

    pMenuView = menuBar()->addMenu( tr("&View") );
    pMenuView->addAction( pActionPageBreaks );
    pMenuView->addAction( pActionGrid );
    pMenuView->addAction( pActionRuler );
    pMenuView->addAction( pActionCrossHairs );

    pMenuView = menuBar()->addMenu( tr("&Settings") );
    pMenuView->addAction( pActionSettingsGrid );
    pMenuView->addAction( pActionSettingsRuler );
}

void DDiagramEditorWidget::doInitToolbar()
{
    QToolBar *pToolBar;
    pToolBar = addToolBar( tr("Edit") );
    pToolBar->setObjectName( "Edit" );
    pToolBar->addAction( pActionCut );
    pToolBar->addAction( pActionCopy );
    pToolBar->addAction( pActionPaste );
    pToolBar->addAction( pActionUndo );
    pToolBar->addAction( pActionRedo );
    pToolBar->addSeparator();
    pToolBar->addAction( pActionSelectAll );
    pToolBar->addAction( pActionSelectNone );
    pClassesWidget = new QComboBox( pToolBar );
    {
        QStringList v = pDiagram->getClasses( true );
        for ( int n = 0; n < v.count(); n++ )
        {
            pClassesWidget->addItem( v.at( n ) );
        }
    }
    pClassesWidget->setMinimumWidth( QFontMetrics( QFont() ).horizontalAdvance( tr("Associative Associative") ) );
    pClassesWidget->setDisabled( false );
    pClassesWidget->setToolTip( tr("select all objects of a certain class") );
    pToolBar->addWidget( pClassesWidget );
    pToolBar->addSeparator();
    pToolBar->addAction( pActionGroup );
    pToolBar->addAction( pActionUngroup );
    pToolBar->addAction( pActionToFront );
    pToolBar->addAction( pActionToBack );
    pToolBar->addSeparator();
    pToolBar->addAction( pActionDelete );

    // the defaults for these should come from preferences->diagram
    pToolBar = addToolBar( tr("Tool Attributes") );
    pToolBar->setObjectName( "ToolAttributes" );
    pToolBar->addWidget( pPenButton   = new WPenButton( pDiagram->getPen(), this ) );
    pToolBar->addWidget( pBrushButton = new WBrushButton( pDiagram->getBrush(), this ) );
    pToolBar->addWidget( pTextButton  = new WTextButton( pDiagram->getText(), this ) );
    
    connect( pPenButton, SIGNAL(signalChanged(const QPen &)), pDiagram, SLOT(slotPen(const QPen &)) );
    connect( pDiagram, SIGNAL(signalChangedPen(const QPen &)), pPenButton, SLOT(slotRefresh(const QPen &)) );
    connect( pBrushButton, SIGNAL(signalChanged(const QBrush &)), pDiagram, SLOT(slotBrush(const QBrush &)) );
    connect( pDiagram, SIGNAL(signalChangedBrush(const QBrush &)), pBrushButton, SLOT(slotRefresh(const QBrush &)) );
    connect( pTextButton, SIGNAL(signalChanged(const CBD::CBDText &)), pDiagram, SLOT(slotText(const CBD::CBDText &)) );
    connect( pDiagram, SIGNAL(signalChangedText(const CBD::CBDText &)), pTextButton, SLOT(slotRefresh(const CBD::CBDText &)) );

    pToolBar = addToolBar( tr("AlignObject") );
    pToolBar->setObjectName( "AlignObject" );
    pToolBar->addWidget( pAlignHObjectWidget = new WObjectHAlignComboBox( Qt::AlignLeft, pToolBar ) );
    pToolBar->addWidget( pAlignVObjectWidget = new WObjectVAlignComboBox( Qt::AlignTop, pToolBar ) );
    pAlignHObjectWidget->setDisabled( true );
    pAlignVObjectWidget->setDisabled( true );

    pToolBar = addToolBar( tr("Size") );
    pToolBar->setObjectName( "Size" );
    pShapeSize = new WShapeSizeComboBox( pToolBar );
    pShapeSize->setDisabled( true );
    pShapeSize->setToolTip( tr("common sizes for shapes\nNote: some shapes are fixed size") );
    pToolBar->addWidget( pShapeSize );
    pToolBar->addAction( pActionOptimizeSize );

    pActionCut->setEnabled( pDiagram->canCut() );
    pActionCopy->setEnabled( pDiagram->canCopy() );
    pActionPaste->setEnabled( pDiagram->canPaste() );
    pActionDelete->setEnabled( pDiagram->canCut() );
    pActionUndo->setEnabled( pDiagram->canUndo() );
    pActionRedo->setEnabled( pDiagram->canRedo() );
    pActionGroup->setEnabled( pDiagram->isSelection() );
    pActionUngroup->setEnabled( pDiagram->isSelection() );
    pActionToFront->setEnabled( pDiagram->isSelection() );
    pActionToBack->setEnabled( pDiagram->isSelection() );
    pActionOptimizeSize->setEnabled( pDiagram->isSelection() ); 
    pActionSize16->setEnabled( pDiagram->isSelection() );       
    pActionSize32->setEnabled( pDiagram->isSelection() );       
    pActionSize48->setEnabled( pDiagram->isSelection() );       
    pActionSize64->setEnabled( pDiagram->isSelection() );       
    pActionSize96->setEnabled( pDiagram->isSelection() );       
    pActionSize128->setEnabled( pDiagram->isSelection() );      
    pActionResetTransformation->setEnabled( pDiagram->isSelection() );      

    pShapeSize->setEnabled( pDiagram->isSelection() );

    // sync values
    pActionSnapGrid->setChecked( pDiagram->isSnapGrid() );
    pActionPageBreaks->setChecked( pDiagram->isPageBreaks() );
    pActionGrid->setChecked( pDiagram->isGrid() );
    pActionRuler->setChecked( pDiagram->isRuler() );
    pActionCrossHairs->setChecked( pDiagram->isCrossHairs() );

    connect( pActionCut,         SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectionCut()) );                 
    connect( pActionCopy,        SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectionCopy()) );               
    connect( pActionPaste,       SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectionPaste()) );             
    connect( pActionUndo,        SIGNAL(triggered()),                                pDiagram, SLOT(slotUndo()) );                        
    connect( pActionRedo,        SIGNAL(triggered()),                                pDiagram, SLOT(slotRedo()) );                        
    connect( pActionSelectAll,   SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectAll()) );              
    connect( pActionSelectNone,  SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectNone()) );            
    connect( pClassesWidget,     SIGNAL(currentTextChanged(const QString &)),        pDiagram, SLOT(slotSelectClass(const QString &)) ); 
    connect( pActionGroup,       SIGNAL(triggered()),                                pDiagram, SLOT(slotGroup()) );                  
    connect( pActionUngroup,     SIGNAL(triggered()),                                pDiagram, SLOT(slotUngroup()) );                    
    connect( pActionToFront,     SIGNAL(triggered()),                                pDiagram, SLOT(slotToFront()) );                  
    connect( pActionToBack,      SIGNAL(triggered()),                                pDiagram, SLOT(slotToBack()) );                    

    connect( pActionOptimizeSize,SIGNAL(triggered()),                                pDiagram, SLOT(slotOptimizeSize()) ); 
    connect( pActionSize16,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSize16()) ); 
    connect( pActionSize32,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSize32()) ); 
    connect( pActionSize48,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSize48()) ); 
    connect( pActionSize64,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSize64()) ); 
    connect( pActionSize96,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSize96()) ); 
    connect( pActionSize128,     SIGNAL(triggered()),                                pDiagram, SLOT(slotSize128()) ); 
    connect( pActionResetTransformation,SIGNAL(triggered()),                         pDiagram, SLOT(slotResetTransformation()) ); 

    connect( pActionDelete,      SIGNAL(triggered()),                                pDiagram, SLOT(slotSelectionDelete()) );           
    connect( pActionSnapGrid,    SIGNAL(toggled(bool)),                              pDiagram, SLOT(slotSnapGrid(bool)) );      
    connect( pActionPageBreaks,  SIGNAL(toggled(bool)),                              pDiagram, SLOT(slotPageBreaks(bool)) );      
    connect( pActionGrid,        SIGNAL(toggled(bool)),                              pDiagram, SLOT(slotGrid(bool)) );                  
    connect( pActionRuler,       SIGNAL(toggled(bool)),                              pDiagram, SLOT(slotRuler(bool)) );                  
    connect( pActionCrossHairs,  SIGNAL(toggled(bool)),                              pDiagram, SLOT(slotCrossHairs(bool)) );                  
    connect( pAlignHObjectWidget,SIGNAL(signalChanged(Qt::AlignmentFlag)),           pDiagram, SLOT(slotAlignHObject(Qt::AlignmentFlag)) ); 
    connect( pAlignVObjectWidget,SIGNAL(signalChanged(Qt::AlignmentFlag)),           pDiagram, SLOT(slotAlignVObject(Qt::AlignmentFlag)) ); 
    connect( pShapeSize,         SIGNAL(signalChanged(int)),                         pDiagram, SLOT(slotShapeSize(int)) ); 

    connect( pActionSettingsGrid,SIGNAL(triggered()),                               pDiagram, SLOT(slotSettingGrid()) ); 
    connect( pActionSettingsRuler,SIGNAL(triggered()),                              pDiagram, SLOT(slotSettingRuler()) ); 

    // allow diagram to enable/disable some controls based upon changes in its state
    connect( pDiagram, SIGNAL(signalChangedCut(bool)),          pActionCut,         SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedCopy(bool)),         pActionCopy,        SLOT(setEnabled(bool)) );
    // For paste we use global clipboard. \sa slotClipboardChanged
    // connect( pDiagram, SIGNAL(signalChangedPaste(bool)),        pActionPaste,       SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedUndo(bool)),         pActionUndo,        SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedRedo(bool)),         pActionRedo,        SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedCut(bool)),          pActionDelete,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionGroup,       SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionUngroup,     SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionToFront,     SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionToBack,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionOptimizeSize,SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize16,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize32,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize48,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize64,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize96,      SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionSize128,     SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pActionResetTransformation,SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pAlignHObjectWidget,SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pAlignVObjectWidget,SLOT(setEnabled(bool)) );
    connect( pDiagram, SIGNAL(signalChangedSelection(bool)),    pShapeSize,         SLOT(setEnabled(bool)) );

    connect( pDiagram, SIGNAL(signalChangedSnapGrid(bool)),     pActionSnapGrid,    SLOT(setChecked(bool)) );
    connect( pDiagram, SIGNAL(signalChangedPageBreaks(bool)),   pActionPageBreaks,  SLOT(setChecked(bool)) );
    connect( pDiagram, SIGNAL(signalChangedGrid(bool)),         pActionGrid,        SLOT(setChecked(bool)) );
    connect( pDiagram, SIGNAL(signalChangedRuler(bool)),        pActionRuler,       SLOT(setChecked(bool)) );
    connect( pDiagram, SIGNAL(signalChangedCrossHairs(bool)),   pActionCrossHairs,  SLOT(setChecked(bool)) );
}

void DDiagramEditorWidget::doInitStatusBar()
{
    QStatusBar *pstatusbar = statusBar();
    plabelCell= new WCoordWidget( pstatusbar, tr("Cell") );
    plabelCell->setToolTip( tr("cell coordinate") );
    pstatusbar->addPermanentWidget( plabelCell, 1 );

    plabelPixel= new WCoordWidget( pstatusbar, tr("Pixel") );
    plabelPixel->setToolTip( tr("pixel coordinate") );
    pstatusbar->addPermanentWidget( plabelPixel, 1 );

    pZoom = new WZoomWidget( this );
    // pZoom->setVisibleFit( false );
    pstatusbar->addPermanentWidget( pZoom, 0 );

    connect( pDiagram, SIGNAL(signalCell(const QPoint &)),      plabelCell,         SLOT(slotCoord(const QPoint &)) ); 
    connect( pDiagram, SIGNAL(signalPixel(const QPointF &)),    plabelPixel,        SLOT(slotCoord(const QPointF &)) ); 
    connect( pZoom, SIGNAL(signalZoom(WZoomWidget::FitTypes,int)), pView, SLOT(slotZoomChanged(WZoomWidget::FitTypes,int)) );
}

void DDiagramEditorWidget::slotSelectionChanged( bool )
{
    // enable/disable some controls here
    pActionCopy->setEnabled( pDiagram->canCopy() );
    pActionDelete->setEnabled( pDiagram->canDelete() );
}

void DDiagramEditorWidget::slotSettingRulerChanged()
{
    SRuler *p = pDiagram->getRuler();
}

/*!
 * \brief Catch when global clipboard changes.
 * 
 * We do this because \sa DDiagram::signalChangedPaste does not handle the global notifications for paste.
 * 
 * \author pharvey (8/16/25)
 * 
 * \param n      
 */
void DDiagramEditorWidget::slotClipboardChanged( QClipboard::Mode n )
{
    Q_UNUSED(n);
    pActionPaste->setEnabled( pDiagram->canPaste() );
}



