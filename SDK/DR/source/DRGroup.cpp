#include "LibInfo.h"
#include "DRGroup.h"

#include "DRDraw.h"

DRGroup::DRGroup( QGraphicsObject *pObjectParent, const QString &stringName )
    : DRRectangle( pObjectParent, stringName )
{
    //
    pen.setColor( Qt::magenta );
}

DRGroup::~DRGroup()
{
}

void DRGroup::setPos( const QPointF &point )
{
    // do self
    DRRectangle::setPos( point );
    // do content
    doGroupSync();
}

void DRGroup::setSize( const QSizeF &size )
{
    // do self
    QSizeF sizeFrom = getSize();
    DRRectangleBase::setSize( size );
    // do content
    QSizeF sizeTo = getSize();
    if ( sizeFrom == sizeTo ) return;
    QSizeF sizeFactor( sizeTo.width() / sizeFrom.width(), sizeTo.height() / sizeFrom.height() );

    doScale( sizeFactor );
}

void DRGroup::setSelected( bool b )
{
   if ( b == isSelected() ) return;

   DRRectangle::setSelected( b );

   // we must redraw self to hide/show outline
   update();
}

QPixmap DRGroup::getIcon()
{
    return QPixmap( ":DR/Group" );
}

QImage DRGroup::getImage()
{
#ifdef CBD_TODO
    qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] ToDo";
#endif
    return QImage();
}

/*!
 * \brief Supports doLoad().
 *  
 * We rely upon DRDraw to create child objects - but we ensure that we are the parent. 
 *  
 * \author pharvey (9/10/20)
 * 
 * \param stringClass 
 * 
 * \return DRObject* 
 */
DRObject *DRGroup::getObject( const QString &stringClass, DRObject * )
{
    Q_UNUSED( stringClass );
//    DRObject *pObject = pLayer->getObject( stringClass, this );
//    if ( !pObject ) return nullptr;
//
//    return pObject;
#ifdef CBD_TODO
    qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] ToDo";
#endif
    return nullptr;
}

/*!
 * \brief Add objects. 
 *  
 * We take ownership (reparent) objects. 
 * Any lines included and connected to object not included - will be disconnected from that object. 
 *  
 * \author pharvey (10/14/20)
 * 
 * \param l 
 * 
 * \return bool 
 */
bool DRGroup::doGroup( QList<DRObject*> l )
{
#pragma message ( "We need permanent OID's (for a variety of reasons). Probably done in copy or paste?" )

    //
    if ( isGrouped() )
    {
        doMessageBox( "WARNING", tr("Group..."), tr("Already has content.") );
        return false;
    }

    // add objects
    QRectF rectBounding;
    DRObject *pObject;                                            
    foreach ( pObject, l )                                        
    {
        // must be a top level
        if ( !pObject->parent()->inherits( "DRDraw" ) ) continue;
        // is it a line?
        if ( pObject->inherits( "DRLine" ) )
        {
            DRLine *pLine = (DRLine*)pObject;
            DRRectangle *pRectangle;
            // we may need to disconnect line
            pRectangle = pLine->getRectangle( CBD::EOLBegin );
            if ( pRectangle && !l.contains( pRectangle ) ) pLine->doDisconnect( CBD::EOLBegin );
            pRectangle = pLine->getRectangle( CBD::EOLEnd );
            if ( pRectangle && !l.contains( pRectangle ) ) pLine->doDisconnect( CBD::EOLEnd );
        }
        //
        QRectF rectBoundingObject = pObject->getProxy()->mapRectToScene( pObject->boundingRect() );
        if ( rectBounding.isNull() ) rectBounding = rectBoundingObject;
        else rectBounding = rectBounding.united( rectBoundingObject );

        // we take ownership of objects we manage
        pObject->doReparent( this );
    }

    // adopt bounding rect
    if ( !rectBounding.isNull() ) 
    {
        setPos( rectBounding.topLeft() );
        DRRectangleBase::setSize( rectBounding.size() );
    }

    // reposition rectangles
    foreach ( pObject, l )                                        
    {
        if ( !pObject->inherits( "DRRectangleBase" ) ) continue; 
        DRRectangleBase *pRectangle = (DRRectangleBase*)p;
        pRectangle->doGroupIn( getPos() );
    }

    // reposition simple (straight, poly, single-90, etc) lines
    foreach( pObject, l )
    {
        if ( !pObject->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // reposition simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupIn( getPos() );
    }

    // reposition auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( pObject, l )
    {
        if ( !pObject->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( !pLine->isAutoRouted() ) continue;

        // reposition auto routed line means adjusting any END point which is NOT connected
        // and invoke auto route
        pLine->doGroupIn( getPos() );
    }

    return true;
}

/*!
 * \brief Ungroup all child objects. 
 *  
 * We restore the objects using the XML we captured in doGroup. 
 *  
 * \author pharvey (10/6/20)
 */
void DRGroup::doUngroup()
{
    if ( !isGrouped() )
    {
        doMessageBox( "WARNING", tr("Group..."), tr("Does not have content.") );
        return;
    }

    QObjectList l = children();
    QObject *p;
    foreach( p, l )
    {
        Q_ASSERT( p->inherits( "DRObject" ) );
        ((DRObject*)p)->doReparent( nullptr );
    }

    // reposition rectangles
    foreach ( p, l )                                        
    {
        if ( !p->inherits( "DRRectangleBase" ) ) continue; 
        DRRectangleBase *pRectangle = (DRRectangleBase*)p;
        pRectangle->doGroupOut( getPos() );
    }

    // reposition simple (straight, poly, single-90, etc) lines
    foreach( p, l )
    {
        if ( !p->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // reposition simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupOut( getPos() );
    }

    // reposition auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( p, l )
    {
        if ( !p->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( !pLine->isAutoRouted() ) continue;

        // reposition auto routed line means adjusting any END point which is NOT connected
        // and invoke auto route
        pLine->doGroupOut( getPos() );
    }
}

void DRGroup::doGroupSync()
{
    QObjectList l = children();
    QObject *p;

    foreach ( p, l )                                        
    {
        if ( !p->inherits( "DRObject" ) ) continue; 
        DRObject *pObject = (DRObject*)p;
        pObject->doGroupSync();
    }
}

QDomElement DRGroup::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRRectangle::doSave( pdomDoc, pdomElemParent );

    return domElemThis;
}

bool DRGroup::doLoad( QDomElement *pdomElemObject )
{
    DRRectangle::doLoad( pdomElemObject );

    return true;
}

/*!
 * \brief Draw self. 
 *  
 * We do not have a presentation on the diagram unless selected. 
 * When selected - we just draw a an empty rect in our pen colour. 
 * 
 * \author pharvey (10/2/20)
 * 
 * \param pPainter 
 * \param pOption 
 * \param pWidget 
 */
void DRGroup::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    if ( !isSelected() ) return;

    pPainter->setPen(pen);

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    pPainter->drawRect( rect );
}

/*!
 * \brief Create resize handles. 
 *  
 * Here we change the purpose of the topLeft handle because we want it to be used to move the entire group. 
 * This is done, in part, to be consistent with \sa DLayout. 
 * 
 * \author pharvey (11/10/20)
 */
void DRGroup::doInitResize()
{
    DRRectangleBase::doInitResize();
    vectorHandles[CBD::RectangleTopLeft]->setFunction( LHandle::FunctionMove );
}

/*!
 * \brief Scale contents.
 * 
 * \author pharvey (10/31/20)
 * 
 * \param sizeFrom 
 */
void DRGroup::doScale( const QSizeF &sizeFactor )
{
    QObjectList l = children();
    QObject *p;

    // scale rectangles
    foreach( p, l )
    {
        // scale rectangles by adjusting pos and size
        if ( !p->inherits( "DRRectangleBase" ) ) continue;

        DRRectangleBase *pRectangle = (DRRectangleBase*)p;
        pRectangle->doGroupScale( sizeFactor );
    }

    // scale simple (straight, poly, single-90, etc) lines
    foreach( p, l )
    {
        if ( !p->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // scale simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupScale( sizeFactor );
    }

    // scale auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( p, l )
    {
        if ( !p->inherits( "DRLine" ) ) continue;
        DRLine *pLine = (DRLine*)p;
        if ( !pLine->isAutoRouted() ) continue;

        // scale auto routed line means adjusting any END point which is NOT connected
        // and invoke auto route
        pLine->doGroupScale( sizeFactor );
    }
}

