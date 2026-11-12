/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
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

    pSelectionManager = new ADObjectSelectionManager( this );
    connect( pSelectionManager, SIGNAL(signalChangedSelection()), this, SIGNAL(signalSelectionChanged()) );

    // settings
    pSettingGeneral = g_SSettings->getSetting( "LSGeneral" ); Q_ASSERT( pSettingGeneral );
    slotRefreshGeneral();
    connect( pSettingGeneral, SIGNAL(signalChanged()), SLOT(slotRefreshGeneral()) );
}

DRDraw::~DRDraw()
{
    if ( pSelectionManager )
    {
        pSelectionManager->doSelectNone();
        delete pSelectionManager; 
    }
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
            pSelectionManager->doSelectNone();
        }
    }
    else
        setToolState( ToolStateIdle );

    nTool = (Tools)n;

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
    if ( !pObjectDrawing ) return QImage();
    return pObjectDrawing->getImage();
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
            break;
        case ToolDrawLine:
            return g_SSettings->getSetting( "LSLine" )->getToolBar( pParent );
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
            {
                // scan selected until we find an object that has a panel...
                QList<ADObject*> l = pSelectionManager->getSelected();
                for ( ADObject *p : l )
                {
                    QString s = p->metaObject()->className();
                    // we know these objects have panels...
                    if ( s == "DRLine" )
                        return g_SSettings->getSetting( "LSLine" )->getPanel( pParent );
                    else if ( s == "DRRectangle" )
                        return g_SSettings->getSetting( "LSRectangle" )->getPanel( pParent );
                    else if ( s == "DREllipse" )
                        return g_SSettings->getSetting( "LSEllipse" )->getPanel( pParent );
                    else if ( s == "DRText" )
                        return g_SSettings->getSetting( "LSText" )->getPanel( pParent );
                }
            }
            break;
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
            break;
        case ToolDrawLine:
            return g_SSettings->getSetting( "LSLine" )->getPanel( pParent );
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
        pSelectionManager->setSelected( pObject );
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

QString DRDraw::getToolStateString()
{
    switch ( nToolState ) 
    {
        case ToolStateIdle:
            return "ToolStateIdle";
        case ToolStateDraw: 
            return "ToolStateDraw";
        case ToolStateDrawManipulate:
            return "ToolStateDrawManipulate";
        case ToolStateManipulate:
            return "ToolStateManipulate";
    }
    return "unknown";
}

QVector<DRObject*> DRDraw::getSelected() 
{
    QVector<DRObject*> vectorResult;
    QList<ADObject *> l = pSelectionManager->getSelected();
    for ( ADObject *p : l )
    {
        vectorResult << (DRObject*)p;
    }

    return vectorResult; 
}

QRectF DRDraw::getSelectedBoundingRect()
{
    QRectF r;
    bool bFirst = true;
    QList<ADObject*> l = pSelectionManager->getSelected();
    foreach( ADObject *p, l )
    {
        DRObject *pObject = (DRObject*)p;
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
 * \author pharvey (3/27/23)
 * 
 * \param pEvent 
 */
void DRDraw::doMousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    //
    // Off-Scene 
    // - happens when outside of scene but within view 
    // - we will ignore all Move and double-click events
    // - tracking can recover by bringing the mouse back to the scene and clicking or double clicking
    //
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) )
    {
        if ( !isTracking() ) pointPress = QPointF();
        return;
    }

    switch ( nToolState )
    {
        case ToolStateIdle:
            doMousePressEventIdle( pEvent );
            break;
        case ToolStateDraw:
            doMousePressEventDraw( pEvent );
            break;
        case ToolStateDrawManipulate:
            doMousePressEventDrawManipulate( pEvent );
            break;
        case ToolStateManipulate:
            doMousePressEventManipulate( pEvent );
            break;
    }
}

/*!
 * \brief Mouse move event. 
 *  
 * Here we determine the intent of the Press and then act accordingly. 
 * Mostly; this results in moving a handle.
 *  
 * This can mean; 
 *  
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

    if ( pointPress.isNull() ) return; // press within view but outside of scene

    // ignore invalid move
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;
    pointCurrent = pEvent->scenePos(); // we need a valid point for doRelease even when release Off-Scene

    switch ( nToolState )
    {
        case ToolStateIdle:
            doMouseMoveEventIdle( pEvent );
            break;
        case ToolStateDraw:
            doMouseMoveEventDraw( pEvent );
            break;
        case ToolStateDrawManipulate:
            doMouseMoveEventDrawManipulate( pEvent );
            break;
        case ToolStateManipulate:
            doMouseMoveEventManipulate( pEvent );
            break;
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

    /****************************************** 
     * Press (off-scene)
     ******************************************/
    if ( pointPress.isNull() ) return;
    if ( pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) pointCurrent = pEvent->scenePos();

    switch ( nToolState )
    {
        case ToolStateIdle:
            doMouseReleaseEventIdle( pEvent );
            break;
        case ToolStateDraw:
            doMouseReleaseEventDraw( pEvent );
            break;
        case ToolStateDrawManipulate:
            doMouseReleaseEventDrawManipulate( pEvent );
            break;
        case ToolStateManipulate:
            doMouseReleaseEventManipulate( pEvent );
            break;
    }
}

void DRDraw::doMousePressEventIdle( QGraphicsSceneMouseEvent *pEvent )
{
    // we may have a selection but the following makes us idle (no need to commit/cancel)
    Q_ASSERT( !pObjectDrawing );
    Q_ASSERT( !pHandleMoving );

    if ( pEvent->buttons() != Qt::LeftButton ) return;

    // what did we press on (if anything)?
    pointPress      = pEvent->scenePos();
    pointCurrent    = pointPress;
    pObjectPress    = nullptr;
    pHandlePress    = getHandle( pointPress );
    if ( pHandlePress ) 
        pObjectPress = (DRObject*)pHandlePress->getLayerObject();
    else
        pObjectPress = getObject( pointPress ); // no handle as object is not selected

    // each tool type is handled a bit differently 
    switch ( nTool )
    {
        case ToolManipulate:
            {
                if ( pHandlePress ) 
                {
                    pHandleMoving = pHandlePress;
                    setToolState( ToolStateManipulate );
                    return;
                }
                if ( pObjectPress )
                {
                    if ( pObjectPress->isSelected() )
                        pObjectPress->doSelectionStateCycle();
                    else
                    {
                        pSelectionManager->doSelectNone();
                        pSelectionManager->setSelected( pObjectPress );
                    }
                    return;
                }
                pSelectionManager->doSelectNone();
                return;
            }
            return;
        case ToolSelectPolygon:
                {
                    pSelectionManager->doSelectNone();
                    pObjectDrawing = getObject( nTool, pointPress );       // first point
                    // select when doubleclick because that is when we know what handles to create
                    setToolState( ToolStateDraw );
                }
                return;
        case ToolDrawPolygon:
        case ToolDrawPolyline:
            {
                pObjectDrawing = getObject( nTool, pointPress );       // first point
                // select when doubleclick because that is when we know what handles to create
                setToolState( ToolStateDraw );
            }
            return;
        case ToolSelectRectangle:
            {
                pSelectionManager->doSelectNone();
                pObjectDrawing = getObject( nTool, pointPress );
                pObjectDrawing->setSelected( DRObject::SelectionStateStandard ); // needed as selectors are !isSelectable()
                pHandleMoving = pObjectDrawing->getGrab( pointPress, true ); // get the sizing handle
                Q_ASSERT( pHandleMoving );
                setToolState( ToolStateDraw );
            }
            return;
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawText:
            {
                // we could defer create until move but for now keep it simple and create here
                pSelectionManager->doSelectNone();
                pObjectDrawing = getObject( nTool, pointPress );
                pSelectionManager->setSelected( pObjectDrawing );
                pHandleMoving = pObjectDrawing->getGrab( pointPress, true ); // get the sizing handle
                Q_ASSERT( pHandleMoving );
                setToolState( ToolStateDraw );
            }
            return;
    }
}

void DRDraw::doMouseMoveEventIdle( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    // nothing to do
    // could use to confirm draw for non-polygon objects
}

void DRDraw::doMouseReleaseEventIdle( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    // nothing to do
}

void DRDraw::doMousePressEventDraw( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    switch ( nTool )
    {
        case ToolSelectPolygon:
        case ToolDrawPolygon:
        case ToolDrawPolyline:
            Q_ASSERT( pObjectDrawing );
            Q_ASSERT( pObjectDrawing->inherits( "DRPolyline" ) );
            ((DRPolyline *)pObjectDrawing)->doPress( pEvent->scenePos() ); // appends point
            break;
        case ToolSelectRectangle:
        case ToolManipulate:
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawText:
            break;
    }
}

void DRDraw::doMouseMoveEventDraw( QGraphicsSceneMouseEvent *pEvent )
{
    switch ( nTool )
    {
        case ToolSelectPolygon:
        case ToolDrawPolygon:
        case ToolDrawPolyline:
            Q_ASSERT( pObjectDrawing );
            Q_ASSERT( pObjectDrawing->inherits( "DRPolyline" ) );
            ((DRPolyline *)pObjectDrawing)->doMove( pEvent->scenePos() ); // updates our working point
            break;
        case ToolSelectRectangle:
        case ToolManipulate:
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawText:
            if ( pEvent->buttons() != Qt::LeftButton ) return;
            Q_ASSERT( pObjectDrawing );
            Q_ASSERT( pHandleMoving );
            pObjectDrawing->doMove( pHandleMoving, pointCurrent );
            break;
    }
}

void DRDraw::doMouseReleaseEventDraw( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    switch ( nTool )
    {
        case ToolSelectRectangle:
            {
                Q_ASSERT( pObjectDrawing );
                Q_ASSERT( pHandleMoving );
                DRSelectRectangle *p = (DRSelectRectangle*)pObjectDrawing;
                p->doDrop( pHandleMoving, pointCurrent );
                pHandleMoving = nullptr;
                doSelectAll( p->getRect() );    
                slotDelete( pObjectDrawing );
                pObjectDrawing = nullptr;
                setTool( ToolManipulate, false );
                emit signalChanged();
            }
            break;
        case ToolSelectPolygon:
        case ToolDrawPolygon:
        case ToolDrawPolyline:
            // these end with a double click
            break;
        case ToolManipulate:
            break;
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawText:
            pObjectDrawing->doDrop( pHandleMoving, pointCurrent );
            pHandleMoving = nullptr;
            if ( bAutoCommit )
            {
                doCommit();
                setModified();
            }
            else
            {
                setToolState( ToolStateDrawManipulate );
            }
            break;
    }
}

void DRDraw::doMousePressEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    Q_ASSERT( pObjectDrawing );
    Q_ASSERT( !pHandleMoving );
    Q_ASSERT( isToolDraw() );

    // what did we press on (if anything)?
    pointPress      = pEvent->scenePos();
    pointCurrent    = pointPress;

    pObjectPress    = nullptr;
    pHandlePress    = getHandle( pointPress );
    if ( pHandlePress ) 
        pObjectPress = (DRObject*)pHandlePress->getLayerObject();
    else
        pObjectPress = getObject( pointPress ); // no handle as object is not selected

    // do we need to commit/cancel current?
    if ( pObjectPress != pObjectDrawing || !pHandlePress )
    {
        if ( bAutoCommit )
        {
            doCommit();
            setModified();
        }
        else
            doCancel();
        return; // we are back to Idle so leave here
    }

    pHandleMoving = pHandlePress;
}

void DRDraw::doMouseMoveEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    switch ( nTool )
    {
        case ToolSelectRectangle:
        case ToolSelectPolygon:
        case ToolManipulate:
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
            if ( !pHandleMoving ) return;
            Q_ASSERT( pObjectDrawing );
            Q_ASSERT( pHandleMoving );
            pObjectDrawing->doDrop( pHandleMoving, pointCurrent ); // move a handle
            break;
    }
}

void DRDraw::doMouseReleaseEventDrawManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    switch ( nTool )
    {
        case ToolSelectRectangle:
        case ToolSelectPolygon:
        case ToolManipulate:
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
            Q_ASSERT( pObjectDrawing );
            if ( !pHandleMoving ) return; // could be here after a double click
            pObjectDrawing->doDrop( pHandleMoving, pointCurrent ); // move a handle
            pHandleMoving = nullptr;
            break;
    }
}

void DRDraw::doMousePressEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );

    if ( pEvent->buttons() != Qt::LeftButton ) return;

    // does not happen 
    // we catch this in Idle and then go back to Idle upon release
}

void DRDraw::doMouseMoveEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    if ( pEvent->buttons() != Qt::LeftButton ) return;

    Q_ASSERT( nTool == ToolManipulate );
    Q_ASSERT( pHandleMoving );

    DRObject *p = (DRObject*)pHandleMoving->getLayerObject();
    p->doMove( pHandleMoving, pointCurrent );
}

void DRDraw::doMouseReleaseEventManipulate( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
    Q_ASSERT( nTool == ToolManipulate );
    Q_ASSERT( pHandleMoving );

    DRObject *p = (DRObject*)pHandleMoving->getLayerObject();
    p->doDrop( pHandleMoving, pointCurrent );
    setModified();
    pHandleMoving = nullptr;
    setToolState( ToolStateIdle );
}

void DRDraw::doDragEnterEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );
    Q_ASSERT( hasSelection() );
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

/*!
 * \brief Handle double click event.
 *
 * This is used when drawing a DRPolyline based object to terminate the tracking.  
 * 
 * \note a double-click results in; doPressEvent + doDoubleClickEvent + doReleaseEvent
 *
 * \sa doMousePressEventSelectPolygon
 * \sa doMouseMoveEventSelectPolygon
 * \sa doMouseReleaseEventSelectPolygon
 *
 * \author pharvey (2026-11-02)
 * 
 * \param pEvent 
 */
void DRDraw::doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    /****************************************** 
     * Original Press Was Off-Scene
     ******************************************/
    if ( pointPress.isNull() ) return;

    // double-click is Off-Scene - ignore
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;

    switch ( nTool )
    {
        case ToolSelectPolygon:
            {
                Q_ASSERT( pObjectDrawing );
                DRPolygon *p = (DRPolygon*)pObjectDrawing;
                p->doDoubleClick( pEvent->scenePos() );
                pHandleMoving = nullptr;
                doSelectAll( p->getPolygon() );    
                slotDelete( pObjectDrawing );
                pObjectDrawing = nullptr;
                setTool( ToolManipulate, false );
                emit signalChanged();
            }
            break;
        case ToolDrawPolygon:
        case ToolDrawPolyline:
            ((DRPolyline *)pObjectDrawing)->doDoubleClick( pEvent->scenePos() );
            pSelectionManager->setSelected( pObjectDrawing );
            setToolState( ToolStateDrawManipulate );
            pHandleMoving = nullptr;
            setModified();
            break;
        case ToolManipulate:
        case ToolSelectRectangle:
        case ToolDrawArc:
        case ToolDrawChord:
        case ToolDrawImage:
        case ToolDrawPie:
        case ToolDrawLine:
        case ToolDrawRectangle:
        case ToolDrawEllipse:
        case ToolDrawText:
            break;
    }
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
    {
        QList<ADObject*> l = pSelectionManager->getSelected();
        foreach( ADObject *pObject, l )
        {
            pObject->doSave( &domDoc, &domElementRoot );
        }
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
        pSelectionManager->doSelectNone();
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
        pSelectionManager->doSelectNone();
    }

    pObjectDrawing = getObject( i );
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
        if ( hasSelection() ) pSelectionManager->doSelectNone();
    }
    else 
        setTool( ToolManipulate );

    Q_ASSERT( !pObjectDrawing );
    pObjectDrawing = getObject( ToolSelectRectangle, QPointF( 0, 0 ) );
    setToolState( ToolStateIdle );
    Q_ASSERT( pObjectDrawing );

    DRSelectRectangle *pSelectRectangle = (DRSelectRectangle *)pObjectDrawing;
    pSelectRectangle->setDrawing( false );
    pSelectRectangle->setRect( boundingRect() ); 
    doSelectAll( boundingRect() );
    // was anything other than self selected?
    if ( hasSelection() ) 
    {
        doCancel();
        return;
    }
    pSelectRectangle->setRect( getSelectedBoundingRect() );
    setSelected( pObjectDrawing, DRObject::SelectionStateStandard ); // selected self in doSelectAll but switch to standard handles
}

/*!
 * \brief Unselect any selected objects.
 * 
 * Caller must deal with pObjectDrawing as needed and this can be done by calling doCommit or doCancel.
 * Special action may be required if pObjectDrawing is a selector - but not in here!
 *
 * \author pharvey (10/19/25)
 */
void DRDraw::doSelectNone()
{
    if ( canCancel() ) doCancel();
    else if ( canCommit() ) doCommit();
    if ( hasSelection() ) pSelectionManager->doSelectNone();
}

void DRDraw::doCrop()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    if ( !canCopy() ) return;                                                            
                                                                                         
    QImage i = pObjectDrawing->getCopy();                                                       
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
    Q_UNUSED( nX );
    Q_UNUSED( nY );
    Q_UNUSED( n );
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    // try shape first                                                                   
    if ( pObjectDrawing && pObjectDrawing->doScale( nX, nY, n ) ) return;                              
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
    if ( pObjectDrawing && pObjectDrawing->doFlipX() ) return;
    // cancel any drawing and flip entire canvas
    if ( canCancel() ) doCancel();
    slotPreCommit();
#if QT_VERSION >= QT_VERSION_CHECK( 6, 10, 0 )
    image = image.flipped( Qt::Horizontal );
#else
    image = image.mirrored( true, false );
#endif
    slotCommitted();
}

void DRDraw::doFlipY()
{
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return;

    // try shape first
    if ( pObjectDrawing && pObjectDrawing->doFlipY() ) return;
    // cancel any drawing and flip entire canvas
    if ( canCancel() ) doCancel();
    slotPreCommit();
#if QT_VERSION >= QT_VERSION_CHECK( 6, 10, 0 )
    image = image.flipped( Qt::Vertical );
#else
    image = image.mirrored( false, true );
#endif
    slotCommitted();
}

void DRDraw::doRotate( int nDegrees )
{
#ifdef CBD_TODO
    Q_UNUSED( nDegrees );
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
/*
    // try shape first                                                                   
    if ( pObjectDrawing && pObjectDrawing->doRotate( nDegrees ) ) return;                              
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
    // now delete selected
    QList<ADObject*> l = pSelectionManager->getSelected();
    for ( ADObject *p : l )
    {
        if ( p == pObjectDrawing )
        {
            pObjectDrawing = nullptr;
            pHandleMoving = nullptr;
        }
        slotDelete( p );
    }

    // cancel selector (if any)
    if ( canCancel() )
    {
        doCancel();
        return;
    }

    emit signalChanged();
}

/*!
 * \brief Commit drawing and/or manipulation.
 * 
 * Keeps the current object.
 * Unselects all.
 * Set tool state to idle.
 * 
 * \note tool stays the same
 * \note selector is NOT valid here as they are auto deleted
 * 
 * \author pharvey (3/30/23)
 */
void DRDraw::doCommit()
{
    Q_ASSERT( isDrawing() );
    Q_ASSERT( !pObjectDrawing->isSelector() );

    pSelectionManager->doSelectNone();

    pObjectDrawing = nullptr;
    pHandleMoving = nullptr;
    setToolState( ToolStateIdle );
    emit signalChanged();
}

/*!
 * \brief Cancel drawing and/or manipulation.
 *  
 * Will delete current object.
 * Set tool state to idle.
 * 
 * \note tool state does NOT have to be ToolStateDraw
 * \note tool stays the same
 * \note selector is NOT valid here as they are auto deleted
 * 
 * \author pharvey (3/30/23)
 */
void DRDraw::doCancel()
{
    Q_ASSERT( pObjectDrawing );
    Q_ASSERT( !pObjectDrawing->isSelector() );

    slotDelete( pObjectDrawing );

    pObjectDrawing = nullptr;
    pHandleMoving = nullptr;
    setToolState( ToolStateIdle );
    emit signalChanged();
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
    // if ( pObjectDrawing ) pObjectDrawing->doZoomChanged( nZoom );
#ifdef CBD_TODO
    Q_UNUSED( nZoom );
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

void DRDraw::doScrollChanged( int nX, int nY )
{
    // if ( pObjectDrawing ) pObjectDrawing->doScrollChanged( nX, nY );
#ifdef CBD_TODO
    Q_UNUSED( nX );
    Q_UNUSED( nY );
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

bool DRDraw::isDrawing()
{
    if ( nToolState == ToolStateDraw || nToolState == ToolStateDrawManipulate )
    {
        Q_ASSERT( pObjectDrawing );
        return true;
    }

    return false;
}

/*!
 * \brief Is tracking on/off.
 * 
 * Tracking is on when a DRPolyline is being drawn.
 * This is the only time when mouse pos changes matter when no button pressed.
 * 
 * \author pharvey (2026-11-04)
 * 
 * \return bool 
 */
bool DRDraw::isTracking()
{
    if ( nToolState == ToolStateDraw )
    {
        Q_ASSERT( pObjectDrawing );
        return pObjectDrawing->inherits( "DRPolyline" );
    }
    return false;
}

bool DRDraw::isSelectionToolConfig()    
{ 
    return isToolSelector(); 
}

bool DRDraw::hasSelection()
{
    return pSelectionManager->hasSelection();
}

bool DRDraw::hasSelector()
{
    if ( !pObjectDrawing ) return false;
    return pObjectDrawing->isSelector();
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

// can commit new object
bool DRDraw::canCommit()
{
    return isDrawing();
}

// can cancel new object or selector
bool DRDraw::canCancel()
{
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

void DRDraw::setToolState( ToolStates n )
{
    if ( n == nToolState ) return;
    nToolState = n;
    emit signalChangedToolState();
}

/*!
 * \brief Select/unselect given objects.
 * 
 * \author pharvey (2026-10-31)
 * 
 * \param p      
 * \param nState 
 */
void DRDraw::setSelected( QList<DRObject *> listObjects, bool b )
{
    for ( DRObject *p : listObjects )
    {
        setSelected( p, b );
    }
}

/*!
 * \brief Select/unselect object.
 * 
 * \author pharvey (2026-10-31)
 * 
 * \param p      
 * \param nState 
 */
void DRDraw::setSelected( DRObject *p, bool b )
{
    pSelectionManager->setSelected( p, b );
    emit signalChanged();
}

/*!
 * \brief Set the object select state.
 * 
 * This will harmonize ADObject::bSelected with DRObject::nSelectionState. 
 * 
 * \author pharvey (2026-10-31)
 * 
 * \param p      
 * \param nState 
 */
void DRDraw::setSelected( DRObject *p, DRObject::SelectionStates nState )
{
    Q_ASSERT( p );

    if ( nState == DRObject::SelectionStateNone )
    {
        pSelectionManager->setSelected( pObjectDrawing, false );
    }
    else
    {
        if ( !p->isSelected() ) pSelectionManager->setSelected( pObjectDrawing );
    }
    p->setSelected( nState );
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
        if ( !pObject->isSelector() && !pObject->isSelected() ) continue;
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
        pSelectionManager->setSelected( pObject );
        // if ( pObject->isSelected() ) setSelected( pObject, DRObject::SelectionStateMinimal );
    }
}

void DRDraw::doSelectAll( const QPolygonF &polygon )
{
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
                pSelectionManager->setSelected( pObject );
                // if ( pObject->isSelected() ) setSelected( pObject, DRObject::SelectionStateMinimal );
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
            pSelectionManager->setSelected( pObject );
            // if ( pObject->isSelected() ) setSelected( pObject, DRObject::SelectionStateMinimal );
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
                pSelectionManager->setSelected( pObject );
                // if ( pObject->isSelected() ) setSelected( pObject, DRObject::SelectionStateMinimal );
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

void DRDraw::slotRefreshGeneral()
{
    bAutoCommit = pSettingGeneral->property( "bAutoCommit" ).toInt();
}


