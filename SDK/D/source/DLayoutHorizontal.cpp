#include "LibInfo.h"
#include "DLayoutHorizontal.h"

#include "DGraphicsProxyItem.h"

DLayoutHorizontal::DLayoutHorizontal( ADObject *pObjectParent, const QString &stringName )
    : DLayoutLinear( pObjectParent, stringName )
{
}

DLayoutHorizontal::~DLayoutHorizontal()
{
}

QPixmap DLayoutHorizontal::getIcon()
{
    return QPixmap( ":D/HLayout" );
}

void DLayoutHorizontal::paintAt( QPainter *pPainter, const QPointF &pointPos )
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
        pPainter->drawLine( r.x(), rect.top(), r.x(), rect.bottom() );
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
QPointF DLayoutHorizontal::getEdge( const QPointF &pointScene )
{
    QPointF pointItem = getProxy()->mapFromScene( pointScene );                                         
    int nCell = 0;

    for ( ; nCell < vectorContents.count(); nCell++ )
    {
        QRectF r = vectorContents.at( nCell ).rect;
        // in cell?
        if ( r.contains( pointItem ) )
        {
            // left or right edge?
            if ( pointItem.x() <= r.width() / 2 )
                return getProxy()->mapToScene( QPointF( r.left(), r.height() / 2 ) );                                    
            else
                return getProxy()->mapToScene( QPointF( r.right(), r.height() / 2 ) );
        }
    }

    if ( pointItem.x() <= rect.width() / 2 )                                        
        return getProxy()->mapToScene( QPointF( rect.left(), rect.height() / 2 ) );                                    

    return getProxy()->mapToScene( QPointF( rect.right(), rect.height() / 2 ) );
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
int DLayoutHorizontal::indexOf( const QPointF &pointScene )
{
    QPointF pointItem = getProxy()->mapFromScene( pointScene );                                         
    int nCell = 0;

    for ( ; nCell < vectorContents.count(); nCell++ )
    {
        QRectF r = vectorContents.at( nCell ).rect;
        // in cell?
        if ( r.contains( pointItem ) )
        {
            // left or right edge?
            if ( pointItem.x() <= r.width() / 2 )
                return nCell;
            else
                return nCell + 1;
        }
    }

    // append
    return nCell;
}

/*!
 * \brief Adjust pos and size of objects. 
 *  
 * The layout is calculated in two ways; 
 *  
 * 1. When one or more Springs (DSpacer set to Spring) are included. 
 *  
 * All Spring are expanded/collapsed based upon their Stretch factor. 
 * Other objects are only considered if Springs have hit max/min and 
 * more needs to be done. 
 *  
 * 2. No Springs (or Springs have hit max/min).
 *  
 * All objects are expanded/collapsed based upon their Stretch factor. 
 * Objects that have hit their max/min are ignored. 
 *  
 * This starts with a call to \sa doUpdateCellWidths - which is where most of the work is done. 
 *  
 * We then use cell widths to indicate object size - while factoring in any margins. 
 * 
 * \author pharvey (9/21/20)
 */
void DLayoutHorizontal::doLayout()
{
    // Update our layout cell rects. This does not update x pos. 
    // Determining the cell widths is the bulk of what happens in this layout.
    doUpdateCellWidths();

    // Set size and pos of objects given the cell width.
    // Also; set the cell rect x pos.
    DLayoutCell cell;
    qreal nX = 0;
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        cell = vectorContents.at( n );

        QMargins margins( 0, 0, 0, 0 );
        if ( !cell.pObject->inherits( "DSpacer" ) ) margins = cell.pObject->getMargins();

        // try to center on y axis when there is extra space
        qreal nMaxHeight = cell.pObject->getSizeMaximum().height();
        qreal nHeightDelta = ( nMaxHeight > 0 ? rect.height() - nMaxHeight : 0 ); 
        if ( nHeightDelta < 0 ) nHeightDelta = 0;
        qreal nMarginTop = ( nHeightDelta / 2 > margins.top() ? nHeightDelta / 2 : margins.top() );
        qreal nMarginBottom = ( nHeightDelta / 2 > margins.bottom() ? nHeightDelta / 2 : margins.bottom() );
        // object pos
        qreal nLeft     = nX + margins.left();
        qreal nTop      = nMarginTop;
        cell.pObject->setPos( QPointF( nLeft, nTop ) );
        // object size
        qreal nWidth    = cell.rect.width() - (margins.left() + margins.right());
        qreal nHeight   = cell.rect.height() - (nMarginTop + nMarginBottom);
        cell.pObject->setSize( QSizeF( nWidth, nHeight ) );
        // set the cell x pos
        cell.rect = QRectF( nX, cell.rect.y(), cell.rect.width(), cell.rect.height() );
        vectorContents[n] = cell;

        nX += cell.rect.width();
    }

    getProxy()->update();
}

/*!
 * \brief Update our layout info based upon our content.
 *  
 * sizeHint 
 *  
 * x. We add all object sizeHints to come up with our own. 
 * y. We adopt the highest object value.
 *  
 * sizeMinimum 
 *  
 * x. We add all object sizeMinimum to come up with our own. 
 * y. We adopt the highest object value.
 *  
 * sizeMaximum 
 *  
 * x. We add all object sizeMaximum to come up with our own. 
 *    BUT if we have an object (even just 1) with unlimited size (sizeMaximum.x = 0) - we have unlimited stretch.
 * y. We adopt the lowest object value > 0. Will be 0 if all objects are 0.
 *  
 * \note Margins are factored in. 
 *  
 * \author pharvey (9/21/20)
 */
void DLayoutHorizontal::doUpdateSelf()
{
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__;
    // Layout MaxWidth will be 0 if *any* object MaxWidth is 0. This indicates unlimited stretch.
    bool bUnlimitedStretchX = false;

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
            nHintWidth += size.width();
            if ( size.height() > nHintHeight ) nHintHeight = size.height();

            // sizeMinimum
            // use sizeHint as minimum 
            nMinWidth += nHintWidth;
            if ( size.height() > nMinHeight ) nMinHeight = size.height();

            // sizeMaximum
            // use sizeHint as maximum
            if ( !bUnlimitedStretchX )
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
        if ( !bUnlimitedStretchX )
        {
            if ( size.width() <= 0 ) 
            {
                bUnlimitedStretchX = true;
                nMaxWidth = 0; 
            }
            else
            {
                nMaxWidth += size.width() + nMarginsX;
            }
        }

        if ( size.height() > 0 && size.height() < nMaxHeight ) nMaxHeight = size.height(); 
    }

    // adopt values
    // \note an empty QSizeF will have values of ( -1, 0 ) and we do not want that
    sizeHint    = QSizeF( nHintWidth, nHintHeight );
    sizeMinimum = QSizeF( nMinWidth, nMinHeight );
    sizeMaximum = QSizeF( nMaxWidth, nMaxHeight );
}

/*!
 * \brief Calculate the layout cell widths. 
 *  
 * 1. Calculate the optimal width of the layout using SizeHint or Minimum. 
 * 2. 
 *  
 * \note cell pos's are invalid - just the widths are updated 
 * 
 * \author pharvey (9/21/20)
 */
void DLayoutHorizontal::doUpdateCellWidths()
{
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << vectorContents.count();
    mapStretchFactors.clear();
    qreal nWidthTotal = 0;

    // set everything to size hint or minimum
    for ( int n = 0; n < vectorContents.count(); n++ )
    {
        DLayoutCell cell = vectorContents.at( n );

        // create index on vectorContents ordered by Stretch - we will need it later
        mapStretchFactors.insert( cell.pObject->getStretch().width(), n );
        // determine cell width - hopefully sizehint and/or min are set!
        qreal nWidth = cell.pObject->getSizeHint().width();
        if ( !nWidth ) nWidth = cell.pObject->getSizeMinimum().width();
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "cell width:" << nWidth;
        // save cell rect
        cell.rect = QRectF( 0, 0, nWidth, rect.height() );                        
        vectorContents[n] = cell;
        nWidthTotal += nWidth;
    }

    // stretch/shrink content?
    qreal nAdjust = rect.width() - nWidthTotal;
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "width current:" << rect.width() << " - " << "width desired: " << nWidthTotal << " = " << nAdjust;
    if ( nAdjust < 0 ) doShrinkCells( nAdjust, true );
    else if ( nAdjust > 0 ) doExpandCells( nAdjust, true );
}

/*!
 * \brief Shrink cell widths.
 *  
 * Here we adjust cell widths - NOT content - that is done later. 
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
void DLayoutHorizontal::doShrinkCells( qreal nAdjust, bool bStretch )
{
    qreal               nAdjusted   = nAdjust;
    DLayoutCell         cell;
    qreal               nMinimum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << nAdjust;
                
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
            nMinimum    = cell.pObject->getSizeMinimum().width();
                                                                                            
            // skip struts - they are always fixed                                                                                                                                                          
            if ( cell.pObject->inherits( "DSpacer" ) && ((DSpacer*)cell.pObject)->getSizeType() == DSpacer::SizeFixed ) continue;                                                

            // is max'd?                                                                                                                                                             
            if ( cell.rect.width() <= 0 ) continue;                                                       
            if ( nMinimum && cell.rect.width() <= nMinimum ) continue;                                                       
                                                                                                                                                                                                 
            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !nStretch ) continue;

            // 
            qreal nShrinkBy = nStretch;                                                                                                                                              
            if ( nShrinkBy < 1 ) nShrinkBy = 1;                                                                                                                                      
            if ( nMinimum && nShrinkBy > nMinimum ) nShrinkBy = cell.rect.width() - nMinimum;
            if ( nShrinkBy <= 0 ) continue;                                                                                                                                          
                                                                                                                                                                                                 
            cell.rect = QRectF( 0, 0, cell.rect.width() - nShrinkBy, rect.height() );
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
 * \brief Expand cell widths.
 *  
 * Here we adjust cell widths - NOT content - that is done later. 
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
void DLayoutHorizontal::doExpandCells( qreal nAdjust, bool bStretch )
{
    qreal               nAdjusted   = nAdjust;                                                                                                                                   
    DLayoutCell         cell;
    qreal               nMaximum;
    qreal               nStretch;                                                                                                                                                
    int                 nIndex;                                                                                                                                                  
// qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << nAdjust;
                                                                                                                                                                                 
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
            nMaximum    = cell.pObject->getSizeMaximum().width();
                                                                                                                                                                                     
            // skip struts                                                                                                                                                           
            if ( cell.pObject->inherits( "DSpacer" ) && ((DSpacer*)cell.pObject)->getSizeType() == DSpacer::SizeFixed ) continue;                                                

            // is max'd?                                                                                                                                                             
            if ( nMaximum && cell.rect.width() >= nMaximum ) continue;                                                       
                                                                                                                                                                                     
            // processing objects which stretch?                                                                                                                                              
            if ( bStretch && !nStretch ) continue;
                                                                                                                                                                                     
            // everything else gets considered                                                                                                                                       
            qreal nExpandBy = nStretch;                                                                                                                                              
            if ( nExpandBy < 1 ) nExpandBy = 1;                                                                                                                                      
            if ( nMaximum && nExpandBy > nMaximum ) nExpandBy = nMaximum - cell.rect.width();
            if ( nExpandBy <= 0 ) continue;                                                                                                                                          
                                                                                                                                                                                                 
            cell.rect = QRectF( 0, 0, cell.rect.width() + nExpandBy, rect.height() );
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


