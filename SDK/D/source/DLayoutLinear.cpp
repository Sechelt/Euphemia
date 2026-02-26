#include "LibInfo.h"
#include "DLayoutLinear.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

#define DLAYOUTCELL_MIN 48

DLayoutLinear::DLayoutLinear( ADObject *pObjectParent, const QString &stringName )
    : DLayout( pObjectParent, stringName )
{
    // init a single cell (we always have at least one cell even if its empty)
    QVector<DLayoutContent> l;
    l.append( DLayoutContent( nullptr, QRectF( 0, 0, DLAYOUTCELL_MIN, DLAYOUTCELL_MIN ) ) );
    vectorContents.append( l );
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

    // we take ownership of objects we manage
    ((DObject*)pObject)->doReparent( this );

    return pObject;
}

bool DLayoutLinear::doEnter( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_ASSERT( !pHandleInsert );

    doInitResize();

    QPointF             pointItem       = getProxy()->mapFromScene( pointScene );
    DLayoutContentIndex indexContent    = getIndex( pointItem );
    CBD::EdgeCenters    nEdge           = getEdge( indexContent, pointItem );
    QPointF             pointEdge       = getEdgePoint( indexContent, nEdge );

    pHandleInsert = new DHandle( this, DHandle::FunctionInsertLayout, DHandle::ConnectTypeNone, getProxy()->mapToScene( pointEdge ) );
    getProxy()->scene()->addItem( pHandleInsert );

    return true;
}

bool DLayoutLinear::doOver( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_ASSERT( pHandleInsert );

    QPointF             pointItem       = getProxy()->mapFromScene( pointScene );
    DLayoutContentIndex indexContent    = getIndex( pointItem );
    CBD::EdgeCenters    nEdge           = getEdge( indexContent, pointItem );
    QPointF             pointEdge       = getEdgePoint( indexContent, nEdge );

    pHandleInsert->setPos( getProxy()->mapToScene( pointEdge ) );

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
    QPointF             pointItem       = getProxy()->mapFromScene( pointScene );
    DLayoutContentIndex indexContent    = getIndex( pointItem );
    CBD::EdgeCenters    nEdge           = getEdge( indexContent, pointItem );
    bool                b;

    if ( nEdge == CBD::EdgeCenterCenter ) 
        b = doSet( p, indexContent );
    else
        b = doInsert( p, indexContent, nEdge );

    // change selection from selection with resize
    getSelectionManager()->setSelected( p, false );
    getSelectionManager()->setSelected( p, true );

    if ( pHandleInsert )
    {
        delete pHandleInsert;
        pHandleInsert = nullptr;
        doFiniResize();
    }

    return b;
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

    QStringList sl = stringOrder.split( '|', Qt::SkipEmptyParts );
    QString s;
    vectorContents.clear();
    foreach( s, sl )
    {
        int nOID = s.toInt();
        ADObject *p = ADObject::getObject( nOID );
        Q_ASSERT(p);
        Q_ASSERT( p->inherits( "DRectangleBase" ) );
        // add to layout - without firing off signals - we do not want calls to doLayout etc
        DRectangleBase *pRectangleBase = (DRectangleBase*)p;
        vectorContents.append( DLayoutContent( pRectangleBase ) );
        connect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    }

    doInitLayout();
    doLayout();

    return true;
}

/*!
 * \brief Determine if contents is empty.
 * 
 * The contents is empty when there is just one cell and it has no pObject in it.
 * 
 * \note There will always be at least 1 row and 1 col (one cell).
 * \note Empty cells are not allowed except when there is only one cell.
 * 
 * \author pharvey (2026-02-21)
 * 
 * \return bool 
 */
bool DLayoutLinear::isEmpty()
{
    return ( isSingleCell() && vectorContents[0].pObject == nullptr );
}

/*!
 * \brief Determine if contents has a single cell.
 * 
 * \author pharvey (2026-02-23)
 * 
 * \return bool 
 */
bool DLayoutLinear::isSingleCell()
{
    return ( vectorContents.size() == 1 );
}

/*!
 * \brief Remove object.
 * 
 * \author pharvey (2026-02-19)
 * 
 * \param p      
 */
void DLayoutLinear::slotChildRemoved( ADObject *p )
{
    Q_ASSERT( p );
    Q_ASSERT( p->inherits( "DRectangleBase" ) );

    DRectangleBase *pRectangleBase = (DRectangleBase*)p;

    // remove from our list
    DLayoutContentIndex indexContent = getIndex( (DRectangleBase*)p );
    Q_ASSERT( !indexContent.isNull() );
    disconnect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );

    if ( isSingleCell() )
        vectorContents[0].pObject = nullptr;
    else
        vectorContents.remove( getIndex( indexContent ) );

    doInitLayout();
    doLayout();
    emit signalChangedLayout(); 
}

/*!
 * \brief Get index at given point.
 * 
 * \author pharvey (9/24/20)
 * 
 * \param pointItem 
 * 
 * \return DLayoutContentIndex 
 */
DLayoutContentIndex DLayoutLinear::getIndex( const QPointF &pointItem )
{
//    QPointF pointItem = getProxy()->mapFromScene( pointScene );                                         

    for ( int nIndex = 0; nIndex < vectorContents.count(); nIndex++ )
    {
        if ( vectorContents.at( nIndex ).rect.contains( pointItem ) ) return DLayoutContentIndex( nIndex, 0 );
    }

    return DLayoutContentIndex(); ///< null
}

/*!
 * \brief Get index to given object.
 *  
 * Scans vectorContents looking for object. Returns index which can be null.
 *  
 * \author pharvey (9/25/20)
 * 
 * \param p 
 * 
 * \return DLayoutContentIndex 
 */
DLayoutContentIndex DLayoutLinear::getIndex( DRectangleBase *p )
{
    for ( int nIndex = 0; nIndex < vectorContents.count(); nIndex++ )
    {
        if ( vectorContents.at( nIndex ).pObject == p ) return DLayoutContentIndex( nIndex, 0 );
    }

    return DLayoutContentIndex();
}

/*!
 * \brief Set the cell object.
 * 
 * In this class it is only valid to call this when single cell with no object.
 * 
 * \author pharvey (2026-02-21)
 * 
 * \param p         
 * \param indexCell 
 * 
 * \return bool 
 */
bool DLayoutLinear::doSet( DRectangleBase *p, DLayoutContentIndex indexCell )
{
    // this not for removing content
    Q_ASSERT( p );
    // we can not set when an object is already in the cell
    Q_ASSERT( vectorContents[indexCell.nRow].pObject == nullptr );
    // we take ownership of objects we manage
    p->doReparent( this );
    // set 
    vectorContents[getIndex( indexCell )] = DLayoutContent( p );
    connect( p, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    // adjust layout
    doInitLayout();
    doLayout();

    emit signalChangedLayout();

    return true;
}

void DLayoutLinear::doRemove( DRectangleBase *p )
{
    Q_ASSERT( p );

    // ie. being dragged from layout
    // this will trigger our slotChildRemoved
    p->doReparent( nullptr );
}



