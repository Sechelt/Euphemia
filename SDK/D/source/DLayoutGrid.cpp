#include "LibInfo.h"
#include "DLayoutGrid.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

#define DLAYOUTGRID_MIN 48

/*!
 * \brief Grid layout.
 * 
 * Lays out objects in a grid.
 * The top left cell is 0,0.
 * All cells in a row have a common height.
 * All cells in a col have a common width.
 * Rows/Cols with all empty cells are removed unless the grid has only one cell.
 * There is always at least one cell.
 * 
 * \author pharvey (2026-02-25)
 * 
 * \param pObjectParent 
 * \param stringName    
 */
DLayoutGrid::DLayoutGrid( ADObject *pObjectParent, const QString &stringName )
    : DLayout( pObjectParent, stringName )
{
    // init a single cell (we always have at least one cell even if its empty)
    QVector<DLayoutContent> l;
    l.append( DLayoutContent( nullptr, QRectF( 0, 0, DLAYOUTGRID_MIN, DLAYOUTGRID_MIN ) ) );
    vectorContents.append( l );

    DLayoutGeometry geometry;
    geometry.nMinimum   = DLAYOUTGRID_MIN;
    geometry.nHint      = DLAYOUTGRID_MIN;
    vectorRowGeometries.append( geometry );

    geometry.nMinimum   = DLAYOUTGRID_MIN;
    geometry.nHint      = DLAYOUTGRID_MIN;
    vectorColGeometries.append( geometry );
}

DLayoutGrid::~DLayoutGrid()
{
}

QPixmap DLayoutGrid::getIcon()
{
    return QPixmap( ":D/GLayout" );
}

/*!
 * \brief This gets called during a load to create child objects.
 * 
 * We do not create any children - we get them from the diagram and then reparent.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param stringClass 
 * 
 * \return ADObject* 
 */
ADObject *DLayoutGrid::getObject( const QString &stringClass, ADObject * )
{
    DDiagram *pDiagram = getDiagram();
    ADObject *pObject = pDiagram->getObject( stringClass, this );
    if ( !pObject ) return nullptr;
    Q_ASSERT( pObject->inherits( "DRectangleBase" ) );

    // we take ownership of objects we manage
    ((DObject*)pObject)->doReparent( this );

    return pObject;
}

/*!
 * \brief Paint ourself at the given pos.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param pPainter Euphemia/Apps/Widgets/Widgets.pro ) && make -f Makefile 
 * \param pointPos 
 */
void DLayoutGrid::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    int nRows = getRows();
    int nCols = getCols();

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    // draw our rect (the outer rect)
    pPainter->drawRect( rect );

    // draw rows (top line)
    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        QRectF r = vectorContents[nRow][0].rect;
        pPainter->drawLine( rect.left(), r.top(), rect.right(), r.top() );
    }

    // draw cols (left line)
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        QRectF r = vectorContents[0][nCol].rect;
        pPainter->drawLine( r.left(), rect.top(), r.left(), rect.bottom() );
    }
}

/*!
 * \brief The diagram calls this when a rectangle based object is being dragged and has just entered this layout.
 * 
 * We show a relevant handle to indicate target for drop.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param p          
 * \param pointScene 
 * 
 * \return bool 
 */
bool DLayoutGrid::doEnter( DRectangleBase *p, const QPointF &pointScene )
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

/*!
 * \brief The diagram calls this when a rectangle based object is being dragged over this layout.
 * 
 * We update handle to indicate target for drop.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param p          
 * \param pointScene 
 * 
 * \return bool 
 */
bool DLayoutGrid::doOver( DRectangleBase *p, const QPointF &pointScene )
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

/*!
 * \brief The diagram calls this when a rectangle based object is being dragged and has just exited this layout.
 * 
 * We get rid of the handle.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param p          
 * \param pointScene 
 * 
 * \return bool 
 */
bool DLayoutGrid::doExit( DRectangleBase *p )
{
    Q_UNUSED( p );
    Q_ASSERT( pHandleInsert );

    delete pHandleInsert;
    pHandleInsert = nullptr;

    doFiniResize();

    return true;
}

/*!
 * \brief This is called when the object of a cell is starting to drag.
 * 
 * We remove the object from the cell.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param p      
 */
void DLayoutGrid::doDrag( DRectangleBase *p )
{
    doRemove( p );
    // change selection to selection with resize
    getSelectionManager()->setSelected( p, false );
    getSelectionManager()->setSelected( p, true );
}

/*!
 * \brief This is called when the rectangle based object is dropped on this layout.
 * 
 * We insert the object into the layout.
 * 
 * \author pharvey (2025-11-16)
 * 
 * \param p      
 */
bool DLayoutGrid::doDrop( DRectangleBase *p, const QPointF &pointScene )
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

QDomElement DLayoutGrid::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangleBase::doSave( pdomDoc, pdomElemParent );
/*
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
*/
    return domElemThis;
}

bool DLayoutGrid::doLoad( QDomElement *pdomElemObject )
{
    DRectangleBase::doLoad( pdomElemObject );
/*
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
        vectorContents.append( DRectangleBase *( pRectangleBase ) );
        connect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    }
*/
    // update presentation - we should not have to do this as load restores state
    // doInitLayout();
    // we do this to calc layout cell rects to be used during paint()
    doLayout();

    return true;
}

/*!
 * \brief Determine if contents is empty.
 * 
 * The contents is empty when there is just one cell and it has no pObject in it.
 * 
 * \note There will always be at least 1 row and 1 col (one cell).
 * \note Empty cells are allowed but an empty row/col is not allowed except when there is only one cell.
 * 
 * \author pharvey (2026-02-21)
 * 
 * \return bool 
 */
bool DLayoutGrid::isEmpty()
{
    return ( isSingleCell() && vectorContents[0][0].pObject == nullptr );
}

/*!
 * \brief Determine if contents has a single cell.
 * 
 * \author pharvey (2026-02-23)
 * 
 * \return bool 
 */
bool DLayoutGrid::isSingleCell()
{
    return ( getRows() == 1 && getCols() == 1 );
}

/*!
 * \brief Remove object.
 * 
 * This is same as \sa slotDeleted except also does disconnect.
 * 
 * \author pharvey (2026-02-19)
 * 
 * \param p      
 */
void DLayoutGrid::slotChildRemoved( ADObject *p )
{
    Q_ASSERT( p );
    Q_ASSERT( p->inherits( "DRectangleBase" ) );

    DRectangleBase *pRectangleBase = (DRectangleBase*)p;

    // remove from our content
    DLayoutContentIndex indexCell = getIndex( (DRectangleBase*)p );
    Q_ASSERT( !indexCell.isNull() );
    disconnect( pRectangleBase, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );

    vectorContents[indexCell.nRow][indexCell.nCol].pObject = nullptr;

    // remove empty row/col
    if ( !isSingleCell() && isRowEmpty( indexCell.nRow ) ) doRowDelete( indexCell.nRow );
    if ( !isSingleCell() && isColEmpty( indexCell.nCol ) ) doColDelete( indexCell.nCol );

    // update presentation
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
DLayoutContentIndex DLayoutGrid::getIndex( const QPointF &pointItem )
{
    Q_ASSERT( rect.contains( pointItem ) );

    DLayoutContentIndex indexCell;

    // find row...
    int nRows = getRows();
    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        if ( pointItem.y() <= vectorContents[nRow][0].rect.bottom() )
        {
            indexCell.nRow = nRow;
            break;
        }
    }

    // find col...
    int nCols = getCols();
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        if ( pointItem.x() <= vectorContents[0][nCol].rect.right() )
        {
            indexCell.nCol = nCol;
            break;
        }
    }

    return indexCell;
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
DLayoutContentIndex DLayoutGrid::getIndex( DRectangleBase *p )
{
    int nRows = getRows();
    int nCols = getCols();

    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        for ( int nCol = 0; nCol < nCols; nCol++ )
        {
            if ( vectorContents[nRow][nCol].pObject == p ) return DLayoutContentIndex( nRow, nCol );
        }
    }

    return DLayoutContentIndex();
}

/*!
 * \brief Get edge for drop indicator.
 * 
 * \author pharvey (2026-02-20)
 * 
 * \param indexContent 
 * \param pointItem 
 * 
 * \return CBD::EdgeCenters 
 */
CBD::EdgeCenters DLayoutGrid::getEdge( const DLayoutContentIndex &indexContent, const QPointF &pointItem )
{
    // Q_ASSERT( !indexContent.isNull() );

    // force center if we are single cell and empty
    if ( isEmpty() ) return CBD::EdgeCenterCenter;

    QRectF r = vectorContents[indexContent.nRow][indexContent.nCol].rect;
    Q_ASSERT( vectorContents[indexContent.nRow][indexContent.nCol].rect.contains( pointItem ) );

    // calc proximity
    QVector<qreal> vectorEdgeProximity;
    vectorEdgeProximity.resize( 5, 0 );
    vectorEdgeProximity[CBD::EdgeCenterLeft]    = pointItem.x() - r.left();
    vectorEdgeProximity[CBD::EdgeCenterRight]   = r.right() - pointItem.x();
    vectorEdgeProximity[CBD::EdgeCenterTop]     = pointItem.y() - r.top();
    vectorEdgeProximity[CBD::EdgeCenterBottom]  = r.bottom() - pointItem.y();
    vectorEdgeProximity[CBD::EdgeCenterCenter]  = qMax( qAbs( r.center().x() - pointItem.x() ), qAbs( r.center().y() - pointItem.y() ) );
    // determine which is closest
    CBD::EdgeCenters nEdge = CBD::EdgeCenterLeft;
    for ( int n = 0; n < 5; n++ )
    {
        // skip center when cell occupied
        if ( n == CBD::EdgeCenterCenter && vectorContents[indexContent.nRow][indexContent.nCol].pObject ) continue;
        if ( vectorEdgeProximity[n] < vectorEdgeProximity[nEdge] ) nEdge = (CBD::EdgeCenters)n;
    }

    return nEdge;
}

/*!
 * \brief Get point (in item coordinates) to place drop indicator.
 * 
 * \author pharvey (2026-02-20)
 * 
 * \param indexContent 
 * \param nEdge        
 * 
 * \return QPointF 
 */
QPointF DLayoutGrid::getEdgePoint( const DLayoutContentIndex &indexContent, CBD::EdgeCenters nEdge )
{
    switch ( nEdge )
    {
        case CBD::EdgeCenterLeft:
            return QPointF( vectorContents[indexContent.nRow][indexContent.nCol].rect.left(), vectorContents[indexContent.nRow][indexContent.nCol].rect.center().y() );
        case CBD::EdgeCenterRight:
            return QPointF( vectorContents[indexContent.nRow][indexContent.nCol].rect.right(), vectorContents[indexContent.nRow][indexContent.nCol].rect.center().y() );
        case CBD::EdgeCenterTop:
            return QPointF( vectorContents[indexContent.nRow][indexContent.nCol].rect.center().x(), vectorContents[indexContent.nRow][indexContent.nCol].rect.top() );
        case CBD::EdgeCenterBottom:
            return QPointF( vectorContents[indexContent.nRow][indexContent.nCol].rect.center().x(), vectorContents[indexContent.nRow][indexContent.nCol].rect.bottom() );
        case CBD::EdgeCenterCenter:
            return vectorContents[indexContent.nRow][indexContent.nCol].rect.center();
    }

    return QPointF();
}

bool DLayoutGrid::doInsert( DRectangleBase *p, DLayoutContentIndex indexCell, CBD::EdgeCenters nEdge )
{
    Q_ASSERT( p );

    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nRows );
    Q_ASSERT( nCols );

    Q_ASSERT( !indexCell.isNull() );
    Q_ASSERT( indexCell.nRow < nRows );
    Q_ASSERT( indexCell.nCol < nCols );

    // catch case where layout has single cell and its empty 
    // force to CBD::EdgeCenterCenter  
    DRectangleBase *pObject = vectorContents[indexCell.nRow][indexCell.nCol].pObject;
    if ( isEmpty() ) nEdge = CBD::EdgeCenterCenter;

    // insert row or col
    switch ( nEdge )
    {
        case CBD::EdgeCenterLeft:
            {
                doColInsert( indexCell.nCol );
            }
            break;
        case CBD::EdgeCenterRight:
            {
                indexCell.nCol++;
                doColInsert( indexCell.nCol );
            }
            break;
        case CBD::EdgeCenterTop:
            {
                doRowInsert( indexCell.nRow );
            }
            break;
        case CBD::EdgeCenterBottom:
            {
                indexCell.nRow++;
                doRowInsert( indexCell.nRow );
            }
            break;
        case CBD::EdgeCenterCenter:
        {
            if ( pObject )
            {
                doMessage( "ERROR", tr("Already have an object there. Remove it first.") );
                return false;
            }
        }
    }

    // ok - this is happening so take ownership of objects we manage
    p->doReparent( this );

    nRows = getRows();
    nCols = getCols();
    Q_ASSERT( indexCell.nRow < nRows );
    Q_ASSERT( indexCell.nCol < nCols );

    vectorContents[indexCell.nRow][indexCell.nCol].pObject = p;
    connect( p, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    doInitLayout();

    // we may need to be larger to fit content
    QSizeF size = getSize();
    if ( sizeMinimum.width() > size.width() ) size.setWidth( sizeMinimum.width() );
    if ( sizeMinimum.height() > size.height() ) size.setHeight( sizeMinimum.height() );
    if ( size != getSize() )
        setSize( size ); // this will resize self and call doLayout
    else 
        doLayout();

    emit signalChangedLayout();

    return true;
}

/*!
 * \brief Set the cell object.
 * 
 * In this class it is only valid to call this when cell with no object.
 * 
 * \author pharvey (2026-02-21)
 * 
 * \param p         
 * \param indexCell 
 * 
 * \return bool 
 */
bool DLayoutGrid::doSet( DRectangleBase *p, DLayoutContentIndex indexCell )
{
    // this not for removing content
    Q_ASSERT( p );
    // we can not set when an object is already in the cell
    Q_ASSERT( vectorContents[indexCell.nRow][indexCell.nCol].pObject == nullptr );
    // we take ownership of objects we manage
    p->doReparent( this );
    // set 
    vectorContents[indexCell.nRow][indexCell.nCol] = DLayoutContent( p );
    connect( p, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    // adjust layout
    doInitLayout();
    doLayout();

    emit signalChangedLayout();

    return true;
}

void DLayoutGrid::doRemove( DRectangleBase *p )
{
    Q_ASSERT( p );

    // ie. being dragged from layout
    // this will trigger our slotChildRemoved
    p->doReparent( nullptr );
}

/*!
/*!
 * \brief Update contents.
 * 
 * When content changed; doInitLayout + doLayout.
 * When layout resized; doLayout.
 * 
 * \author pharvey (2026-02-25)
 */
void DLayoutGrid::doLayout()
{
    // single cell and empty?
    if ( isEmpty() )
    {
        vectorContents[0][0].rect = QRectF( 0, 0, rect.width(), rect.height() );
        return;
    }

    int nRows = getRows();
    int nCols = getCols();

    qreal nRowY = 0;
    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        qreal nColX = 0;
        DLayoutGeometry geometryRow = vectorRowGeometries[nRow];
        for ( int nCol = 0; nCol < nCols; nCol++ )
        {
            DRectangleBase *pObject = vectorContents[nRow][nCol].pObject;
            DLayoutGeometry geometryCol = vectorColGeometries[nCol];

            // pos/size object within cell rect
            if ( pObject )
            {
                // center within cell.rect factor in margins
                QMargins margins( 0, 0, 0, 0 );
                if ( !pObject->inherits( "DSpacer" ) ) margins = pObject->getMargins();

                qreal nX        = nColX + margins.left();
                qreal nY        = nRowY + margins.top();
                qreal nWidth    = geometryCol.WH.nWidth - (margins.left() + margins.right());
                qreal nHeight   = geometryRow.WH.nHeight - (margins.top() + margins.bottom());

                pObject->setPos( QPointF( nX, nY ) );
                pObject->setSize( QSizeF( nWidth, nHeight ) );
            }
            nColX += geometryCol.WH.nWidth;
        }
        nRowY += geometryRow.WH.nHeight;
    }

    getProxy()->update();
}

/*!
 * \brief Update our layout info based upon our content.
 * 
 * When content changed; doInitLayout + doLayout.
 * When layout resized; doLayout.
 * 
 * \author pharvey (2026-02-25)
 */
void DLayoutGrid::doInitLayout()
{
    // todo: handle struts better?
    // if ( cell.pObject && cell.pObject->inherits( "DSpacer" ) && ((DSpacer*)cell.pObject)->getSizeType() == DSpacer::SizeFixed ) continue;                                                

    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nRows >= 1 );
    Q_ASSERT( nCols >= 1 );

    //
    // AGGREGATE
    //
    {
        // Set vectorRowGeometries for all rows.
        //
        {
            vectorRowGeometries.resize( nRows );
            mapStretchFactorRows.clear();
            for ( int nRow = 0; nRow < nRows; nRow++ )
            {
                DLayoutGeometry geometry;
                for ( int nCol = 0; nCol < nCols; nCol++ )
                {
                    DRectangleBase *pObject = vectorContents[nRow][nCol].pObject;
                    if ( pObject )
                    {
                        QMargins margins    = pObject->getMargins();
                        geometry.nMinimum   = qMax( geometry.nMinimum, pObject->getSizeMinimum().height() + margins.top() + margins.bottom() );
                        geometry.nMaximum   = qMin( geometry.nMaximum, pObject->getSizeMaximum().height() + margins.top() + margins.bottom() );
                        geometry.nStretch   = qMax( geometry.nStretch, pObject->getStretch().height() );
                        geometry.nHint      = qMax( geometry.nHint, pObject->getSizeHint().height() + margins.top() + margins.bottom() );
                        continue;
                    }
                }
                // geometry.nHint = qMin( geometry.nHint, geometry.nMaximum );
                if ( geometry.nMinimum < 1 ) geometry.nMinimum = DLAYOUTGRID_MIN;
                vectorRowGeometries[nRow] = geometry;
                mapStretchFactorRows.insert( geometry.nStretch, nRow );
            }
        }

        //
        // Set vectorColGeometries for all cols.
        //
        {
            vectorColGeometries.resize( nCols );
            mapStretchFactorCols.clear();
            for ( int nCol = 0; nCol < nCols; nCol++ )
            {
                DLayoutGeometry geometry;
                for ( int nRow = 0; nRow < nRows; nRow++ )
                {
                    DRectangleBase *pObject = vectorContents[nRow][nCol].pObject;
                    if ( pObject )
                    {
                        QMargins margins    = pObject->getMargins();
                        geometry.nMinimum   = qMax( geometry.nMinimum, pObject->getSizeMinimum().width() + margins.left() + margins.right() );
                        geometry.nMaximum   = qMin( geometry.nMaximum, pObject->getSizeMaximum().width() + margins.left() + margins.right() );
                        geometry.nStretch   = qMax( geometry.nStretch, pObject->getStretch().width() );
                        geometry.nHint      = qMax( geometry.nHint, pObject->getSizeHint().width() + margins.left() + margins.right() );
                        continue;
                    }
                }
                // geometry.nHint = qMin( geometry.nHint, geometry.nMaximum );
                if ( geometry.nMinimum < 1 ) geometry.nMinimum = DLAYOUTGRID_MIN;
                vectorColGeometries[nCol] = geometry;
                mapStretchFactorCols.insert( geometry.nStretch, nCol );
            }
        }
    }

    //
    // SET ROW HEIGHTS
    //
    doUpdateRowHeights();

    //
    // SET COL HEIGHTS
    //
    doUpdateColWidths();

    //
    // Set cell rect for all cells.
    //
    {
        qreal nRowY = 0;
        for ( int nRow = 0; nRow < nRows; nRow++ )
        {
            qreal nColX = 0;
            DLayoutGeometry geometryRow = vectorRowGeometries[nRow];
            for ( int nCol = 0; nCol < nCols; nCol++ )
            {
                DRectangleBase *pObject = vectorContents[nRow][nCol].pObject;
                DLayoutGeometry geometryCol = vectorColGeometries[nCol];

                // set cell rect
                vectorContents[nRow][nCol].rect = QRectF( QPointF( nColX, nRowY ), QSizeF( geometryCol.WH.nWidth, geometryRow.WH.nHeight ) );

                nColX += geometryCol.WH.nWidth;
            }
            nRowY += geometryRow.WH.nHeight;
        }
    }

}

/*
DLayoutSelfGeometry DLayoutGrid::getUpdateSelfRow( int nRow, DLayoutSelfGeometry geometryRow )
{
    int     nCols       = getCols();
    qreal   nHintWidth  = 0;
    qreal   nHintHeight = 0;
    qreal   nMinWidth   = 0;
    qreal   nMinHeight  = 0;
    qreal   nMaxWidth   = 0;
    qreal   nMaxHeight  = 0;

    DRectangleBase *pRectangleBase;
    DRectangleBase * cell;

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        cell = vectorContents[nRow][nCol];
        if ( !cell.pObject ) continue;

        pRectangleBase = cell.pObject;

        QSizeF size;
        qreal nMarginsX = 0;
        qreal nMarginsY = 0;

        if ( !pRectangleBase->inherits( "DSpacer" ) )
        {
            // margins are not relevant for DSpacer - but is for all else
            nMarginsX = pRectangleBase->getMargins().left() + pRectangleBase->getMargins().right();
            nMarginsY = pRectangleBase->getMargins().top() + pRectangleBase->getMargins().bottom();
        }

        if ( pRectangleBase->inherits( "DSpacer" ) && ((DSpacer *)pRectangleBase)->getSizeType() == DSpacer::SizeFixed )
        {
            // sizeHint
            // in this case sizeHint is not just a hint - it is also a constraint
            size = pRectangleBase->getSizeHint();
            nHintHeight += size.height();
            if ( size.width() > nHintWidth ) nHintWidth = size.width();

            // sizeMinimum
            // use sizeHint as minimum 
            nMinHeight += nHintHeight;
            if ( size.width() > nMinWidth ) nMinWidth = size.width();

            // sizeMaximum
            // use sizeHint as maximum
            if ( !geometryRow.bUnlimitedStretch )
            {
                nMaxHeight += nHintHeight;
            }
            if ( size.width() > 0 && size.width() < nMaxWidth ) nMaxWidth = size.width(); 

            continue;
        }

        // sizeHint
        size = pRectangleBase->getSizeHint();
        nHintHeight += ( size.height() + nMarginsY );
        if ( size.width() + nMarginsX > nHintWidth ) nHintWidth = size.width() + nMarginsX;

        // sizeMinimum
        size = pRectangleBase->getSizeMinimum();
        nMinHeight += ( size.height() + nMarginsY );
        if ( size.width() + nMarginsX > nMinWidth ) nMinWidth = size.width() + nMarginsX;

        // sizeMaximum
        size = pRectangleBase->getSizeMaximum();
        if ( !geometryRow.bUnlimitedStretch )
        {
            if ( size.height() <= 0 ) 
            {
                geometryRow.bUnlimitedStretch = true;
                nMaxHeight = 0; 
            }
            else
            {
                nMaxHeight += size.height() + nMarginsY;
            }
        }

        if ( size.width() > 0 && size.width() < nMaxWidth ) nMaxWidth = size.width(); 
    }

    geometryRow.sizeHint    = QSizeF( nHintWidth, nHintHeight );
    geometryRow.sizeMinimum = QSizeF( nMinWidth, nMinHeight );
    geometryRow.sizeMaximum = QSizeF( nMaxWidth, nMaxHeight );

    return geometryRow;
}

DLayoutSelfGeometry DLayoutGrid::getUpdateSelfCol( int nCol, DLayoutSelfGeometry geometryCol )
{
    int     nRows       = getRows();
    qreal   nHintWidth  = 0;
    qreal   nHintHeight = 0;
    qreal   nMinWidth   = 0;
    qreal   nMinHeight  = 0;
    qreal   nMaxWidth   = 0;
    qreal   nMaxHeight  = 0;

    DRectangleBase *pRectangleBase;
    DRectangleBase * cell;

    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        cell = vectorContents[nRow][nCol];
        if ( !cell.pObject ) continue;

        pRectangleBase = cell.pObject;

        QSizeF size;
        qreal nMarginsX = 0;
        qreal nMarginsY = 0;

        if ( !pRectangleBase->inherits( "DSpacer" ) )
        {
            // margins are not relevant for DSpacer - but is for all else
            nMarginsX = pRectangleBase->getMargins().left() + pRectangleBase->getMargins().right();
            nMarginsY = pRectangleBase->getMargins().top() + pRectangleBase->getMargins().bottom();
        }

        if ( pRectangleBase->inherits( "DSpacer" ) && ((DSpacer *)pRectangleBase)->getSizeType() == DSpacer::SizeFixed )
        {
            // sizeHint
            // in this case sizeHint is not just a hint - it is also a constraint
            size = pRectangleBase->getSizeHint();
            nHintWidth += size.width();
            if ( size.height() > nHintHeight ) nHintHeight = size.height();

            // sizeMinimum
            // use sizeHint as minimum 
            nMinWidth += nHintWidth;
            if ( size.height() > nMinHeight ) nMinHeight = size.height();

            // sizeMaximum
            // use sizeHint as maximum
            if ( !geometryCol.bUnlimitedStretch )
            {
                nMaxWidth += nHintWidth;
            }
            if ( size.height() > 0 && size.height() < nMaxHeight ) nMaxHeight = size.height(); 

            continue;
        }

        // sizeHint
        size = pRectangleBase->getSizeHint();
        nHintWidth += ( size.width() + nMarginsX );
        if ( size.height() + nMarginsY > nHintHeight ) nHintHeight = size.height() + nMarginsY;

        // sizeMinimum
        size = pRectangleBase->getSizeMinimum();
        nMinWidth += ( size.width() + nMarginsX );
        if ( size.height() + nMarginsY > nMinHeight ) nMinHeight = size.height() + nMarginsY;

        // sizeMaximum
        size = pRectangleBase->getSizeMaximum();
        if ( !geometryCol.bUnlimitedStretch )
        {
            if ( size.width() <= 0 ) 
            {
                geometryCol.bUnlimitedStretch = true;
                nMaxWidth = 0; 
            }
            else
            {
                nMaxWidth += size.width() + nMarginsX;
            }
        }

        if ( size.height() > 0 && size.height() < nMaxHeight ) nMaxHeight = size.height(); 
    }

    geometryCol.sizeHint    = QSizeF( nHintWidth, nHintHeight );
    geometryCol.sizeMinimum = QSizeF( nMinWidth, nMinHeight );
    geometryCol.sizeMaximum = QSizeF( nMaxWidth, nMaxHeight );

    return geometryCol;
}
*/

/*!
 * \brief Set row heights adjusting cell rect.
 * 
 * Geometry for all cells must already be aggregated. Which means we know;
 *      - row min/max height
 *      - col min/max height
 *      - row stretch
 *      - col stretchhttps://youtu.be/0-Byic0e0fY?si=tMZkMyjaSGdt8Df2
 * We use cell.pObject.hint
 * 
 * 
 * Will process each row determining the default height (and accumilating total height) setting a default cell.rect.height for all cells in row.
 * Will call to shrink/expand row heights to fit all rows in layout.height.
 * 
 * \author pharvey (2025-11-23)
 */
void DLayoutGrid::doUpdateRowHeights()
{
    // we start off with everything at hint and then expand/shrink as needed
    int     nRows         = getRows();
    qreal   nHintTotal    = 0;
    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        vectorRowGeometries[nRow].WH.nHeight = vectorRowGeometries[nRow].nHint;
        nHintTotal += vectorRowGeometries[nRow].WH.nHeight;
    }

    // stretch/shrink content?
    qreal nAdjust = rect.height() - nHintTotal;
    if ( nAdjust < 0 ) doShrinkRowHeights( nAdjust, true );
    else if ( nAdjust > 0 ) doExpandRowHeights( nAdjust, true );
}

void DLayoutGrid::doShrinkRowHeights( qreal nAdjust, bool bStretch )
{
    qreal nAdjusted = nAdjust;                                                                                                                                   

    // process by stretch                
    QMultiMapIterator<qreal,int> i(mapStretchFactorRows);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            

        qreal nStretch = i.key();                                                                                                                                               

        // process all cols with nStretch
        QList<int> listValues = mapStretchFactorRows.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted >= 0 ) return;                                                                                                                                            

            // col info
            int             nRow        = listValues.at( i );
            DLayoutGeometry geometry    = vectorRowGeometries[nRow];

            // col can not shrink?                               
            if ( geometry.WH.nHeight <= 0 ) continue;                                                       
            if ( geometry.nMinimum && geometry.WH.nHeight <= geometry.nMinimum ) continue;                                                       

            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !geometry.nStretch ) continue;

            //
            qreal nShrinkBy = geometry.nStretch;                                                                                                                                              
            if ( nShrinkBy < 1 ) nShrinkBy = 1;                                                                                                                                      
            if ( geometry.nMinimum && nShrinkBy > geometry.nMinimum ) nShrinkBy = geometry.WH.nHeight - geometry.nMinimum;
            if ( nShrinkBy <= 0 ) continue;                                                                                                                                          

            geometry.WH.nHeight -= nShrinkBy;
            vectorRowGeometries[nRow] = geometry;

            nAdjusted += nShrinkBy;                                                                                                                                                  
        }
    }                                                                                                                                                                            

    // fully adjusted
    if ( nAdjusted >= 0 ) return;

    // nothing changed ?
    if ( nAdjusted == nAdjust )                                                                                                                                                  
    {                                                                                                                                                                            
        // expand options and keep going                                                                                                                                         
        if ( bStretch )
        {
            doShrinkRowHeights( nAdjusted, false );                                                                                                                     
        }
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doShrinkRowHeights( nAdjusted, bStretch );                                                                                                                                  
}

void DLayoutGrid::doExpandRowHeights( qreal nAdjust, bool bStretch )
{
    qreal nAdjusted = nAdjust;                                                                                                                                   

    QMultiMapIterator<qreal,int> i(mapStretchFactorRows);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            

        qreal nStretch = i.key();                                                                                                                                               

        // process all rows with nStretch
        QList<int> listValues = mapStretchFactorRows.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted <= 0 ) return;                                                                                                                                            

            // row info
            int             nRow        = listValues.at( i );
            DLayoutGeometry geometry    = vectorRowGeometries[nRow];

            // col can not expand?                               
            if ( geometry.nMaximum && geometry.WH.nHeight >= geometry.nMaximum ) continue;                                                       

            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !geometry.nStretch ) continue;

            //
            qreal nExpandBy = geometry.nStretch;                                                                                                                                              
            if ( nExpandBy < 1 ) nExpandBy = 1;                                                                                                                                      
            if ( geometry.nMaximum && nExpandBy > geometry.nMaximum ) nExpandBy = geometry.nMaximum - geometry.WH.nHeight;
            if ( nExpandBy <= 0 ) continue;                                                                                                                                          

            geometry.WH.nHeight += nExpandBy;
            vectorRowGeometries[nRow] = geometry;
            nAdjusted -= nExpandBy;                                                                                                                                                  
        }
    }                                                                                                                                                                            

    // fully adjusted
    if ( nAdjusted <= 0 ) return;

    // nothing changed ?
    if ( nAdjusted == nAdjust )                                                                                                                                                  
    {                                                                                                                                                                            
        // expand options and keep going                                                                                                                                         
        if ( bStretch ) doExpandRowHeights( nAdjusted, false );                                                                                                                     
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doExpandRowHeights( nAdjusted, bStretch );                                                                                                                                  
}

void DLayoutGrid::doUpdateColWidths()
{
    // we start off with everything at hint and then expand/shrink as needed
    int     nCols         = getCols();
    qreal   nHintTotal    = 0;
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        vectorColGeometries[nCol].WH.nWidth = vectorColGeometries[nCol].nHint;
        nHintTotal += vectorColGeometries[nCol].WH.nWidth;
    }

    // stretch/shrink content?
    qreal nAdjust = rect.width() - nHintTotal;
    if ( nAdjust < 0 ) doShrinkColWidths( nAdjust, true );
    else if ( nAdjust > 0 ) doExpandColWidths( nAdjust, true );
}

void DLayoutGrid::doShrinkColWidths( qreal nAdjust, bool bStretch )
{
    qreal nAdjusted = nAdjust;
                
    // process by stretch                
    QMultiMapIterator<qreal,int> i(mapStretchFactorCols);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            
                                                                                                                                                                                         
        qreal nStretch = i.key();                                                                                                                                               

        // process all cols with nStretch
        QList<int> listValues = mapStretchFactorCols.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted >= 0 ) return;                                                                                                                                            

            // col info
            int             nCol        = listValues.at( i );
            DLayoutGeometry geometry    = vectorColGeometries[nCol];

            // col can not shrink?                               
            if ( geometry.WH.nWidth <= 0 ) continue;                                                       
            if ( geometry.nMinimum && geometry.WH.nWidth <= geometry.nMinimum ) continue;                                                       

            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !geometry.nStretch ) continue;

            //
            qreal nShrinkBy = geometry.nStretch;                                                                                                                                              
            if ( nShrinkBy < 1 ) nShrinkBy = 1;                                                                                                                                      
            if ( geometry.nMinimum && nShrinkBy > geometry.nMinimum ) nShrinkBy = geometry.WH.nWidth - geometry.nMinimum;
            if ( nShrinkBy <= 0 ) continue;                                                                                                                                          

            geometry.WH.nWidth -= nShrinkBy;
            vectorColGeometries[nCol] = geometry;

            nAdjusted += nShrinkBy;                                                                                                                                                  
        }
    }                                                                                                                                                                            

    // fully adjusted
    if ( nAdjusted >= 0 ) return;

    // nothing changed ?
    if ( nAdjusted == nAdjust )                                                                                                                                                  
    {                                                                                                                                                                            
        // expand options and keep going                                                                                                                                         
        if ( bStretch )
        {
            doShrinkColWidths( nAdjusted, false );                                                                                                                     
        }
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doShrinkColWidths( nAdjusted, bStretch );                                                                                                                                  
}

void DLayoutGrid::doExpandColWidths( qreal nAdjust, bool bStretch )
{
    qreal nAdjusted = nAdjust;                                                                                                                                   
                                                                                                                                                                                 
    QMultiMapIterator<qreal,int> i(mapStretchFactorCols);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            
                                                                                                                                                                                         
        qreal nStretch = i.key();                                                                                                                                               

        // process all cols with nStretch
        QList<int> listValues = mapStretchFactorCols.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted <= 0 ) return;                                                                                                                                            

            // col info
            int             nCol        = listValues.at( i );
            DLayoutGeometry geometry    = vectorColGeometries[nCol];

            // col can not expand?                               
            if ( geometry.nMaximum && geometry.WH.nWidth >= geometry.nMaximum ) continue;                                                       

            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !geometry.nStretch ) continue;

            //
            qreal nExpandBy = geometry.nStretch;                                                                                                                                              
            if ( nExpandBy < 1 ) nExpandBy = 1;                                                                                                                                      
            if ( geometry.nMaximum && nExpandBy > geometry.nMaximum ) nExpandBy = geometry.nMaximum - geometry.WH.nWidth;
            if ( nExpandBy <= 0 ) continue;                                                                                                                                          

            geometry.WH.nWidth += nExpandBy;
            vectorColGeometries[nCol] = geometry;

            nAdjusted -= nExpandBy;                                                                                                                                                  
        }
    }                                                                                                                                                                            

    // fully adjusted
    if ( nAdjusted <= 0 ) return;

    // nothing changed ?
    if ( nAdjusted == nAdjust )                                                                                                                                                  
    {                                                                                                                                                                            
        // expand options and keep going                                                                                                                                         
        if ( bStretch ) doExpandColWidths( nAdjusted, false );                                                                                                                     
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doExpandColWidths( nAdjusted, bStretch );                                                                                                                                  
}


/*!
 * \brief Determine if a row is empty.
 * 
 * A row is empty when no cells contain an object.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nRow   
 * 
 * \return bool 
 */
bool DLayoutGrid::isRowEmpty( int nRow )
{
    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nRows >= 1 );
    Q_ASSERT( nRow >= 0 );
    Q_ASSERT( nRow < nRows );

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        if ( vectorContents[nRow][nCol].pObject ) return false;
    }

    return true;
}

/*!
 * \brief Determine if a col is empty.
 * 
 * A cool is empty when no cells contain an object.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nRow   
 * 
 * \return bool 
 */
bool DLayoutGrid::isColEmpty( int nCol )
{
    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nRows >= 1 );
    Q_ASSERT( nCol >= 0 );
    Q_ASSERT( nCol < nCols );

    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        if ( vectorContents[nRow][nCol].pObject ) return false;
    }
    return true;
}

/*!
 * \brief Delete a row.
 * 
 * A row should only be deleted if isRowEmpty(). This is NOT verified here.
 * Fails if getRows() == 1 because we must always have 1 row.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nRow   
 * 
 * \return bool 
 */
bool DLayoutGrid::doRowDelete( int nRow )
{
    int nRows = getRows();

    Q_ASSERT( nRows >= 2 );
    Q_ASSERT( nRow >= 0 );
    Q_ASSERT( nRow < nRows );

    vectorContents.removeAt( nRow );

    return true;
}

/*!
 * \brief Delete a col.
 * 
 * A col should only be deleted if isColEmpty(). This is NOT verified here.
 * Fails if getCols() == 1 because we must always have 1 col.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nCol   
 * 
 * \return bool 
 */
bool DLayoutGrid::doColDelete( int nCol )
{
    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nCols >= 2 );
    Q_ASSERT( nCol >= 0 );
    Q_ASSERT( nCol < nCols );

    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        vectorContents[nRow].removeAt( nCol );
    }

    return true;
}

/*!
 * \brief Insert a new row.
 * 
 * Will insert before given row.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nRow   0=prepend nRows=append
 * 
 * \return bool 
 */
bool DLayoutGrid::doRowInsert( int nRow )
{
    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nRow >= 0 );
    Q_ASSERT( nRow <= nRows );

    if ( nRow == nRows )
        vectorContents.append( QVector<DLayoutContent>( nCols, DLayoutContent() ) );
    else
        vectorContents.insert( nRow, QVector<DLayoutContent>( nCols, DLayoutContent() ) );

    return true;
}

/*!
 * \brief Insert a new col.
 * 
 * Will insert before given col.
 * 
 * \author pharvey (2025-11-17)
 * 
 * \param nCol   0=prepend nCols=append
 * 
 * \return bool 
 */
bool DLayoutGrid::doColInsert( int nCol )
{
    int nRows = getRows();
    int nCols = getCols();

    Q_ASSERT( nCol >= 0 );
    Q_ASSERT( nCol <= nCols );

    // for each row insert a col
    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        if ( nCol == nCols )
            vectorContents[nRow].append( nullptr );
        else
            vectorContents[nRow].insert( nCol, nullptr );
    }

    return true;
}

/*!
 * \brief For debugging.
 * 
 * \author pharvey (2026-02-26)
 */
void DLayoutGrid::doPrintRects()
{
    int nRows = getRows();
    int nCols = getCols();

    for ( int nRow = 0; nRow < nRows; nRow++ )
    {
        for ( int nCol = 0; nCol < nCols; nCol++ )
        {
            qDebug() << nRow << "," << nCol << ":" << vectorContents[nRow][nCol].rect;
        }
    }
}

