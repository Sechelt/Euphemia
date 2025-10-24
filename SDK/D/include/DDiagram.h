#ifndef H_DDiagram
#define H_DDiagram

#include <QPrinter>

#include <ADDocument.h>
#include <ADObjectSelectionManager.h>

#include <AWObject.h>

class SGrid;
class SRuler;

class AWDataObjectWidget;

class DObject;
class DGraphicsSceneProxy;
class DGraphicsProxyItem;
class DHandle;
class DRubberBand;
class DSource;
class DGraphicsItemLine;

// stuff that we can create
class DRectangle;
class DLine;
class DTitle;
class DEllipse;
class DArc;
class DImage;
class DText;
class DLayout;

/*!
 * \brief Basic diagramming. This is the basis for all other diagrams.
 *  
 * The goal of DDiagram is to provide the basic diagramming needed by other diagrams such as; 
 *  
 *  - Organization Charts \sa ORGDiagram
 *  - Flow Charts \sa FLDiagram
 *  - Entity Relationship Diagrams \sa LMDiagram
 *  etc
 *  
 * This is NOT meant to be a complete generic diagramming tool in its own right. 
 * Think of it as an SDK.
 *  
 * \author pharvey (4/20/20)
 */
class DDiagram : public AWObject, public ADDocument
{
    Q_OBJECT
public:
    enum States
    {
        StateWaiting,
        StatePressCanvas,
        StatePressHandleForMove,
        StatePressObject,
        StatePressObjectForMove,
        StateMoveRubberBand,
        StateMoveHandle,
        StateMoveObject
    };

    enum SetPage
    {
        Top, Left, Bottom, Right
    };

    explicit DDiagram( ADObject *pParent, const QString &stringName = QString() );
    virtual ~DDiagram();

    virtual void setCell( const QSize &size );
    virtual void setScene( DGraphicsSceneProxy *p );

    virtual QPixmap                     getIcon();
    virtual QString                     getFriendlyClassName() { return tr( "Diagram" ); }
    virtual ADObjectSelectionManager *  getSelectionManager() { return pSelectionManager; }
    virtual AWDataWidget *              getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *              getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual ADTransaction::CopyModes    getCutMode() { return nCutMode; }
    virtual ADTransaction::CopyModes    getCopyMode() { return nCopyMode; }
    virtual ADTransaction::DeleteModes  getDeleteMode() { return nDeleteMode; }
    virtual QSize                       getCell();
    virtual QSize                       getCells();
    virtual QVector<DGraphicsItemLine*> getGuidesX() { return vectorGuidesX; }
    virtual QVector<DGraphicsItemLine*> getGuidesY() { return vectorGuidesY; }
    virtual ADObject *                  getObject( int nOID ) { return AWObject::getObject( nOID ); }
    virtual DObject *                   getObject( const QString &stringClass, const QPointF &pointScene );
    virtual DLayout *                   getLayout( const QPointF &pointScene, DLayout *pLayoutExclude = nullptr );
    virtual DGraphicsSceneProxy *       getScene() { return pScene; }
    virtual ADObject *                  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual DRectangle *                getRectangle( const QPoint &pointCell, const QString &stringInherits = "DRectangle" );
    virtual DRectangle *                getRectangle( const QPointF &pointPos, const QString &stringInherits = "DRectangle" );
    virtual QMenu *                     getContextMenu( QWidget * );
    virtual ADValidation::Strictness    getStrictness() { return nStrictness; }
    virtual SGrid *                     getGrid()    { return pGrid; }
    virtual SRuler *                    getRuler()   { return pRuler; }
    virtual QPen                        getPen();
    virtual QBrush                      getBrush();
    virtual CBD::CBDText                getText();
    virtual QPen                        getPenBackground()    { return penBackground;   }
    virtual QBrush                      getBrushBackground()  { return brushBackground; }
    virtual bool                        getBitmapBackgroundUse() { return bPixmapBackgroundUse; }
    virtual QPixmap                     getBitmapBackground() { return pixmapBackground; }
    virtual W::WImagePlacements         getBitmapBackgroundPlacement() { return nBitmapBackgroundPlacement; }
    virtual QSizeF                      getPage()   { return sizePage;  }
    virtual QSizeF                      getPages()  { return sizePages; }
    virtual QByteArray                  getAsSvg();
    virtual QPixmap                     getAsPixmap( bool bCrop = false );
    virtual QImage                      getAsPng( bool bCrop = false );
    virtual bool                        isPagePrinter() { return bPagePrinter; }
    virtual bool                        canCut();
    virtual bool                        canCopy();
    virtual bool                        canPaste();
    virtual bool                        canUndo() { return false; }
    virtual bool                        canRedo() { return false; }
    virtual bool                        canDelete();
    virtual bool                        isSnapGrid();
    virtual bool                        isPageBreaks() { return bPageBreaks; }
    virtual bool                        isGrid();
    virtual bool                        isRuler() { return bRuler; }
    virtual bool                        isCrossHairs() { return bCrossHairs; }
    virtual bool                        isOffScene( const QPointF &pointScene );
    virtual bool                        isOffScene( const QRectF &rectScene );
    virtual bool                        isOffGrid( const QPoint &pointCell );
    virtual bool                        isSelection();

    virtual QPoint                      mapSceneToCell( const QPointF &pointScene );
    virtual QPointF                     mapCellToScene( const QPoint &pointCell );
    virtual QRect                       mapSceneToCell( const QRectF &rectScene );
    virtual QRectF                      getCellRect( const QPoint &pointCell );

    // Main event handlers called by \sa DGraphicsSceneProxy
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void drawBackground( QPainter *painter, const QRectF &rect, bool bPanner = false );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *pEvent );
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent );

    // Refined events handlers called by main event handlers.
    virtual void mouseMoveHandle( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseMoveObjects( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseReleaseHandle( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseReleaseObjects( QGraphicsSceneMouseEvent *pMouseEvent );

    // supports for drawBackground (public so panner get use them)
    virtual void doPaintPages( QPainter *pPainter );
    virtual void doPaintGrid( QPainter *pPainter );

    virtual void doSelectByClassName( const QString &stringClassName );
    virtual void doSelectByInherits( const QString &stringInherits );
    virtual void doSelectByArea( const QRectF &r );

    virtual void doHideByClassName( const QString &stringClassName, bool b = true );
    virtual void doHideByInherits( const QString &stringInherits, bool b = true );

    virtual void doGoTo( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doGoTo();
    virtual void doGoTo( DObject * );

    virtual void doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria );

    // validation
    virtual void setValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation, ADValidation::Severity ); // update
    virtual void getValidationConfiguration();                                                  // clear then load our default validation configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual ADValidation::Severity getValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation ); // lookup severity
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemModel );                               
    virtual void doPostLoad();                                                       
    virtual void doEnsurePages();   // ensures we have the appropriate number of pages

//    virtual bool isSelected() { return listSelected.count(); }

signals:
    void signalChangedCut( bool b );
    void signalChangedCopy( bool b );
    void signalChangedPaste( bool b );
    void signalChangedSelection( bool b );
    void signalChangedUndo( bool b );
    void signalChangedRedo( bool b );
    void signalChangedSnapGrid( bool b );
    void signalChangedPageBreaks( bool b );
    void signalChangedGrid( bool b );
    void signalChangedRuler( bool b );
    void signalChangedCrossHairs( bool b );
    void signalChangedBrush( const QBrush &brush );
    void signalChangedPen( const QPen &pen );
    void signalChangedText( const CBD::CBDText & );
    void signalChangedBackgroundBrush( const QBrush &brush );
    void signalChangedBackgroundPen( const QPen &pen );
    void signalBitmapBackgroundUse( bool );
    void signalBitmapBackground( const QPixmap & );
    void signalBitmapBackgroundPlacement( W::WImagePlacements );
    void signalCell( const QPoint & );
    void signalPixel( const QPointF & );
    void signalPage( const QSizeF &size );
    void signalPagePrinter( bool b );

public slots:
    // settings
    virtual void slotSettingGrid();
    virtual void slotSettingRuler();

    //
    virtual bool slotOpenEditor();
    virtual void slotPrint();
    virtual void slotSelectionCut();
    virtual void slotSelectionCopy();
    virtual void slotSelectionPaste();
    virtual void slotSelectAll();
    virtual void slotSelectNone();
    virtual void slotSelectionDelete();

    // get rid of this stuff
    virtual void slotCutMode( int );
    virtual void slotCopyMode( int );
    virtual void slotDeleteMode( int );

    virtual void slotUndo();
    virtual void slotRedo();
    virtual void slotGroup();
    virtual void slotUngroup();
    virtual void slotToFront();
    virtual void slotToBack();
    virtual void slotOptimizeSize();
    virtual void slotSize16();  
    virtual void slotSize32();  
    virtual void slotSize48();  
    virtual void slotSize64();  
    virtual void slotSize96();  
    virtual void slotSize128(); 
    virtual void slotResetTransformation(); 
    virtual void slotAlignHObject( Qt::AlignmentFlag n );
    virtual void slotAlignVObject( Qt::AlignmentFlag n );
    virtual void slotSnapGrid( bool b = true );
    virtual void slotPageBreaks( bool b = true );
    virtual void slotGrid( bool b = true );
    virtual void slotRuler( bool b = true );
    virtual void slotCrossHairs( bool b = true );

    virtual void slotPenBackground( const QPen &pen );
    virtual void slotBrushBackground( const QBrush &brush );
    virtual void slotBitmapBackgroundUse( bool );
    virtual void slotBitmapBackground( const QPixmap & );
    virtual void slotBitmapBackgroundPlacement( W::WImagePlacements );
    virtual void slotPage( const QSizeF &size );
    virtual void slotPagePrinter( bool b );

    virtual void slotPen( const QPen &pen );
    virtual void slotBrush( const QBrush &brush );
    virtual void slotText( const CBD::CBDText &text );
    virtual void slotShapeSize( int );

    virtual void slotGuideNewRequest( Qt::Orientation nLineOrientation, qreal nOnScene );
    virtual void slotGuideDeleteRequest( Qt::Orientation nLineOrientation, int nIndex );
    virtual void slotGuideMoveRequest( Qt::Orientation nLineOrientation, int nIndex, qreal nOnScene );

    virtual void slotStrictness( ADValidation::Strictness n );
    virtual void slotExport();

protected:
    // background/canvas
    QPen                penBackground;
    QBrush              brushBackground;
    bool                bPixmapBackgroundUse;
    QPixmap             pixmapBackground;
    W::WImagePlacements nBitmapBackgroundPlacement;

    // these are applied to new canvas objects
    // controls should be sync with these unless a canvas object is selected 
    QPen                pen;
    QBrush              brush;
    CBD::CBDText        text;

    //
    QClipboard *pClipboard;
    ADTransaction::CopyModes    nCutMode;       // get rid of these
    ADTransaction::CopyModes    nCopyMode;
    ADTransaction::DeleteModes  nDeleteMode;
    ADObjectSelectionManager *  pSelectionManager;
    DGraphicsSceneProxy *       pScene;    // the QGraphicsScene we are using

    // validation
    // Validation can be invoked at a higher level in the object hierarchy but...
    // validation only 'kicks in' at the diagram and involves all of its children.
    // Defaults are loaded when this is created but are replaced when loaded as 
    // mods to the config are saved/loaded with this.
    ADValidationDatabase mapValidationConfiguration;       // our validation configuration
    ADValidation::Strictness nStrictness;
    virtual QDomElement doSaveValidation( QDomDocument *pdomDoc );
    virtual bool doLoadValidation( QDomElement *pdomElem );                               
    virtual bool doLoadValidationStrictness( QDomElement *pdomElem, const QString &stringClass );
    virtual bool doLoadValidationCheck( QDomElement *pdomElem, const QString &stringClass, ADValidation::Strictness nStrictness );

    // state stuff
    States  nState;                 // our state (mostly mouse event related)
    QPointF pointLast;              // last point during a move
    DObject *pRectangleOver;        // for StateMoveSource
    DLayout *pLayoutOver;           // for StateMoveObject
    DObject *pObjectPressed;        // for StatePressObject
    DHandle *pHandlePressed;        // for StatePressHandleForMove/StateMoveHandle
    DSource *pSourceMoving;         // for StateMoveHandle/StateMoveSource
    Qt::MouseButton nButton;        // for all mouse events
    DRubberBand *pRubberBand;       // for StateMoveRubberBand

    // settings 
    // - for this specific instance
    SGrid * pGrid   = nullptr;
    SRuler *pRuler  = nullptr;
    bool    bPageBreaks;               // true to show page breaks
    bool    bRuler;                    // true to show a ruler
    bool    bCrossHairs;               // true to show cross hairs
    DGraphicsItemLine *pCrossH;
    DGraphicsItemLine *pCrossV;


    // Guides are typically created/deleted by a ruler but they can exist without a ruler and are saved/loaded with the diagram.
    // So the diagram is the 'model' and the ruler (if any) is simply a 'controller' that can come and go.
    // Guides can be a horizontal line or a vertical line. 
    // Orientation can also be determined by checking line coordinates. For example; IF x1==x2 THEN line is vertical.
    // Guides that find themselves off of the scene due to a scene resize... are deleted.
    QVector<DGraphicsItemLine*> vectorGuidesX; 
    QVector<DGraphicsItemLine*> vectorGuidesY; 

    bool        bPagePrinter;           // use current printer for page size (default)
    QPrinter    printer;                // besides the obvious - we can use this to create our scene pages
    QSizeF      sizePage;               // manual page size - used by getCalcPageSize() - do not use directly - use getCalcPageSize()
    QSizeF      sizePages;              // number of pages wide and high (page sizes derived from QPrinter)
    int         nBorder;                // border around page (on screen not printer)

    virtual void setSceneRect();    // set scene rect
    virtual void setPages( DDiagram::SetPage nSetPage, int nPages );    // adds/removes pages, set scene rect, and adjusts objects as needed

    virtual QSizeF getCalcSceneSize();      // calculates size for screen rect
    virtual QSizeF getCalcPageSize();       // calculates page size (using QPrinter)
    virtual QRectF getCalcBoundingRect();   // determines rect used by all items (excludes DGraphicsLineItems)
    virtual QPointF getGridCenter( const QPointF &pointScene ); // returns center of grid cell (for snapping to grid) 

    virtual DGraphicsProxyItem *getProxy( QGraphicsObject *p );
    virtual bool hasProxy( QGraphicsObject *p );

    virtual bool canDrop( const QString &stringClass, const QPointF &pointScene );

    virtual bool doDragEnter( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragMove( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragDrop( const QStringList &stringList, const QPointF &pointScene );

    virtual void doScrollBy( int nX, int nY );
    virtual void doCreateCrossHairs();
    virtual void doLoadGuide( Qt::Orientation nLineOrientation, qreal nOnScene ); 

    virtual void doClear();

protected slots:
    virtual void slotChangedSelection();
    virtual void slotClipboardDataChanged();        // connected to QApp clipboard
    virtual void slotClipboardSelectionChanged();   // connected to QApp clipboard
    virtual void slotRouteChanged();
    virtual void slotSelectClass( QAction *pAction );
    virtual void slotSelectClass( const QString &stringClass );
};

#endif


