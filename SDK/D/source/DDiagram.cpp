#include "LibInfo.h"
#include "DDiagram.h"

#include <QSvgGenerator>

#include <CBDPersist.h>

#include <WBrush.h>
#include <WPen.h>
#include <WBitmapWidget.h> 
#include <WCanvasSizeWidget.h>

#include <ADTransaction.h>

#include <AWTreeWidget.h>
#include <AWDataWidget.h>
#include <AWDocumentNameWidget.h>
#include <AWValidationWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include <SSettings.h>
#include <SGrid.h>
#include <SRuler.h>

#include "DGraphicsSceneProxy.h"
#include "DGraphicsProxyItem.h"
#include "DDiagramEditorWidget.h"
#include "DGraphicsItemLine.h"
#include "DObject.h"
#include "DStencilItem.h"
#include "DBackgroundWidget.h"
#include "DRectangle.h"
#include "DLine.h"
#include "DLinePoly.h"
#include "DLinePolyAuto.h"
#include "DLine90H.h"
#include "DLine90V.h"
#include "DLine90Auto.h"
#include "DLineDynamic.h"
#include "DCallOut.h"
#include "DTitle.h"
#include "DTitleDoc.h"
#include "DEllipse.h"
#include "DArc.h"
#include "DBitmap.h"
#include "DVector.h"
#include "DText.h"
#include "DRubberBand.h"
#include "DLayoutHorizontal.h"
#include "DLayoutVertical.h"
#include "DLayoutGrid.h"
#include "DGroup.h"
#include "DSpacerHorizontal.h"
#include "DSpacerVertical.h"

DDiagram::DDiagram( ADObject *pParent, const QString &stringName )
    : AWObject( pParent, stringName ), ADDocument( this )
{
    bCanCreateEditor = true;

    // meta for object factory (getObject())
    mapMeta.clear();

    // layouts
    mapMeta.insert( "DLayoutHorizontal", ADMeta( QIcon(), "DLayoutHorizontal", "Layout Horizontal" ) );
    mapMeta.insert( "DLayoutVertical", ADMeta( QIcon(), "DLayoutVertical", "Layout Vertical" ) );
    mapMeta.insert( "DLayoutGrid", ADMeta( QIcon(), "DLayoutGrid", "Layout Grid" ) );
    mapMeta.insert( "DGroup", ADMeta( QIcon(), "DGroup", "Group" ) );
    
    // lines
    mapMeta.insert( "DArc", ADMeta( QIcon(), "DArc", "Arc" ) );
    mapMeta.insert( "DLine", ADMeta( QIcon(), "DLine", "Line" ) );
    mapMeta.insert( "DLinePoly", ADMeta( QIcon(), "DLinePoly", "Polyline" ) );
    mapMeta.insert( "DLinePolyAuto", ADMeta( QIcon(), "DLinePolyAuto", "Polyline - AutoRoute" ) );
    mapMeta.insert( "DLine90H", ADMeta( QIcon(), "DLine90H", "90 Horizontal" ) );
    mapMeta.insert( "DLine90V", ADMeta( QIcon(), "DLine90V", "90 Vertical" ) );
    mapMeta.insert( "DLine90Auto", ADMeta( QIcon(), "DLine90Auto", "90 - AutoRoute" ) );
    mapMeta.insert( "DLineDynamic", ADMeta( QIcon(), "DLineDynamic", "Dynamic" ) );
    // rectangles
    mapMeta.insert( "DEllipse", ADMeta( QIcon(), "DEllipse", "Ellipse" ) );
    mapMeta.insert( "DBitmap", ADMeta( QIcon(), "DBitmap", "Bitmap" ) );
    mapMeta.insert( "DVector", ADMeta( QIcon(), "DVector", "Vector" ) );
    mapMeta.insert( "DRectangle", ADMeta( QIcon(), "DRectangle", "Rectangle" ) );
    mapMeta.insert( "DText", ADMeta( QIcon(), "DText", "Text" ) );
    mapMeta.insert( "DSpacerHorizontal", ADMeta( QIcon(), "DSpacerHorizontal", "Spacer Horizontal" ) );
    mapMeta.insert( "DSpacerVertical", ADMeta( QIcon(), "DSpacerVertical", "Spacer Vertical" ) );
    // markup
    mapMeta.insert( "DCallOut", ADMeta( QIcon(), "DCallOut", "CallOut" ) );
    mapMeta.insert( "DTitle", ADMeta( QIcon(), "DTitle", "Title" ) );
    mapMeta.insert( "DTitleDoc", ADMeta( QIcon(), "DTitleDoc", "Title Doc" ) );

    // settings
    pGrid = new SGrid( this );
    {
        SGrid *p = (SGrid *)g_SSettings->getSetting( "SGrid" ); Q_ASSERT( p );
        pGrid->setVisible( p->getVisible() );
        pGrid->setSize( p->getSize() );
        pGrid->setSizeRulerMajour( p->getSizeRulerMajour() );
        pGrid->setSizeRulerMinor( p->getSizeRulerMinor() );
        pGrid->setSnap( p->getSnap() );
    }

    pRuler = new SRuler( this );
    {
        SRuler *p = (SRuler *)g_SSettings->getSetting( "SRuler" ); Q_ASSERT( p );
        pRuler->setPixelMajour( p->getPixelMajour() );
        pRuler->setPixelMinor( p->getPixelMinor() );
    }

    // \sa doClear
    nState          = StateWaiting;
    pScene          = nullptr;
    nButton         = Qt::NoButton;
    pRectangleOver  = nullptr;
    pLayoutOver     = nullptr;
    pHandlePressed  = nullptr;
    pObjectPressed  = nullptr;
    pSourceMoving   = nullptr;
    pRubberBand     = nullptr;
    bPageBreaks     = false;            // off expected by DStencil::slotAddDiagram()
    bRuler          = false;
    bCrossHairs     = false;
    pCrossH         = nullptr;
    pCrossV         = nullptr;
    pClipboard      = QApplication::clipboard();
    nCutMode        = ADTransaction::CopyReferenceOnly;
    nCopyMode       = ADTransaction::CopyReferenceOnly;
    nDeleteMode     = ADTransaction::DeleteReferencedUnused;
    nStrictness     = ADValidation::High;
    printer.setPageSize( QPageSize() );
    printer.setFullPage( false );
    nBorder         = 0; // 10; // any ruler is not currently informed about a border - default to 0 until this is corrected
    bPagePrinter    = true;
    sizePages       = QSizeF( 1, 1 );
    sizePage        = getCalcPageSize();
    pen.setColor( Qt::black );
    pen.setWidth( 1 );
    brush.setStyle( Qt::NoBrush );
    text.color = Qt::black;
    penBackground.setColor( Qt::black );
    penBackground.setWidth( 0 );
    brushBackground.setColor( Qt::white );
    brushBackground.setStyle(  Qt::SolidPattern );
    bPixmapBackgroundUse        = false;
    nBitmapBackgroundPlacement  = W::WImagePlacementScaled; 

    connect( pClipboard, SIGNAL(dataChanged()), SLOT(slotClipboardDataChanged()) );
    if ( pClipboard->supportsSelection() ) // Only X has this.
        connect( pClipboard, SIGNAL(selectionChanged()), SLOT(slotClipboardSelectionChanged()) );

    pSelectionManager = new ADObjectSelectionManager( this );
    connect( pSelectionManager, SIGNAL(signalChangedSelection()), SLOT(slotChangedSelection()) );
}

DDiagram::~DDiagram()
{
    if ( pSelectionManager )
    {
        pSelectionManager->doSelectNone();
        delete pSelectionManager; 
    }

    // All descendants have been deleted so our ties to pScene have been
    // removed. Safe to delete now.
    if ( pScene ) delete pScene;
}

void DDiagram::setCell( const QSize &size ) 
{
    pGrid->setSize( size ); 
}

void DDiagram::setScene( DGraphicsSceneProxy *p ) 
{ 
    pScene = p; 
    setSceneRect();
}

void DDiagram::setValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation, ADValidation::Severity nSeverity )
{
    mapValidationConfiguration[stringClass][nStrictness][stringValidation] = nSeverity;
}

void DDiagram::doGoTo( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    ADObject::doGoTo( listObjectPathItems, nDepth );
}

void DDiagram::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return;
    }

    //
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

/*!
 * \brief Set focus to diagram editor (if exists). 
 *  
 * This occurs when an object is clicked in the object browser. 
 * The object may be this or one of our descendents. 
 * 
 * \author pharvey (4/27/20)
 */
void DDiagram::doGoTo()
{
    // recurse our way to root
    if ( parent() )
    { 
        AWObject *p = (AWObject*)parent();
        p->doGoTo();
    }

    // do what we can on the way back down
    if ( isEditor() ) getEditorWidget()->doGoTo();
}

/*!
 * \brief Set focus to diagram object (if editor exists). 
 *  
 * This occurs when an object is clicked in the object browser and similar. 
 * 
 * \author pharvey (4/27/20)
 */
void DDiagram::doGoTo( DObject *p )
{
    if ( !isEditor() ) return; 
    DDiagramEditorWidget *pDiagramEditorWidget = (DDiagramEditorWidget*)getEditorWidget();
    pDiagramEditorWidget->doGoTo( p );
}

QPixmap DDiagram::getIcon()
{
    return QPixmap( ":D/Diagram" );
}

QSize DDiagram::getCell() 
{ 
    return pGrid->getSize(); 
}

QSize DDiagram::getCells()
{
    return QSize( getScene()->width() / pGrid->getSize().width(), getScene()->height() / pGrid->getSize().height() );
}

/*!
 * \brief  Creates a tab widget with a number of tabs for editing/viewing object properties.
 * 
 * We manage the validation configuration data \sa mapValidationConfiguration for the diagram and all below it. 
 * We add \sa AWValidationWidget to allow viewing/editing of this data.  
 *   
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DDiagram::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    // document version of Name widget
    p->removeByClass( "AWObjectNameWidget" );
    p->insertTab( 0, new AWDocumentNameWidget( this, p ), tr("Document") );

    AWValidationWidget *pValidation = new AWValidationWidget( this, &mapValidationConfiguration, nStrictness, p );
    connect( pValidation, SIGNAL(signalStrictness(ADValidation::Strictness)), this, SLOT(slotStrictness(ADValidation::Strictness)) );
    p->insertTab( 2, pValidation,  tr("Validation") );

    p->insertTab( 1, new DBackgroundWidget( this, p ), tr("Canvas") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DDiagram::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );
    // brush
    {
        WBrushPanel *pWidget = new WBrushPanel( getBrush(), pPropWidget );
        connect( pWidget, SIGNAL(signalChanged(const QBrush &)), SLOT(slotBrushBackground(const QBrush &)) );
        connect( this, SIGNAL(signalChangedBackgroundBrush(const QBrush &)), pWidget, SLOT(slotRefresh(const QBrush &)) );
        pPropWidget->addWidget( tr("Brush"), pWidget );
    }
    // bitmap
    {
        WBitmapWidget *pWidget = new WBitmapWidget( getBitmapBackgroundUse(), getBitmapBackground(), getBitmapBackgroundPlacement(), pPropWidget );
        connect( pWidget, SIGNAL(signalUse(bool)), this, SLOT(slotBitmapBackgroundUse(bool)) );
        connect( pWidget, SIGNAL(signalBitmap(const QPixmap &)), this, SLOT(slotBitmapBackground(const QPixmap &)) );
        connect( pWidget, SIGNAL(signalPlacement(W::WImagePlacements)), this, SLOT(slotBitmapBackgroundPlacement(W::WImagePlacements)) );
        connect( this, SIGNAL(signalBitmapBackgroundUse(bool)), pWidget, SLOT(slotUse(bool)) );
        connect( this, SIGNAL(signalBitmapBackground(const QPixmap &)), pWidget, SLOT(slotBitmap(const QPixmap &)) );
        connect( this, SIGNAL(signalBitmapBackgroundPlacement(W::WImagePlacements)), pWidget, SLOT(slotPlacement(W::WImagePlacements)) );
        pPropWidget->addWidget( tr("Bitmap"), pWidget );
    }
    // pen
    {
        WPenPanel *pWidget = new WPenPanel( getPen(), pPropWidget );
        connect( pWidget, SIGNAL(signalChanged(const QPen &)), SLOT(slotPenBackground(const QPen &)) );
        connect( this, SIGNAL(signalChangedBackgroundPen(const QPen &)), pWidget, SLOT(slotRefresh(const QPen &)) );
        pPropWidget->addWidget( tr("Pen"), pWidget );
    }
    // geometry
    { 
        WCanvasSizeWidget *pWidget = new WCanvasSizeWidget( getPage(), isPagePrinter(), pPropWidget );
        connect( pWidget, SIGNAL(signalSize(const QSizeF &)), this, SLOT(slotPage(const QSizeF &)) );
        connect( pWidget, SIGNAL(signalPrinter(bool)), this, SLOT(slotPagePrinter(bool)) );
        connect( this, SIGNAL(signalPage(const QSizeF &)), pWidget, SLOT(slotSize(const QSizeF &)) );
        connect( this, SIGNAL(signalPagePrinter(bool)), pWidget, SLOT(slotPrinter(bool)) );
        pPropWidget->addWidget( tr("Geometry"), pWidget );
    }
    return pPropWidget;
}

/*!
 * \brief Returns first rectangle in cell.
 *  
 * For diagrams restricting to 0-1 rectangles per cell. 
 *  
 * Inherits can be specified to avoid mark-up objects such as a call-out. 
 *  
 * More than 1 rectangle may be in a cell temporarily during diagram adjustments for a; drop, insert or whatever. In this case; 
 * calling here will not make much sense. 
 *  
 * \author pharvey (3/22/20)
 * 
 * \param pointCell 
 * \param stringInherits MUST be a DRectangle based class. Default is DRectangle. 
 * 
 * \return DRectangle* 
 */
DRectangle *DDiagram::getRectangle( const QPoint &pointCell, const QString &stringInherits )
{
    QList<QGraphicsItem *> l = getScene()->items( getCellRect( pointCell ) );
    QGraphicsItem *p;
    foreach( p, l )
    {
        // all of our items are QGraphicsObject based so...
        DGraphicsProxyItem *pProxy = getProxy( (QGraphicsObject*)p );
        if ( !pProxy ) continue;
        DObject *pObject = pProxy->getObject();
        if ( !pObject ) continue;

        if ( pObject->inherits( stringInherits.toLatin1() ) ) return (DRectangle *)pObject;
    }
    return nullptr;
}

DRectangle *DDiagram::getRectangle( const QPointF &pointPos, const QString &stringInherits )
{
    QList<QGraphicsItem *> l = getScene()->items( pointPos );
    QGraphicsItem *pItem;
    foreach( pItem, l )
    {
        // all of our items are QGraphicsObject based so...
        DGraphicsProxyItem *pProxy = getProxy( (QGraphicsObject*)pItem );
        if ( !pProxy ) continue;
        if ( pProxy->getObject()->inherits( stringInherits.toLatin1() ) ) return (DRectangle *)pProxy->getObject();
    }
    return nullptr;
}

QMenu *DDiagram::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu   = AWObject::getContextMenu( pParent );
    QAction *   pAction;

    // add our stuff at the top of the menu = reverse order
    pAction = pMenu->insertSeparator( pMenu->actions().first() );

    if ( isEditor() )
    {
        pAction = new QAction( QPixmap(), tr("Close Editor"), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotCloseEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );

        pAction = new QAction( QPixmap(), tr("Export..."), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotExport()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );

        QMenu *pMenuSelectAll = new QMenu( tr( "Select All" ) );
        pMenu->insertMenu( pMenu->actions().first(), pMenuSelectAll );
        QStringList vectorClasses = getClasses( true );
        for ( int n = 0; n < vectorClasses.count(); n++ )
        {
            QAction *pAction = new QAction( vectorClasses.at( n ), pMenuSelectAll );
            connect( pMenuSelectAll, SIGNAL(triggered(QAction*)), this, SLOT(slotSelectClass(QAction*)) );
            pMenuSelectAll->addAction( pAction );
        }
    }
    else
    {
        pAction = new QAction( QPixmap(), tr("Open Editor"), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotOpenEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    return pMenu; // caller invokes and then deletes
}

QPen DDiagram::getPen()
{
    return pen;
}

QBrush DDiagram::getBrush()
{
    return brush;
}

CBD::CBDText DDiagram::getText()
{
    return text;
}

/*!
 * \brief Draws into a SVG and returns as a byte array.
 * 
 * \author pharvey (8/11/21)
 * 
 * \return QByteArray 
 */
QByteArray DDiagram::getAsSvg()
{
    QByteArray      ByteArray;

    QBuffer         Buffer( &ByteArray );   // IO device which writes to ByteArray                                                
    QSvgGenerator   SvgGenerator;           // paint device which generates SVG and writes to IO device                           
    QPainter        Painter;                // painter interface paints to paint device                                           

    SvgGenerator.setOutputDevice( &Buffer );                                                                                      
    Painter.begin( &SvgGenerator );                                                                                               
    Painter.setRenderHint( QPainter::Antialiasing );                                                                              

    getScene()->render( &Painter );

    Painter.end();                                                                                                                

    return ByteArray;
}

/*!
 * \brief Draws into a pixmap and returns the pixmap.
 * 
 * \author pharvey (8/11/21)
 * 
 * \return QPixmap 
 */
QPixmap DDiagram::getAsPixmap( bool bCrop )
{
    QPixmap pixmap( getScene()->sceneRect().width(), getScene()->sceneRect().height() );
    {
        QPainter painter( &pixmap );
        getScene()->render( &painter );
        painter.end();
    }

    if ( bCrop )
    {
        QPixmap pixmapCropped = pixmap.copy( getScene()->itemsBoundingRect().toRect() );
        pixmap = pixmapCropped;
    }

    return pixmap;
}

/*!
 * \brief Draws into an image as a PNG.
 * 
 * \author pharvey (8/11/21)
 * 
 * \return QImage
 */
QImage DDiagram::getAsPng( bool bCrop )
{
    QImage image( getScene()->sceneRect().width(), getScene()->sceneRect().height(), QImage::Format_RGB32 );

    {
        QPainter painter( &image );
        getScene()->render( &painter );
        painter.end();
    }

    if ( bCrop )
    {
        QImage imageCropped = image.copy( getScene()->itemsBoundingRect().toRect() );
        image = imageCropped;
    }

    return image;
}

/*!
    NOTE: Should check to see if we are read-only.
*/

/*!
 * \brief Can 'cut' occur. 
 *  
 * The simple thing is to say it can occur as long as there is a selection but 
 * we may want to apply restrictions. For example; is the data read-only? 
 * 
 * \author pharvey (11/16/19)
 * 
 * \return bool 
 */
bool DDiagram::canCut()
{
    return pSelectionManager->hasSelection();
}

/*!
 * \brief Can 'copy' occur. 
 *  
 * The default is to say it can occur as long as there is a selection. 
 * However; this may not always be the case. 
 *  
 * Example 1; 
 *  
 * A read-only diagram may want to also prevent a copy. 
 *  
 * \sa slotSelectionCopy 
 *  
 * \author pharvey (11/16/19)
 * 
 * \return bool 
 */
bool DDiagram::canCopy()
{
    return pSelectionManager->hasSelection();
}

/*!
 * \brief Can 'paste' occur.
 *  
 * Does the clipboard have viable data to paste?
 *  
 * \sa slotSelectionPaste 
 *  
 * \author pharvey (11/16/19)
 * 
 * \return bool 
 */
bool DDiagram::canPaste()
{
    // does clipboard have any kind of text?
    QString stringClipboard = pClipboard->text();
    if ( stringClipboard.isEmpty() )
    {
        // this is triggered numerous times when screen saver on? so avoid message
        // doMessageBox( "ERROR", tr("Paste"), tr("No text in the clipboard.") );
        return false;
    }

    // is the text viable xml?
    QDomDocument    domDoc( metaObject()->className() );
    QDomElement     domElemDiagram;

    if ( !domDoc.setContent( stringClipboard ) )
    {
        // no message - 
        // doMessageBox( "ERROR", tr("Paste"), tr("Clipboard text is not viable XML.") );
        return false;
    }

    // \todo We know its XML but check to ensure its a document we understand

    return true;
}

bool DDiagram::canDelete()
{
    return pSelectionManager->hasSelection();
}

/*!
 * \brief Find an object which inherits stringClass and is on pointScene.
 * 
 * \author pharvey (12/20/19)
 * 
 * \param stringClass Must inherit this class.
 * \param pointScene Must on this point.
 * 
 * \return DObject* 
 */
DObject *DDiagram::getObject( const QString &stringClass, const QPointF &pointScene )
{
    QList<QGraphicsItem*> listItems = pScene->items( pointScene );
    QGraphicsItem *p;

    foreach( p, listItems )
    {
        // all of our items are QGraphicsObject based so...
        DGraphicsProxyItem *pProxy = getProxy( (QGraphicsObject*)p );
        if ( !pProxy ) continue;
        DObject *pObject = pProxy->getObject();
        if ( !pObject ) continue;

        if ( pObject->inherits( stringClass.toLatin1() ) ) return pObject;
    }

    return nullptr;
}

DLayout *DDiagram::getLayout( const QPointF &pointScene, DLayout *pLayoutExclude )
{
    QList<QGraphicsItem*> listItems = pScene->items( pointScene );
    QGraphicsItem *p;

    foreach( p, listItems )
    {
        // all of our items are QGraphicsObject based so...
        DGraphicsProxyItem *pProxy = getProxy( (QGraphicsObject*)p );
        if ( !pProxy ) continue;
        DObject *pObject = pProxy->getObject();
        if ( !pObject ) continue;

        if ( pObject->inherits( "DLayout" ) && pObject != pLayoutExclude ) return (DLayout*)pObject;
    }

    return nullptr;
}

QPoint DDiagram::mapSceneToCell( const QPointF &pointScene )
{
    // - always round UP if there is a remainder
    // - cells are zero based
    div_t divX = div( pointScene.x(), pGrid->getSize().width() );
    div_t divY = div( pointScene.y(), pGrid->getSize().height() );

    int nCellX = ( divX.rem > 0 ? divX.quot : divX.quot - 1 );
    int nCellY = ( divY.rem > 0 ? divY.quot : divY.quot - 1 );

    return QPoint( nCellX, nCellY );
}

QPointF DDiagram::mapCellToScene( const QPoint &pointCell )
{
    // - return top-left of cell in scene coords
    qreal nX = pointCell.x() * pGrid->getSize().width();
    qreal nY = pointCell.y() * pGrid->getSize().height();

    return QPointF( nX, nY );
}

QRect DDiagram::mapSceneToCell( const QRectF &rectScene )
{
    // topleft as 0-based cell coord
    QPoint pointTopLeft = mapSceneToCell( rectScene.topLeft() );
    // we get the width/height as 0-based cell coord...
    QPoint pointSize = mapSceneToCell( QPointF( rectScene.width(), rectScene.height() ) );
    // ...and adjust to 1-based here
    return QRect( pointTopLeft.x(), pointTopLeft.y(), pointSize.x() + 1, pointSize.y() + 1 );
}

QRectF DDiagram::getCellRect( const QPoint &pointCell )
{
    return QRectF( pointCell.x() * pGrid->getSize().width(), pointCell.y() * pGrid->getSize().height(), pGrid->getSize().width(), pGrid->getSize().height() );
}

ADObject *DDiagram::getObject( const QString &s, ADObject *p )
{
    bool                    bFolders        = false;
    ADObject *              pObjectParent   = ( p ? p : this );
    DObject *               pObject         = nullptr;
    QString                 stringClass     = getClass( s );
    DGraphicsProxyItem *    pProxy          = nullptr;

    if ( stringClass == "DLayoutHorizontal" )
    {
        pObject = new DLayoutHorizontal( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DLayoutVertical" )
    {
        pObject = new DLayoutVertical( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DLayoutGrid" )
    {
        pObject = new DLayoutGrid( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DGroup" )
    {
        pObject = new DGroup( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DSpacerHorizontal" )
    {
        pObject = new DSpacerHorizontal( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DSpacerVertical" )
    {
        pObject = new DSpacerVertical( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Layouts" ) );
    }
    else if ( stringClass == "DArc" )
    {
        pObject = new DArc( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DCallOut" )
    {
        pObject = new DCallOut( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DEllipse" )
    {
        pObject = new DEllipse( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DBitmap" )
    {
        pObject = new DBitmap( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DVector" )
    {
        pObject = new DVector( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DLine" )
    {
        pObject = new DLine( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLinePoly" )
    {
        pObject = new DLinePoly( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLinePolyAuto" )
    {
        pObject = new DLinePolyAuto( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLine90V" )
    {
        pObject = new DLine90V( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLine90H" )
    {
        pObject = new DLine90H( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLine90Auto" )
    {
        pObject = new DLine90Auto( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DLineDynamic" )
    {
        pObject = new DLineDynamic( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Connectors" ) );
    }
    else if ( stringClass == "DRectangle" )
    {
        pObject = new DRectangle( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DText" )
    {
        pObject = new DText( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DTitle" )
    {
        pObject = new DTitle( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }
    else if ( stringClass == "DTitleDoc" )
    {
        pObject = new DTitleDoc( pObjectParent );
        if ( bFolders ) pObject->setFolder( tr( "Shapes" ) );
    }

    if ( !pObject )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

#pragma message ( "we could have temp OID's clashing when object reparented to diagram" )

    // ensure we are the OID source regardless of parent (avoids messing up OID's when reparenting DObject's)
    pObject->setOIDSource( this );
    // provide a two-way interface to the scene via a proxy
    pProxy = new DGraphicsProxyObject( pObject );
    // get the proxy to create a QGraphicsObject based object - in this case DGraphicsObject
    pProxy->doCreateGraphicsObject( p && p->inherits( "DObject" ) ? ((DObject*)p)->getProxy()->getItem() : nullptr );
    // add QGraphicsObject to scene
    pProxy->setScene( pScene );
    // set proxy in DObject - may also be used by DObject based objects to init some other things such as create sinks 
    pObject->setProxy( pProxy );
    // default pos is center of scene
    pObject->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) );
    // announce the birth of a new child
    emit pObjectParent->signalCreated( pObject );
    // set parent modified by default (can be set back if this is a load)               
    pObjectParent->setModified();              

    // the caller should doConnect to a model ASAP
    return pObject;
}

/*!
 * \brief Handle mouse press event. 
 *  
 * <right-click> 
 *  
 * Invoke context menu. It will be the top DObject or the DDiagram. 
 * This is not handled here but in \sa contextMenuEvent().
 *  
 * <left-click> 
 *  
 * On DDiagram -> StatePressCanvas. 
 * On DHandle -> StatePressHandleForMove (as per DObject if DHandle::FunctionMove)
 * On DObject and selected -> StatePressObject
 * On DObject and !selected -> StatePressObjectForMove
 *  
 * \author pharvey (3/7/19)
 * 
 * \param pMouseEvent 
 */
void DDiagram::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( nState == StateWaiting );

    if ( !pScene ) return;

    // we only care about the left mouse button
    if ( pEvent->button() != Qt::LeftButton ) return;

    nButton = Qt::LeftButton;
    pointLast = pEvent->scenePos();

    // did we click on anything?
    QList<QGraphicsItem*> listItemsClicked = pScene->items( pEvent->scenePos() );
    if ( listItemsClicked.count() < 1 )
    {
        // no - so simply unselect anything selected
        slotSelectNone();
        nState = StatePressCanvas; // possible rubber-band selection
        return;
    }

    // what did we click on?
    QGraphicsItem *pItem;
    foreach( pItem, listItemsClicked )
    {
        // all of our items are QGraphicsObject based so...
        QGraphicsObject *pGraphicsObject = (QGraphicsObject*)pItem;

        // handle case where it is a handle and it is for moving object
        if ( pGraphicsObject->inherits( "DHandle" ) && ((DHandle*)pItem)->getFunction() == DHandle::FunctionMove )
        {
            // selecting a move handle is always switched to the object
            pGraphicsObject = ((DHandle*)pItem)->getObject()->getProxy()->getItem();
        }

        // is it a handle?
        if ( pGraphicsObject->inherits( "DHandle" ) )
        {
            pHandlePressed = (DHandle*)pItem;
            // prepare for moving handle
            pHandlePressed->getObject()->mousePressHandle( pHandlePressed, pEvent->scenePos() );
            nState = StatePressHandleForMove;
            return;
        }
        // is it a DObject?
        else if ( hasProxy( pGraphicsObject ) )
        {
#pragma message ( "add logic to select DRectangle instead of DLine when DLine is connect & truncated and over rectangle regardless of order" )
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
            DGraphicsProxyItem *pProxy = getProxy( pGraphicsObject );
            DObject *pObject = pProxy->getObject();
printf( "[PAH][%s][%s][%d] pProxy=%p pObject=%p\n", __FILE__, __FUNCTION__, __LINE__, pProxy, pObject );
            // selecting an object in a group is always switched to selecting group (should be outter most group if group nest in another group with no other layout between)
            if ( pObject->inGroup() ) pObject = pObject->getGroup( true );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
            // are we just altering the selection?
            if ( pEvent->modifiers() == Qt::ControlModifier )
            { 
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
                // toggle selection
                pSelectionManager->setSelected( pObject, !pObject->isSelected() );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
                return;
            }
            if ( pObject->isSelected() )
            {
                pObjectPressed = pObject;
                nState = StatePressObject; 
                // next?
                // move? moveevent will move all selected objects
                // no move? releaseevent will set selection to exclusive
            }
            else
            { 
printf( "[PAH][%s][%s][%d] num selected %lld\n", __FILE__, __FUNCTION__, __LINE__, pSelectionManager->getSelected().count() );
                // unselect everything 
                slotSelectNone();
                // select this
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, pObject );
                pSelectionManager->setSelected( pObject );
                pObjectPressed = pObject;
                nState = StatePressObjectForMove;
                // next?
                // move? moveevent will move this object
                // no move? release event does nothing
            }
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
            return;
        }
    }
}

/*!
 * \brief Handle mouse move event based upon state. 
 *           
 * \author pharvey (3/7/19)
 * 
 * \param pMouseEvent 
 */
void DDiagram::mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
    // for an app statusbar perhaps
    emit signalPixel( pEvent->scenePos() );
    emit signalCell( mapSceneToCell( pEvent->scenePos() ) );

    if ( !pScene ) return;

    // update ruler
    if ( isRuler() && getEditorWidget() )
    { 
        DDiagramEditorWidget *p = (DDiagramEditorWidget*)getEditorWidget();
        p->getRuler( Qt::Horizontal )->slotPos( pEvent->scenePos().x() );
        p->getRuler( Qt::Vertical )->slotPos( pEvent->scenePos().y() );
    }

    // update cross hairs
    if ( isCrossHairs() && getEditorWidget() )
    {
        QPointF p1;
        QPointF p2;
        qreal   nWidth    = getScene()->width();
        qreal   nHeight   = getScene()->height();

        p1 = QPointF( 0, pEvent->scenePos().y() );
        p2 = QPointF( nWidth, pEvent->scenePos().y() );
        pCrossH->setBegin( pCrossH->mapFromScene( p1 ) ); 
        pCrossH->setEnd( pCrossH->mapFromScene( p2 ) ); 

        p1 = QPointF( pEvent->scenePos().x(), 0 );
        p2 = QPointF( pEvent->scenePos().x(), nHeight );
        pCrossV->setBegin( pCrossV->mapFromScene( p1 ) ); 
        pCrossV->setEnd( pCrossH->mapFromScene( p2 ) ); 
    }

    // we only move stuff using left mouse button
    if ( nButton != Qt::LeftButton ) return;

    // did we actually move?
    if ( pointLast == pEvent->scenePos() ) return;

    // you can only drag something when no modifiers
    if ( pEvent->modifiers() != Qt::NoModifier ) return;

    //
    // HANDLE STATE TRANSITION
    //
    if ( nState == StatePressCanvas )
    {
        pRubberBand = new DRubberBand();
        getScene()->addItem( pRubberBand );
        pRubberBand->setPos(  pEvent->scenePos() );
        nState = StateMoveRubberBand;
        return; 
    }
    if ( nState == StatePressHandleForMove )
    {
        Q_ASSERT(pHandlePressed); 
        // have we just started to move a source?
        if ( pHandlePressed->getObject()->inherits("DLine") )
        {
            DLine *pLine = (DLine *)pHandlePressed->getObject();
            pSourceMoving = pLine->getSource( pHandlePressed );
            // moving a source so ensure we are disconnected 
            if ( pSourceMoving && pSourceMoving->isGlued() )
                pSourceMoving->getLine()->doDisconnect( pSourceMoving->getType() ); 
        }
        nState = StateMoveHandle; 
    }
    else if ( nState == StatePressObjectForMove || nState == StatePressObject )
    {
        // we know we are moving now so converge these two states
        // see release event for what happens when no move
        nState = StateMoveObject;
    }

    // MOVE HANDLE
    if ( nState == StateMoveHandle )
    {
        Q_ASSERT(pHandlePressed);
        mouseMoveHandle( pEvent ); 
    }
    // MOVE OBJECT(S)
    else if ( nState == StateMoveObject )
    {
        mouseMoveObjects( pEvent );
    }
    // RUBBER BAND SELECTION
    else if ( nState == StateMoveRubberBand )
    {
        pRubberBand->mouseMoveHandle( pEvent );
    }
}

/*!
 * \brief   Handle mouse release event based upon the state. 
 * 
 * \author pharvey (3/7/19)
 * 
 * \param pMouseEvent 
 */
void DDiagram::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    if ( !pScene ) return;

    // we only move stuff using left mouse button
    if ( nButton != Qt::LeftButton ) return;

    if ( nState == StateMoveHandle )
        mouseReleaseHandle( pEvent );
    else if ( nState == StateMoveObject )
    {
        mouseReleaseObjects( pEvent );
    }
    else if ( nState == StatePressObject )
    {
        if ( pObjectPressed->inherits( "DRectangleBase" ) )
        { 
            DRectangleBase *pRectangleBase = (DRectangleBase*)pObjectPressed;
            if ( pRectangleBase->inLayout() )
            {
                // select layout
                pSelectionManager->setSelected( pRectangleBase, false );
                pSelectionManager->setSelected( pRectangleBase->getLayout() );
            }
            else if ( pObjectPressed->inherits( "DRectangle" ) )
            {
                // allow object to cycle through select mode for itself ie for transform
                ((DRectangle *)pObjectPressed)->setSelectType(); // cycle select type
            }
        }
        else // DLine
        {
            // ensure pObjectPressed is only object selected
            slotSelectNone();               // unselect all
            pSelectionManager->setSelected( pObjectPressed );  // re-select 
        }
    }
    else if ( nState == StateMoveRubberBand )
    {
        pRubberBand->mouseReleaseHandle( pEvent );
        doSelectByArea( pRubberBand->mapRectToScene( pRubberBand->boundingRect() ) );
        delete pRubberBand; 
        pRubberBand = nullptr;
    }

    // reset state
    nState = StateWaiting;
    pointLast = QPointF();
    pRectangleOver = nullptr;
    pLayoutOver = nullptr;
    pObjectPressed = nullptr;
    pHandlePressed = nullptr;
    pSourceMoving = nullptr;
    pRubberBand = nullptr;
    nButton = Qt::NoButton;
}

void DDiagram::dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    // calling pEvent->ignore() does not seem to stop the drop event 
    // so just do a minimal check here and be more thorough in the 
    // drop event
    if ( !pEvent->mimeData()->hasText() ) { pEvent->ignore(); return; }

    QStringList listStrings = pEvent->mimeData()->text().split( ',' );
    if ( listStrings.count() < 1 ) { pEvent->ignore(); return; }

    if ( doDragEnter( listStrings.first(), pEvent->scenePos() ) ) pEvent->accept();
    else pEvent->ignore();
}

void DDiagram::dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    if ( !pEvent->mimeData()->hasText() ) { pEvent->ignore(); return; }

    QStringList listStrings = pEvent->mimeData()->text().split( ',' );
    if ( listStrings.count() < 1 ) { pEvent->ignore(); return; }

    if ( doDragMove( listStrings.first(), pEvent->scenePos() ) ) pEvent->accept();
    else pEvent->ignore();
}

/*!
 * \brief Handle something being dragged and then dropped on us. 
 *  
 * We create a new object if the class name is provided and it makes sense to us.
 * 
 * \author pharvey (12/11/19)
 * 
 * \param pEvent 
 */
void DDiagram::dropEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    if ( !pEvent->mimeData()->hasText() ) { pEvent->ignore(); return; }

    QStringList listStrings = pEvent->mimeData()->text().split( ',' );
    if ( listStrings.count() < 1 ) { pEvent->ignore(); return; }

    if ( doDragDrop( listStrings, pEvent->scenePos() ) ) pEvent->accept();
    else pEvent->ignore();
    // pEvent->acceptProposedAction();
}

void DDiagram::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
    // lets give top item (if any) a chance

    // all our items are based upon QGraphicsObject so...
    QGraphicsObject *pItem = (QGraphicsObject*)pScene->itemAt( pEvent->scenePos(), QTransform() );
    if ( pItem )
    {
        // DHandle?
        if ( pItem->inherits( "DHandle" ) )
        {
            // no context menu for handles - give it to the object
            DHandle *pHandle = (DHandle*)pItem;
            pHandle->getObject()->contextMenu( pHandle, pEvent );
            if ( pEvent->isAccepted() ) return;
        }
        // must be a DGraphicsProxyItem but check anyway
        if ( hasProxy( pItem ) )
        { 
            DGraphicsProxyItem *pProxy = getProxy( pItem );
            pProxy->getObject()->contextMenu( pEvent );
            if ( pEvent->isAccepted() ) return;
        }
    }

    // hmmm - lets handle it here
    QMenu *pMenu = getContextMenu( nullptr );
    if ( !pMenu )
        return;

    pMenu->exec( pEvent->screenPos() );

    delete pMenu;
}

void DDiagram::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    // lets give top item (if any) a chance

    // we only have QGraphicsObject items so get straight to it
    QGraphicsObject *pItem = (QGraphicsObject*)pScene->itemAt( pEvent->scenePos(), QTransform() );
    if ( pItem )
    {
        // DHandle?
        if ( pItem->inherits( "DHandle" ) )
        {
            // no Data Dialog for handles so give it to the object
            DHandle *pHandle = (DHandle*)pItem;
            pHandle->getObject()->slotObjectDialog();
            pEvent->setAccepted( true );
            return;
        }
        // must be a DGraphicsProxyItem but check anyway
        if ( hasProxy( pItem ) )
        { 
            DGraphicsProxyItem *pProxy = getProxy( pItem );
            pProxy->getObject()->slotObjectDialog();
            pEvent->setAccepted( true );
            return;
        }
    }

    // ok - so we can handle it hereDILine90Auto
    slotObjectDialog();
}

void DDiagram::drawBackground( QPainter *pPainter, const QRectF &rect, bool bPanner )
{
    Q_UNUSED(rect);

    if ( !pScene ) return;

    pPainter->setPen( getPenBackground() );
    pPainter->setBrush( getBrushBackground() );

    // painting pages establishs the background
    // doPaintPages will determine if the page edge (lines) are drawn
    doPaintPages( pPainter );
    QSizeF sizePage = getCalcPageSize();
    if ( bPixmapBackgroundUse && !pixmapBackground.isNull() )
    {
        // QRectF rect( 0, 0, getScene()->width(), getScene()->height() );
        switch ( nBitmapBackgroundPlacement )
        {
        case W::WImagePlacementNormal:
            pPainter->drawPixmap( 0, 0, pixmapBackground, 0, 0, sizePage.width(), sizePage.height() );
            break;
        case W::WImagePlacementScaled:
            pPainter->drawPixmap( 0, 0, pixmapBackground.scaled( sizePage.width(), sizePage.height() ), 0, 0, sizePage.width(), sizePage.height() );
            break;
        case W::WImagePlacementTiled:
            pPainter->drawTiledPixmap( QRectF( 0, 0, sizePage.width(), sizePage.height() ), pixmapBackground );
            break;
        }
    }

    if ( !bPanner && pGrid->getVisible() ) doPaintGrid( pPainter );
}


void DDiagram::mouseMoveHandle( QGraphicsSceneMouseEvent *pMouseEvent )
{
    // move handle
    pHandlePressed->getObject()->mouseMoveHandle( pHandlePressed, pMouseEvent->scenePos() );
    // is handle a Source
    if ( pSourceMoving )
    {
        // is handle over a DRectangle
        DObject *p = getObject( "DRectangle", pHandlePressed->pos() );
        if ( p == pRectangleOver )
        {
            // over same rect so do nothing
        }
        else if ( p )
        {
            // over new rect 
            if ( pRectangleOver ) ((DRectangle*)pRectangleOver)->doSourceExit( pSourceMoving );
            pRectangleOver = p;
            ((DRectangle*)pRectangleOver)->doSourceEnter( pSourceMoving );
        }
        else if ( pRectangleOver )
        {
            // no longer over rect...
            ((DRectangle*)pRectangleOver)->doSourceExit( pSourceMoving );
            pRectangleOver = nullptr;
        }
    }
}

void DDiagram::mouseMoveObjects( QGraphicsSceneMouseEvent *pEvent )
{
    ADObject *p;
    QList<ADObject*> l = pSelectionManager->getSelected();
    foreach( p, l )
    {
        // we are only putting DIObjects into our selection manager so...
        DObject *pObject = (DObject*)p;
        // is it in a layout?
        if ( pObject->inherits( "DRectangleBase" ) && ((DRectangleBase*)pObject)->inLayout() )
        {
            DRectangleBase *pRectangleBase = (DRectangleBase*)pObject;
            // remove from layout
            // move to new pos
            DLayout *pLayout = pRectangleBase->getLayout();
            pLayout->doDrag( pRectangleBase );
            pRectangleBase->setPos( pEvent->scenePos() ); 
        }
        else
        {
            // move to new pos
            pObject->setPosDelta( pEvent->scenePos() - pointLast ); 
        }

        // can only add a single object to layout at a time
        if ( l.count() == 1 && pObject->inherits( "DRectangleBase" ) )                                                                          
        {
            DRectangleBase *pRectangleBase = (DRectangleBase*)pObject;
            // are we over a layout?                                                                                              
            DLayout *pLayoutExclude = ( pRectangleBase->inherits( "DLayout" ) ? (DLayout*)pRectangleBase : nullptr );
            DLayout *p = getLayout( pEvent->scenePos(), pLayoutExclude );                              
            if ( p && p == pLayoutOver )                                                                    
            {                                                                                          
                // over same layout
                pLayoutOver->doOver( pRectangleBase, pEvent->scenePos() );                                    
            }                                                                                          
            else if ( p )                                                                              
            {                                                                                          
                // over new layout                                                                     
                if ( pLayoutOver ) pLayoutOver->doExit( pRectangleBase );                                     
                pLayoutOver = p;                                                                       
                pLayoutOver->doEnter( pRectangleBase, pEvent->scenePos() );                                    
            }                                                                                          
            else if ( pLayoutOver )                                                                    
            {                                                                                          
                // no longer over layout                                                               
                pLayoutOver->doExit( pRectangleBase );                                                        
                pLayoutOver = nullptr;                                                                 
            }                                                                                          
        }
    }
    pointLast = pEvent->scenePos();
}

void DDiagram::mouseReleaseHandle( QGraphicsSceneMouseEvent *pEvent )
{
    // as a precaution \sa mouseReleaseObjects
    nState = StateWaiting;
    nButton = Qt::NoButton;

    // stop moving handle
    pHandlePressed->getObject()->mouseReleaseHandle( pHandlePressed, pEvent->scenePos() );

    // clear any sinks
    if ( pSourceMoving && pRectangleOver ) 
        ((DRectangle*)pRectangleOver)->doSourceDrop( pSourceMoving, pEvent->scenePos() );

    setModified();
    doEnsurePages();
}

void DDiagram::mouseReleaseObjects( QGraphicsSceneMouseEvent *pEvent )
{
    // Bug killer! 
    // A mouse move event was sneaking its way into this processing?
    // This only happened under a very specific circumstance (losing a page top or bottom).
    // Doing the following (two lines) here ensures that any such event will be ignored.
    nState = StateWaiting;
    nButton = Qt::NoButton;

    ADObject *p;
    QList<ADObject*> l = pSelectionManager->getSelected();
    foreach( p, l )
    {
        DObject *pObject = (DObject*)p;
        pObject->setPosDelta( pEvent->scenePos() - pointLast ); 
        if ( pLayoutOver )
        {
            Q_ASSERT( pObject->inherits( "DRectangleBase" ) );
            pLayoutOver->doDrop( (DRectangleBase*)pObject, pEvent->scenePos() );
        }
    }

    doEnsurePages();

    if ( isSnapGrid() )                                                                                    
    {                                                                                                      
        // We do this for all rectangles in doEnsurePages but only sometimes. So we
        // have to do it here (sometimes 'again') to ensure it is done.
        ADObject *p;                                                                                       
        QList<ADObject*> l = pSelectionManager->getSelected();
        foreach( p, l )                                                                         
        {                                                                                                  
            DObject *pObject = (DObject*)p;                                                                
            if ( pObject->inherits( "DRectangle" ) ) 
            {
                pObject->setPos( getGridCenter( pObject->getPos() ) );
            }
        }                                                                                                  
    }                                                                                                      

    setModified();
}

void DDiagram::slotSettingGrid()
{
    pGrid->doDialog( getEditorWidgetParent(), true );
}

void DDiagram::slotSettingRuler()
{
    pRuler->doDialog( getEditorWidgetParent(), true );
}

bool DDiagram::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new DDiagramEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

void DDiagram::slotPrint()
{
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
}

/*!
 * \brief Copy selected objects to clipboard as XML and then delete objects.
 *  
 * \sa slotSelectionCopy 
 * \sa slotSelectionDelete 
 *  
 * \author pharvey (11/16/19)
 */
void DDiagram::slotSelectionCut()
{
    slotSelectionCopy();
    slotSelectionDelete();
}

/*!
 * \brief Copy selected objects to clipboard as XML.
 *  
 * \sa isCopy 
 *  
 * \author pharvey (11/16/19)
 */
void DDiagram::slotSelectionCopy()
{
    // ini transaction
    g_Transaction->initCopy( this, getCopyMode() );
        
    // create an XML document with class name and document version...
    QDomDocument domDoc( metaObject()->className() );
    QDomElement domElementRoot = domDoc.createElement( metaObject()->className() );
    // domElementRoot.setAttribute( "DocVer", SDK_DOC_VER );
    domDoc.appendChild( domElementRoot );

    QList<ADObject*> listChildren = getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {   
        if ( !pChild->isSelectable() || !pChild->isSelected() )
           continue;

        pChild->doSave( &domDoc, &domElementRoot );
    }

    // fini transaction
    g_Transaction->fini();

    // write
    pClipboard->setText( domDoc.toString() );
    emit signalChangedPaste( true );
}

/*!
 * \brief Paste using clipboard (presumably it has viable XML). 
 *  
 * We support cut/copy/paste via XML. In other words the copy/cut will create a     
 * XML version of the data and we Paste it by translating the XML back into         
 * objects.                                                                         
 *  
 * \sa isPaste 
 * \sa canPaste 
 *  
 * \author pharvey (11/16/19)
 */
void DDiagram::slotSelectionPaste()
{
    if ( !canPaste() )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Called paste but nothing to paste.";
        return;
    }

    // get data from system clipboard
    QString string = pClipboard->text();
    if ( string.isEmpty() ) return;

    QDomDocument    domDoc( metaObject()->className() );
    QDomElement     domElemDiagram;
    if ( !domDoc.setContent( string ) )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid paste format.\n\n") );
//        doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
        return;
    }

    // interpret
    domElemDiagram = domDoc.documentElement();
    if ( domElemDiagram.isNull() )
    {
        doMessageBox( "WARNING", LIB_NAME, tr("Invalid paste format.\nCould not find a document element.\n\n") );
        return;
    }
    if ( domElemDiagram.tagName() != metaObject()->className() )
    {
        doMessageBox( "WARNING", LIB_NAME, tr("Invalid paste format.\nCould not find a document element.\n\n") + QString( metaObject()->className() ) );
        return;
    }

    // ini transaction
    g_Transaction->initPaste( this );

    //
    slotSelectNone();

    // process child nodes...
    QDomNode    domNode = domElemDiagram.firstChild();
    int         nRef    = 0;

    while ( !domNode.isNull() )
    {
        if ( domNode.nodeType() != QDomNode::ElementNode )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        QDomElement domElem = domNode.toElement();
        if ( domElem.isNull() )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Class" )
        { 
            doLoadClass( &domElem );
        }
        else if ( domElem.tagName() == "Reference" )
        { 
            doLoadReference( &nRef, &domElem );
        }

        domNode = domNode.nextSibling();
    }

    // glue stuff together
    doPostLoad();
    // do this again as doPostLoad selects everything?
    slotSelectNone();

    // select pasted objects (avoid selecting objects within a group - just select the group)
    {
        QList<QString> listPasted = g_Transaction->mapXRef.values();
        QString stringPath;
        foreach( stringPath, listPasted )
        {
            ADObject *p = g_Transaction->pRoot->getObject( stringPath.split( ',' ) );
            if ( !p ) continue;
            if ( !p->inherits("DObject") ) continue;
            DObject *pObject = (DObject*)p;
            if ( pObject->inGroup() ) continue;
            if ( pObject->inLayout() ) continue;
            if ( !pObject->isSelectable() ) continue;
            pSelectionManager->setSelected( p );
        }
    }

    // fini transaction
    g_Transaction->fini();
}

/*!
 * \brief Select all objects. 
 *  
 * Here we want to select all DIObjects which are a child of DDiagram - nothing more. 
 * This is important as we do not want to select objects in a DGroup. 
 * 
 * \sa slotSelectNone 
 * \sa slotSelectClass 
 * \sa doSelectByClassName 
 * \sa doSelectByInherits 
 * 
 * \author pharvey (5/11/20)
 */
void DDiagram::slotSelectAll()
{
    pSelectionManager->doSelectAll();
}

void DDiagram::slotSelectNone()
{
    pSelectionManager->doSelectNone();
}

void DDiagram::slotSelectionDelete()
{
    g_Transaction->initDelete( this, getDeleteMode() );
    pSelectionManager->doDelete();
    doEnsurePages();
    g_Transaction->fini();
}

void DDiagram::slotCutMode( int n )
{
    nCutMode = (ADTransaction::CopyModes)n;
}

void DDiagram::slotCopyMode( int n )
{
    nCopyMode = (ADTransaction::CopyModes)n;
}

void DDiagram::slotDeleteMode( int n )
{
    nDeleteMode = (ADTransaction::DeleteModes)n;
}

void DDiagram::slotUndo()
{
}

void DDiagram::slotRedo()
{
}

void DDiagram::slotGroup()
{
    // nothing to group?
    if ( !pSelectionManager->hasSelection() ) return;

    // create group
    DGroup *pGroup = (DGroup*)getObject( "DGroup" );
    if ( !pGroup ) return;

    // group
    if ( pGroup->doGroup( pSelectionManager->getSelected() ) ) 
    {
        pSelectionManager->doSelectNone();
        pSelectionManager->setSelected( pGroup );
        setModified();
        return;
    }

    // could not group selected objects so get rid of group
    ADObject::slotDelete( (ADObject*)pGroup );
}

void DDiagram::slotUngroup()
{
   if ( !pSelectionManager->hasSelection() ) return;
   QList<ADObject*> l = pSelectionManager->getSelected();
   ADObject *p;
   foreach( p, l )
   {
       if ( !p->inherits( "DGroup" ) ) continue;
       DGroup *pGroup = (DGroup*)p;
       pGroup->doUngroup();
       // delete group if it is empty
       if ( !pGroup->getObjects().count() )
       {
           ADObject *pParent = pGroup->getParent( "ADObject" );
           Q_ASSERT( pParent );
           pParent->slotDelete( pGroup );
       }
   }
   setModified();
}

void DDiagram::slotToFront()
{
    if ( !pSelectionManager->hasSelection() ) return;
    doToFront( pSelectionManager->getSelected() );
    setModified();
}

void DDiagram::slotToBack()
{
    if ( !pSelectionManager->hasSelection() ) return;
    doToBack( pSelectionManager->getSelected() );
    setModified();
}

void DDiagram::slotOptimizeSize()
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotOptimizeSize();
    }
}

void DDiagram::slotSize16()  
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 16 );
    }
}

void DDiagram::slotSize32()  
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 32 );
    }
}

void DDiagram::slotSize48()  
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 48 );
    }
}

void DDiagram::slotSize64()  
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 64 );
    }
}

void DDiagram::slotSize96()  
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 96 );
    }
}

void DDiagram::slotSize128() 
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotShapeSize( 128 );
    }
}

void DDiagram::slotResetTransformation() 
{
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        pRectangle->slotResetTransformation();
    }
}

void DDiagram::slotAlignHObject( Qt::AlignmentFlag n )
{
    // get bounding rect of selected rectangle based objects                                       
    QRectF r;
    QList<ADObject*> l = pSelectionManager->getSelected();            
    if ( !l.count() ) return;
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *    pRectangle          = (DRectangle*)p;
        QPointF         pointTopLeft        = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().topLeft() );
        QPointF         pointBottomRight    = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().bottomRight() );
        r = r.united( QRectF( pointTopLeft, pointBottomRight ) );
    }

    // set X
    foreach( p, l )
    {
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        if ( n == Qt::AlignLeft )
        {
            QPointF pointPos = pRectangle->getProxy()->pos();
            qreal nLeft = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().topLeft() ).x();
            qreal nDelta = pointPos.x() - nLeft;
            pRectangle->setPos( QPointF( r.left() + nDelta, pointPos.y() ) );
        }
        else if ( n == Qt::AlignRight )
        {
            QPointF pointPos = pRectangle->getProxy()->pos();
            qreal nRight = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().topRight() ).x();
            qreal nDelta = nRight - pointPos.x();
            pRectangle->setPos( QPointF( r.right() - nDelta, pointPos.y() ) );
        }
        else if ( n == Qt::AlignHCenter )
        {
            pRectangle->setPos( QPointF( r.center().x(), pRectangle->getPos().y() ) );
        }
    }
}

void DDiagram::slotAlignVObject( Qt::AlignmentFlag n )
{
    // get bounding rect of selected rectangle based objects                                       
    QRectF r;
    QList<ADObject*> l = pSelectionManager->getSelected();            
    if ( !l.count() ) return;
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *    pRectangle          = (DRectangle*)p;
        QPointF         pointTopLeft        = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().topLeft() );
        QPointF         pointBottomRight    = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().bottomRight() );
        r = r.united( QRectF( pointTopLeft, pointBottomRight ) );
    }

    // set X
    foreach( p, l )
    {
        if ( !p->inherits("DRectangle") ) continue;
        DRectangle *pRectangle = (DRectangle*)p;
        if ( n == Qt::AlignTop )
        {
            QPointF pointPos = pRectangle->getProxy()->pos();
            qreal nTop = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().topLeft() ).y();
            qreal nDelta = pointPos.y() - nTop;
            pRectangle->setPos( QPointF( pointPos.x(), r.top() + nDelta ) );
        }
        else if ( n == Qt::AlignBottom )
        {
            QPointF pointPos = pRectangle->getProxy()->pos();
            qreal nBottom = pRectangle->getProxy()->mapToScene( pRectangle->boundingRect().bottomLeft() ).y();
            qreal nDelta = nBottom - pointPos.y();
            pRectangle->setPos( QPointF( pointPos.x(), r.bottom() - nDelta ) );
        }
        else if ( n == Qt::AlignVCenter )
        {
            pRectangle->setPos( QPointF( pRectangle->getPos().x(), r.center().y() ) );
        }
    }
}

void DDiagram::slotSnapGrid( bool b )
{
    if ( b == isSnapGrid() ) return;
    if ( pGrid->getSnap() == S::GridSnapNone )
        pGrid->setSnap( S::GridSnapLine );
    else
        pGrid->setSnap( S::GridSnapNone );

    emit signalChangedSnapGrid( isSnapGrid() );
    setModified();
}

void DDiagram::slotPageBreaks( bool b )
{
    if ( b == bPageBreaks ) return;
    bPageBreaks = b;
    if ( pScene ) pScene->update();
    emit signalChangedPageBreaks( bPageBreaks );
    setModified();
}

void DDiagram::slotGrid( bool b )
{
    if ( b == pGrid->getVisible() ) return;
    pGrid->setVisible( b );
    if ( pScene ) pScene->update();
    emit signalChangedGrid( pGrid->getVisible() );
    setModified();
}

void DDiagram::slotRuler( bool b )
{
    if ( b == bRuler ) return;
    bRuler = b;
    if ( getEditorWidget() ) ((DDiagramEditorWidget *)getEditorWidget())->setRuler( b );
    emit signalChangedRuler( bRuler );
    setModified();
}

void DDiagram::slotCrossHairs( bool b )
{
    if ( b == bCrossHairs ) return;
    bCrossHairs = b;

    if ( !pScene ) return;

    if ( !b && pCrossH )
    { 
        delete pCrossH;
        delete pCrossV;
        pCrossH = nullptr;
        pCrossV = nullptr;
    }

    if ( b ) doCreateCrossHairs();

    emit signalChangedCrossHairs( bCrossHairs );
    setModified();
}

void DDiagram::slotPenBackground( const QPen &pen )
{
    if ( pen == this->pen ) return;
    this->pen = pen;
    if ( pScene ) pScene->update();
    setModified();
    emit signalChangedBackgroundPen( pen );
}

void DDiagram::slotBrushBackground( const QBrush &brush )
{
    if ( brush == this->brush ) return;
    this->brush = brush;
    if ( pScene ) pScene->update();
    setModified();
    emit signalChangedBackgroundBrush( brush );
}

void DDiagram::slotBitmapBackgroundUse( bool b )
{
    if ( b == bPixmapBackgroundUse ) return;
    bPixmapBackgroundUse = b;
    if ( getScene() ) getScene()->update();
    setModified( true );
    emit signalBitmapBackgroundUse( b );
}

void DDiagram::slotBitmapBackground( const QPixmap &pixmap )
{
    // if ( pixmap == pixmapBackground ) return;
    pixmapBackground = pixmap;
    if ( getScene() ) getScene()->update();
    setModified( true );
    emit signalBitmapBackground( pixmap );
}

void DDiagram::slotBitmapBackgroundPlacement( W::WImagePlacements nPlacement )
{
    if ( nPlacement == nBitmapBackgroundPlacement ) return;
    nBitmapBackgroundPlacement = nPlacement;
    if ( getScene() ) getScene()->update();
    emit signalBitmapBackgroundPlacement( nPlacement );
}

void DDiagram::slotPage( const QSizeF &size )
{
    if ( size == sizePage ) return;
    sizePage = size;
    doEnsurePages();
    setModified( true );
    emit signalPage( size );
}

void DDiagram::slotPagePrinter( bool b )
{
    if ( b == bPagePrinter ) return;
    bPagePrinter = b;
    doEnsurePages();
    setModified( true );
    emit signalPagePrinter( b );
}

void DDiagram::slotPen( const QPen &pen )
{
    // apply to all selected DObject(s)
    int n = 0;
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DObject") ) continue;
        ((DObject*)p)->slotPen( pen );
        n++;
    }
    // have applied to selected DObject(s) - then we are done
    if ( n ) return;
    // set default for new DObject
    this->pen = pen;
    emit signalChangedPen( pen );
    setModified();
}

void DDiagram::slotBrush( const QBrush &brush )
{
    // apply to all selected DObject(s)
    int n = 0;
    QList<ADObject*> l = pSelectionManager->getSelected();            
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DObject") ) continue;
        ((DObject*)p)->slotBrush( brush );
        n++;
    }
    // have applied to selected DObject(s) - then we are done
    if ( n ) return;
    // set default for new DObject
    if ( this->brush == brush ) return; 
    emit signalChangedBrush( brush );
    setModified();
}

void DDiagram::slotText( const CBD::CBDText &t1 )
{
    // apply to all selected DObject(s)
    int n = 0;
    QList<ADObject *> l = pSelectionManager->getSelected();
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DObject") ) continue;
        ((DObject*)p)->slotText( t1 );
        n++;
    }
    // have applied to selected DObject(s) - then we are done
    if ( n ) return;
    // set default for new DObject
    text = t1;
    emit signalChangedText( text );
    setModified();
}

void DDiagram::slotShapeSize( int n )
{
    if ( n == 0 ) return;
    QList<ADObject *> l = pSelectionManager->getSelected();
    ADObject *p;
    foreach( p, l )                                
    {                                              
        if ( !p->inherits("DRectangle") ) continue;
        ((DRectangle*)p)->slotShapeSize( n );
    }
}


void DDiagram::slotGuideNewRequest( Qt::Orientation nLineOrientation, qreal nOnScene )
{
    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    DDiagramEditorWidget *p = (DDiagramEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // restrict to width and height
    if ( nOnScene < 0 ) return;

    // add a guide line
    DGraphicsItemLine *pItem;
    if ( nLineOrientation == Qt::Vertical )
    {
        if ( nOnScene > getScene()->width() ) return;
        pRuler->doGuideNew( nOnScene );
        pItem = new DGraphicsItemLine( QLineF( nOnScene, 0, nOnScene, getScene()->height() ) );
        vectorGuidesX.append( pItem );
    }
    else
    {
        if ( nOnScene > getScene()->height() ) return;
        pRuler->doGuideNew( nOnScene );
        pItem = new DGraphicsItemLine( QLineF( 0, nOnScene, getScene()->width(), nOnScene ) );
        vectorGuidesY.append( pItem );
    }
    
    QPen pen( Qt::SolidLine );
    pen.setColor( Qt::lightGray );

    pItem->setPen( pen );
    pScene->addItem( pItem );

    setModified();
}

void DDiagram::slotGuideDeleteRequest( Qt::Orientation nLineOrientation, int nIndex )
{
    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    DDiagramEditorWidget *p = (DDiagramEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // accept all requests

    // get ruler to delete a guide marker
    pRuler->doGuideDelete( nIndex );

    // delete guide line
    if ( nLineOrientation == Qt::Vertical )
    {
        delete vectorGuidesX[nIndex];
        vectorGuidesX.remove( nIndex );
    }
    else
    {
        delete vectorGuidesY[nIndex];
        vectorGuidesY.remove( nIndex );
    }

    // guide could be only thing holding a page - in such a case - we want to get rid of the page(s)
    // when the guide is deleted
    doEnsurePages();

    setModified();
}

void DDiagram::slotGuideMoveRequest( Qt::Orientation nLineOrientation, int nIndex, qreal nOnScene )
{
    Q_ASSERT( nIndex >= 0 );

    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    DDiagramEditorWidget *p = (DDiagramEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // restrict to width and height
    if ( nOnScene < 0 ) return;

    // move guide line
    if ( nLineOrientation == Qt::Vertical )
    {
        if ( nOnScene > getScene()->width() ) return;
        Q_ASSERT( nIndex < vectorGuidesX.count() );
        pRuler->doGuideMove( nIndex, nOnScene );
        vectorGuidesX[nIndex]->setLine( QLineF( nOnScene, 0, nOnScene, getScene()->height() ) );
    }
    else
    {
        if ( nOnScene > getScene()->height() ) return;
        Q_ASSERT( nIndex < vectorGuidesY.count() );
        pRuler->doGuideMove( nIndex, nOnScene );
        vectorGuidesY[nIndex]->setLine( QLineF( 0, nOnScene, getScene()->width(), nOnScene ) );
    }

    setModified();
}

void DDiagram::slotStrictness( ADValidation::Strictness n )
{
    if ( n == nStrictness ) return;
    nStrictness = n;
    setModified();
}

void DDiagram::slotExport()
{
    QString stringFileName = QFileDialog::getSaveFileName( 0, tr( "Export..." ), QString(), "Images (*.png *.xpm *.svg);;XML files (*.xml)" );                                                                                                      

    if ( stringFileName.isEmpty() ) return;

    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         

    QString stringSuffix = fileinfo.suffix();
    stringSuffix = stringSuffix.toLower();

    if ( stringSuffix == "png" )
    {
        QImage image = getAsPng();
        if ( !image.save( stringFileName ) )
        {
            doMessageBox( "ERROR", LIB_NAME, "Failed to write " + stringFileName );
        }
        return;
    }

    if ( stringSuffix == "xpm" )
    {
        QPixmap image = getAsPixmap();
        if ( !image.save( stringFileName ) )
        {
            doMessageBox( "ERROR", LIB_NAME, "Failed to write " + stringFileName );
        }
        return;
    }

    if ( stringSuffix == "svg" )
    {
        QByteArray image = getAsSvg();
        QSaveFile file( stringFileName );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            doMessageBox( "ERROR", LIB_NAME, "Could not open " + file.fileName() );
            return;
        }

        file.write( image );
        file.commit();
        return;
    }

    if ( stringSuffix == "xml" )
    {
        // create
        QFile file( stringFileName );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            doMessageBox( "ERROR", LIB_NAME, "Could not open " + file.fileName() );
            return;
        }

        // ini transaction
        g_Transaction->initSave( this );

        // We may need to switch some temp OID's to persist OID's and other things.
        // This will work its way down the hierarchy covering everything to be saved in this file.
        // !!! There is NO need to call doPreSave again ie for objects to be saved in this file. 
        doPreSave();

        // create an XML document with class name and document version...
        QDomDocument domDoc( metaObject()->className() );
        QDomElement domElementRoot = domDoc.createElement( metaObject()->className() );
        domElementRoot.setAttribute( "DocVer", SDK_DOC_VER );

        domDoc.appendChild( domElementRoot );

        // save ourself (incl. all descendants)
        QDomElement domElemThis = doSave( &domDoc, &domElementRoot );
        doSaveADDocument( &domDoc, &domElemThis );

        // write
        QString stringXML = domDoc.toString();
        file.write( stringXML.toLatin1(), stringXML.length() );

        // close
        file.close();

        // fini transaction
        g_Transaction->fini();
        return;
    }

    doMessageBox( "ERROR", LIB_NAME, "Can not export to suffix " + stringSuffix );
}

void DDiagram::doPaintPages( QPainter *pPainter )
{
    qreal  nPageX;
    qreal  nPageY;
    QSizeF sizePage    = getCalcPageSize();
    qreal  nPageWidth  = sizePage.width();
    qreal  nPageHeight = sizePage.height();

    if ( !bPageBreaks ) pPainter->setPen( QPen( Qt::NoPen ) /* getBrush().color() */ );

    // Draw pages.
    for ( nPageY=0; nPageY < sizePages.height(); nPageY++ )
    {
        for ( nPageX=0; nPageX < sizePages.width(); nPageX++ )
        {
            pPainter->drawRect( nBorder + nPageX * nPageWidth,
                                nBorder + nPageY * nPageHeight,
                                nPageWidth,
                                nPageHeight );
        }
    }

    if ( !bPageBreaks ) pPainter->setPen( getPen() );
}

void DDiagram::doPaintGrid( QPainter *pPainter )
{
    QSizeF sizePage       = getCalcPageSize();
    qreal nPageWidth      = sizePage.width();
    qreal nPageHeight     = sizePage.height();
    qreal nX              = 0;
    qreal nY              = 0;
    qreal nGridHeight     = (nBorder+(sizePages.height()*nPageHeight)) - 1;
    qreal nGridWidth      = (nBorder+(sizePages.width()*nPageWidth)) - 1;

    QPen pen = getPen();
    pen.setStyle( Qt::DotLine );
    pen.setWidth( 0 );
    pPainter->setPen( pen );

    // top to bottom
    int nInc = pGrid->getSize().height();
    for ( nY=nBorder+1; nY <= nGridHeight; nY += nInc )
    {
        pPainter->drawLine( nBorder + 1, nY, nGridWidth, nY );
    }
    // left to right
    nInc = pGrid->getSize().width();
    for ( nX=nBorder+1; nX <= nGridWidth; nX += nInc )
    {
        pPainter->drawLine( nX, nBorder+1, nX, nGridHeight );
    }

    pPainter->setPen( getPen() );
}

void DDiagram::doSelectByClassName( const QString &stringClassName )                     
{
    // unselect all
    slotSelectNone();
    // select by class
    QList<ADObject *> listChildren = getObjects( QString(), stringClassName, "DObject" );
    ADObject *       pChild;                                                             
    DObject *        pObject;                                                            
                                                                                         
    foreach( pChild, listChildren )                                                      
    {                                                                                    
        pObject = (DObject*)pChild;                                                      
        if ( !pObject->isSelected() )                                                    
        {                                                                                
            pSelectionManager->setSelected( pObject );
        }                                                                                
    }                                                                                    
}                                                                                        

void DDiagram::doSelectByInherits( const QString &stringInherits )
{
    // unselect all
    slotSelectNone();
    // select by inherits
    QList<ADObject *> listChildren = getObjects( QString(), QString(), stringInherits );
    ADObject *       pChild;
    DObject *        pObject;

    foreach( pChild, listChildren )
    {
        pObject = (DObject*)pChild;
        if ( !pObject->isSelected() )
        {
            pSelectionManager->setSelected( pObject );
        }
    }
}

void DDiagram::doSelectByArea( const QRectF &r )
{
    // unselect all
    slotSelectNone();
    // select by inherits
    QList<QGraphicsItem *>  listItems = getScene()->items( r, Qt::ContainsItemShape );
    QGraphicsItem *         pItem;
    foreach( pItem, listItems )
    {
        QGraphicsObject *p = (QGraphicsObject*)pItem;
        if ( hasProxy( p ) )
        {
            DGraphicsProxyItem *pProxy = getProxy( p );
            DObject *           pObject = pProxy->getObject();
            if ( pObject->inGroup() ) continue;
            if ( pObject->inLayout() ) continue;
            if ( pObject->isSelectable() && !pObject->isSelected() )
            { 
                pSelectionManager->setSelected( pObject );
            }
        }
    }
}

void DDiagram::doHideByClassName( const QString &stringClassName, bool b )
{
    QList<ADObject *> listChildren = getObjects( QString(), stringClassName, "DObject" );
    ADObject *       pChild;

    foreach( pChild, listChildren )
    {
        ((DObject*)pChild)->setVisible( b );
    }
}

void DDiagram::doHideByInherits( const QString &stringInherits, bool b )
{
    QList<ADObject *> listChildren = getObjects( QString(), QString(), stringInherits );
    ADObject *       pChild;

    foreach( pChild, listChildren )
    {
        ((DObject*)pChild)->setVisible( b );
    }
}

void DDiagram::doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{
    if ( Criteria.bOther && getAuthor().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getAuthor() );
    if ( Criteria.bOther && getVer().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getVer() );                              
    AWObject::doFind( nValueType, listObjectPathItems, Criteria );                                                                 
    return;
}

QDomElement DDiagram::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )   
{                                                                                    
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );           
    doSaveADDocument( pdomDoc, &domElemThis );                                       

    domElemThis.setAttribute( "PageBreaks", bPageBreaks );
    domElemThis.setAttribute( "Grid", pGrid->getVisible() );
    domElemThis.setAttribute( "CellWidth", pGrid->getSize().width() );
    domElemThis.setAttribute( "CellHeight", pGrid->getSize().height() );
    domElemThis.setAttribute( "Ruler", bRuler );
    domElemThis.setAttribute( "CrossHairs", bCrossHairs );
    domElemThis.setAttribute( "Strictness", ADValidation::getStrictness( nStrictness ) );
    domElemThis.setAttribute( "PageSizePrinter", bPagePrinter );
    domElemThis.setAttribute( "PageSizeWidth", sizePage.width() );
    domElemThis.setAttribute( "PageSizeHeight", sizePage.height() );
    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmapBackground.toImage(), &nBytesCompressed );
    domElemThis.setAttribute( "BitmapSize", QString::number( nBytesCompressed ) );
    domElemThis.setAttribute( "Bitmap", stringEncodedImage );
    domElemThis.setAttribute( "BitmapPlacement", int(nBitmapBackgroundPlacement) );
    domElemThis.setAttribute( "BitmapUse", bPixmapBackgroundUse );

    domElemThis.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveBrush( brush, pdomDoc ) );

    // save guides
    {
        // X
        QString s;
        DGraphicsItemLine *pItem;
        foreach( pItem, vectorGuidesX )
        {
            if ( !s.isEmpty() ) s += ":";
            s += QString::number( pItem->getLine().x1() );
        }
        if ( !s.isEmpty() ) domElemThis.setAttribute( "GuidesX", s );

        // Y
        s = QString();
        foreach( pItem, vectorGuidesY )
        {
            if ( !s.isEmpty() ) s += ":";
            s += QString::number( pItem->getLine().y1() );
        }
        if ( !s.isEmpty() ) domElemThis.setAttribute( "GuidesY", s );
    }

    domElemThis.appendChild( doSaveValidation( pdomDoc ) );

    return domElemThis;                                                              
}                                                                                    
                                                                                     
bool DDiagram::doLoad( QDomElement *pdomElemThis )                                   
{                                                                                    
   AWObject::doLoad( pdomElemThis );                                                
   doLoadADDocument( pdomElemThis );                                                
                                                                                     
   bPageBreaks = pdomElemThis->attribute( "PageBreaks", "1" ).toInt();
   pGrid->setSize( QSize( pdomElemThis->attribute( "CellWidth", "48" ).toInt(), pdomElemThis->attribute( "CellHeight", "48" ).toInt() ) );                                                                                     
   pGrid->setVisible( pdomElemThis->attribute( "Grid", "1" ).toInt() );
   bRuler = pdomElemThis->attribute( "Ruler", "1" ).toInt();
   slotCrossHairs( pdomElemThis->attribute( "CrossHairs", "1" ).toInt() );
   nStrictness = ADValidation::getStrictness( pdomElemThis->attribute( "Strictness", "" ) );
   bPagePrinter = pdomElemThis->attribute( "PageSizePrinter", "1" ).toInt();                                                                                     
   sizePage.setWidth( pdomElemThis->attribute( "PageSizeWidth", "500" ).toInt() );                                                                                     
   sizePage.setHeight( pdomElemThis->attribute( "PageSizeHeight", "500" ).toInt() );                                                                                     
   bPixmapBackgroundUse = pdomElemThis->attribute( "BitmapUse", "0" ).toInt(); 
   nBitmapBackgroundPlacement = (W::WImagePlacements)pdomElemThis->attribute( "BitmapPlacement", QString::number( W::WImagePlacementScaled ) ).toInt();
   ulong nBytesCompressed = pdomElemThis->attribute( "BitmapSize", "0" ).toULong();
   if ( nBytesCompressed )
   { 
       QString stringAttribute = pdomElemThis->attribute( "Bitmap" );
       if ( stringAttribute.isNull() ) pixmapBackground = QPixmap();
       else pixmapBackground = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
   }

   // we may have changed page size so...
   setSceneRect();

   // load guides
   {
       QString s1, s2;
       QStringList l;
       // X
       s1 = pdomElemThis->attribute( "GuidesX" );
       if ( !s1.isEmpty() )
       {
           l = s1.split( ':' );
           foreach( s2, l )
           {
               doLoadGuide( Qt::Vertical, s2.toDouble() ); 
           }
       }
       // Y
       s1 = pdomElemThis->attribute( "GuidesY" );
       if ( !s1.isEmpty() )
       {
           l = s1.split( ':' );
           foreach( s2, l )
           {
               doLoadGuide( Qt::Horizontal, s2.toDouble() ); 
           }
       }
   }

   // load additional nodes...
   QDomElement         domElem;
   QDomNode            domNode;
   domNode = pdomElemThis->firstChild();
   while( !domNode.isNull() ) 
   {
       domElem = domNode.toElement();
       if( domElem.isNull() ) 
       {
           domNode = domNode.nextSibling();
           continue;
       }

       if ( domElem.tagName() == "Validation" ) doLoadValidation( &domElem );
       else if ( domElem.tagName() == "Pen" ) pen = CBDPersistNative::doLoadPen( &domElem );
       else if ( domElem.tagName() == "Brush" ) brush = CBDPersistNative::doLoadBrush( &domElem );

       domNode = domNode.nextSibling();
   }

   return true;                                                                     
}                                                                                    
                                                                                     
void DDiagram::doPostLoad()                                                          
{                                                                                    
    slotSelectNone();                                                                
                                                                                     
    // Paste; so just do the new stuff.                                              
    // \todo what if it was a paste but no objects pasted?                           
    if ( g_Transaction->canPaste() )                                                  
    {                                                                                
        QList<QString> listPasted = g_Transaction->mapXRef.values();                 
        QString stringPath;                                                          
        foreach( stringPath, listPasted )                                            
        {                                                                            
            ADObject *p = g_Transaction->pRoot->getObject( stringPath.split( ',' ) );
            Q_ASSERT(p);
            if ( p->inherits("DObject") )                                            
            {                                                                        
                // this will recurse on children                                     
                p->doPostLoad();                                                     
                pSelectionManager->setSelected( p );
            }                                                                        
        }                                                                            
    }                                                                                
    else                                                                             
    {                                                                                
        AWObject::doPostLoad();                                                      
    }                                                                                
                                                                                     
    // add more pages (if needed)                                                    
    doEnsurePages();                                                                 
}                                                                                    

/*!
 * \brief Load validation configuration defaults. 
 *  
 * This is one of two ways to load the validation configuration. The 
 * other way is from XML when loading the diagram. The latter may be 
 * a User configured version. 
 *  
 * \author pharvey (11/29/19)
 */
void DDiagram::getValidationConfiguration()
{
    mapValidationConfiguration.clear();
    getValidationConfiguration( mapValidationConfiguration );
}

/*!
 * \brief Load our validation data. 
 *  
 * Each class has a static method that is called to load its validation data. 
 * This does not do the validation just build the data representing all of the 
 * validations we can do. 
 *  
 * The validation level can then be configured. 
 *  
 * Each class will then access this data during a validation to determine what 
 * kind of a message (if any) to emit for a failed validation. 
 *  
 * \note This is maintained in the diagram as the diagram is the top level for 
 * running a validation.  
 * 
 * \author pharvey (11/28/19)
 * 
 * \param mapClasses 
 */
ADValidationDatabase DDiagram::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }

    //
    mapClasses.insert( "DDiagram", mapStrictness );

    //
    DRectangle::getValidationConfiguration( mapClasses );             
    DLine::getValidationConfiguration( mapClasses );             

    return mapClasses;
}

/*!
 * \brief Lookup the validation and return its Severity. 
 *  
 * By default this request is just passed up the object hierarchy and returns 
 * ADValidation::Ignore once that is exhausted. 
 *  
 * But we maintain the validation configuration database so we override the 
 * default by looking for the validation and returning the severity. 
 * 
 * \author pharvey (11/29/19)
 * 
 * \param stringClass 
 * \param nStrictness 
 * \param stringValidation 
 * 
 * \return ADValidation::Severity 
 */
ADValidation::Severity DDiagram::getValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation )
{
    // \todo sanity check args
    return mapValidationConfiguration[stringClass][nStrictness][stringValidation];
}

/*!
 * \brief Validate self and ask children to do the same.
 *  
 * We override the Strictness given to us - and use our own instead. 
 *  
 * \author pharvey (4/29/20)
 * 
 * \param nStrictness 
 * \param nValueType 
 * \param listObjectPathItems 
 */
void DDiagram::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "DDiagram", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "DDiagram", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "DDiagram", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Version...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "DDiagram", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getVer().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           

    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      

        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

QDomElement DDiagram::doSaveValidation( QDomDocument *pdomDoc )
{
    QDomElement domElem = pdomDoc->createElement( "Validation" );

    QList<QString> listClasses = mapValidationConfiguration.keys();
    QString stringClass;
    foreach( stringClass, listClasses )
    {
        QDomElement domElemClass = pdomDoc->createElement( stringClass );

        QList<ADValidation::Strictness> listStrictness = mapValidationConfiguration[stringClass].keys();
        ADValidation::Strictness nStrictness;
        foreach( nStrictness, listStrictness )
        {
            QDomElement domElemStrictness = pdomDoc->createElement( ADValidation::getStrictness( nStrictness ) );

            QList<QString> listChecks = mapValidationConfiguration[stringClass][nStrictness].keys();
            QString stringCheck;
            foreach( stringCheck, listChecks )
            {
                QDomElement domElemCheck = pdomDoc->createElement( "Check" );
                domElemCheck.setAttribute( "Text", stringCheck );
                domElemCheck.setAttribute( "Severity", ADValidation::getSeverity( mapValidationConfiguration[stringClass][nStrictness][stringCheck] ) );
                domElemStrictness.appendChild( domElemCheck );
            }
            domElemClass.appendChild( domElemStrictness );
        }
        domElem.appendChild( domElemClass );
    }

    return domElem;
}

/*!
 * \brief Load validation from the given XML. 
 *  
 * We got here because the load picked up a "Validation" tag. All the stuff 
 * within that tag is for this method. 
 *  
 * Data structure is; 
 *  
 * ClassName <<-- Strictness <<-- Validation <-- Severity
 *    0-n              3              0-n           1
 *  
 * \author pharvey (11/29/19)
 * 
 * \param pdomElemValidation 
 */
bool DDiagram::doLoadValidation( QDomElement *pdomElem )
{
    // clear default
    mapValidationConfiguration.clear();

    // process all classes
    QDomElement domElem;
    QDomNode    domNode = pdomElem->firstChild();
    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();
        if ( !domElem.isNull() )
        {
            doLoadValidationStrictness( &domElem, domElem.tagName() );
        }
        domNode = domNode.nextSibling();
    }

    return true;
}

bool DDiagram::doLoadValidationStrictness( QDomElement *pdomElem, const QString &stringClass )
{
    // process all strictness levels
    QDomElement domElem;
    QDomNode    domNode = pdomElem->firstChild();
    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();
        if ( !domElem.isNull() )
        {
            doLoadValidationCheck( &domElem, stringClass, ADValidation::getStrictness( domElem.tagName() ) );
        }
        domNode = domNode.nextSibling();
    }

    return true;
}

bool DDiagram::doLoadValidationCheck( QDomElement *pdomElem, const QString &stringClass, ADValidation::Strictness nStrictness )
{
    // process all checks
    QDomElement domElem;
    QDomNode    domNode = pdomElem->firstChild();
    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();
        if ( !domElem.isNull() )
        {
            QString stringCheck = domElem.attribute( "Text", "unknown" );
            ADValidation::Severity nSeverity =  ADValidation::getSeverity( domElem.attribute( "Severity", "Ignore" ) );
            mapValidationConfiguration[stringClass][nStrictness][stringCheck] = nSeverity;
        }
        domNode = domNode.nextSibling();
    }

    return true;
}

void DDiagram::setSceneRect()
{
    if ( !getScene() ) return;
    QSizeF sizePixels = getCalcSceneSize();
    getScene()->setSceneRect( 0, 0, sizePixels.width(), sizePixels.height() );
}

/*!
 * \brief Add nPages to the nSetPage side/end of the scene. 
 *  
 * Supports \sa doEnsurePages. 
 *  
 * Here we resize the sceneRect (via setSceneRect()) but also move all objects 
 * towards the bottom (when DDiagram::Top) or to the right (when DDiagram::Left). 
 *  
 * \author pharvey (11/19/19)
 * 
 * \param nSetPage  Top, Left, Bottom....
 * \param nPages    Number of pages to add (usually just 1).
 */
void DDiagram::setPages( DDiagram::SetPage nSetPage, int nPages )
{
    QSizeF  sizePage        = getCalcPageSize();
    qreal   nPageWidth      = sizePage.width();
    qreal   nPageHeight     = sizePage.height();

    // do this so its more convenient for caller (they do not have to check for 0)
    if ( nPages == 0 ) return;

    //
    switch ( nSetPage )
    {
        case DDiagram::Top:
        {
            QList<ADObject *> listChildren = getObjects( QString(), QString(), "DObject" );
            ADObject *pChild;
            foreach( pChild, listChildren )
            {
               DObject *pObject = (DObject *)pChild;
               pObject->setPosDeltaRaw( QPointF( 0, nPageHeight * nPages ) );
            }
            sizePages.setHeight( sizePages.height() + nPages );
            setSceneRect();
            doScrollBy( 0, nPageHeight * nPages );

            if ( isSnapGrid() )
            { 
                // We have adjusted from page edge but grid is no longer same distance from edge
                // so we snap all rectangles to nearest cell.
                QList<ADObject *> listChildren = getObjects( QString(), QString(), "DRectangle" );
                ADObject *pChild;
                foreach( pChild, listChildren )
                {
                    DObject *pObject = (DObject *)pChild;
                    pObject->setPos( getGridCenter( pObject->getPos() ) );
                }
            }
        }
            break;

        case DDiagram::Left:
        {
            QList<ADObject *> listChildren = getObjects( QString(), QString(), "DObject" );
            ADObject *pChild;
            foreach( pChild, listChildren )
            {
                DObject *pObject = (DObject *)pChild;
                pObject->setPosDeltaRaw( QPointF( nPageWidth * nPages, 0 ) );
            }
            sizePages.setWidth( sizePages.width() + nPages );
            setSceneRect();
            doScrollBy( nPageWidth * nPages, 0 );

            if ( isSnapGrid() )
            { 
                // We have adjusted from page edge but grid is no longer same distance from edge
                // so we snap all rectangles to nearest cell.
                QList<ADObject *> listChildren = getObjects( QString(), QString(), "DRectangle" );
                ADObject *pChild;
                foreach( pChild, listChildren )
                {
                    DObject *pObject = (DObject *)pChild;
                    pObject->setPos( getGridCenter( pObject->getPos() ) );
                }
            }
        }
            break;

        case DDiagram::Right:
            sizePages.setWidth( sizePages.width() + nPages );
            setSceneRect();
            break;

        case DDiagram::Bottom:
            sizePages.setHeight( sizePages.height() + nPages );
            setSceneRect();
            break;
        default:
            setSceneRect();
    }
}


QSizeF DDiagram::getCalcSceneSize()
{
    QSizeF sizePage        = getCalcPageSize();
    qreal  nWidth          = (nBorder*2 + sizePages.width() * sizePage.width());
    qreal  nHeight         = (nBorder*2 + sizePages.height() * sizePage.height());
    return QSizeF( nWidth, nHeight );
}

QSizeF DDiagram::getCalcPageSize()
{
    if ( !isPagePrinter() ) return sizePage;
    qreal nPageWidth  = printer.pageRect( QPrinter::DevicePixel ).width();
    qreal nPageHeight = printer.pageRect( QPrinter::DevicePixel ).height();

    if ( nPageWidth < 2 ) nPageWidth = 1100;
    if ( nPageHeight < 2 ) nPageHeight = 2879;

    return QSizeF( nPageWidth, nPageHeight);
}

QRectF DDiagram::getCalcBoundingRect()
{
    // DIObjects
    QRectF r;
    QList<DObject*> listChildren = findChildren<DObject*>( QString(), Qt::FindDirectChildrenOnly );
    DObject *pObject;
    foreach( pObject, listChildren )
    {
        r = r.united( pObject->getProxy()->mapToScene( pObject->boundingRect() ).boundingRect() );
    }

    // guides
    // - lets not orphan guides
    {
        DGraphicsItemLine *pItem;
        QLineF line;
        foreach( pItem, vectorGuidesX )
        {
            line = pItem->getLine();
            if ( line.x1() > r.right() ) r.setRight( line.x1() );
            if ( line.x1() < r.left() ) r.setLeft( line.x1() );
        }
        foreach( pItem, vectorGuidesY )
        {
            line = pItem->getLine();
            if ( line.y1() > r.bottom() ) r.setBottom( line.y1() );
            if ( line.y1() < r.top() ) r.setTop( line.y1() );
        }
    }

    return r;
}

QPointF DDiagram::getGridCenter( const QPointF &pointScene )
{
    return mapCellToScene( mapSceneToCell( pointScene ) );
}

bool DDiagram::isSnapGrid() 
{ 
    return ( !pGrid->getSnap() == S::GridSnapNone );
}

bool DDiagram::isGrid() 
{ 
    return pGrid->getVisible(); 
}

bool DDiagram::isOffScene( const QPointF &pointScene )
{
    if ( pointScene.x() < 0 ) return true;
    if ( pointScene.y() < 0 ) return true;
    if ( pointScene.x() >= getScene()->width() ) return true;
    if ( pointScene.y() >= getScene()->height() ) return true;
    return false;
}

bool DDiagram::isOffScene( const QRectF &rectScene )
{
    if ( rectScene.left() < 0 ) return true;
    if ( rectScene.top() < 0 ) return true;
    if ( rectScene.right() >= getScene()->width() ) return true;
    if ( rectScene.bottom() >= getScene()->height() ) return true;
    return false;
}

bool DDiagram::isOffGrid( const QPoint &pointCell )
{
    return isOffScene( mapCellToScene( pointCell ) );
}

bool DDiagram::isSelection()
{
    return pSelectionManager->hasSelection();
}

DGraphicsProxyItem *DDiagram::getProxy( QGraphicsObject *p )
{
    if ( !p ) return nullptr;
    if ( !p->inherits( "DGraphicsObject" ) ) return nullptr;
    return ((DGraphicsObject*)p)->getProxy();
}

bool DDiagram::hasProxy( QGraphicsObject *p )
{
    return getProxy( p );
}

/*!
 * \brief Is diagram prepared to accept a drop.
 *  
 * Returns true if the given class can be instatiated and placed at the given position. 
 *  
 * The default is to validate the class and accept any position. 
 * However; this may not always be the case. 
 *  
 * Example 1; 
 *  
 * FLDiagram only accepts a drop on a flow. 
 *  
 * \author pharvey (3/25/20)
 * 
 * \param stringClass 
 * \param pointScene 
 * 
 * \return bool 
 */
bool DDiagram::canDrop( const QString &stringClass, const QPointF &pointScene )
{
    Q_UNUSED(pointScene);
    if ( stringClass != "DStencilItemSvg" && 
         stringClass != "DStencilItemBitmap" && 
         stringClass != "DStencilItemDiagram" && 
         !getClasses( true ).contains( stringClass ) ) return false;
    return true;
}

bool DDiagram::doDragEnter( const QString &stringClass, const QPointF &pointScene )
{
    Q_UNUSED(pointScene);

    if ( stringClass != "DStencilItemSvg" && 
         stringClass != "DStencilItemBitmap" && 
         stringClass != "DStencilItemDiagram" && 
         !getClasses( true ).contains( stringClass ) ) return false;
    slotSelectNone();
    return true;
}

bool DDiagram::doDragMove( const QString &stringClass, const QPointF &pointScene )
{
    if ( !canDrop( stringClass, pointScene ) ) return false;
    return true;
}

/*!
 * \brief Instantiates the class and positions on the diagram. 
 *  
 * The object will be 'snapped-to-grid' if this feature is turned on. 
 * The object will be selected (and be only object selected). 
 * Pages will be added as needed. 
 * 
 * \author pharvey (3/25/20)
 * 
 * \param stringClass 
 * \param pointScene 
 * 
 * \return bool 
 */
bool DDiagram::doDragDrop( const QStringList &stringList, const QPointF &pointScene )
{
    QString stringClass = stringList.first();

    if ( !canDrop( stringClass, pointScene ) ) return false;

    ADObject *p = nullptr;
    if ( stringClass == "DStencilItemSvg" )
    {
        // package is a pointer to a DStencilItemSvg 
        DVector *pVector;
        p = pVector = (DVector *)getObject( "DVector" );
        if ( stringList.count() > 1 )
        {
            QString stringPointer = stringList.at( 1 );
            DStencilItemSvg *pStencilItem = (DStencilItemSvg *)stringPointer.toULongLong();
            if ( pStencilItem )
            {
                pVector->slotVector( pStencilItem->getVector() );
            }
        }
    }
    else if ( stringClass == "DStencilItemBitmap" )
    {
        // package is a pointer to a DStencilItemBitmap 
        DBitmap *pBitmap;
        p = pBitmap = (DBitmap *)getObject( "DBitmap" );
        if ( stringList.count() > 1 )
        {
            QString stringPointer = stringList.at( 1 );
            DStencilItemBitmap *pStencilItem = (DStencilItemBitmap *)stringPointer.toULongLong();
            if ( pStencilItem )
            {
                pBitmap->slotPixmap( pStencilItem->getPixmap() );
            }
        }
    }
    else
    {
        p = getObject( stringClass );
    }

    //
    slotSelectNone();

    //
    if ( !p ) return false;
    if ( !p->inherits( "DObject" ) ) return false;

    DObject *pObject = (DObject*)p;

    // common
    if ( isSnapGrid() && pObject->inherits( "DRectangle" ) ) pObject->setPos( getGridCenter( pointScene ) );
    else pObject->setPos( pointScene );

    pSelectionManager->setSelected( pObject );
    doEnsurePages();

   return true; 
}

/*!
 * \brief Ensures that we have the appropriate number of pages.
 *  
 *  This supports case where;
 *  1. An object has been moved outside of the current scene rect.
 *      In such a case; we automatically add page(s).
 *  2. An object has been moved off of a page and the page is now empty.
 *      In such a case the page(s) are removed - last page excepted.
 *  
 * \author pharvey (11/19/19)
 */
void DDiagram::doEnsurePages()
{
    bool bFoundObject = false;
    QSizeF sizePage        = getCalcPageSize();
    qreal  nPageWidth      = sizePage.width();
    qreal  nPageHeight     = sizePage.height();
    qreal  nMinX = 0;
    qreal  nMinY = 0;
    qreal  nMaxX = 0;
    qreal  nMaxY = 0;

    // get width and height (in pixels) required
    // - do NOT use the Qt version as it will include DGraphicsItemLine
    // QRectF rectAllItems = getScene()->itemsBoundingRect();
    QRectF rectAllItems = getCalcBoundingRect();
    if ( !rectAllItems.isNull() && rectAllItems.width() > 0 )
    {
        nMinX   = rectAllItems.x();
        nMinY   = rectAllItems.y();
        nMaxX   = rectAllItems.x() + rectAllItems.width();
        nMaxY   = rectAllItems.y() + rectAllItems.height();
        bFoundObject = true; 
    }

    // a single page when no objects
    if ( !bFoundObject )
    {
        sizePages = QSizeF( 1, 1 );
        setSceneRect();
        return;
    }

    // add/remove pages Right
    if ( nMaxX > nPageWidth * sizePages.width() + nBorder )
        setPages( DDiagram::Right, (nMaxX - (nPageWidth * sizePages.width() + nBorder)) / nPageWidth + 1 ); // ADD SOME PAGE(S)
    else if ( (nPageWidth * sizePages.width() + nBorder) - nMaxX > nPageWidth )
        setPages( DDiagram::Right, -((nPageWidth * sizePages.width() + nBorder) - nMaxX) / nPageWidth ); // REMOVE SOME PAGE(S)

    // add/remove pages Bottom
    if ( nMaxY > nPageHeight * sizePages.height() + nBorder )
        setPages( DDiagram::Bottom, (nMaxY - (nPageHeight * sizePages.height() + nBorder)) / nPageHeight + 1 ); // ADD SOME PAGE(S)
    else if ( (nPageHeight * sizePages.height() + nBorder) - nMaxY > nPageHeight )
        setPages( DDiagram::Bottom, -((nPageHeight * sizePages.height() + nBorder) - nMaxY) / nPageHeight ); // REMOVE SOME PAGE(S)

    // add/remove pages Left
    if ( nMinX < nBorder )
        setPages( DDiagram::Left, abs(nMinX) / nPageWidth + 1 ); // ADD SOME PAGE(S)
    else if ( nMinX - nPageWidth - nBorder > 0 )
        setPages( DDiagram::Left, -((nMinX - nBorder) / nPageWidth) ); // REMOVE SOME PAGE(S)

    // add/remove pages Top
    if ( nMinY < nBorder )
        setPages( DDiagram::Top, abs(nMinY) / nPageHeight + 1 ); // ADD SOME PAGE(S)
    else if ( nMinY - nPageHeight - nBorder > 0 )
        setPages( DDiagram::Top, -((nMinY - nBorder) / nPageHeight) ); // REMOVE SOME PAGE(S)

    // adjust our guides as needed
    // - ensure lines cross width/height (no more and no less)
    {
        DGraphicsItemLine *pItem;
        foreach( pItem, vectorGuidesX )
        {
            QLineF line = pItem->getLine();
            if ( line.x1() < 0 || line.x1() > getScene()->width() )
            {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "guide off scene: should not happen - check scroll adjust during ensure pages";
            }
            if ( line.y2() != getScene()->height() ) pItem->setLine( QLineF( line.x1(), 0, line.x1(), getScene()->height() ) );
        }
        foreach( pItem, vectorGuidesY )
        {
            QLineF line = pItem->getLine();
            if ( line.y1() < 0 || line.y1() > getScene()->height() )
            {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "guide off scene: should not happen - check scroll adjust during ensure pages";
            }
            if ( line.x2() != getScene()->width() ) pItem->setLine( QLineF( 0, line.y1(), getScene()->width(), line.y1() ) );
        }
    }
}

void DDiagram::doScrollBy( int nXDelta, int nYDelta )
{
    DGraphicsSceneProxy *pScene = getScene();
    if ( !pScene ) return;
    QList<QGraphicsView*> listViews = pScene->views();
    QGraphicsView *pView;
    foreach( pView, listViews )
    {
        if ( pView->metaObject()->className() == QString( "DPanner" ) ) continue;
        int nX = pView->horizontalScrollBar()->value();
        int nY = pView->verticalScrollBar()->value();

        int nXNew = nX + nXDelta;
        int nYNew = nY + nYDelta;

        pView->horizontalScrollBar()->setValue( nXNew );
        pView->verticalScrollBar()->setValue( nYNew );
    }
}

void DDiagram::doCreateCrossHairs()
{
    QPen pen( Qt::SolidLine );
    pen.setColor( Qt::red );

    pCrossH = new DGraphicsItemLine( QLineF( qreal(0), qreal(0), qreal(0), qreal(0) ) ); 
    pCrossV = new DGraphicsItemLine( QLineF( qreal(0), qreal(0), qreal(0), qreal(0) ) );
    pCrossH->setPen( pen );
    pCrossV->setPen( pen );
    pScene->addItem( pCrossH );
    pScene->addItem( pCrossV );
}

/*!
 * \brief Create a guide. For use during a load.
 *  
 * Similar to slotGuideNewRequest. The validation is different. 
 *  
 * \author pharvey (2/24/20)
 * 
 * \param nLineOrientation 
 * \param nOnScene 
 */
void DDiagram::doLoadGuide( Qt::Orientation nLineOrientation, qreal nOnScene )
{
    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call works whether we have an editor/ruler or not
    // but if a ruler is created it will have to create markers for these
    WRulerWidget *pRuler = nullptr;
    if ( pEditorWidget ) pRuler = ((DDiagramEditorWidget*)pEditorWidget)->getRuler( nRulerOrientation );

    // accept all requests

    // get ruler to add a guide marker
    if ( pRuler ) pRuler->doGuideNew( nOnScene );

    // add a guide line
    DGraphicsItemLine *pItem;
    if ( nLineOrientation == Qt::Vertical )
    {
        pItem = new DGraphicsItemLine( QLineF( nOnScene, 0, nOnScene, getScene()->height() ) );
        vectorGuidesX.append( pItem );
    }
    else
    {
        pItem = new DGraphicsItemLine( QLineF( 0, nOnScene, getScene()->width(), nOnScene ) );
        vectorGuidesY.append( pItem );
    }

    QPen pen( Qt::SolidLine );
    pen.setColor( Qt::lightGray );

    pItem->setPen( pen );
    pScene->addItem( pItem );
}

void DDiagram::doClear()
{
    nState          = StateWaiting;
    // do not clear scene - this needs to be done explicitly
    // pScene          = nullptr;
    nButton         = Qt::NoButton;
    pRectangleOver  = nullptr;
    pHandlePressed  = nullptr;
    pObjectPressed  = nullptr;
    pSourceMoving   = nullptr;
    pRubberBand     = nullptr;
    bPageBreaks     = false;            // off expected by DStencil::slotAddDiagram()
 //   bGrid           = false;            // off expected by DStencil::slotAddDiagram()
 //   sizeCell        = QSize( 48, 48 );
 //   bSnapGrid       = false;
    bRuler          = true;
    bCrossHairs     = false;
    pCrossH         = nullptr;
    pCrossV         = nullptr;
    pClipboard      = QApplication::clipboard();
    nCutMode        = ADTransaction::CopyReferenceOnly;
    nCopyMode       = ADTransaction::CopyReferenceOnly;
    nDeleteMode     = ADTransaction::DeleteReferencedUnused;
    nBorder         = 0; // 10; // any ruler is not currently informed about a border - default to 0 until this is corrected
    sizePages       = QSizeF( 1, 1 );
    printer.setPageSize( QPageSize() );
    printer.setFullPage( false );
    pen.setColor( Qt::black );
    brush.setColor( Qt::white );

    AWObject::doClear();
}

void DDiagram::slotChangedSelection()
{
    emit signalChangedCut( canCut() );
    emit signalChangedCopy( canCopy() );
    emit signalChangedSelection( isSelection() );

    // pen/brush/text will indicate a single selected DObject if one selected...
    QList<ADObject *> l = pSelectionManager->getSelected();
    ADObject *p;
    DObject *pObject = nullptr;
    foreach( p, l )                                
    {                                              
        if ( p->inherits("DObject") )
        {
            pObject = (DObject*)p;
            emit signalChangedPen( pObject->getPen() );
            emit signalChangedBrush( pObject->getBrush() );
            emit signalChangedText( pObject->getText() );
            return;
        }
    }

    // no DObject selected so pen/brush/text will indicate what is being used by default for a new DObject
    emit signalChangedPen( getPen() );
    emit signalChangedBrush( getBrush() );
    emit signalChangedText( getText() );
}

/*!
 * \brief Let others know whether we can or can not paste.
 *  
 * Others may want to update a paste indicator in the UI. 
 *  
 * \author pharvey (11/16/19)
 */
void DDiagram::slotClipboardDataChanged()
{
    if ( canPaste() )
    {
        emit signalChangedPaste( false );
    }
    else
    {
        emit signalChangedPaste( true );
    }
}

/*!
 * \brief Handle when user selects text; ie in a terminal or whatever.
 *  
 * Its unlikley that someone has put viable XML into the clipboard in this way 
 * but it is possible. 
 *  
 * \note Only X has this. 
 *  
 * \author pharvey (11/16/19)
 */
void DDiagram::slotClipboardSelectionChanged()
{
    // pClipboard->setSelectionMode( true );
    slotClipboardDataChanged();
    // pClipboard->setSelectionMode( false );
}

void DDiagram::slotRouteChanged()
{
    if ( !pScene ) return;
    pScene->invalidate( 0, 0, pScene->width(), pScene->height() );
//    pScene->update();
}

void DDiagram::slotSelectClass( QAction *pAction )
{
    doSelectByClassName( getClass( pAction->text() ) );
}

void DDiagram::slotSelectClass( const QString &s )
{
    doSelectByClassName( getClass( s ) );
}


