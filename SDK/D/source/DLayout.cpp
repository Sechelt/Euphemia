#include "LibInfo.h"
#include "DLayout.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

DLayout::DLayout( ADObject *pObjectParent, const QString &stringName )
    : DRectangleBase( pObjectParent, stringName )
{
    //
    pen.setColor( Qt::magenta );
    pHandleInsert = nullptr;

    // we want to know when a child object is deleted or reparented (simpler than overriding methods with the signals)
    connect( this, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotDeleted(ADObject*)) );
    connect( this, SIGNAL(signalChildRemoved(ADObject*)), this, SLOT(slotChildRemoved(ADObject*)) );
}

DLayout::~DLayout()
{
}

void DLayout::setPos( const QPointF &point )
{
    // do self
    DRectangleBase::setPos( point );
    // do content
    doGroupSync();
}

void DLayout::setSize( const QSizeF &size )
{
    // do self
    DRectangleBase::setSize( size );
    // do content
    doLayout();
}

void DLayout::setSelected( bool b )
{
   if ( b == isSelected() ) return;

   DRectangleBase::setSelected( b );

   // we must redraw self to hide/show outline
   getProxy()->update();
}

void DLayout::doGroupSync()
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

/*!
 * \brief Prevent sizeHint from being changed by a controller. 
 *  
 * The size hint is calculated, based upon the objects being managed, and cached in sizeHint.
 * 
 * \author pharvey (9/21/20)
 * 
 * \param size 
 */
void DLayout::slotSizeHint( const QSizeF &size )
{
    if ( !children().count() ) return DRectangleBase::slotSizeHint( size );

    // let controller know request failed
    emit signalChangedSizeHint( sizeHint );
}

/*!
 * \brief Prevent size constraints from being changed by a controller. 
 *  
 * Size constraints are calculated, based upon the objects being managed, and cached in sizeMinimum and sizeMaximum.
 * 
 * \author pharvey (9/21/20)
 * 
 * \param size 
 */
void DLayout::slotSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax )
{
    if ( !children().count() ) return DRectangleBase::slotSizeConstraints( sizeMin, sizeMax );

    // let controller know request failed
    emit signalChangedSizeConstraints( sizeMinimum, sizeMaximum );
}

/*!
 * \brief Captures when a child (a DObject we are managing) gets deleted.
 *   
 * Derived classes will want to do some house keeping.
 *  
 * Triggered by this::signalDeleted().
 *  
 * \author pharvey (9/9/20)
 */
void DLayout::slotDeleted( ADObject * )
{
}

/*!
 * \brief Captures when a child (a DObject we are managing) gets reparented.
 *  
 * Derived classes will want to do some house keeping.
 *  
 * Triggered by this::signalChildRemoved().
 *  
 * \author pharvey (9/9/20)
 */
void DLayout::slotChildRemoved( ADObject * )
{
}

void DLayout::slotChangedContent()
{
    doUpdateSelf();
    doLayout();
    emit signalChangedLayout();
}

void DLayout::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doResize( pHandle, pointScenePos );
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

void DLayout::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doInitResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

void DLayout::doHandlesDelete()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doFiniResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

void DLayout::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doSyncResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

/*!
 * \brief Create resize handles. 
 *  
 * We let DRectangleBase create the standard 4 handles but then change the function of 
 * the top-left handle to FunctionMove. We do this because a layout can get completely 
 * covered by the objects it manages. This means we can not move it in the usual way. 
 * 
 * \author pharvey (9/19/20)
 */
void DLayout::doInitResize()
{
    DRectangleBase::doInitResize();
    vectorHandles[CBD::RectangleTopLeft]->setFunction( DHandle::FunctionMove );
}


