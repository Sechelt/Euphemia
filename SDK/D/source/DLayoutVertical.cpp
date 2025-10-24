#include "LibInfo.h"
#include "DLayoutVertical.h"

#include "DGraphicsProxyItem.h"

DLayoutVertical::DLayoutVertical( ADObject *pObjectParent, const QString &stringName )
    : DLayoutLinear( pObjectParent, stringName )
{
}

DLayoutVertical::~DLayoutVertical()
{
}

QPixmap DLayoutVertical::getIcon()
{
    return QPixmap( ":D/VLayout" );
}

void DLayoutVertical::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

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
 * \brief Get closest layout cell edge.
 *  
 * This is so we can indicate possible insertion point. 
 * Is is also used to get insertion index. 
 *  
 * \author pharvey (9/24/20)
 * 
 * \param pointScene 
 * 
 * \return QPointF 
 */
QPointF DLayoutVertical::getEdge( const QPointF &pointScene )
{
    QPointF pointItem = getProxy()->mapFromScene( pointScene );                                         
    int nCell = 0;

    for ( ; nCell < vectorContents.count(); nCell++ )
    {
        QRectF r = vectorContents.at( nCell ).rect;
        // in cell?
        if ( r.contains( pointItem ) )
        {
            // top or bottom edge?
            if ( pointItem.y() <= r.height() / 2 )
                return getProxy()->mapToScene( QPointF( r.width() / 2, r.top() ) );                                    
            else
                return getProxy()->mapToScene( QPointF( r.width() / 2, r.bottom() ) );
        }
    }

    // no cells so use layout rect
    if ( pointItem.y() <= rect.height() / 2 )                                        
        return getProxy()->mapToScene( QPointF( rect.width() / 2, rect.top() ) );                                    

    return getProxy()->mapToScene( QPointF( rect.width() / 2, rect.bottom() ) );
}

/*!
 * \brief Get index (among layout cells) for given point.
 * 
 * \author pharvey (9/24/20)
 * 
 * \param pointScene 
 * 
 * \return int 
 */
int DLayoutVertical::indexOf( const QPointF &pointScene )
{
    QPointF pointItem = getProxy()->mapFromScene( pointScene );                                         
    int nCell = 0;

    for ( ; nCell < vectorContents.count(); nCell++ )
    {
        QRectF r = vectorContents.at( nCell ).rect;
        // in cell?
        if ( r.contains( pointItem ) )
        {
            // top or bottom edge?
            if ( pointItem.y() <= r.height() / 2 )
                return nCell;
            else
                return nCell + 1;
        }
    }

    // append
    return nCell;
}

void DLayoutVertical::doLayout()
{
    // Update our layout cell rects. This does not update y pos. 
    // Determining the cell heights is the bulk of what happens in this layout.
    doUpdateCellHeights();

    // Set size and pos of objects given the cell height.
    // Also; set the cell rect y pos.
    DLayoutCell cell;
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
 * \note Margins are factored in. 
 *  
 * \author pharvey (9/21/20)
 */
void DLayoutVertical::doUpdateSelf()
{
    // Layout MaxHeight will be 0 if *any* object MaxHeight is 0. This indicates unlimited stretch.
    bool bUnlimitedStretchY = false;

    qreal nHintWidth    = 0;
    qreal nHintHeight   = 0;
    qreal nMinWidth     = 0;
    qreal nMinHeight    = 0;
    qreal nMaxWidth     = 0;
    qreal nMaxHeight    = 0;

    DRectangleBase *pRectangleBase;
    DLayoutCell cell;
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
        DLayoutCell cell = vectorContents.at( n );

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
 * should end when nAdjust==0 because of the work done in \sa doUpdateSelf.
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
    DLayoutCell         cell;
    qreal               nMinimum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
                
#if QT_VERSION < 0x060000
    QMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
#else
    QMultiMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
#endif
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
 * should end when nAdjust==0 because of the work done in \sa doUpdateSelf.
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
    DLayoutCell         cell;
    qreal               nMaximum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
                                                                                                                                                                                 
#if QT_VERSION < 0x060000
    QMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
#else
    QMultiMapIterator<int,int> i(mapStretchFactors);                                                                                                                                  
#endif
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


