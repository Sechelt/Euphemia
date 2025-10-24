#include "LibInfo.h"
#include "DGroup.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

DGroup::DGroup( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
    //
    pen.setColor( Qt::magenta );
}

DGroup::~DGroup()
{
}

void DGroup::setPos( const QPointF &point )
{
    // do self
    DRectangle::setPos( point );
    // do content
    doGroupSync();
}

void DGroup::setSize( const QSizeF &size )
{
    // do self
    QSizeF sizeFrom = getSize();
    DRectangleBase::setSize( size );
    // do content
    QSizeF sizeTo = getSize();
    if ( sizeFrom == sizeTo ) return;
    QSizeF sizeFactor( sizeTo.width() / sizeFrom.width(), sizeTo.height() / sizeFrom.height() );

    doScale( sizeFactor );
}

void DGroup::setSelected( bool b )
{
   if ( b == isSelected() ) return;

   DRectangle::setSelected( b );

   // we must redraw self to hide/show outline
   getProxy()->update();
}

QPixmap DGroup::getIcon()
{
    return QPixmap( ":D/GroupLayout" );
}

/*!
 * \brief Supports doLoad().
 *  
 * We rely upon DDiagram to create child objects - but we ensure that we are the parent. 
 *  
 * \author pharvey (9/10/20)
 * 
 * \param stringClass 
 * 
 * \return ADObject* 
 */
ADObject *DGroup::getObject( const QString &stringClass, ADObject * )
{
    DDiagram *pDiagram = getDiagram();
    ADObject *pObject = pDiagram->getObject( stringClass, this );
    if ( !pObject ) return nullptr;

    return pObject;
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
bool DGroup::doGroup( QList<ADObject*> l )
{
#pragma message ( "We need permanent OID's (for a variety of reasons). Probably done in copy or paste?" )

    //
    if ( isGrouped() )
    {
        doMessageBox( "WARNING", tr("Group..."), tr("Already have content.") );
        return false;
    }

    // add objects
    QRectF rectBounding;
    ADObject *p;                                            
    foreach ( p, l )                                        
    {
        // must be a top level
        if ( !p->parent()->inherits( "DDiagram" ) ) continue;
        // only DIObjects
        if ( !p->inherits( "DObject" ) ) continue;
        DObject *pObject = (DObject*)p;
        // is it a line?
        if ( p->inherits( "DLine" ) )
        {
            DLine *pLine = (DLine*)pObject;
            DRectangle *pRectangle;
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
        DRectangleBase::setSize( rectBounding.size() );
    }

    // reposition rectangles
    foreach ( p, l )                                        
    {
        if ( !p->inherits( "DRectangleBase" ) ) continue; 
        DRectangleBase *pRectangle = (DRectangleBase*)p;
        pRectangle->doGroupIn( getPos() );
    }

    // reposition simple (straight, poly, single-90, etc) lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // reposition simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupIn( getPos() );
    }

    // reposition auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
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
void DGroup::doUngroup()
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
        Q_ASSERT( p->inherits( "DObject" ) );
        ((DObject*)p)->doReparent( nullptr );
    }

    // reposition rectangles
    foreach ( p, l )                                        
    {
        if ( !p->inherits( "DRectangleBase" ) ) continue; 
        DRectangleBase *pRectangle = (DRectangleBase*)p;
        pRectangle->doGroupOut( getPos() );
    }

    // reposition simple (straight, poly, single-90, etc) lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // reposition simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupOut( getPos() );
    }

    // reposition auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
        if ( !pLine->isAutoRouted() ) continue;

        // reposition auto routed line means adjusting any END point which is NOT connected
        // and invoke auto route
        pLine->doGroupOut( getPos() );
    }
}

void DGroup::doGroupSync()
{
    QObjectList l = children();
    QObject *p;

    foreach ( p, l )                                        
    {
        if ( !p->inherits( "DObject" ) ) continue; 
        DObject *pObject = (DObject*)p;
        pObject->doGroupSync();
    }
}

QDomElement DGroup::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangle::doSave( pdomDoc, pdomElemParent );

    return domElemThis;
}

bool DGroup::doLoad( QDomElement *pdomElemObject )
{
    DRectangle::doLoad( pdomElemObject );

    return true;
}

/*!
 * \brief Paint self. 
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
void DGroup::paintAt( QPainter *pPainter, const QPointF &pointPos )
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
void DGroup::doInitResize()
{
    DRectangleBase::doInitResize();
    vectorHandles[CBD::RectangleTopLeft]->setFunction( DHandle::FunctionMove );
}

/*!
 * \brief Scale contents.
 * 
 * \author pharvey (10/31/20)
 * 
 * \param sizeFrom 
 */
void DGroup::doScale( const QSizeF &sizeFactor )
{
    QObjectList l = children();
    QObject *p;

    // scale rectangles
    foreach( p, l )
    {
        // scale rectangles by adjusting pos and size
        if ( !p->inherits( "DRectangleBase" ) ) continue;

        DRectangleBase *pRectangle = (DRectangleBase*)p;
        pRectangle->doGroupScale( sizeFactor );
    }

    // scale simple (straight, poly, single-90, etc) lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
        if ( pLine->isAutoRouted() ) continue;

        // scale simple lines means adjusting ALL points which are NOT connected
        pLine->doGroupScale( sizeFactor );
    }

    // scale auto route lines
    // we do these last because they may want to route around the rectangles and avoid crossing other lines
    foreach( p, l )
    {
        if ( !p->inherits( "DLine" ) ) continue;
        DLine *pLine = (DLine*)p;
        if ( !pLine->isAutoRouted() ) continue;

        // scale auto routed line means adjusting any END point which is NOT connected
        // and invoke auto route
        pLine->doGroupScale( sizeFactor );
    }
}

