#include "LibInfo.h"
#include "DLayoutLinear.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

DLayoutLinear::DLayoutLinear( ADObject *pObjectParent, const QString &stringName )
    : DLayout( pObjectParent, stringName )
{
}

DLayoutLinear::~DLayoutLinear()
{
}

/*!
 * \brief Supports doLoad().
 *  
 * We rely upon DDiagram to create child objects - but we ensure that we are the parent. 
 * We also ensure that we are only creating children which are derived from DRectangleBase. 
 *  
 * \author pharvey (9/10/20)
 * 
 * \param stringClass 
 * 
 * \return ADObject* 
 */
ADObject *DLayoutLinear::getObject( const QString &stringClass, ADObject * )
{
    DDiagram *pDiagram = getDiagram();
    ADObject *pObject = pDiagram->getObject( stringClass, this );
    if ( !pObject ) return nullptr;
    Q_ASSERT( pObject->inherits( "DRectangleBase" ) );

    return pObject;
}

bool DLayoutLinear::doEnter( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_ASSERT( !pHandleInsert );

    doInitResize();

    QPointF pointEdge = getEdge( pointScene );

    pHandleInsert = new DHandle( this, DHandle::FunctionInsertLayout, DHandle::ConnectTypeNone, pointEdge );
    getProxy()->scene()->addItem( pHandleInsert );

    return true;
}

bool DLayoutLinear::doOver( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_ASSERT( pHandleInsert );

    pHandleInsert->setPos( getEdge( pointScene ) );

    return true;
}

bool DLayoutLinear::doExit( DRectangleBase *p )
{
    Q_UNUSED( p );
    Q_ASSERT( pHandleInsert );

    delete pHandleInsert;
    pHandleInsert = nullptr;

    doFiniResize();

    return true;
}

void DLayoutLinear::doDrag( DRectangleBase *p )
{
    doRemove( p );
    // change selection to selection with resize
    getSelectionManager()->setSelected( p, false );
    getSelectionManager()->setSelected( p, true );
}

bool DLayoutLinear::doDrop( DRectangleBase *p, const QPointF &pointScene )
{
    doInsert( p, indexOf( pointScene ) );

    // change selection from selection with resize
    getSelectionManager()->setSelected( p, false );
    getSelectionManager()->setSelected( p, true );

    if ( pHandleInsert )
    {
        delete pHandleInsert;
        pHandleInsert = nullptr;

        doFiniResize();
    }

    return true;
}

QDomElement DLayoutLinear::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangleBase::doSave( pdomDoc, pdomElemParent );

    // Any temp OID's should have been turned to OID's for persistence at this point.
    // Create a string with OID's in the correct order and delimited. 
    QString stringOrder;
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        if ( !stringOrder.isEmpty() ) stringOrder += "|";
        stringOrder += QString::number( vectorContents.at( n ).pObject->getOID() );
    }

    // save child OID's/order
    domElemThis.setAttribute( "Order", stringOrder );

    return domElemThis;
}

bool DLayoutLinear::doLoad( QDomElement *pdomElemObject )
{
    DRectangleBase::doLoad( pdomElemObject );

    // At this point any objects we are managing have been created and are
    // a child of this layout. This was done, in part, by this layout
    // intercepting getObject and redirecting request to DDiagram::getObject.
    // 
    // Here we use the Order we saved in doSave to add the objects to the 
    // layout fully - in the order specified.

    QString stringOrder = pdomElemObject->attribute( "Order", "" );

#if QT_VERSION < 0x060000
    QStringList sl = stringOrder.split( '|', QString::SkipEmptyParts );
#else
    QStringList sl = stringOrder.split( '|', Qt::SkipEmptyParts );
#endif
    QString s;
    vectorContents.clear();
    foreach( s, sl )
    {
        int nOID = s.toInt();
        ADObject *p = ADObject::getObject( nOID );
        Q_ASSERT( p );
        Q_ASSERT( p->inherits( "DRectangleBase" ) );
        // add to layout - without firing off signals - we do not want calls to doLayout etc
        DRectangleBase *pRectangleBase = (DRectangleBase*)p;
        vectorContents.append( DLayoutCell( pRectangleBase ) );
        connect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    }

    // update presentation - we should not have to do this as load restores state
    // doUpdateSelf();
    // we do this to calc layout cell rects to be used during paint()
    doLayout();

    return true;
}

/*!
 * \brief Remove the object. 
 *  
 * Respond to signalDeleted (one of our children has been deleted) by removing 
 * from this layout. 
 * 
 * \author pharvey (9/25/20)
 * 
 * \param p 
 */
void DLayoutLinear::slotDeleted( ADObject *p )
{
    Q_ASSERT( p );

    // avoid accessing the object
    // Q_ASSERT( p->inherits( "DRectangleBase" ) );

    // remove from our list
    int nIndex = indexOf( (DRectangleBase*)p );
    Q_ASSERT( nIndex >= 0 );
    vectorContents.remove( nIndex );

    // update presentation
    doUpdateSelf();
    doLayout();
    emit signalChangedLayout(); 
}

void DLayoutLinear::slotChildRemoved( ADObject *p )
{
    Q_ASSERT( p );
    Q_ASSERT( p->inherits( "DRectangleBase" ) );

    DRectangleBase *pRectangleBase = (DRectangleBase*)p;

    // remove from our list
    int nIndex = indexOf( (DRectangleBase*)p );
    Q_ASSERT( nIndex >= 0 );
    vectorContents.remove( nIndex );

    // we no longer need to know when its layout details change 
    disconnect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );

    // update presentation
    doUpdateSelf();
    doLayout();
    emit signalChangedLayout(); 
}

/*!
 * \brief Return index of cell containing object.
 *  
 * Scans vectorContents looking for object. Returns index or, if not found, -1. 
 *  
 * \author pharvey (9/25/20)
 * 
 * \param p 
 * 
 * \return int 
 */
int DLayoutLinear::indexOf( DRectangleBase *p )
{
    for ( int nIndex = 0; nIndex < vectorContents.count(); nIndex++ )
    {
        if ( vectorContents.at( nIndex ).pObject == p ) return nIndex;
    }

    return -1;
}

void DLayoutLinear::doInsert( DRectangleBase *p, int nIndex )
{
    Q_ASSERT( p );

    // we take ownership of objects we manage
    p->doReparent( this );
    // insert to desired index
    Q_ASSERT( nIndex >= 0 );
    Q_ASSERT( nIndex <= vectorContents.count() );
    vectorContents.insert( nIndex, DLayoutCell( p ) );
    // update our geometry based upon content
    doUpdateSelf();
    // we may need to be larger to fit content
    QSizeF size = getSize();
    if ( sizeMinimum.width() > size.width() ) size.setWidth( sizeMinimum.width() );
    if ( sizeMinimum.height() > size.height() ) size.setHeight( sizeMinimum.height() );
    if ( size != getSize() )
        setSize( size ); // this will resize self and call doLayout
    else 
        doLayout();

    emit signalChangedLayout();

    return;
}

void DLayoutLinear::doRemove( DRectangleBase *p )
{
    Q_ASSERT( p );

    // ie. being dragged from layout
    // this will trigger our slotChildRemoved
    p->doReparent( nullptr );
}


