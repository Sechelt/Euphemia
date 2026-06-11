#include "LibInfo.h"
#include "DLayoutVertical.h"

#include "DGraphicsProxyItem.h"

/*!
 * \brief Vertical layout.
 * 
 * Lays out objects in a single, vertical, column.
 * The top row is index 0.
 * Empty cells are removed unless there is only one cell left.
 * The is always at least one cell.
 * 
 * \author pharvey (2026-02-24)
 * 
 * \param pObjectParent 
 * \param stringName    
 */
DLayoutVertical::DLayoutVertical( ADObject *pObjectParent, const QString &stringName )
    : DLayoutLinear( pObjectParent, stringName )
{
    // single cell init in DLayoutLinear
}

DLayoutVertical::~DLayoutVertical()
{
}

QPixmap DLayoutVertical::getIcon()
{
    return QPixmap( ":D/LayoutVertical" );
}

void DLayoutVertical::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    // draw our rect
    pPainter->drawRect( rect );

    // draw cell lines
    // cells are larger than the object when the object has margins
    QRectF r;
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        r = vectorContents.at( n ).rect;
        pPainter->drawLine( rect.left(), r.y(), rect.right(), r.y() );
    }
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
CBD::EdgeCenters DLayoutVertical::getEdge( const DLayoutContentIndex &indexContent, const QPointF &pointItem )
{
//    Q_ASSERT( !indexContent.isNull() );
    Q_ASSERT( vectorContents[indexContent.nRow].rect.contains( pointItem ) );

    // force center if we are empty
    if ( isEmpty() ) return CBD::EdgeCenterCenter;

    // we are vertical so; top or bottom
    QRectF r = vectorContents[indexContent.nRow].rect;
    if ( pointItem.y() - r.top() <= r.bottom() - pointItem.y() ) return CBD::EdgeCenterTop; 

    return CBD::EdgeCenterBottom;
}

/*!
 * \brief Get point (in item coordinates) to place drop indicator.
 * 
 * \author pharvey (2026-02-20)
 * 
 * \param indexContent 
 * \param nEdge        
 * 
 * \return QPointF in item coordinates
 */
QPointF DLayoutVertical::getEdgePoint( const DLayoutContentIndex &indexContent, CBD::EdgeCenters nEdge )
{
    switch ( nEdge )
    {
        case CBD::EdgeCenterLeft:
            qFatal( "CBD::EdgeCenterLeft is invalid here." );
//            return QPointF( vectorContents[indexContent.nRow].rect.left(), vectorContents[indexContent.nRow].rect.center().y() );
        case CBD::EdgeCenterRight:
            qFatal( "CBD::EdgeCenterRight is invalid here." );
//            return QPointF( vectorContents[indexContent.nRow].rect.right(), vectorContents[indexContent.nRow].rect.center().y() );
        case CBD::EdgeCenterTop:
            return QPointF( vectorContents[indexContent.nRow].rect.center().x(), vectorContents[indexContent.nRow].rect.top() );
        case CBD::EdgeCenterBottom:
            return QPointF( vectorContents[indexContent.nRow].rect.center().x(), vectorContents[indexContent.nRow].rect.bottom() );
        case CBD::EdgeCenterCenter:
            return vectorContents[indexContent.nRow].rect.center();
    }

    return QPointF();
}

bool DLayoutVertical::doInsert( DRectangleBase *p, DLayoutContentIndex indexContent, CBD::EdgeCenters nEdge )
{
    Q_ASSERT( p );

    // we take ownership of objects we manage
    p->doReparent( this );
    // insert to desired index
    Q_ASSERT( indexContent.nRow >= 0 );
    Q_ASSERT( indexContent.nRow <= vectorContents.count() );
    if ( nEdge == CBD::EdgeCenterBottom )
        vectorContents.insert( indexContent.nRow + 1, DLayoutContent( p ) );
    else
        vectorContents.insert( indexContent.nRow, DLayoutContent( p ) );
    connect( p, SIGNAL(signalChangedLayout()), this, SLOT(slotChangedContent()) );
    // update our geometry based upon content
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
 * \brief Update contents.
 * 
 * When content changed; doInitLayout + doLayout.
 * When layout resized; doLayout.
 * 
 * This means updating each;
 *  - cell rect
 *  - cell object geometry
 * 
 * \author pharvey (2026-02-24)
 */
void DLayoutVertical::doLayout()
{
    // single cell and empty?
    if ( isEmpty() )
    {
        vectorContents[0].rect = QRectF( 0, 0, rect.width(), rect.height() );
        return;
    }

    // Update our layout cell rects. This does not update y pos. 
    // Determining the cell heights is the bulk of what happens in this layout.
    doUpdateCellHeights();

    // Set size and pos of objects given the cell height.
    // Also; set the cell rect y pos.
    DLayoutContent cell;
    qreal nY = 0;
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        cell = vectorContents.at( n );

        QMargins margins( 0, 0, 0, 0 );
        if ( !cell.pObject->inherits( "DSpacer" ) ) margins = cell.pObject->getMargins();

        // try to center on x axis when there is extra space
        qreal nMaxWidth = cell.pObject->getSizeMaximum().width();
        qreal nWidthDelta = ( nMaxWidth > 0 ? rect.width() - nMaxWidth : 0 ); 
        if ( nWidthDelta < 0 ) nWidthDelta = 0;
        qreal nMarginLeft = ( nWidthDelta / 2 > margins.left() ? nWidthDelta / 2 : margins.left() );
        qreal nMarginRight = ( nWidthDelta / 2 > margins.right() ? nWidthDelta / 2 : margins.right() );
        // object pos
        qreal nTop     = nY + margins.top();
        qreal nLeft    = nMarginLeft;
        cell.pObject->setPos( QPointF( nLeft, nTop ) );
        // object size
        qreal nHeight  = cell.rect.height() - (margins.top() + margins.bottom());
        qreal nWidth   = cell.rect.width() - (nMarginLeft + nMarginRight);
        cell.pObject->setSize( QSizeF( nWidth, nHeight ) );
        // set the cell y pos
        cell.rect = QRectF( cell.rect.x(), nY, cell.rect.width(), cell.rect.height() );
        vectorContents[n] = cell;

        nY += cell.rect.height();
    }

    getProxy()->update();
}

/*!
 * \brief Update our layout info based upon our content.
 * 
 * When content changed; doInitLayout + doLayout.
 * When layout resized; doLayout.
 * 
 * sizeHint 
 *  
 * x. We adopt the highest object value.
 * y. We add all object sizeHints to come up with our own. 
 *  
 * sizeMinimum 
 *  
 * x. We adopt the highest object value.
 * y. We add all object sizeMinimum to come up with our own. 
 *  
 * sizeMaximum 
 *  
 * x. We adopt the lowest object value > 0. Will be 0 if all objects are 0.
 * y. We add all object sizeMaximum to come up with our own. 
 *    BUT if we have an object (even just 1) with unlimited size (sizeMaximum.x = 0) - we have unlimited stretch.
 * 
 * \note Margins are factored in. 
 *  
 * \author pharvey (9/21/20)
 */
void DLayoutVertical::doInitLayout()
{
    // single cell and empty? nothing to do
    if ( isEmpty() ) return;

    // Layout MaxHeight will be 0 if *any* object MaxHeight is 0. This indicates unlimited stretch.
    bool bUnlimitedStretchY = false;

    qreal nHintWidth    = 0;
    qreal nHintHeight   = 0;
    qreal nMinWidth     = 0;
    qreal nMinHeight    = 0;
    qreal nMaxWidth     = 0;
    qreal nMaxHeight    = 0;

    DRectangleBase *pRectangleBase;
    DLayoutContent cell;
    foreach( cell, vectorContents )
    {
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
            if ( !bUnlimitedStretchY )
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
        if ( !bUnlimitedStretchY )
        {
            if ( size.height() <= 0 ) 
            {
                bUnlimitedStretchY = true;
                nMaxHeight = 0; 
            }
            else
            {
                nMaxHeight += size.height() + nMarginsY;
            }
        }

        if ( size.width() > 0 && size.width() < nMaxWidth ) nMaxWidth = size.width(); 
    }

    // adopt values
    // \note an empty QSizeF will have values of ( -1, 0 ) and we do not want that
    sizeHint    = QSizeF( nHintWidth, nHintHeight );
    sizeMinimum = QSizeF( nMinWidth, nMinHeight );
    sizeMaximum = QSizeF( nMaxWidth, nMaxHeight );
}

/*!
 * \brief Calculate the layout cell sizes. 
 *  
 * Calculate cell widths. 
 *  
 * \note cell pos's are invalid - just the heights are updated 
 * 
 * \author pharvey (9/21/20)
 */
void DLayoutVertical::doUpdateCellHeights()
{
    mapStretchFactors.clear();
    qreal nY = 0;

    // set everything to size hint or minimum
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        DLayoutContent cell = vectorContents.at( n );

        // create index on vectorContents ordered by Stretch - we will need it later
        mapStretchFactors.insert( cell.pObject->getStretch().height(), n );
        // determine cell width - hopefully sizehint and/or min are set!
        qreal nHeight = cell.pObject->getSizeHint().height();
        if ( !nHeight ) nHeight = cell.pObject->getSizeMinimum().height();
        // save cell rect
        cell.rect = QRectF( 0, 0, rect.width(), nHeight );                        
        vectorContents[n] = cell;
        nY += nHeight;
    }

    // stretch/shrink content?
    qreal nAdjust = rect.height() - nY;
    if ( nAdjust < 0 ) doShrinkCells( nAdjust, true );
    else if ( nAdjust > 0 ) doExpandCells( nAdjust, true );
}

/*!
 * \brief Shrink cell heights.
 *  
 * Here we adjust cell heights - NOT content - that is done later. 
 *  
 * This will be called with nAdjust<0. This is the amount of shrink we want.
 *  
 * Typically the initial call will be with bSprings==true. This indicates that we 
 * only want to adjust cells with springs. 
 *  
 * The call will recurse until no cells with springs can be adjusted then bSprings becomes 
 * false and the call will continue to recurse - this time considering all cells. 
 *  
 * The recursion will end when nAdjust==0 or cells can not be adjusted anymore. It 
 * should end when nAdjust==0 because of the work done in \sa doInitLayout.
 *  
 * Cells are adjusted iteratively by 1 pixel or nStretch (if set). 
 *  
 * \author pharvey (9/27/20)
 * 
 * \param nAdjust 
 * \param bSprings 
 */
void DLayoutVertical::doShrinkCells( qreal nAdjust, bool bStretch )
{
    qreal               nAdjusted   = nAdjust;
    DLayoutContent         cell;
    qreal               nMinimum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
                
    QMultiMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            
                                                                                                                                                                                         
        nStretch = i.key();                                                                                                                                               

        QList<int> listValues = mapStretchFactors.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted >= 0 ) return;                                                                                                                                            

            nIndex      = listValues.at( i );
            cell        = vectorContents.at( nIndex );
            nMinimum    = cell.pObject->getSizeMinimum().height();
                                                                                            
            // skip struts - they are always fixed                                                                                                                                                          
            if ( cell.pObject->inherits( "DSpacer" ) && ((DSpacer*)cell.pObject)->getSizeType() == DSpacer::SizeFixed ) continue;                                                

            // is max'd?                                                                                                                                                             
            if ( cell.rect.height() <= 0 ) continue;                                                       
            if ( nMinimum && cell.rect.height() <= nMinimum ) continue;                                                       
                                                                                                                                                                                                 
            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !nStretch ) continue;

            // 
            qreal nShrinkBy = nStretch;                                                                                                                                              
            if ( nShrinkBy < 1 ) nShrinkBy = 1;                                                                                                                                      
            if ( nMinimum && nShrinkBy > nMinimum ) nShrinkBy = cell.rect.height() - nMinimum;
            if ( nShrinkBy <= 0 ) continue;                                                                                                                                          
                                                                                                                                                                                                 
            cell.rect = QRectF( 0, 0, rect.width(), cell.rect.height() - nShrinkBy );
            vectorContents[nIndex] = cell;
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
            doShrinkCells( nAdjusted, false );                                                                                                                     
        }
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doShrinkCells( nAdjusted, bStretch );                                                                                                                                  
}

/*!
 * \brief Expand cell heights.
 *  
 * Here we adjust cell heights - NOT content - that is done later. 
 *  
 * This will be called with nAdjust>0. This is the amount of expand we want.
 *  
 * Typically the initial call will be with bStretch==true. This indicates that we 
 * only want to adjust cells with springs. 
 *  
 * The call will recurse until no cells with springs can be adjusted then bStretch becomes 
 * false and the call will continue to recurse - this time considering all cells. 
 *  
 * The recursion will end when nAdjust==0 or cells can not be adjusted anymore. It 
 * should end when nAdjust==0 because of the work done in \sa doInitLayout.
 *  
 * Cells are adjusted iteratively by 1 pixel or nStretch (if set). 
 *  
 * \author pharvey (9/27/20)
 * 
 * \param nAdjust 
 * \param bStretch 
 */
void DLayoutVertical::doExpandCells( qreal nAdjust, bool bStretch )
{
    qreal               nAdjusted   = nAdjust;                                                                                                                                   
    DLayoutContent         cell;
    qreal               nMaximum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
                                                                                                                                                                                 
    QMultiMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
    i.toBack();                                                                                                                                                                  
    while ( i.hasPrevious() )                                                                                                                                                    
    {                                                                                                                                                                            
        i.previous();                                                                                                                                                            
                                                                                                                                                                                         
        nStretch = i.key();                                                                                                                                               

        QList<int> listValues = mapStretchFactors.values( nStretch );
        for ( int i = 0; i < listValues.size(); ++i )
        {
            // fully adjusted
            if ( nAdjusted <= 0 ) return;                                                                                                                                            

            nIndex      = listValues.at( i );
            cell        = vectorContents.at( nIndex );
            nMaximum    = cell.pObject->getSizeMaximum().height();
                                                                                                                                                                                     
            // skip struts                                                                                                                                                           
            if ( cell.pObject->inherits( "DSpacer" ) && ((DSpacer*)cell.pObject)->getSizeType() == DSpacer::SizeFixed ) continue;                                                

            // is max'd?                                                                                                                                                             
            if ( nMaximum && cell.rect.height() >= nMaximum ) continue;                                                       
                                                                                                                                                                                     
            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !nStretch ) continue;
                                                                                                                                                                                     
            // everything else gets considered                                                                                                                                       
            qreal nExpandBy = nStretch;                                                                                                                                              
            if ( nExpandBy < 1 ) nExpandBy = 1;                                                                                                                                      
            if ( nMaximum && nExpandBy > nMaximum ) nExpandBy = nMaximum - cell.rect.height();
            if ( nExpandBy <= 0 ) continue;                                                                                                                                          
                                                                                                                                                                                                 
            cell.rect = QRectF( 0, 0, rect.width(), cell.rect.height() + nExpandBy );
            vectorContents[nIndex] = cell;
            nAdjusted -= nExpandBy;                                                                                                                                                  
        }
    }                                                                                                                                                                            

    // fully adjusted
    if ( nAdjusted <= 0 ) return;

    // nothing changed ?
    if ( nAdjusted == nAdjust )                                                                                                                                                  
    {                                                                                                                                                                            
        // expand options and keep going                                                                                                                                         
        if ( bStretch ) doExpandCells( nAdjusted, false );                                                                                                                     
        return;                                                                                                                                                                  
    }                                                                                                                                                                            

    doExpandCells( nAdjusted, bStretch );                                                                                                                                  
}

/*!
 * \brief Get the coordinate (row or col) that matters.
 * 
 * This allows many methods to be generalized into DLayoutLinear.
 * 
 * \author pharvey (2026-02-26)
 * 
 * \param indexContent 
 * 
 * \return int 
 */
int DLayoutVertical::getIndex( const DLayoutContentIndex &indexContent )
{
    return indexContent.nRow;
}

