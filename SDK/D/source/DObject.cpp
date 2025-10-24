#include "LibInfo.h"
#include "DObject.h"

#include <CBDPersist.h>

#include <ADTransaction.h>

#include <WPen.h>
#include <WBrush.h>
#include <WText.h>
#include <WCoordWidget.h>

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsItemLine.h"

#include "DGroup.h"

DObject::DObject( ADObject *pObjectParent, const QString &stringName )
    : AWObject( pObjectParent, stringName )
{
    // \NOTE Handles can not be created in here as they need proxy. 
    //       Handles are created when setSelected( true ).
    //       Handles are deleted when setSelected( false ).
    // 
    //      \sa setSelected()
    bSelectable     = true;
    pProxy          = nullptr;
    bMovingHandle   = false;
    nTextDisplay    = TextDisplayName;
    nSelectType     = SelectResize;

    // events
    mapEvents.insert( "Resize", QString() );
    mapEvents.insert( "Move", QString() );
}

DObject::~DObject()
{
    // unselected by now so no need to delete handles

    //
    if ( pProxy ) delete pProxy;
}

/*!
 * \brief Return the selection manager. 
 *  
 * We know that DDiagram is up-stream of us and that it has the selection manager. 
 * 
 * \author pharvey (9/2/20)
 * 
 * \return ADObjectSelectionManager* 
 */
ADObjectSelectionManager *DObject::getSelectionManager()
{
    return getDiagram()->getSelectionManager();
}

/*!
 * \brief Return the diagram. 
 *  
 * We know that DDiagram is up-stream of us. It may be our parent or further up. 
 * So this should always return a valid reference.  
 * 
 * \author pharvey (9/2/20)
 * 
 * \return DDiagram* 
 */
DDiagram *DObject::getDiagram()
{
    return (DDiagram*)getParent( "DDiagram" );
}

QPointF DObject::getPosCenter()
{
    return getProxy()->mapToScene( boundingRect().center() );
}

QMenu *DObject::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = AWObject::getContextMenu( pParent );

    if ( inGroup() )
    {
        // remove option to delete
        QList<QAction*> l = pMenu->actions();
        QAction *pAction;
        foreach( pAction, l )
        {
            if ( pAction->iconText().indexOf( tr("Delete ") ) == 0 )
            {
                pMenu->removeAction( pAction );
                break;
            }
        }
    }

    return pMenu; // caller invokes and then deletes
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We introduce a number of properties - mostly related to drawing. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DObject::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    // lets just have this for DDiagram
    p->removeByClass( "AWFactoryWidget" );

    // font
    {
        WTextPanel *pTextPanel = new WTextPanel( getText(), p );
        connect( pTextPanel, SIGNAL(signalChanged(const CBD::CBDText &)), SLOT(slotText(const CBD::CBDText &)) );
        connect( this, SIGNAL(signalChanged(const CBD::CBDText &)), pTextPanel, SLOT(slotRefresh(const CBD::CBDText &)) );
        p->insertTab( 1, pTextPanel, tr("Font") );
    }
    // brush
    {
        WBrushPanel *pBrushPanel = new WBrushPanel( getBrush(), p );
        connect( pBrushPanel, SIGNAL(signalChanged(const QBrush &)), SLOT(slotBrush(const QBrush &)) );
        connect( this, SIGNAL(signalChanged(const QBrush &)), pBrushPanel, SLOT(slotRefresh(const QBrush &)) );
        p->insertTab( 1, pBrushPanel, tr("Brush") );
    }
    // pen
    {
        WPenPanel *pPenPanel = new WPenPanel( getPen(), p );
        connect( pPenPanel, SIGNAL(signalChanged(const QPen &)), SLOT(slotPen(const QPen &)) );
        connect( this, SIGNAL(signalChanged(const QPen &)), pPenPanel, SLOT(slotRefresh(const QPen &)) );
        p->insertTab( 1, pPenPanel, tr("Pen") );
    }

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DObject::getPropWidget( QWidget *pWidgetParent )
{
#ifdef DEBUG_PROPDOCK
    AWPropWidget *pPropWidget = new AWPropWidget( this, pWidgetParent );
    // debug
    {
        QWidget *       pWidget         = new QWidget( pPropWidget );
        QVBoxLayout *   pLayout         = new QVBoxLayout( pWidget );
        WCoordWidget *  pLabelCell      = new WCoordWidget( pWidget );
        WCoordWidget *  pLabelPixel     = new WCoordWidget( pWidget );
        pLabelPixel->slotCoord( getPos() );
        pLabelCell->slotCoord( ((DDiagram*)getParent( "DDiagram" ))->mapSceneToCell( getPos() ) );
        pLayout->addWidget( pLabelCell );
        pLayout->addWidget( pLabelPixel );
        pPropWidget->addWidget( tr("Debug"), pWidget );
    }
#else
    AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );
    // pen
    {
        WPenPanel *pPenPanel = new WPenPanel( getPen(), pPropWidget );
        connect( pPenPanel, SIGNAL(signalChanged(const QPen &)), SLOT(slotPen(const QPen &)) );
        connect( this, SIGNAL(signalChanged(const QPen &)), pPenPanel, SLOT(slotRefresh(const QPen &)) );
        pPropWidget->addWidget( tr("Pen"), pPenPanel );
    }
    // brush
    {
        WBrushPanel *pBrushPanel = new WBrushPanel( getBrush(), pPropWidget );
        connect( pBrushPanel, SIGNAL(signalChanged(const QBrush &)), SLOT(slotBrush(const QBrush &)) );
        connect( this, SIGNAL(signalChanged(const QBrush &)), pBrushPanel, SLOT(slotRefresh(const QBrush &)) );
        pPropWidget->addWidget( tr("Brush"), pBrushPanel );
    }
    // font
    {
        WTextPanel *pTextPanel = new WTextPanel( getText(), pPropWidget );
        connect( pTextPanel, SIGNAL(signalChanged(const CBD::CBDText &)), SLOT(slotText(const CBD::CBDText &)) );
        connect( this, SIGNAL(signalChanged(const CBD::CBDText &)), pTextPanel, SLOT(slotRefresh(const CBD::CBDText &)) );
        pPropWidget->addWidget( tr("Font"), pTextPanel );
    }
#endif
    // caller will add more (if needed) and/or off to the app
    // caller must delete
    return pPropWidget;

}

DGroup *DObject::getGroup( bool bOutter )
{
    QObject *p = parent();
    if ( !p ) return nullptr;
    if ( !p->inherits( "DGroup" ) ) return nullptr;

    DGroup *pGroup = (DGroup*)p; 

    if ( bOutter )
    {
        if ( pGroup->inGroup() ) return pGroup->getGroup( true );
    }

    return pGroup;
}

DLayout *DObject::getLayout()
{
    QObject *p = parent();
    if ( !p ) return nullptr;
    if ( !p->inherits( "DLayout" ) ) return nullptr;

    return (DLayout*)p;
}

QDomElement DObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common and children...
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    domElemThis.setAttribute( "TextDisplay", nTextDisplay );

    // save additional nodes...
    domElemThis.appendChild( CBDPersistNative::doSaveText( font, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveBrush( brush, pdomDoc ) );

    return domElemThis;
}

bool DObject::doLoad( QDomElement *pdomElemThis )
{
    // load common and children... 
    AWObject::doLoad( pdomElemThis );

    // load additional attributes...
    nTextDisplay = (TextDisplayTypes)pdomElemThis->attribute( "TextDisplay", "0" ).toInt();

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

        if ( domElem.tagName() == "Pen" )
            pen = CBDPersistNative::doLoadPen( &domElem );
        else if ( domElem.tagName() == "Brush" )
            brush = CBDPersistNative::doLoadBrush( &domElem );
        else if ( domElem.tagName() == "Font" )
            font = CBDPersistNative::doLoadText( &domElem );

        domNode = domNode.nextSibling();
    }

    return true;
}

/*!
 * \brief Catch case where we are being connected to a DObject.
 *  
 * Sets pObject.
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void DObject::doUsesAdd( ADObject *pObject, const QString &s )
{
    ADObject::doUsesAdd( pObject, s );

    // We should have exactly one of these
    if ( pObject->inherits( "DObject" ) )
    {
        // Intercept this echo because we always care if our model object has changed.
        connect( pObject, SIGNAL(signalModified(ADObject*)), this, SLOT(slotModifiedReference(ADObject*)) );
    }
}

/*!
 * \brief Catch case where we are being disconnected from DObject.
 *  
 * Sets pObject to nullptr. 
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void DObject::doUsesRemove( ADObject *pObject, const QString &s  )
{
    ADObject::doUsesRemove( pObject, s );

    //
    if ( pObject->inherits( "DObject" ) )
    {
        disconnect( pObject, SIGNAL(signalModified(ADObject*)), this, SLOT(slotModifiedReference(ADObject*)) );
    }
}

/*!
 * \brief Move to back. 
 *  
 * We adjust self AND QGraphicsItem's. 
 * 
 * \author pharvey (8/22/20)
 */
void DObject::doToBack()
{
    AWObject::doToBack();
    getProxy()->doToBack();
}

/*!
 * \brief Move to front.
 * 
 * We adjust self AND QGraphicsItem's. 
 * 
 * \author pharvey (8/22/20)
 */
void DObject::doToFront()
{
    AWObject::doToFront();
    getProxy()->doToFront();
}

/*!
 * \brief Move to prev.
 * 
 * We adjust self AND QGraphicsItem's. 
 * The filter *must* result in a DObject - otherwise app data and diagram can get out of sync! 
 * 
 * \author pharvey (8/22/20)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 */
void DObject::doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    AWObject::doToPrev( stringName, stringClassName, stringInheritsClass );
    getProxy()->doToPrev( stringName, stringClassName, stringInheritsClass );
}

/*!
 * \brief Move to next.
 * 
 * We adjust self AND QGraphicsItem's. 
 * The filter *must* result in a DObject - otherwise app data and diagram can get out of sync! 
 *  
 * \author pharvey (8/22/20)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 */
void DObject::doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    AWObject::doToNext( stringName, stringClassName, stringInheritsClass );
    getProxy()->doToNext( stringName, stringClassName, stringInheritsClass );
}

void DObject::doToBefore( ADObject *pObject )
{
    Q_ASSERT( pObject );
    Q_ASSERT( pObject->inherits( "DObject" ) );

    AWObject::doToBefore( pObject );
    getProxy()->doToBefore( (DObject*)pObject );
}

/*!
 * \brief Move after.
 * 
 * We adjust ADObject's AND QGraphicsItem's. 
 * pObject *must* be a DObject. 
 *  
 * \author pharvey (8/22/20)
 * 
 * \param pObjectReference 
 */
void DObject::doToAfter( ADObject *pObject )
{
    Q_ASSERT( pObject );
    Q_ASSERT( pObject->inherits( "DObject" ) );

    AWObject::doToAfter( pObject );
    getProxy()->doToAfter( (DObject*)pObject );
}

/*!
 * \brief Move self to index among siblings. 
 *  
 * We do the default but then we ensure that the proxy is also moved. 
 * 
 * \author pharvey (9/24/20)
 * 
 * \param nIndex 
 */
void DObject::doToIndex( int nIndex )
{
    AWObject::doToIndex( nIndex );
    // do we really care what order the proxy is in? - not for DLayout anyway
    // although DLayout can call ADObject::doToIndex directly and skip this - hmmm
    // getProxy()->doToIndex( nIndex );
}

void DObject::doGoTo( ADObjectPathList listObjectPathItems, uint nDepth )
{
    DDiagram *p = (DDiagram*)getParent( "DDiagram" );
    p->doGoTo( this );

    p->slotSelectNone();
    p->getSelectionManager()->setSelected( this );

    ADObject::doGoTo( listObjectPathItems, nDepth );
}

void DObject::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    DDiagram *p = (DDiagram*)getParent( "DDiagram" );
    p->doGoTo( this );

    p->slotSelectNone();
    p->getSelectionManager()->setSelected( this );

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return;
    }

    //
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

/*!
 * \brief Reparent this object. 
 *  
 * We have to reparent the proxy as well. 
 *  
 * We DO NOT reparent the DObject using QObject::setParent because we want to emit 
 * some signals to notify the object browser. So we use ADObject::doChildRemove 
 * and ADObject::doChildAdd. 
 * 
 * \author pharvey (9/7/20)
 * 
 * \param pObjectParentNew 
 */
void DObject::doReparent( ADObject *pObjectParentNew )
{
    QObject *p = parent();
    Q_ASSERT( p );
    Q_ASSERT( p->inherits( "ADObject" ) );

    if ( !pObjectParentNew ) pObjectParentNew = getDiagram();

    // reparent self
    ADObject *pObjectParent = (ADObject*)p;
    pObjectParent->doChildRemove( this );
    pObjectParentNew->doChildAdd( this );
    // reparent proxy
    getProxy()->doReparent( pObjectParentNew );

    setModified();
}

void DObject::doGoTo()
{
    // recurse our way to root
    if ( parent() )
    { 
        AWObject *p = (AWObject*)parent();
        p->doGoTo();
    }

    // do what we can on the way back down
    DDiagram *p = (DDiagram*)getParent( "DDiagram" );
    p->doGoTo( this );
}

void DObject::setProxy( DGraphicsProxyItem *p )
{
    pProxy = p;
}

void DObject::setPos( const QPointF &pointScene )
{
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "class:" << metaObject()->className();
    pointPos = pointScene;
    if ( pProxy ) pProxy->setPos( pointScene );
}

// default is same as setPos() but derived classes may want something different
void DObject::setPosRaw( const QPointF &pointScene )
{
    pointPos = pointScene;
    if ( pProxy ) pProxy->setPos( pointScene );
}

void DObject::setPosDelta( const QPointF &pointDelta )
{
    setPos( pointPos + pointDelta );
}

void DObject::setPosDeltaRaw( const QPointF &pointDelta )
{
    setPosRaw( pointPos + pointDelta );
}

void DObject::setVisible( bool b ) 
{ 
    getProxy()->setVisible( b ); 
}

/*!
 * \brief Usual select stuff but lets ensure that the correct handles are shown.
 *  
 * A selected DObject has resize handles by default. 
 * The DDiagram can change this by calling setSelectType. 
 * We always go back to the default when we are unselected. 
 *  
 * \sa setSelectType 
 *  
 * \author pharvey (2/13/20)
 * 
 * \param b 
 */
void DObject::setSelected( bool b )
{
   if ( b == isSelected() ) return;

   if ( b )
   {
       if ( inLayout() || inGroup() ) nSelectType = SelectOnly;
       else nSelectType = SelectResize;
       doHandlesCreate();
   }
   else
   {
       doHandlesDelete();
       if ( inLayout() || inGroup() ) nSelectType = SelectOnly;
       else nSelectType = SelectResize;
   }

   AWObject::setSelected( b );
}

/*!
 * \brief Cycle select type.
 *  
 * Presumably called because of repeated single clicks on this. 
 *  
 * The default is to NOT support cycling the selection type. 
 *  
 * \author pharvey (2/15/20)
 */
void DObject::setSelectType()
{
   switch ( getSelectType() )
   {
        case SelectResize:
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectOnly:
        case SelectMenu:
            break;
    }
}

/*!
 * \brief Set the selection type. 
 *  
 * This is done, by DDiagram, independently of setSelect. 
 * The idea is that DDiagram can implement a scenario where selecting a DObject once 
 * provides resize handles, clicking it again causes the resize handles to go away and the 
 * transform handles to be shown. Clicking more cycles between the two. 
 *  
 * \note The default is SelectResize or SelectOnly and this is set whenever unselected. 
 *  
 * \sa setSelected 
 *  
 * \author pharvey (2/13/20)
 * 
 * \param n 
 */
void DObject::setSelectType( SelectTypes n )
{
    if ( n == nSelectType ) return;

    if ( isSelected() ) doHandlesDelete();
    nSelectType = n;
    if ( isSelected() ) doHandlesCreate();
}

void DObject::setTextDisplay( TextDisplayTypes n )
{
    if ( n == nTextDisplay ) return;
    nTextDisplay = n;
}

void DObject::contextMenu( QGraphicsSceneContextMenuEvent *pEvent )
{
    QMenu *pMenu = getContextMenu( nullptr );
    if ( !pMenu ) return;

    pMenu->exec( pEvent->screenPos() );

    delete pMenu;
    pEvent->setAccepted( true );
}

void DObject::contextMenu( DHandle *pHandle, QGraphicsSceneContextMenuEvent *pEvent )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pEvent);
}

/*!
 * \brief Delete dependent objects and then ourself.
 * 
 * \author pharvey (12/14/19)
 */
void DObject::slotDelete()
{
    // ADObject destructor will clear any refs but will not delete other objects.
    // In this case; we need to delete any DObjects which are using us. 
    // We take care of this here and then resume normal processing.
    QList<ADObjectReferenceKey> listKeys = getUsedByKeys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( Key.pObject->inherits( "DObject" ) )
        {
            Key.pObject->slotDelete(); 
        }
    }

    AWObject::slotDelete();
}

/*!
 * \brief Handle case when something we ref - has changed. 
 *  
 * We are not modified when this happens as we do not own that data. 
 *  
 * The default is to simply tell our proxy that we want to update our bit of the diagram.
 *  
 * \author pharvey (12/11/19)
 * 
 */
void DObject::slotModifiedReference( ADObject *pObject )
{
    // We only care if something has changed (ie saving does not matter).
    if ( !pObject->isModified() ) return;

    // Default: We care regardless of origin.
    // if ( pObject != this->pObject ) return;

    // update presentation...

    // diagram
    if ( getProxy() ) getProxy()->update();

    // fake modified status change to init ie tree widget item 
    emit signalModified(); 
}

void DObject::slotPen( const QPen &p ) 
{
    if ( p == pen ) return;
    pen = p;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalChanged( p );
}

void DObject::slotBrush( const QBrush &b ) 
{
    if ( b == brush ) return;
    brush = b;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalChanged( b );
}

void DObject::slotText( const CBD::CBDText &f ) 
{
    if ( f.font == font.font &&
         f.color == font.color &&
         f.brush == font.brush &&
         f.nHAlign == font.nHAlign &&
         f.nVAlign == font.nVAlign ) return;

    font = f;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalChanged( f );
}

void DObject::slotTextDisplay( int n )
{
    nTextDisplay = (TextDisplayTypes)n;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalTextDisplay( n );
}

/*!
 * \brief Init selection handles. 
 *  
 * Selection handles are created for each corner of the bounding rectangle. 
 * These handles can not be dragged by the User to manipulate the object - they 
 * only exist to indicate the object is selected. 
 *  
 * Examples; 
 *  
 * - when the object is being managed by a layout
 * - when the object has fixed geometry 
 *  
 * \author pharvey (10/5/20)
 */
void DObject::doInitSelect()
{
    Q_ASSERT( vectorHandles.count() == 0 );
    Q_ASSERT( getProxy() );

    vectorHandles.resize( 4 );

    QRectF rect = boundingRect();
    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionSelectTopLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topLeft() ) );
    vectorHandles[CBD::RectangleTopLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectTopRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    vectorHandles[CBD::RectangleTopRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomRight() ) );
    vectorHandles[CBD::RectangleBottomRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomLeft() ) );
    vectorHandles[CBD::RectangleBottomLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}

void DObject::doFiniSelect()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void DObject::doSyncSelect()
{
    Q_ASSERT( vectorHandles.count() > 0 );
    Q_ASSERT( getProxy() );

    QRectF rect = boundingRect();
    DHandle *pHandle;

    pHandle = vectorHandles[CBD::RectangleTopLeft];
    pHandle->setPos( getProxy()->mapToScene( rect.topLeft() ) );

    pHandle = vectorHandles[CBD::RectangleTopRight];
    pHandle->setPos( getProxy()->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[CBD::RectangleBottomRight];
    pHandle->setPos( getProxy()->mapToScene( rect.bottomRight() ) );

    pHandle = vectorHandles[CBD::RectangleBottomLeft];
    pHandle->setPos( getProxy()->mapToScene( rect.bottomLeft() ) );
}


