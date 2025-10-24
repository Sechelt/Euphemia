/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "DRDraw.h"

#include <CBDMath.h>
#include <CBDPersist.h>
#include <SSettings.h>
#include <LGraphicsObject.h>
#include <LGraphicsLayer.h>
#include <LGraphicsScene.h>

// #define DDRAW_DEBUG_DRAW 1

DRDraw::DRDraw( LFile *pParent, const QString &stringName )
    : LLayer( pParent, stringName )
{
    Q_ASSERT( pParent );
    // here is what getObject() can create
    mapMeta.clear();
    mapMeta.insert( "DRArc", ADMeta( QIcon( ":DR/Arc" ), "DRArc", "Arc" ) );
    mapMeta.insert( "DRChord", ADMeta( QIcon( ":DR/Chord" ), "DRChord", "Chord" ) );
    mapMeta.insert( "DRImage", ADMeta( QIcon( ":DR/Image" ), "DRImage", "Image" ) );
    mapMeta.insert( "DREllipse", ADMeta( QIcon( ":DR/Ellipse" ), "DREllipse", "Ellipse" ) );
    mapMeta.insert( "DRLine", ADMeta( QIcon( ":DR/Line" ), "DRLine", "Line" ) );
    mapMeta.insert( "DRPie", ADMeta( QIcon( ":DR/Pie" ), "DRPie", "Pie" ) );
    mapMeta.insert( "DRPolygon", ADMeta( QIcon( ":DR/Polygon" ), "DRPolygon", "Polygon" ) );
    mapMeta.insert( "DRPolyline", ADMeta( QIcon( ":DR/Polyline" ), "DRPolyline", "Polyline" ) );
    mapMeta.insert( "DRRectangle", ADMeta( QIcon( ":DR/Rectangle" ), "DRRectangle", "Rectangle" ) );
    mapMeta.insert( "DRText", ADMeta( QIcon( ":PA/Text" ), "DRText", "Text" ) );
}

void DRDraw::setImage( const QImage &i )
{
    // sanity check
    if ( canCancel() ) doCancel();
    image = i;
    // caller (presumably scene) needs to set an appropriate scene rect - we do not do it here
    setModified();
}

void DRDraw::setTool( int n, bool bCancelDrawing )
{
    if ( n == nTool ) return;
    if ( bCancelDrawing )
    {
        if ( canCancel() ) doCancel();
        else if ( canCommit() ) doCommit();
        if ( hasSelection() ) doSelectNone2();
    }

    nTool = (Tools)n;

    switch ( nTool )
    {
        case ToolSelectRectangle:
            nMouseEventState = MouseEventStateSelectRectangle;
            break;
        case ToolSelectPolygon:
            nMouseEventState = MouseEventStateSelectPolygon;
            break;
        case ToolManipulate:
            nMouseEventState = MouseEventStateManipulate;
            break;
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawPolygon:
        case ToolDrawPolyline:
        case ToolDrawText:
            nMouseEventState = MouseEventStateCreate;
            break;
    }

    emit signalChangedTool();
}

QPixmap DRDraw::getIcon()
{
    return QPixmap( ":DR/Draw" );
}

QVector<LTool> DRDraw::getTools()
{
    QVector<LTool> vectorReturn;

    vectorReturn.append( LTool( ToolSelectRectangle, tr( "select using a rectangle" ), QPixmap( ":DR/SelectRectangle" ), tr( "select using a rectangle" ) ) );
    vectorReturn.append( LTool( ToolSelectPolygon, tr( "select using a polygon" ), QPixmap( ":DR/SelectPolygon" ), tr( "select using a polygon" ) ) );
    vectorReturn.append( LTool( ToolManipulate, tr( "select and manipulate" ), QPixmap( ":DR/Manipulate" ), tr( "select and manipulate" ) ) );
    vectorReturn.append( LTool( ToolDrawArc, tr( "arc" ), QPixmap( ":DR/Arc" ), tr( "arc" ) ) );
    vectorReturn.append( LTool( ToolDrawChord, tr( "chord" ), QPixmap( ":DR/Chord" ), tr( "chord" ) ) );
    vectorReturn.append( LTool( ToolDrawImage, tr( "image" ), QPixmap( ":DR/Image" ), tr( "image" ) ) );
    vectorReturn.append( LTool( ToolDrawPie, tr( "pie" ), QPixmap( ":DR/Pie" ), tr( "pie" ) ) );
    vectorReturn.append( LTool( ToolDrawLine, tr( "line" ), QPixmap( ":DR/Line" ), tr( "line" ) ) );
    vectorReturn.append( LTool( ToolDrawRectangle, tr( "rectangle" ), QPixmap( ":DR/Rectangle" ), tr( "rectangle" ) ) );
    vectorReturn.append( LTool( ToolDrawEllipse, tr( "ellipse" ), QPixmap( ":DR/Ellipse" ), tr( "ellipse" ) ) );
    vectorReturn.append( LTool( ToolDrawPolygon, tr( "polygon" ), QPixmap( ":DR/Polygon" ), tr( "polygon" ) ) );
    vectorReturn.append( LTool( ToolDrawPolyline, tr( "polyline" ), QPixmap( ":DR/Polyline" ), tr( "polyline" ) ) );
    vectorReturn.append( LTool( ToolDrawText, tr( "text" ), QPixmap( ":DR/Text" ), tr( "text" ) ) );

    return vectorReturn;
}

QImage DRDraw::getImage()
{
    QImage image( pGraphics->boundingRect().size().toSize(), QImage::Format_ARGB32 );
    QPainter painter( &image );
    
    QList<QGraphicsItem *>  listItems = pGraphics->childItems();

    QStyleOptionGraphicsItem options;
    foreach( QGraphicsItem * pItem, listItems )
    {
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;

        pObject->paint( &painter, &options );
    }

    return image;                 
}

QDomDocument DRDraw::getCopyXML()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return QDomDocument();
}

QImage DRDraw::getCopyImage()
{
    if ( !pObject ) return QImage();
    return pObject->getImage();
}

QWidget* DRDraw::getToolBar( QWidget *pParent )
{
    switch ( nTool )
    {
        case ToolSelectRectangle:
        case ToolSelectPolygon:
        case ToolManipulate:
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
            break;
        case ToolDrawRectangle:
            return g_SSettings->getSetting( "LSRectangle" )->getToolBar( pParent );
        case ToolDrawEllipse:
            return g_SSettings->getSetting( "LSEllipse" )->getToolBar( pParent );
        case ToolDrawPolygon:
            break;
        case ToolDrawPolyline:
            break;
        case ToolDrawText:
            return g_SSettings->getSetting( "LSText" )->getToolBar( pParent );
    }
    return nullptr;
}

QWidget* DRDraw::getPanel( QWidget *pParent )
{
    switch ( nTool )
    {
        case ToolSelectRectangle:
        case ToolSelectPolygon:
        case ToolManipulate:
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
            break;
        case ToolDrawRectangle:
            return g_SSettings->getSetting( "LSRectangle" )->getPanel( pParent );
        case ToolDrawEllipse:
            return g_SSettings->getSetting( "LSEllipse" )->getPanel( pParent );
        case ToolDrawPolygon:
            break;
        case ToolDrawPolyline:
            break;
        case ToolDrawText:
            return g_SSettings->getSetting( "LSText" )->getPanel( pParent );
    }
    return nullptr;
}

ADObject* DRDraw::getObject( const QString &s, ADObject * )
{
    DRObject *pObject = nullptr;
    QString   stringClass     = getClass( s );

    if ( stringClass == "DRArc" )
    {
        pObject = new DRArc( this );
    }
    else if ( stringClass == "DRChord" )
    {
        pObject = new DRChord( this );
    }
    else if ( stringClass == "DRImage" )
    {
        pObject = new DRImage( this );
    }
    else if ( stringClass == "DRPie" )
    {
        pObject = new DRPie( this );
    }
    else if ( stringClass == "DRLine" )
    {
        pObject = new DRLine( this );
    }
    else if ( stringClass == "DRRectangle" )
    {
        pObject = new DRRectangle( this );
    }
    else if ( stringClass == "DREllipse" )
    {
        pObject = new DREllipse( this );
    }
    else if ( stringClass == "DRPolygon" )
    {
        pObject = new DRPolygon( this );
    }
    else if ( stringClass == "DRPolyline" )
    {
        pObject = new DRPolyline( this );
    }
    else if ( stringClass == "DRText" )
    {
        pObject = new DRText( this );
    }

    if ( !pObject )
    {
        QMessageBox::warning( nullptr, tr( "Create Object..." ), tr( "Unknown class requested " ) + s );
        return nullptr;
    }

    if ( pGraphics ) pObject->doGraphicsCreate( pGraphics );

    // ensure we are the OID source regardless of parent (avoids messing up OID's when reparenting object's)
    pObject->setOIDSource( this );

    if ( g_Transaction->getState() == ADTransaction::TransactionPaste )
    {
        setSelected( pObject );
    }

    // announce the birth of a new child
    emit signalCreated( pObject );
    // set parent modified by default (can be set back if this is a load)
    setModified();

    // the caller should doConnect to a model ASAP
    return pObject;
}

QString DRDraw::getToolString()
{
    switch ( nTool ) 
    {
        case ToolSelectRectangle:
            return "ToolSelectRectangle";
        case ToolSelectPolygon: 
            return "ToolSelectPolygon";
        case ToolManipulate:
            return "ToolManipulate";
        case ToolDrawArc:
            return "ToolDrawArc";
        case ToolDrawChord:
            return "ToolDrawChord";
        case ToolDrawImage:
            return "ToolDrawImage";
        case ToolDrawPie:
            return "ToolDrawPie";
        case ToolDrawLine:
            return "ToolDrawLine";
        case ToolDrawRectangle:
            return "ToolDrawRectangle";
        case ToolDrawEllipse:
            return "ToolDrawEllipse";
        case ToolDrawPolygon:
            return "ToolDrawPolygon";
        case ToolDrawPolyline:
            return "ToolDrawPolyline";
        case ToolDrawText:
            return "ToolDrawText";
    }
    return "unknown";
}

QString DRDraw::getMouseEventStateString()
{
    switch (nMouseEventState ) 
    {
        case MouseEventStateSelectRectangle:
            return "MouseEventStateSelectRectangle";
        case MouseEventStateSelectPolygon:
            return "MouseEventStateSelectPolygon";
        case MouseEventStateManipulate:
            return "MouseEventStateManipulate";
        case MouseEventStateCreate:
            return "MouseEventStateCreate";
    }
    return "unknown";
}

QRectF DRDraw::getSelectedBoundingRect()
{
    QRectF r;
    bool bFirst = true;
    foreach( DRObject *pObject, vectorSelected )
    {
        if ( pObject->isSelector() ) continue;
        if ( bFirst ) 
        {
            r = pObject->getRect();
            bFirst = false;
            continue;
        }
        r = r.united( pObject->getRect() );
    }

    return r;
}

/*!
 * \brief Mouse press event. 
 *  
 * Not a lot is done here because we do not know if we are a Press + Release or 
 * a Press + Move + Release. The \sa doMoveEvent has most of the logic.
 *  
 * \author pharvey (3/27/23)
 * 
 * \param pEvent 
 */
void DRDraw::doMousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif

    /****************************************** 
     * Off-Scene 
     * - happens when outside of scene but within view 
     * - we will ignore all Move and double-click events
     * - tracking can recover by bringing the mouse back to the scene and clicking or double clicking
     ******************************************/
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) )
    {
        if ( !bTracking ) pointPress = QPointF();
#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
        return;
    }

    switch ( nMouseEventState )
    {
        case MouseEventStateSelectRectangle:
            doMousePressEventSelectRectangle( pEvent );
            break;
        case MouseEventStateSelectPolygon:
            doMousePressEventSelectPolygon( pEvent );
            break;
        case MouseEventStateManipulate:
            doMousePressEventManipulate( pEvent );
            break;
        case MouseEventStateCreate:
            doMousePressEventCreate( pEvent );
            break;
    }
#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
}

/*!
 * \brief Mouse move event. 
 *  
 * Here we determine the intent of the Press and then act accordingly. 
 * Mostly; this results in moving a handle.
 *  
 * This can mean; 
 *  
 * - adding points to a polygon to initialize it (tracking) 
 * - creating a new object (and initalizing it by moving one of its handles)
 * - moving a handle on a selected object 
 * 
 * \author pharvey (3/29/23)
 * 
 * \param pEvent 
 */
void DRDraw::doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );
    /****************************************** 
     * Off-Scene
     * - was press event outside of scene
     ******************************************/
    if ( pointPress.isNull() ) return;

    // ignore invalid move
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;
    pointCurrent = pEvent->scenePos(); // we need a valid point for doRelease even when release Off-Scene

    switch ( nMouseEventState )
    {
        case MouseEventStateSelectRectangle:
            return doMouseMoveEventSelectRectangle( pEvent );
        case MouseEventStateSelectPolygon:
            return doMouseMoveEventSelectPolygon( pEvent );
        case MouseEventStateManipulate:
            return doMouseMoveEventManipulate( pEvent );
        case MouseEventStateCreate:
            return doMouseMoveEventCreate( pEvent );
    }
}

/*!
 * \brief Mouse release event.
 *  
 * This can result in; 
 *  
 * - a selected object being unselected 
 * - a different object being selected (Press + Release) 
 * - selection state being advanced 
 * - 1-n objects being selected (Press + Move + Release with a selector object) 
 *  
 * \author pharvey (3/29/23)
 * 
 * \param pEvent 
 */
void DRDraw::doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
    /****************************************** 
     * Press (off-scene)
     ******************************************/
    if ( pointPress.isNull() ) 
    {
#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
        return;
    }
    if ( pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) pointCurrent = pEvent->scenePos();

    switch ( nMouseEventState )
    {
        case MouseEventStateSelectRectangle:
            doMouseReleaseEventSelectRectangle( pEvent );
            break;
        case MouseEventStateSelectPolygon:
            doMouseReleaseEventSelectPolygon( pEvent );
            break;
        case MouseEventStateManipulate:
            doMouseReleaseEventManipulate( pEvent );
            break;
        case MouseEventStateCreate:
            doMouseReleaseEventCreate( pEvent );
            break;
    }
#ifdef DDRAW_DEBUG_DRAW 
    qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
    if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
}

void DRDraw::doMousePressEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    Q_ASSERT( !bTracking );
    Q_ASSERT( !bMoving );
    Q_ASSERT( !pObject );
    Q_ASSERT( !pHandle );

    doSelectNone2();
    pointCurrent = pointPress = pEvent->scenePos();
    pObject = getObject( ToolSelectRectangle, pointPress );
    Q_ASSERT( pObject );
    pObject->setSelected(); // select WITHOUT adding to vectorSelected
    pHandle = pObject->getGrab( pointPress, true );
    Q_ASSERT( pHandle );
    bDrawing = true;
    bMoving = true;
}

void DRDraw::doMouseMoveEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    Q_ASSERT( !bTracking );
    Q_ASSERT( pObject );
    Q_ASSERT( pHandle );

    // have we moved
    if ( pointCurrent == pointPress ) return;

    pObject->doMove( pHandle, pointCurrent );
}

void DRDraw::doMouseReleaseEventSelectRectangle( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    Q_ASSERT( !bTracking );
    Q_ASSERT( pHandle );
    Q_ASSERT( bMoving );

    // wrong mouse button but that info is lost so we check this way 
    if ( !pObject ) return;

    pObject->doDrop( pHandle, pointCurrent );
    QRectF r = pObject->getRect();
    pHandle = nullptr;
    bDrawing = false;
    bMoving = false;

    // select all in rect to SelectionStateMinimal (this will include pObject)
    doSelectAll( r );       
    // was anything other than self selected?
    if ( vectorSelected.count() <= 1 ) 
    {
        doCancel();
        // get rid of the selection object
        // doSelectNone();
        // slotDelete( pObject );
        // pObject = nullptr;
    }
    else
    {
        // select pObject back to SelectionStateStandard
        setSelected( pObject, DRObject::SelectionStateStandard ); // selected self in doSelectAll but switch to standard handles
        // shrink selection rectangle to just what we need
        DRSelectRectangle *pSelectRectangle = (DRSelectRectangle *)pObject;
        pSelectRectangle->setRect( getSelectedBoundingRect() );       
    }

    setTool( ToolManipulate, false );
}

void DRDraw::doMousePressEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( 1 == 2 );

    if ( bTracking )
    {
        Q_ASSERT( pObject->inherits( "DRSelectPolygon" ) );
        ((DRSelectPolygon *)pObject)->doPress( pEvent->scenePos() );
        return;
    }

    pointCurrent = pointPress = pEvent->scenePos();
}

void DRDraw::doMouseMoveEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( bTracking );
    if ( bTracking )
    {
        Q_ASSERT( pObject->inherits( "DRPolyline" ) );
        ((DRPolyline *)pObject)->doMove( pEvent->scenePos() );
        return;
    }
}

void DRDraw::doMouseReleaseEventSelectPolygon( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    Q_ASSERT( !bTracking );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif

    // did not move (press/release only)?
    if ( !bMoving )
    {
        return;
    }

    bMoving = false;
}

void DRDraw::doMousePressEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    Q_ASSERT( !bTracking );
    Q_ASSERT( !pHandle );

    pointCurrent = pointPress = pEvent->scenePos();

    //
    // WHAT DID WE CLICK ON?
    //
    DRObject *pObjectClicked = nullptr;
    LHandle * pHandleClicked = nullptr;

    // Handles can sometimes be outside of the shapes bounding rect. So lets start by going after a handle...
    pHandleClicked = getHandle( pointPress );
    if ( pHandleClicked )
    {
        // Get the object the handle is working with...
        pObjectClicked = dynamic_cast<DRObject *>( pHandleClicked->getLayerObject() );
        Q_ASSERT( pObjectClicked );

    }
    else
    {
        // No handle so lets see if we clicked on an object...
        pObjectClicked = getObject( pointPress );
    }

    //
    // CLICKED ON OBJECT ALREADY SELECTED?
    //
    if ( pObject && pObject == pObjectClicked ) 
    {
        pHandle = pHandleClicked; // may be nullptr
        // any selection state cycle will happen in mouse release
        return;
    }
    else if ( pObject ) 
    {
        if ( canCommit() ) doCommit();
        if ( hasSelection() ) doSelectNone2();
    }

    //
    // CLICKED ON NOTHING
    //
    if ( !pObjectClicked ) return; 

    //
    // CLICKED ON AN OBJECT (not already selected)
    //
    pObject = pObjectClicked;
    pHandle = pHandleClicked;   // may be nullptr
    // setSelected( pObject );
}

void DRDraw::doMouseMoveEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;
    if ( !pHandle ) return;
    Q_ASSERT( pObject );
    
    // manipulate self and selected if isSelector   
    pObject->doMove( pHandle, pointCurrent );
}

void DRDraw::doMouseReleaseEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    // Handle - so we were probably moving it...
    if ( pHandle )
    {
        Q_ASSERT( pObject );
        // manipulate self and selected if isSelector   
        pObject->doDrop( pHandle, pointCurrent );
        setModified();
        pHandle = nullptr;
        return;
    }

    // No object? Then we are done here.
    if ( !pObject ) return;

    // no handle but we could be changing selection of an object...
    if ( pObject->isSelected() )
    {
        // setSelected( pObject, false );
        pObject->doSelectionStateCycle();
    }
    else
    {
        setSelected( pObject );
    }

    return;
}

void DRDraw::doMousePressEventCreate( QGraphicsSceneMouseEvent *pEvent )
{
    /****************************************** 
     * Tracking
     ******************************************/
    if ( bTracking )
    {
        Q_ASSERT( pObject->inherits( "DRPolyline" ) );
        ((DRPolyline *)pObject)->doPress( pEvent->scenePos() );
        return;
    }

    /****************************************** 
     * Prep For Move/Release
     ******************************************/
    Q_ASSERT( !bMoving );
    Q_ASSERT( !pHandle );
    pointCurrent = pointPress = pEvent->scenePos();
}

void DRDraw::doMouseMoveEventCreate( QGraphicsSceneMouseEvent *pEvent )
{
    /****************************************** 
     * Tracking
     ******************************************/
    if ( bTracking )
    {
        Q_ASSERT( pObject->inherits( "DRPolyline" ) );
        ((DRPolyline *)pObject)->doMove( pEvent->scenePos() );
        return;
    }

    // NOTE: tracking       - buttons() will return last button pressed even when released
    //       no tracking    - buttons() will return button currently pressed (if any)
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    /****************************************** 
     * Init Move 
     * - we want a handle to move 
     * - may create a new object to get a handle
     ******************************************/
    if ( !bMoving )
    {
#ifdef DDRAW_DEBUG_DRAW 
        qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
        if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
        // have object? try to grab one of its handles
        if ( pObject ) pHandle = pObject->getGrab( pointPress );
        // no handle? try a different object
        if ( !pHandle )
        {
            // have object? stop using it
            if ( hasSelection() ) doSelectNone2();
            //
            // create new object
            //
            pObject = getObject( nTool, pointPress );
            bDrawing = true;
            Q_ASSERT( pObject );
            setSelected( pObject );
            // polyline based objects are initialized with tracking (and no handles until done)
            if ( pObject->inherits( "DRPolyline" ) )
            {
                bTracking = true;
                ((DRPolyline *)pObject)->doMove( pEvent->scenePos() );
                bMoving = true; // needed for doReleaseEvent that happens after doDoubleClickEvent
                return;
            }
            // get the sizing handle
            pHandle = pObject->getGrab( pointPress, true );
#ifdef DDRAW_DEBUG_DRAW 
            qInfo() << "\n[INFO]" << __FILE__ << __FUNCTION__ << __LINE__ << "\nnTool=" << getToolString() << "\nnMouseEventState=" << getMouseEventStateString() << "\nbMoving=" << bMoving << "\nbTracking=" << bTracking << "\nbDrawing=" << bDrawing << "\npObject=" << pObject << "\npHandle=" << pHandle << "\nvectorSelected=" << vectorSelected.count();
            if ( pObject ) qInfo() << "SelectionState=" << pObject->getSelectionStateString();
#endif
            Q_ASSERT( pHandle );
        }
        bMoving = true;
    }

    /****************************************** 
     * Move (handle)
     ******************************************/
    if ( pHandle ) pObject->doMove( pHandle, pEvent->scenePos() );
}

void DRDraw::doMouseReleaseEventCreate( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    /****************************************** 
     * Tracking
     ******************************************/
    if ( bTracking ) return; // created new point in Press - nothing to do here

    /****************************************** 
     * Press + Move (a handle)
     ******************************************/
    if ( bMoving )
    {
        // in some cases - doDrop will result in the handle being deleted by the object
        if ( pHandle )
        {
            pObject->doDrop( pHandle, pointCurrent );
            setModified();
        }
        pHandle = nullptr;
        bMoving = false;
        setTool( ToolManipulate, false );
        return;
    }
}


void DRDraw::doDragEnterEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );
    Q_ASSERT( vectorSelected.count() > 1 );
}

void DRDraw::doDragMoveEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );

}

void DRDraw::doDropEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );

}

void DRDraw::doContextMenuEvent( QGraphicsSceneContextMenuEvent * )
{
    Q_ASSERT( pGraphics );

}

void DRDraw::doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );
    // NOTE: a double-click results in; doPressEvent + doDoubleClickEvent + doReleaseEvent

    /****************************************** 
     * Original Press Was Off-Scene
     ******************************************/
    if ( pointPress.isNull() ) return;

    // double-click is Off-Scene - ignore
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;

    /****************************************** 
     * Tracking - lets end it
     ******************************************/
    if ( bTracking )
    {
        // initializing done - adopt same logic as other DRObject's for further manipulation
        // which is all about moving handles
        ((DRPolyline *)pObject)->doDoubleClick( pEvent->scenePos() );
        bTracking = false;
        pHandle = nullptr;
        setModified();
        return;
    }
    // doReleaseEvent gets called next
}

QDomElement DRDraw::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = LLayer::doSave( pdomDoc, pdomElemParent );

    return domElemThis;
}

bool DRDraw::doLoad( QDomElement *pdomElemObject )
{
    LLayer::doLoad( pdomElemObject );

    return true;
}

void DRDraw::doCut()
{
    doCopy();
    doDelete();
}

void DRDraw::doCopy()
{
    Q_ASSERT( !isDrawing() );
    Q_ASSERT( hasSelection() );

    // ini transaction
    g_Transaction->initSave( this );

    // create an XML document with class name and document version...
    QDomDocument domDoc( metaObject()->className() );
    QDomElement domElementRoot = domDoc.createElement( metaObject()->className() );
    domElementRoot.setAttribute( "DocVer", CBD_DOC_VER );

    domDoc.appendChild( domElementRoot );

    // save selected
    foreach( DRObject *pObject, vectorSelected )
    {
        pObject->doSave( &domDoc, &domElementRoot );
    }

    // write
    QString stringXML = domDoc.toString();

    // fini transaction
    g_Transaction->fini();

    QGuiApplication::clipboard()->setText( stringXML );
}

void DRDraw::doPaste()
{
    if ( canCancel() ) 
    {
        doCancel();
    }
    else if ( canCommit() ) 
    {
        doCommit();
    }

    if ( hasSelection() ) 
    {
        doSelectNone2();
    }

    QClipboard *pClipboard = QGuiApplication::clipboard();
    const QMimeData *pMimeData = pClipboard->mimeData();

    if ( pMimeData->hasText() )
    {
        QString stringText = QGuiApplication::clipboard()->text();

        // read file into XML document...
        QDomDocument domDoc( metaObject()->className() );

        // if ( !domDoc.setContent( QAnyStringView( stringText ) ) )
        if ( !domDoc.setContent( stringText ) )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Invalid format.\nCould not create an XML document from paste buffer.\n\n") );
            // doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
            return;
        }

        // validate XML...
        // Root element is only used to check that class in XML matches this class.
        QDomElement domElemRoot = domDoc.documentElement();
        if ( domElemRoot.isNull() )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Invalid format.\nCould not find a document element.\n\n") );
            return;
        }
        if ( domElemRoot.tagName() != metaObject()->className() )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Invalid format.\nDoes not appear to be a ") + QString( metaObject()->className() ) + "." );
            return;
        }
        if ( domElemRoot.attribute( "DocVer" ) != CBD_DOC_VER ) 
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Invalid format.\nXML is not Version ") + CBD_DOC_VER );
            return;
        }

        // Its our data so lets start processing using first child (only child)...
        QDomNode domNode = domElemRoot.firstChild();
        if ( domNode.isNull() )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Failed to get first child node in xml.") );
            return;
        }

        // ini transaction
        g_Transaction->initPaste( this );
        // process all nodes
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

            domNode = domNode.nextSibling();
        }
        // fini transaction
        g_Transaction->fini();

        return;
    }
    else if ( pMimeData->hasImage() )
    {
        return;
    }
    doMessageBox( "INFO", tr("Paste"), tr("Draw layer only supports; image or text.") );
}

void DRDraw::doPaste( const QImage &i )
{
    if ( canCancel() ) 
    {
        doCancel();
    }
    else if ( canCommit() ) 
    {
        doCommit();
    }

    if ( hasSelection() ) 
    {
        doSelectNone2();
    }

    pObject = getObject( i );
}

void DRDraw::doPaste( QDomElement * )
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

void DRDraw::doUndo()
{
    Q_ASSERT( !stackUndo.isEmpty() );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif

/*
    stackRedo.push( image );
    image = stackUndo.pop();
    pGraphics->update();    
    setModified();          
    emit signalChanged();   
*/
}

void DRDraw::doRedo()
{
    Q_ASSERT( !stackRedo.isEmpty() );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    stackUndo.push( image );
    image = stackRedo.pop();
    pGraphics->update();    
    setModified();          
    emit signalChanged();   
*/
}

/*!
 * \brief Select all objects.
 * 
 * Creates a DRSelectRectangle over the entire layer then select all inside.
 * DRSelectRectangle is deleted if nothing selected.
 * DRSelectRectangle rect reduced to just include area needed to include selected objects.
 * 
 * \author pharvey (10/19/25)
 */
void DRDraw::doSelectAll()
{
    if ( nTool == ToolManipulate ) 
    {
        if ( canCancel() ) doCancel();
        else if ( canCommit() ) doCommit();
        if ( hasSelection() ) doSelectNone2();
    }
    else 
        setTool( ToolManipulate );

    Q_ASSERT( !pObject );
    pObject = getObject( ToolSelectRectangle, QPointF( 0, 0 ) );
    bDrawing = true;
    Q_ASSERT( pObject );

    DRSelectRectangle *pSelectRectangle = (DRSelectRectangle *)pObject;
    pSelectRectangle->setDrawing( false );
    pSelectRectangle->setRect( boundingRect() ); 
    doSelectAll( boundingRect() );
    // was anything other than self selected?
    if ( vectorSelected.count() <= 1 ) 
    {
        doCancel();
        return;
    }
    pSelectRectangle->setRect( getSelectedBoundingRect() );
    setSelected( pObject, DRObject::SelectionStateStandard ); // selected self in doSelectAll but switch to standard handles
}

/*!
 * \brief Unselect any selected objects.
 * 
 * Caller must deal with pObject as needed and this can be done by calling doCommit or doCancel.
 * Special action may be required if pObject is a selector - but not in here!
 *
 * \author pharvey (10/19/25)
 */
void DRDraw::doSelectNone()
{
    if ( canCancel() ) doCancel();
    else if ( canCommit() ) doCommit();
    if ( hasSelection() ) doSelectNone2();
}

void DRDraw::doSelectNone2()
{
    while ( vectorSelected.count() )
    {
        setSelected( vectorSelected.at( 0 ), false );
    }
}

void DRDraw::doCrop()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    if ( !canCopy() ) return;                                                            
                                                                                         
    QImage i = pObject->getCopy();                                                       
    doCancel();                                                                          
    slotPreCommit();                                                                     
    image = i;                                                                           
    scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    slotCommitted();                                                                     
*/
}

void DRDraw::doScale( qreal nX, qreal nY, Qt::AspectRatioMode n )
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    // try shape first                                                                   
    if ( pObject && pObject->doScale( nX, nY, n ) ) return;                              
    // cancel any drawing and flip entire canvas                                         
    if ( isDrawing() ) doCancel();                                                       
    slotPreCommit();                                                                     
    image = image.scaled( nX, nY, n, Qt::SmoothTransformation );                         
    scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    slotCommitted();                                                                     
*/
}

void DRDraw::doFlipX()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return;

    // try shape first
    if ( pObject && pObject->doFlipX() ) return;
    // cancel any drawing and flip entire canvas
    if ( canCancel() ) doCancel();
    slotPreCommit();
    image = image.mirrored( true, false );
    slotCommitted();
}

void DRDraw::doFlipY()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return;

    // try shape first
    if ( pObject && pObject->doFlipY() ) return;
    // cancel any drawing and flip entire canvas
    if ( canCancel() ) doCancel();
    slotPreCommit();
    image = image.mirrored( false, true );
    slotCommitted();
}

void DRDraw::doRotate( int nDegrees )
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    // try shape first                                                                   
    if ( pObject && pObject->doRotate( nDegrees ) ) return;                              
    // cancel any drawing and rotate entire canvas                                       
    if ( isDrawing() ) doCancel();                                                       
    slotPreCommit();                                                                     
    image = image.transformed( QTransform().rotate( nDegrees ) );                        
    scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    slotCommitted();                                                                     
*/
}

void DRDraw::doDelete()
{
    if ( canCancel() )
    {
        doCancel();
        return;
    }
    // now delete selected
    while ( vectorSelected.count() )
    {
        DRObject *p = vectorSelected.at( 0 );
        setSelected( p, false );
        slotDelete( p );
    }

    emit signalChanged();
}

/*!
 * \brief Cancel drawing and/or manipulation.
 *  
 *  
 * \author pharvey (3/30/23)
 */
void DRDraw::doCommit()
{
    if ( bTracking )
    {
        ((DRPolyline *)pObject)->doDoubleClick();
        bTracking   = false;
        bMoving     = false;
        pHandle     = nullptr;
    }
    if ( pObject )
    {
        bDrawing = false; // must be before doSelectNone
        if ( pHandle ) pObject->doDrop( pHandle );
        if ( pObject->isSelector() )
        {
            doSelectNone2();
            slotDelete( pObject );
        }
        else 
            setSelected( pObject, false );
        pObject = nullptr;
        emit signalChanged();
    }
}

/*!
 * \brief Cancel drawing and/or manipulation.
 *  
 * Will also delete current object. 
 *  
 * \author pharvey (3/30/23)
 */
void DRDraw::doCancel()
{
    // do commit if we are just manipulating an existing object
    if ( !bDrawing ) return doCommit();
    // we are doing the initial drawing for an object so...
    // stop tracking
    if ( bTracking )
    {
        ((DRPolyline *)pObject)->doDoubleClick();
        bTracking = false;
        bMoving = false;
        pHandle = nullptr;
    }
    // delete current object
    if ( pObject )
    {
        bDrawing = false; // must be before doSelectNone
        if ( pHandle ) pObject->doDrop( pHandle );
        if ( pObject->isSelector() ) doSelectNone2();
        slotDelete( pObject );
        pObject = nullptr;
        emit signalChanged();
    }
}

void DRDraw::doPushUndo()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    stackRedo.clear();                                           
    stackUndo.push( image );                                     
    if ( stackUndo.count() >= nMaxUndo ) stackUndo.removeFirst();
*/
}

void DRDraw::doZoomChanged( int nZoom )
{
    // if ( pObject ) pObject->doZoomChanged( nZoom );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

void DRDraw::doScrollChanged( int nX, int nY )
{
    // if ( pObject ) pObject->doScrollChanged( nX, nY );
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

bool DRDraw::isDrawing()
{
    return bDrawing;
}

bool DRDraw::hasSelection()
{
    return vectorSelected.count();
}

bool DRDraw::canCut()
{
    return hasSelection();
}

bool DRDraw::canCopy()
{
    return hasSelection();
}

bool DRDraw::canPaste()
{
    QClipboard *pClipboard = QGuiApplication::clipboard();
    const QMimeData *pMimeData = pClipboard->mimeData();

    if ( pMimeData->hasImage() || pMimeData->hasText() ) return true;

    return false;
}

bool DRDraw::canUndo()
{
    return !stackUndo.isEmpty();
}

bool DRDraw::canRedo()
{
    return !stackRedo.isEmpty();
}

bool DRDraw::canSelectAll()
{
    return true;
}

bool DRDraw::canSelectNone()
{
    return hasSelection();
}

bool DRDraw::canDelete()
{
    return hasSelection();
}

bool DRDraw::canCommit()
{
    return hasSelection();
    // return isDrawing();
}

bool DRDraw::canCancel()
{
    // return hasSelection();
    return isDrawing();
}

bool DRDraw::canScale()
{
    return hasSelection();
}

bool DRDraw::canFlipX()
{
    return hasSelection();
}

bool DRDraw::canFlipY()
{
    return hasSelection();
}

bool DRDraw::canRotate()
{
    return hasSelection();
}

void DRDraw::slotSceneRectChanged( const QRectF &r )
{
    Q_UNUSED( r );
    if ( canCancel() ) doCancel();
}

void DRDraw::slotPreCommit()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    stackRedo.clear();
    stackUndo.push( image );
    if ( stackUndo.count() >= nMaxUndo ) stackUndo.removeFirst();

    // this will always be followed by a slotCommitted() so
    // no need to signalChangedState here
*/
}

void DRDraw::slotCommitted()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    // we have been modified                                                              
    setModified();                                                                        
    // setModified may - or may not - have done this so we do it here to ensure it happens
    emit signalChanged();                                                                 
    update();                                                                             
*/
}

void DRDraw::setSelected( QList<DRObject *> listObjects, bool b )
{
    for ( DRObject *p : listObjects )
    {
        // add it
        if ( b )
        {
            if ( vectorSelected.contains( p ) )
            {
                qDebug() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object already in selection.";
                return;
            }
            vectorSelected.append( p );
            if ( p->isSelected() )
            {
                qDebug() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object already selected.";
                return;
            }
            p->setSelected( DRObject::SelectionStateMinimal );
        }
        // remove it
        else
        {
            if ( !vectorSelected.contains( p ) )
            {
                qDebug() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object was not in selection.";
                return;
            }
            vectorSelected.removeAll( p );
            if ( !p->isSelected() )
            {
                qDebug() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object is not selected.";
                return;
            }
            p->setSelected( DRObject::SelectionStateNone );
        }
    }
    emit signalChanged();
}

void DRDraw::setSelected( DRObject *p, bool b )
{
    if ( b ) setSelected( p, DRObject::SelectionStateStandard );
    else setSelected( p, DRObject::SelectionStateNone );
}

void DRDraw::setSelected( DRObject *p, DRObject::SelectionStates nState )
{
    Q_ASSERT( p );

    // remove it
    if ( nState == DRObject::SelectionStateNone )
    {
        if ( !vectorSelected.contains( p ) )
        {
            qInfo() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object was not in selection.";
            return;
        }
        vectorSelected.removeAll( p );
        if ( !p->isSelected() )
        {
            qInfo() << "[PAH][" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Object is not selected.";
            return;
        }
        p->setSelected( nState );
    }
    // add/change it
    else
    {
        if ( !vectorSelected.contains( p ) ) vectorSelected.append( p );
        p->setSelected( nState );
    }
    emit signalChanged();
}

DRObject* DRDraw::getObject( const QPointF &point )
{
    QList<QGraphicsItem *> listItems = pGraphics->childItems();
    QGraphicsItem *pItem;

    // reverse order because we want front to back
    for ( qsizetype i = listItems.size() - 1; i >= 0; --i )
    {
        pItem = listItems.at( i );
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;
        if ( pObject->boundingRect().contains( point ) ) return pObject;
    }

    return nullptr;
}

DRObject* DRDraw::getObject( Tools nTool, const QPointF &point )
{
    DRObject *pObject = nullptr;

    Q_ASSERT( pGraphics );

    switch ( nTool )
    {
        case ToolSelectRectangle:
            pObject = new DRSelectRectangle( this, point );
            break;
        case ToolSelectPolygon:
            pObject = new DRSelectPolygon( this, point );
            break;
        case ToolManipulate:
            // no tool object for this tool
            break;
        case ToolDrawArc:
            pObject = new DRArc( this, point );
            break;
        case ToolDrawChord:
            pObject = new DRChord( this, point );
            break;
        case ToolDrawImage:
            pObject = new DRImage( this, point );
            break;
        case ToolDrawPie:
            pObject = new DRPie( this, point );
            break;
        case ToolDrawLine:
            pObject = new DRLine( this, point );
            break;
        case ToolDrawRectangle:
            pObject = new DRRectangle( this, point );
            break;
        case ToolDrawEllipse:
            pObject = new DREllipse( this, point );
            break;
        case ToolDrawPolygon:
            pObject = new DRPolygon( this, point );
            break;
        case ToolDrawPolyline:
            pObject = new DRPolyline( this, point );
            break;
        case ToolDrawText:
            pObject = new DRText( this, point );
            break;
    }

    if ( !pObject )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Invalid tool:" << nTool;
        return nullptr;
    }

    pObject->doGraphicsCreate( pGraphics );
    if ( nTool == ToolSelectPolygon || nTool == ToolDrawPolygon || nTool == ToolDrawPolyline )
    {
        pObject->getGraphics()->setAcceptHoverEvents( true );
    }

    // shape will make itself a child of the canvas in its constructor - no need to add it to the scene again
    // scene()->addItem( pObject );
    connect( pObject, SIGNAL( signalChanged() ), SIGNAL( signalChanged() ) );
    connect( pObject, SIGNAL( signalPreCommit() ), SLOT( slotPreCommit() ) );
    connect( pObject, SIGNAL( signalCommitted() ), SLOT( slotCommitted() ) );
    emit signalChanged();

    return pObject;
}

DRObject* DRDraw::getObject( const QImage & )
{
    DRObject *pObject = nullptr;
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    // paste image: create DImage and apply i
/*
    Q_ASSERT( !pObject );                                                
                                                                         
    pObject = new PPasteRectangle( this, i );                            
                                                                         
    connect( pObject, SIGNAL(signalChanged()), SIGNAL(signalChanged()) );
    connect( pObject, SIGNAL(signalCommitted()), SLOT(slotCommitted()) );
    emit signalChanged();                                                
*/
    return pObject;
}

QList<DRObject *> DRDraw::getObjects( const QRectF &rect )
{
    QList<DRObject *>         listObjects;
    QList<QGraphicsItem *>  listItems = pGraphics->childItems();

    foreach( QGraphicsItem * pItem, listItems )
    {
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;
        if ( !rect.contains( pObject->getRect() ) ) continue;

        listObjects.append( pObject );
    }

    return listObjects;
}

LHandle* DRDraw::getHandle( const QPointF &point )
{
    // we 
    QList<QGraphicsItem *> listItems = pGraphics->childItems();
    QGraphicsItem *pItem;

    // reverse order because we want front to back
    for ( qsizetype i = listItems.size() - 1; i >= 0; --i )
    {
        pItem = listItems.at( i );
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;
        if ( !pObject->isSelected() ) continue;
        LHandle *pHandle = pObject->getHandle( point );
        if ( pHandle ) return pHandle;
    }

    return nullptr;
}

void DRDraw::doSelectAll( const QRectF &rect )
{
    Q_ASSERT( pGraphics );

    QList<QGraphicsItem *> listItems = pGraphics->childItems();
    foreach( QGraphicsItem * pItem, listItems )
    {
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;
        if ( !rect.contains( pObject->boundingRect() ) ) continue;
        setSelected( pObject, DRObject::SelectionStateMinimal );
    }
}

void DRDraw::doSelectAll( const QPolygonF &polygon )
{
    // unselect all (without cancelling drawing)
    // NOTE: any selector object is not in vectorSelected so will be unaffected
    while ( vectorSelected.count() )
    {
        setSelected( vectorSelected.at( 0 ), false );
    }

    QList<QGraphicsItem *> listItems = pGraphics->childItems();

    foreach( QGraphicsItem * pItem, listItems )
    {
        LGraphicsObject *pGraphicsObject = dynamic_cast<LGraphicsObject *>( pItem );
        if ( !pGraphicsObject ) continue;
        DRObject *pObject = dynamic_cast<DRObject *>( pGraphicsObject->getObject() );
        if ( !pObject ) continue;
        if ( pObject->isSelector() ) continue; // selectors are always in selected state
        // if ( !rect.contains( rectObject ) ) continue;

        // So the object's rect is in our bounding rect.
        // Lets get a little more accurate by checkinjg that each point of the object is within polygon.
        // The results get less exact the more irregular the polygon.
        if ( pObject->inherits( "DRLine" ) )
        {
            DRLine *pLine = (DRLine *)pObject;
            if ( polygon.containsPoint( pLine->getBegin(), Qt::OddEvenFill ) &&
                 polygon.containsPoint( pLine->getEnd(), Qt::OddEvenFill ) )
            {
                setSelected( pObject, DRObject::SelectionStateMinimal );
            }
        }
        else if ( pObject->inherits( "DRPolyline" ) )
        {
            DRPolyline *pPolyline = (DRPolyline *)pObject;
            QPolygonF poly = pPolyline->getPolygon();
            foreach( QPointF point, poly )
            {
                if ( !polygon.containsPoint( point, Qt::OddEvenFill ) ) break;
            }
            setSelected( pObject, DRObject::SelectionStateMinimal );
        }
        else
        {
            // handle as if shape is a rect
            QRectF rectObject = pObject->getRect();
            if ( polygon.containsPoint( rectObject.topLeft(), Qt::OddEvenFill ) &&
                 polygon.containsPoint( rectObject.topRight(), Qt::OddEvenFill ) &&
                 polygon.containsPoint( rectObject.bottomLeft(), Qt::OddEvenFill ) &&
                 polygon.containsPoint( rectObject.bottomRight(), Qt::OddEvenFill ) )
            {
                setSelected( pObject, DRObject::SelectionStateMinimal );
            }
        }
    }
}

void DRDraw::doClear()
{
    image.fill( colorBackground );
    setModified( false );
    pGraphics->update();
}


