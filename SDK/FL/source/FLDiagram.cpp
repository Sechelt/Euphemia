#include "LibInfo.h"
#include "FLDiagram.h"

#include <AWFactoryWidget.h>

#include <DGraphicsSceneProxy.h>
#include <DGraphicsProxyItem.h>
#include <DGraphicsView.h>

#include "FLDiagramEditorWidget.h"

#include "FLAsync.h"
#include "FLBegin.h"
#include "FLConditionOpen.h"
#include "FLConditionClose.h"
#include "FLEnd.h"
#include "FLFlow.h"
#include "FLLoopClose.h"
#include "FLLoopOpen.h"
#include "FLStep.h"
#include "FLSync.h"

#include "FLLayout.h"

/*!
 * \brief Diagram
 * 
 * \author pharvey (4/5/20)
 * 
 * \param pParent 
 * \param stringName 
 */
FLDiagram::FLDiagram( ADObject *pParent, const QString &stringName )
    : DDiagram( pParent, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.clear();

    mapMeta.insert( "FLAsync", ADMeta( QIcon(), "FLAsync", "Async" ) );
    mapMeta.insert( "FLBegin", ADMeta( QIcon(), "FLBegin", "Begin" ) );
    mapMeta.insert( "FLConditionOpen", ADMeta( QIcon(), "FLConditionOpen", "ConditionOpen" ) );
    mapMeta.insert( "FLConditionClose", ADMeta( QIcon(), "FLConditionClose", "ConditionClose" ) );
    mapMeta.insert( "FLEnd", ADMeta( QIcon(), "FLEnd", "End" ) );
    mapMeta.insert( "FLFlow", ADMeta( QIcon(), "FLFlow", "Flow" ) );
    mapMeta.insert( "FLLoopClose", ADMeta( QIcon(), "FLLoopClose", "LoopBottom" ) );
    mapMeta.insert( "FLLoopOpen", ADMeta( QIcon(), "FLLoopOpen", "LoopTop" ) );
    mapMeta.insert( "FLStep", ADMeta( QIcon(), "FLStep", "Step" ) );
    mapMeta.insert( "FLSync", ADMeta( QIcon(), "FLSync", "Sync" ) );

    // \sa doClear
    pBegin          = nullptr;
    pEnd            = nullptr;
    pFlowDragOver   = nullptr;
    pFlowPaste      = nullptr;
    // bSnapGrid       = true;
    // nGridSize = 16;
    pSelectionStart = nullptr;
    pSelectionFinish= nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

FLDiagram::~FLDiagram()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void FLDiagram::setScene( DGraphicsSceneProxy *p ) 
{
    DDiagram::setScene( p );

    // we always start with a Begin and an End
    QPoint pointCell( p->width() / getCell().width() / 2, 1 );

    FLBegin *   pBegin  = (FLBegin*)getObject( "FLBegin" );
    FLEnd *     pEnd    = (FLEnd*)getObject( "FLEnd" );
    FLFlow *    pFlow   = (FLFlow*)getObject( "FLFlow" );

    pBegin->setPos( mapCellToScene( pointCell ) );
    pointCell.setY( pointCell.y() + 2 );
    pEnd->setPos( mapCellToScene( pointCell ) );
    pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pBegin->getSink( "Bottom" ) );
    pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pEnd->getSink( "Top" ) );
}

QPixmap FLDiagram::getIcon()
{
    return QPixmap( ":FL/FlowChart" );
}

/*!
 * \brief  Creates a tab widget with a number of tabs for editing/viewing object properties.
 * 
 * The only thing we want to change from inherited is that we DO NOT want the object factory to be able to create/delete objects.
 * We can not create objects unless we know the valid desired pos (not any pos). 
 * We should not delete some objects ie Begin/End. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *FLDiagram::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DDiagram::getObjectWidget( pWidgetParent );

    int n = p->indexOf( "AWFactoryWidget" );
    if ( n < 0 ) return p;

    AWFactoryWidget *pFactoryWidget = (AWFactoryWidget*)p->widget( n );
    pFactoryWidget->setAllowCreate( false );
    pFactoryWidget->setAllowDelete( false );

    p->setCurrentIndex( 0 );

    return p;
}

ADObject *FLDiagram::getObject( const QString &s, ADObject * )
{
    DObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "FLAsync" )
        p = new FLAsync( this );
    else if ( stringClass == "FLBegin" )
        p = new FLBegin( this );
    else if ( stringClass == "FLConditionOpen" )
        p = new FLConditionOpen( this );
    else if ( stringClass == "FLConditionClose" )
        p = new FLConditionClose( this );
    else if ( stringClass == "FLEnd" )
        p = new FLEnd( this );
    else if ( stringClass == "FLFlow" )
    {
        p = new FLFlow( this );
        p->setFolder( tr( "Flows" ) );
    }
    else if ( stringClass == "FLLoopClose" )
        p = new FLLoopClose( this );
    else if ( stringClass == "FLLoopOpen" )
        p = new FLLoopOpen( this );
    else if ( stringClass == "FLStep" )
        p = new FLStep( this );
    else if ( stringClass == "FLSync" )
        p = new FLSync( this );

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // provide a two-way interface to the scene via a proxy
    DGraphicsProxyItem *pProxy = new DGraphicsProxyObject( p );
    pProxy->doCreateGraphicsObject();
    pProxy->setScene( pScene );
    p->setProxy( pProxy );
    
    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    // the caller should doConnect to a model ASAP
    return (ADObject*)p;
}

void FLDiagram::mouseMoveObjects( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
}

void FLDiagram::mouseReleaseObjects( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
}

bool FLDiagram::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    FLDiagramEditorWidget *p;
    pEditorWidget = p = new FLDiagramEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );
    
    Q_ASSERT( getBegin() );    

    p->doGoTo( getBegin() );

    return true;
}

/*!
 * \brief Copy selected objects to XML. 
 *  
 * The default is copy all selected objects but we handle special considerations here. 
 *  
 * 1. We do not want to copy 'dangling' flows (possible at either end). 
 * 2. We do not want to break conditions or loops. If an Open or Close is included - the whole thing should be included. 
 * 3. We do not want to include Begin or End. 
 * 4. The selection must be contiguous. 
 *  
 * We handle these special considerations by validating the selection first.
 *  
 * \author pharvey (3/25/20)
 */
void FLDiagram::slotSelectionCopy()
{
    // validate selection 
    if ( !canCopy() ) return;

    // call default
    DDiagram::slotSelectionCopy();
}

/*!
 * \brief Delete selected objects.
 *  
 * The default is delete all selected objects but we handle special considerations here.
 *  
 * 1. We do not want to break conditions or loops. If an Open or Close is included - the whole thing should be included. 
 * 2. We do not want to include Begin or End. 
 * 3. The selection must be contiguous. 
 *  
 * We handle these special considerations by validating the selection first.
 *  
 * \author pharvey (3/25/20)
 */
void FLDiagram::slotSelectionDelete()
{
    if ( !canDelete() ) return;

    FLRectangle *   pTop    = pSelectionStart->getPrevious();
    FLRectangle *   pBottom = pSelectionFinish->getNext();
    DSource *       pSource = pSelectionStart->getFlow( "Top" )->getSource( CBD::EOLEnd );
    DSink *         pSink   = pSelectionFinish->getFlow( "Bottom" )->getSource( CBD::EOLEnd )->getSink();

    // use top flow to patch up the gap - the bottom flow gets deleted
    getSelectionManager()->setSelected( pSelectionFinish->getFlow( "Bottom" ) );

    //
    g_Transaction->initDelete( this, getDeleteMode() );
    pSelectionManager->doDelete();

    // connect dangling flow
    pSource->getLine()->doConnect( pSource, pSink );
    pSource->getLine()->doRoute();

    // Compress (vertical) to use up empty space we created.
    // Compress from here to bottom. Short-circuit if we get to a point where no compression is needed.
    {
        // Compress
        FLLayout Layout( this );
        Layout.doCompressVertical( pTop, pBottom );
    }

    // Compress (horizontal) to use up empty space we created.
    // Only compress if all content (from one side of condition) deleted (its the only way we compress and create free space).
    // Compress from here to Right/Left. Short-circuit if we get to a point where no compression is needed.
    if ( pTop->inherits( "FLConditionOpen" ) && pBottom->inherits( "FLConditionClose" ) )
    {
        // determine direction (side of condition) compression occured on
        CBD::DirectionTypes nDir = CBD::DirectionUnknown;
        if ( pSink->objectName() == "Left" ) nDir = CBD::DirectionW;
        else if ( pSink->objectName() == "Right" ) nDir = CBD::DirectionE;

        Q_ASSERT( nDir != CBD::DirectionUnknown );

        // Compress
        // - its possible that we need to compress both sides... so do both
        FLLayout Layout( this );
        Layout.doLayoutHorizontal( pTop );
        // Layout.doCompressHorizontal( (FLConditionOpen*)pTop, nDir );
        // Layout.doCompressHorizontal( (FLConditionOpen*)pTop, (nDir == CBD::DirectionE ? CBD::DirectionW : CBD::DirectionE) );
    }

    //
    pSelectionStart     = nullptr;
    pSelectionFinish    = nullptr;

    doEnsurePages();
    g_Transaction->fini();
    emit signalChangedCut( canCut() );
    emit signalChangedCopy( canCopy() );
    emit signalChangedSelection( isSelected() );
}

/*!
 * \brief Returns flow in the cell. 
 *  
 * There can be 0-1 flow in a cell. 
 * A cell with a rectangle may have more than one but they are ignored in favour of the rectangle.
 *  
 * \author pharvey (3/22/20)
 * 
 * \param pointCell 
 * 
 * \return FLFlow* 
 */
FLFlow *FLDiagram::getFlow( const QPoint &pointCell )
{
   if ( getRectangle( pointCell ) ) return nullptr;

    QList<QGraphicsItem *> l = getScene()->items( getCellRect( pointCell ) );
    QGraphicsItem *p;
    foreach( p, l )
    {
        // all of our items are QGraphicsObject based so...
        DGraphicsProxyItem *pProxy = getProxy( (QGraphicsObject*)p );
        if ( !pProxy ) continue;
        DObject *pObject = pProxy->getObject();
        if ( !pObject ) continue;

        if ( pObject->inherits( "FLFlow" ) ) return (FLFlow *)pObject;
    }
    return nullptr;
}

/*!
 * \brief Returns the rectangle(s) leading to the cell (or potentially leading to the cell).
 * 
 * \author pharvey (3/22/20)
 * 
 * \param pointCell 
 * 
 * \return FLRectangle* 
 */
FLRectangle *FLDiagram::getPrevious( const QPoint &pointCell )
{
    // use flow if we have one...
    FLFlow *pFlow = getFlow( pointCell );
    if ( pFlow ) return (FLRectangle*)pFlow->getRectangle( CBD::EOLBegin );

    // search up...
    FLRectangle *pRectangle;
    // try 2 up (begin, step, loop-open, loop-close, condition-close)
    pRectangle = (FLRectangle*)getRectangle( QPoint( pointCell.x(), pointCell.y() - 2 ) );
    if ( pRectangle ) return pRectangle;

    // try 2 up and 2 left (true for a condition-open)
    pRectangle = (FLRectangle*)getRectangle( QPoint( pointCell.x() - 2, pointCell.y() - 2 ) );
    if ( pRectangle ) return pRectangle;

    // try 2 up and 2 right (false for a condition-open)
    pRectangle = (FLRectangle*)getRectangle( QPoint( pointCell.x() + 2, pointCell.y() - 2 ) );
    if ( pRectangle ) return pRectangle;

    return nullptr;
}

FLRectangle *FLDiagram::getNext( const QPoint &pointCell )
{
    // use flow if we have one...
    FLFlow *pFlow = getFlow( pointCell );
    if ( pFlow ) return (FLRectangle*)pFlow->getRectangle( CBD::EOLEnd );

    // do NOT search down

    return nullptr;
}

void FLDiagram::doMoveBy( FLRectangle *pCurrent, FLRectangle *pFinish, const QPointF &pointDelta )
{
    if ( !pCurrent ) return;

    if ( pCurrent->inherits( "FLConditionClose" ) && ((FLConditionClose*)pCurrent)->getIgnore() ) return;
     
    QPointF pointPos = pCurrent->getPos() + pointDelta;
    pCurrent->setPos( pointPos ); // will cause FLFlow::doRoute and harmonize handles/connections

    if ( pCurrent == pFinish ) return;

    if ( pCurrent->inherits( "FLConditionOpen" ) )
    {
        FLConditionOpen *   pOpen   = (FLConditionOpen*)pCurrent;
        FLConditionClose *  pClose  = pOpen->getClose();
        pClose->setIgnore(); 
        doMoveBy( pCurrent->getNext( CBD::DirectionE ), pFinish, pointDelta );
        pClose->setIgnore( false ); 
        doMoveBy( pCurrent->getNext( CBD::DirectionW ), pFinish, pointDelta );
        return;
    }
    doMoveBy( pCurrent->getNext(), pFinish, pointDelta );
}


/*!
 * \brief Call to check if an FL object of Class can be placed in the diagram at the moment. 
 *  
 * This can be used to determine if a drop is valid (anywhere). 
 * Can be used to enable/disable pallette items.
 *  
 * Begin/End 
 *  
 * Diagrams automatically start with a Begin, an End, and a Flow connecting them. 
 * There can only be one Begin and one End so adding any via; 
 *  - drag from class browser (pallette)
 *  - drag from object browser
 *  - paste
 *  Is an error.
 *  
 * Flow
 *  
 * A Flow is added or removed automatically when rectangles are added/removed. So dragging 
 * or pasting a Flow is an error. An exception is that flows can be pasted when they are 
 * used to connect rectangles in a multi-rectangle paste. 
 *  
 * \author pharvey (3/22/20)
 * 
 * \param stringClass 
 * 
 * \return bool 
 */
bool FLDiagram::canInsert( const QString &stringClass )
{
    if ( stringClass == "Async" ) return true;
    if ( stringClass == "Step" ) return true;
    if ( stringClass == "Sync" ) return true;
    if ( stringClass == "Loop" ) return true;
    if ( stringClass == "Condition" ) return true;

    return false;
}

bool FLDiagram::canDrop( const QString &stringClassDropped, const QPointF &pointScene )
{
    return canInsert( stringClassDropped, mapSceneToCell( pointScene ) );
}

bool FLDiagram::doDragEnter( const QString &stringClass, const QPointF &pointScene )
{
    Q_UNUSED(pointScene);

    if ( !canInsert( stringClass ) )
    {
        doMessage( "ERROR", stringClass + tr( " can not be added this way." ) );
        return false;
    }
    pSelectionManager->doSelectNone();
    pFlowDragOver = nullptr;
    return true;
}

bool FLDiagram::doDragMove( const QString &stringClass, const QPointF &pointScene )
{
    if ( pFlowDragOver ) getSelectionManager()->setSelected( pFlowDragOver, false );

    if ( !canInsert( stringClass ) ) return false;
    if ( !canDrop( stringClass, pointScene ) ) return false;

    pFlowDragOver = getFlow( mapSceneToCell( pointScene ) );
    if ( pFlowDragOver ) getSelectionManager()->setSelected( pFlowDragOver );

    return true;
}

bool FLDiagram::doDragDrop( const QStringList &stringList, const QPointF &pointScene )
{
    QString stringClass = stringList.first();

    if ( !canInsert( stringClass ) ) return false;
    if ( !canDrop( stringClass, pointScene ) ) return false;

    if ( pFlowDragOver ) getSelectionManager()->setSelected( pFlowDragOver, false );

    return doInsert( stringClass, mapSceneToCell( pointScene ) );
}

/*!
 * \brief Call to check if an FL object of Class can be placed in the cell. 
 *  
 * \note This assumes canInsert( stringClass ) would (or has) returned true. 
 * 
 * \author pharvey (3/22/20)
 * 
 * \param stringClass 
 * \param pointCell 
 * 
 * \return bool 
 */
bool FLDiagram::canInsert( const QString &stringClass, const QPoint &pointCell )
{
    Q_UNUSED(stringClass);

    if ( !getFlow( pointCell ) ) return false;

    return true;
}

/*!
 * \brief Insert into a Flow.
 *  
 * To support drop/paste. 
 * The cell must contain a single Flow and nothing else. The cell must not be empty and must not have a Rectangle. 
 * If stringClass is FLCondition or FLLoop - two objects will be inserted (Open and Close).
 *  
 * \author pharvey (3/23/20)
 * 
 * \param stringClass 
 * \param pointCell 
 * 
 * \return bool 
 */
bool FLDiagram::doInsert( const QString &stringClass, const QPoint &pointCell )
{
    if ( getRectangle( pointCell ) ) return false;
    FLFlow *pFlow = getFlow( pointCell );
    if ( !pFlow ) return false;

    DSink *pSink = pFlow->getSource( CBD::EOLBegin )->getSink();
    FLRectangle *pRectangle = (FLRectangle*)pSink->getRectangle();

    return doInsert( stringClass, pRectangle, pSink );
}

/*!
 * \brief Insert after a given rectangle.
 *  
 * The target cell is determined by; 
 *      - class of the previous object
 *      - sink name "Left"/"Right"/"Bottom"
 *      - whether we are expanding (or not)
 *  
 * If stringClass is FLCondition or FLLoop - two objects will be inserted (Open and Close). 
 *  
 * \author pharvey (3/23/20)
 * 
 * \param stringClass 
 * \param pRectanglePrevious 
 * \param stringSinkPrevious 
 * 
 * \return bool 
 */
bool FLDiagram::doInsert( const QString &stringClass, FLRectangle *pPrevious, DSink *pSink )
{
    QPoint              pointCellPrevious       = mapSceneToCell( pPrevious->getPosCenter() );  // work in cell coordinates
    QPoint              pointCellDestination    = pointCellPrevious;                            // destination cell 
    FLFlow *            pFlow                   = pPrevious->getFlow( pSink->objectName() );    // we will insert into this flow
    FLRectangle *       pInsertedFirst          = nullptr;                                      // will be (first) object inserted
    FLRectangle *       pBottom                 = nullptr;                                      // current next is the bottom
    CBD::DirectionTypes nDir                    = CBD::DirectionS;                              // default is adding to bottom
    bool                bExpandVertical         = false;                                        // default is not expand downward
    bool                bExpandHorizontal       = false;                                        // default is not expand Left/Right
    QSize               sizeCellsNeeded( 1, 2 );                                                // space needed including V spacing (but not H spacing)

    // determine size (in cells) needed
    if ( stringClass == "FLCondition" || stringClass == "Condition" || stringClass == "FLLoop" || stringClass == "Loop" )
        sizeCellsNeeded.setHeight( 4 );

    // determine; destination cell, H expansion, and bottom
    if ( pSink->objectName() == "Left" )
    {
        Q_ASSERT( pPrevious->inherits( "FLConditionOpen" ) );
        FLConditionOpen *pOpen = (FLConditionOpen*)pPrevious;
        // adding on left
        nDir = CBD::DirectionW;
        pBottom = pPrevious->getNext( nDir );
        if ( pBottom->inherits( "FLConditionClose" ) )
        {
            // expanding 
            int nCellStretch = pOpen->getStretch( nDir ) / getCell().width();
            pOpen->setStretch( nDir );
            if ( nCellStretch != 0 )
                pointCellDestination.setX( pointCellPrevious.x() + nCellStretch );
            else
                pointCellDestination.setX( pointCellPrevious.x() - 2 );
            bExpandHorizontal = true;
        }
        else
            // already expanded and possibly on a stretched elbow
            pointCellDestination.setX( mapSceneToCell( pBottom ->getPosCenter() ).x() );
    }
    else if ( pSink->objectName() == "Right" )
    {
        Q_ASSERT( pPrevious->inherits( "FLConditionOpen" ) );
        FLConditionOpen *pOpen = (FLConditionOpen*)pPrevious;
        // adding on left
        nDir = CBD::DirectionE;
        pBottom = pPrevious->getNext( nDir );
        if ( pBottom->inherits( "FLConditionClose" ) )
        {
            // expanding 
            int nCellStretch = pOpen->getStretch( nDir ) / getCell().width();
            pOpen->setStretch( nDir );
            if ( nCellStretch != 0 )
                pointCellDestination.setX( pointCellPrevious.x() + nCellStretch );
            else
                pointCellDestination.setX( pointCellPrevious.x() + 2 );
            bExpandHorizontal = true;
        }
        else
            // already expanded and possibly on a stretched elbow
            pointCellDestination.setX( mapSceneToCell( pBottom ->getPosCenter() ).x() );
    }
    else
    {
        Q_ASSERT( !pPrevious->inherits( "FLConditionOpen" ) );
        pBottom = pPrevious->getNext();
    }

    // adjust Y destination and determine V expansion
    // - Y = prev.y when bExpandVertical = true
    if ( (pBottom->getPosCenter().y() - pPrevious->getPosCenter().y()) > (getCell().height() * sizeCellsNeeded.height()) )
        pointCellDestination.setY( pointCellPrevious.y() + 2 );
    else
        bExpandVertical = true;

    // insert
    if ( stringClass == "FLCondition" || stringClass == "Condition" )
    { 
        // create new objects
        // set their pos AFTER connecting because moving sinks with sources will cause doRoute to be called

        FLConditionOpen *pConditionOpen     = (FLConditionOpen *)getObject( "FLConditionOpen" );
        FLConditionClose *pConditionClose   = (FLConditionClose *)getObject( "FLConditionClose" );

        pInsertedFirst = pConditionOpen;
        // pInsertedLast  = pConditionClose;

        // wire it all together 

        // Close -> next (reuse existing flow for this)
        pFlow->doDisconnect( CBD::EOLBegin );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pConditionClose->getSink( "Bottom" ) );

        // Open -> Close
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pConditionClose->getPosCenter() ); // pos in proximity to where we are using it - just for kicks
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pConditionOpen->getSink( "Left" ) );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pConditionClose->getSink( "Left" ) );

        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pConditionClose->getPosCenter() ); // pos in proximity to where we are using it - just for kicks
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pConditionOpen->getSink( "Right" ) );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pConditionClose->getSink( "Right" ) );

        // previous -> Open
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pConditionOpen->getPosCenter() ); // pos in proximity to where we are using it - just for kicks
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pSink );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pConditionOpen->getSink( "Top" ) );

        // set up a mutual-dependency between these two
        // - where one goes they both go... and all between
        pConditionOpen->setClose( pConditionClose );
        pConditionClose->setOpen( pConditionOpen );

        // set pos AFTER connecting will cause doRoute to get called on all of the connected flows
        pConditionOpen->setPos( mapCellToScene( pointCellDestination ) );
        if ( !bExpandVertical ) pConditionClose->setPos( mapCellToScene( QPoint( pointCellDestination.x(), pointCellDestination.y() + 2 ) ) );
        else pConditionClose->setPos( mapCellToScene( pointCellDestination ) );
    }
    else if ( stringClass == "FLLoop" || stringClass == "Loop" )
    { 
        // create new objects
        FLLoopOpen *   pLoopOpen  = (FLLoopOpen *)getObject( "FLLoopOpen" );
        FLLoopClose *  pLoopClose = (FLLoopClose *)getObject( "FLLoopClose" );

        pInsertedFirst = pLoopOpen; 
        // pInsertedLast  = pLoopClose; 

        // wire it all together

        // Close -> next (reuse existing flow for this)
        pFlow->doDisconnect( CBD::EOLBegin );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pLoopClose->getSink( "Bottom" ) );

        // Open -> Close
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pLoopClose->getPos() );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pLoopOpen->getSink( "Bottom" ) );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pLoopClose->getSink( "Top" ) );

        // previous -> Open
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pLoopOpen->getPos() );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pSink );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pLoopOpen->getSink( "Top" ) );

        pLoopOpen->setPos( mapCellToScene( pointCellDestination ) );
        if ( !bExpandVertical ) pLoopClose->setPos( mapCellToScene( QPoint( pointCellDestination.x(), pointCellDestination.y() + 2 ) ) );
        else pLoopClose->setPos( mapCellToScene( pointCellDestination ) );

        // set up a mutual-dependency between these two
        // - where one goes they both go... and all between
        pLoopOpen->setClose( pLoopClose );
        pLoopClose->setOpen( pLoopOpen );
    }
    else if ( stringClass == "FLStep" || stringClass == "Step" )
    {
        // create new object
        FLStep *pStep = (FLStep *)getObject( "FLStep" );

        pInsertedFirst = pStep; 
        // pInsertedLast  = pStep; 

        // wire it all together

        // step -> next (reuse existing flow for this)
        pFlow->doDisconnect( CBD::EOLBegin );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pStep->getSink( "Bottom" ) );

        // previous -> step
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pStep->getPos() );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pSink );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pStep->getSink( "Top" ) );

        pStep->setPos( mapCellToScene( pointCellDestination ) );
    }
    else if ( stringClass == "FLAsync" || stringClass == "Async" )
    {
        // create new object
        FLAsync *pStep = (FLAsync *)getObject( "FLAsync" );

        pInsertedFirst = pStep; 
        // pInsertedLast  = pStep; 

        // wire it all together

        // step -> next (reuse existing flow for this)
        pFlow->doDisconnect( CBD::EOLBegin );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pStep->getSink( "Bottom" ) );

        // previous -> step
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pStep->getPos() );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pSink );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pStep->getSink( "Top" ) );

        pStep->setPos( mapCellToScene( pointCellDestination ) );
    }
    else if ( stringClass == "FLSync" || stringClass == "Sync" )
    {
        // create new object
        FLSync *pStep = (FLSync *)getObject( "FLSync" );

        pInsertedFirst = pStep; 
        // pInsertedLast  = pStep; 

        // wire it all together

        // step -> next (reuse existing flow for this)
        pFlow->doDisconnect( CBD::EOLBegin );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pStep->getSink( "Bottom" ) );

        // previous -> step
        pFlow = (FLFlow *)getObject( "FLFlow" );
        pFlow->setPos( pStep->getPos() );
        pFlow->doConnect( pFlow->getSource( CBD::EOLBegin ), pSink );
        pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pStep->getSink( "Top" ) );

        pStep->setPos( mapCellToScene( pointCellDestination ) );
    }
    else
    {
       return false;
    }

    // make space
    if ( bExpandVertical )
    {
        FLLayout Layout( this );
        Layout.doExpandVertical( pPrevious, pInsertedFirst ); // - Y axis - move stuff down the Y axis until all have 1 empty cell between (may return before going all the way to End - a good thing)
    }
    if ( bExpandHorizontal )
    {
        FLLayout Layout( this );
        Layout.doLayoutHorizontal( pInsertedFirst ); // - X axis - move stuff Left/Right
    }
    doEnsurePages();

    return true;
}

bool FLDiagram::doInsert( FLFlow *pFlow, FLRectangle *pRectangleInsertFirst, FLRectangle *pRectangleInsertLast )
{
    // 'wire' it together...
    FLRectangle *pRectangleBeforeInsert = (FLRectangle*)pFlow->getRectangle( CBD::EOLBegin );
    FLRectangle *pRectangleAfterInsert  = (FLRectangle*)pFlow->getRectangle( CBD::EOLEnd );

    // get direction info (in case we are connecting to (and in) a condition open/close
    // top/open
    QString stringSinkTop = pFlow->getSource( CBD::EOLBegin )->getSink()->objectName();
    CBD::DirectionTypes nDirTop = CBD::DirectionUnknown;
    if ( stringSinkTop == "Left" ) nDirTop = CBD::DirectionW;
    else if ( stringSinkTop == "Right" ) nDirTop = CBD::DirectionE;
    // bottom/close
    QString stringSinkBottom = pFlow->getSource( CBD::EOLEnd )->getSink()->objectName();
    CBD::DirectionTypes nDirBottom = CBD::DirectionUnknown;
    if ( stringSinkBottom == "Left" ) nDirBottom = CBD::DirectionW;
    else if ( stringSinkBottom == "Right" ) nDirBottom = CBD::DirectionE;
    
    // connect top
    pFlow->doDisconnect( CBD::EOLEnd );
    pFlow->doConnect( pFlow->getSource( CBD::EOLEnd ), pRectangleInsertFirst->getSink( "Top" ) );

    // connect bottom
    FLFlow *pFlowNew = (FLFlow *)getObject( "FLFlow" );
    pFlowNew->setPos( pRectangleBeforeInsert->getPos() );
    pFlowNew->doConnect( pFlowNew->getSource( CBD::EOLBegin ), pRectangleInsertLast->getSink( "Bottom" ) );
    pFlowNew->doConnect( pFlowNew->getSource( CBD::EOLEnd ), pRectangleAfterInsert->getSink( stringSinkBottom ) );

    // Lets move the first paste object into position. Bring all of the other pasted objects along - maintaining
    // their relative positions.
    qreal   nInc = getCell().width() * 2; 
    QPointF pointDesired( pRectangleBeforeInsert->getPos().x(), pRectangleBeforeInsert->getPos().y() + nInc );
    if ( nDirTop != CBD::DirectionUnknown && nDirBottom != CBD::DirectionUnknown )
    { 
        Q_ASSERT(nDirTop == nDirBottom);
        if ( nDirTop == CBD::DirectionE ) pointDesired.setX( pointDesired.x() + nInc );
        else  pointDesired.setX( pointDesired.x() - nInc );
    }

    QPointF pointDelta = pointDesired - pRectangleInsertFirst->getPos();
    doMoveBy( pRectangleInsertFirst, pRectangleInsertLast, pointDelta );

    // do layout
    FLLayout Layout( this );

    // Vertical
    // we only expand 'down'
    Layout.doExpandVertical( pRectangleBeforeInsert, pRectangleInsertFirst );

    // Horizontal
    // we only expand when first content added to dir side of condition 
    // OR when we inserted content wider then 1 cell 
    // if ( pRectangleBeforeInsert->inherits( "FLConditionOpen" ) && pRectangleAfterInsert == ((FLConditionOpen*)pRectangleBeforeInsert)->getClose() )
    Layout.doLayoutHorizontal( pRectangleInsertFirst );

    doEnsurePages();

    return true;
}

bool FLDiagram::doLoad( QDomElement *pdomElemModel )                               
{
    slotSelectAll();
    pSelectionManager->doDelete();
    return DDiagram::doLoad( pdomElemModel );
}

/*!
 * \brief Intercept paste so we can 'wire in' pasted objects.
 * 
 * \author pharvey (4/16/20)
 */
void FLDiagram::doPostLoad()                                                          
{
    // we only care about paste...
    if ( !g_Transaction->canPaste() ) return DDiagram::doPostLoad();

    // canPaste should have been called and it would have set the target for paste
    Q_ASSERT(pFlowPaste);

    // do the default...
    DDiagram::doPostLoad();

    // insert into flow 
    FLRectangle *pFirst = nullptr;
    FLRectangle *pLast  = nullptr;

    QList<QString> listPasted = g_Transaction->mapXRef.values();                 
    QString stringPath;                                                          
    foreach( stringPath, listPasted )                                            
    {                                                                            
        ADObject *p = g_Transaction->pRoot->getObject( stringPath.split( ',' ) );
        Q_ASSERT(p);

        if ( p->inherits("FLConditionOpen") )                                            
        {
            FLRectangle *pRectangle = (FLRectangle*)p;
            if ( !pRectangle->getSink( "Top" )->getSources().count() )
            {
                Q_ASSERT(!pFirst);
                pFirst = pRectangle;
                continue;
            }
        }                                                                        
        else if ( p->inherits("FLConditionClose") )                                            
        {
            FLRectangle *pRectangle = (FLRectangle*)p;
            if ( !pRectangle->getSink( "Bottom" )->getSources().count() )
            {
                Q_ASSERT(!pLast);
                pLast = pRectangle;
                continue;
            }
        }                                                                        
        else if ( p->inherits("FLRectangle") )                                            
        {
            FLRectangle *pRectangle = (FLRectangle*)p;
            if ( !pRectangle->getSink( "Top" )->getSources().count() )
            {
                Q_ASSERT(!pFirst);
                pFirst = pRectangle;
            }
            if ( !pRectangle->getSink( "Bottom" )->getSources().count() )
            {
                Q_ASSERT(!pLast);
                pLast = pRectangle;
            }
        }                                                                        
    }                                                                            

    Q_ASSERT(pFirst);
    Q_ASSERT(pLast);

    doInsert( pFlowPaste, pFirst, pLast );

    pFlowPaste = nullptr;
}                                                                                    


void FLDiagram::doClear()
{
    pBegin          = nullptr;
    pEnd            = nullptr;
    pFlowDragOver   = nullptr;
    pFlowPaste      = nullptr;
    // bSnapGrid       = true;
    pSelectionStart = nullptr;
    pSelectionFinish= nullptr;

    DDiagram::doClear();
}

bool FLDiagram::canPaste()
{
    if ( !DDiagram::canPaste() ) return false;

    QList<ADObject *> l = pSelectionManager->getSelected();

    if ( l.count() < 1 )
    { 
        doMessageBox( "WARNING", tr("Paste"), tr("Please select a Flow.") ); 
        return false;
    }

    if ( l.count() > 1 )
    { 
        doMessageBox( "WARNING", tr("Paste"), tr("Too many selected objects. Please select exactly one Flow.") ); 
        return false;
    }

    ADObject *p = l.last();
    if ( !p->inherits( "FLFlow" ) )
    { 
        doMessageBox( "WARNING", tr("Paste"), tr("Please select exactly one Flow.") ); 
        return false;
    }

     pFlowPaste = (FLFlow*)p;

    return true;
}

/*!
 * \brief Check if selection is valid (for cut/delete). 
 *  
 * Unlike a generic diagram - we can not delete/copy any set of selected objects. 
 * We must maintain continuity. This means we can not do such things as; 
 *  
 * - break the dependency between Open/Close of a Condition/Loop 
 * - leave a diagram without a FLBegin/FLEnd 
 *  
 * For simplicity (for developer) we only allow contigious objects. 
 *  
 * For simplicity (for the User) Flows between selected objects are always assumed to be selected. 
 *  
 * For simplicity (for the User) we allow an Open to imply all objects to, and including, its Close. This 
 * is only valid if the Open object is the only object selected. 
 * 
 * \author pharvey (4/1/20)
 * 
 * \return bool 
 */
bool FLDiagram::isSelectionValid()
{
    pSelectionStart = nullptr;
    pSelectionFinish= nullptr;

    // get selection
    // - unselect all FLFlow's because we want to focus on FLRectangle's
    QList<FLRectangle*> listSelection;
    {
        QList<ADObject*> l = pSelectionManager->getSelected();
        ADObject *p;
        foreach( p, l )
        {
            if ( p->inherits( "FLRectangle" ) )
                listSelection.append( (FLRectangle*)p );
            else
                getSelectionManager()->setSelected( p, false );
        }
    }

    if ( listSelection.count() < 1 )
    {
        doMessageBox( "WARNING", tr("Selection Validate"), tr("No viable selection.") ); 
        return false; 
    }

    // ensure that we have complete Open/Close pairs
    // ensure that selection does NOT include FLBegin/FLEnd
    FLRectangle *pRectangle;
    foreach( pRectangle, listSelection )
    {
        if ( pRectangle->inherits( "FLConditionOpen" ) )
        { 
            FLConditionOpen *p = (FLConditionOpen*)pRectangle;
            if ( !listSelection.contains( p->getClose() ) )
            { 
                doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete incomplete Condition.") ); 
                return false;
            }
        }
        else if ( pRectangle->inherits( "FLConditionClose" ) )
        { 
            FLConditionClose *p = (FLConditionClose*)pRectangle;
            if ( !listSelection.contains( p->getOpen() ) )
            { 
                doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete incomplete Condition.") ); 
                return false;
            }
        }
        else if ( pRectangle->inherits( "FLLoopOpen" ) )
        { 
            FLLoopOpen *p = (FLLoopOpen*)pRectangle;
            if ( !listSelection.contains( p->getClose() ) )
            { 
                doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete incomplete Loop.") ); 
                return false;
            }
        }
        else if ( pRectangle->inherits( "FLLoopClose" ) )
        { 
            FLLoopClose *p = (FLLoopClose*)pRectangle;
            if ( !listSelection.contains( p->getOpen() ) )
            { 
                doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete incomplete Loop.") ); 
                return false;
            }
        }
        else if ( pRectangle->inherits( "FLBegin" ) )
        { 
            doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete Begin.") ); 
            return false;
        }
        else if ( pRectangle->inherits( "FLEnd" ) )
        { 
            doMessageBox( "WARNING", tr("Selection Validate"), tr("Can not cut/delete End.") ); 
            return false;
        }
    }

    // ensure that we are contigious
    // - we want exactly 1 start and 1 finish object
    // - select lines between selected rectangles
    FLRectangle *pStart = nullptr;
    FLRectangle *pFinish = nullptr;
    foreach( pRectangle, listSelection )
    {
        if ( pRectangle->inherits( "FLConditionOpen" ) )
        {
            FLConditionOpen *p = (FLConditionOpen*)pRectangle;
            FLFlow *pFlow;
            DRectangle *p2;

            // Start
            pFlow = p->getFlow( "Top" ); 
            p2 = pFlow->getRectangle( CBD::EOLBegin );
            if ( p2->isSelected() )
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pStart )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one start.") ); 
                    return false; 
                }
                pStart = pRectangle;
            }

            // Finish - Left
            pFlow = p->getFlow( "Left" ); 
            p2 = pFlow->getRectangle( CBD::EOLEnd );
            if ( p2->isSelected() )
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pFinish )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one finish.") ); 
                    return false; 
                }
                pFinish = pRectangle;
            }

            // Finish - Right
            pFlow = p->getFlow( "Right" ); 
            p2 = pFlow->getRectangle( CBD::EOLEnd );
            if ( p2->isSelected() )
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pFinish )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one finish.") ); 
                    return false; 
                }
                pFinish = pRectangle;
            }

            getSelectionManager()->setSelected( p->getFlow( "Left" ) ); 
            getSelectionManager()->setSelected( p->getFlow( "Right" ) ); 
        }
        else if ( pRectangle->inherits( "FLConditionClose" ) )
        {
            FLConditionClose *p = (FLConditionClose*)pRectangle;
            FLFlow *pFlow;
            DRectangle *p2;

            // Start - we have already verified that it has an Open 
            // so this can never be a Start
            // Also; we already ensured that the Left/Right flows are selected.

            // Finish
            pFlow = p->getFlow( "Bottom" ); 
            p2 = pFlow->getRectangle( CBD::EOLEnd );
            if ( p2->isSelected() ) 
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pFinish )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one finish.") ); 
                    return false; 
                }
                pFinish = pRectangle;
            }
        }
        else if ( pRectangle->inherits( "FLRectangle" ) )
        {
            FLRectangle *p = (FLRectangle*)pRectangle;
            FLFlow *pFlow;
            DRectangle *p2;

            // start
            pFlow = p->getFlow( "Top" ); 
            p2 = pFlow->getRectangle( CBD::EOLBegin );
            if ( p2->isSelected() )
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pStart )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one start.") ); 
                    return false; 
                }
                pStart = pRectangle;
            }

            // finish
            pFlow = p->getFlow( "Bottom" ); 
            p2 = pFlow->getRectangle( CBD::EOLEnd );
            if ( p2->isSelected() )
                getSelectionManager()->setSelected( pFlow );
            else
            {
                if ( pFinish )
                {
                    doMessageBox( "WARNING", tr("Selection Validate"), tr("More than one finish.") ); 
                    return false; 
                }
                pFinish = pRectangle;
            }
        }
    }

    if ( !pStart )
    {
        doMessageBox( "WARNING", tr("Selection Validate"), tr("Could not determine start.") ); 
        return false;
    }

    if ( !pFinish )
    {
        doMessageBox( "WARNING", tr("Selection Validate"), tr("Could not determine finish.") ); 
        return false;
    }

    pSelectionStart = pStart;
    pSelectionFinish= pFinish;

    return true;
}

