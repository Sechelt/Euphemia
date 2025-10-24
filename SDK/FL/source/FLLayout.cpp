#include "LibInfo.h"
#include "FLLayout.h"

#include <CBD.h>

// #include <DGraphicsSceneProxy.h>
// #include <DGraphicsProxyItem.h>
// #include <DGraphicsView.h>

#include "FLDiagram.h"
#include "FLBegin.h"
#include "FLConditionOpen.h"
#include "FLConditionClose.h"
#include "FLEnd.h"
#include "FLFlow.h"
#include "FLLoopClose.h"
#include "FLLoopOpen.h"
#include "FLStep.h"

FLLayout::FLLayout( FLDiagram *p )
{
    pDiagram    = p;
    nTolerance  = pDiagram->getCell().width() * 2;
    nInc        = pDiagram->getCell().width();
}

/*!
 * \brief Move objects down, as needed, to ensure adequate spacing. 
 *  
 * Presumably we have inserted an object and we have called here to adjust the layout 
 * so objects are spaced properly on the vertical axis.  
 *  
 * This will recurse on itself until it comes to FLEnd.
 *  
 * \sa doInsert 
 *  
 * \author pharvey (4/1/20)
 *      
 * \param pPrevious This is the previous object to pNext.
 * \param pNext This is the inserted object on the first call and next object during recurse.
 */
void FLLayout::doExpandVertical( FLRectangle *pPrevious, FLRectangle *pNext )
{
    // calculate delta...
    int nPreviousY = pDiagram->mapSceneToCell( pPrevious->getPos() ).y();
    int nNextY     = pDiagram->mapSceneToCell( pNext->getPos() ).y();
    int nDeltaY    = nNextY - nPreviousY;

   if ( nDeltaY < 0 )  
   {
       nDeltaY = abs(nDeltaY) + 2;
       pNext->setPos( QPointF( pNext->getPos().x(), pNext->getPos().y() + nDeltaY * pDiagram->getCell().height() ) );
   }
   else if ( nDeltaY < 2 ) 
   {
       nDeltaY = 2 - nDeltaY;
       pNext->setPos( QPointF( pNext->getPos().x(), pNext->getPos().y() + nDeltaY * pDiagram->getCell().height() ) );
   }
   else
   {
       // spacing ok but we carry on because spacing can be ok when working way through 
       // pasted objects... BUT still need to push objects down after the pasted objects
   }

    // do next
    if ( pNext->inherits( "FLConditionOpen" ) )
    {
        DRectangle *pLeft  = pNext->getSink( "Left" )->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
        DRectangle *pRight = pNext->getSink( "Right" )->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
        doExpandVertical( pNext, (FLRectangle*)pLeft );
        if ( pLeft != pRight ) doExpandVertical( pNext, (FLRectangle*)pRight );
    }
    else
    {
        DSink *pSink = pNext->getSink( "Bottom" );
        if ( pSink )
        { 
            DRectangle *pBottom = pSink->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
            doExpandVertical( pNext, (FLRectangle *)pBottom );
        }
    }
}

/*!
 * \brief Move objects up, as needed, to avoid wasting space.
 *  
 * Presumably we have deleted 1-n objects and we have called here to adjust the layout 
 * so objects are spaced properly on the vertical axis.  
 *  
 * This will recurse on itself until it comes to FLEnd. 
 *  
 * \sa slotSelectionDelete() 
 *  
 * \author pharvey (4/1/20)
 *      
 * \param pPrevious This is the previous object to whatever object(s) were deleted.
 * \param pNext This is the next object to whatever object(s) were deleted.
 */
void FLLayout::doCompressVertical( FLRectangle *pPrevious, FLRectangle *pNext )
{
    if ( !pNext ) return;
    // 
    if ( pNext->inherits( "FLConditionClose" ) )
    {

        // take the longer of the two sides
        FLConditionClose *pClose = (FLConditionClose*)pNext;
        FLRectangle *pPrev1 = pClose->getPrevious( CBD::DirectionW );
        FLRectangle *pPrev2 = pClose->getPrevious( CBD::DirectionE );
        if ( pPrev1->getPos().y() > pPrevious->getPos().y() ) pPrevious = pPrev1;
        else if ( pPrev2->getPos().y() > pPrevious->getPos().y() ) pPrevious = pPrev2;
    }

    // calculate delta...
    int nPreviousY = pDiagram->mapSceneToCell( pPrevious->getPos() ).y();
    int nNextY     = pDiagram->mapSceneToCell( pNext->getPos() ).y();
    int nDeltaY    = nNextY - nPreviousY;

    if ( nDeltaY < 0 ) nDeltaY = abs(nDeltaY) + 2; 
    else if ( nDeltaY > 2 ) nDeltaY = 2 - nDeltaY;
    else return;

    // apply delta
    pNext->setPos( QPointF( pNext->getPos().x(), pNext->getPos().y() + nDeltaY * pDiagram->getCell().height() ) );

    // do next
    if ( pNext->inherits( "FLConditionOpen" ) )
    {
        DRectangle *pLeft  = pNext->getSink( "Left" )->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
        DRectangle *pRight = pNext->getSink( "Right" )->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
        doCompressVertical( pNext, (FLRectangle*)pLeft );
        if ( pLeft != pRight ) doCompressVertical( pNext, (FLRectangle*)pRight );
    }
    else
    {
        DSink *pSink = pNext->getSink( "Bottom" );
        if ( pSink )
        { 
            DRectangle *pBottom = pSink->getSources().first()->getLine()->getRectangle( CBD::EOLEnd );
            doCompressVertical( pNext, (FLRectangle *)pBottom );
        }
    }
}

/*!
 * \brief Expand East and/or West to make space for pStart to pFinish. 
 *  
 * \author pharvey (4/5/20)
 * 
 * \param pStart 
 * \param pFinish 
 * \param nDirExpanded 
 */
void FLLayout::doLayoutHorizontal( FLRectangle *pStart )                                            
{                                                                                                                                
    // get nesting from begin to pStart                                                                                               
    if ( !getNesting( pStart ) )                                                                                                      
    {                                                                                                                            
        qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " Nesting could not be determined.";        
        return;                                                                                                                  
    }                                                                                                                            
                                                                                                                                 
    if ( stackNesting.count() < 1 )                                                                                              
    {                                                                                                                            
        qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " No nesting. Should not have been called.";
        return;                                                                                                                  
    }
                                                                                                                                    
// doDumpNesting();
    
    FLConditionOpen *   pOpen       = stackNesting.last().pOpen;
    CBD::DirectionTypes nDirExpand  = stackNesting.last().nDir;
    CBD::DirectionTypes nDirNest    = ( nDirExpand == CBD::DirectionE ? CBD::DirectionW : CBD::DirectionE );

    while ( pOpen )
    {
        doLayoutH( pOpen, nDirExpand );
        pOpen = getPreviousNest( pOpen, nDirNest );
    }
}                                                                                                                                

/*!
 * \brief Returns the extent (East or West) of a block of connected rectangles (including Stretch).
 *  
 * Starts at pCurrent and recurses until pFinish. Processing includes pCurrent and pFinish. 
 * Works its way down (South) always choosing nDir when pCurrent is a FLConditionOpen. 
 * Will use extent of a Flow when the Flow is stretched. 
 *  
 * \author pharvey (4/5/20)
 * 
 * \param pCurrent 
 * \param pFinish 
 * \param nDir 
 * \param nMax 
 * 
 * \return qreal 
 */
qreal FLLayout::getExtent( FLRectangle *pCurrent, FLRectangle *pFinish, CBD::DirectionTypes nDir, qreal nMax )
{
    if ( !pCurrent ) return nMax;

    qreal nX = pCurrent->getPos().x();

    // assumes nStretch != 0 means stretch is in effect
    // assumes nStretch is negative for West
    // assumes nStretch is from pCurrent->getPos().x() (not sink pos)
    if ( pCurrent->inherits( "FLConditionOpen" ) && ((FLConditionOpen*)pCurrent)->getStretch( nDir ) )
        nX = pCurrent->getPos().x() + ((FLConditionOpen*)pCurrent)->getStretch( nDir );

    if ( nDir == CBD::DirectionE )
    {
        if ( nX > nMax ) nMax = nX;
    }
    else
    {
        if ( nX < nMax ) nMax = nX;
    }

//    qDebug() << __FUNCTION__ << "] REVISED > Current:" << pDiagram->mapSceneToCell( pCurrent->getPos() ) << "nMax:" << nMax;

    if ( pCurrent == pFinish ) return nMax;

    return getExtent( pCurrent->getNext( nDir ), pFinish, nDir, nMax );
}

qreal FLLayout::getExtent( FLConditionOpen *pOpen, CBD::DirectionTypes nDirStart, CBD::DirectionTypes nDirContinue )
{
    // Handle case where we have no content. In such a case we use the Stretch (if any) and fall back
    // on the pos of Open. Then return - we are done. 
    FLRectangle *       pNext   = pOpen->getNext( nDirStart );
    FLConditionClose *  pClose  = pOpen->getClose();
    if ( pNext == pClose )
    { 
// qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
       if ( pOpen->getStretch( nDirContinue ) ) return pOpen->getPos().x() + pOpen->getStretch( nDirContinue );
// qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
       return pOpen->getPos().x();
    }

//    qDebug() << __FUNCTION__ << "] INIT > Open:" << pDiagram->mapSceneToCell( pOpen->getPos() ) << "Stretch:" << pOpen->getStretch( nDirStart ) << "nX:" << nX;
    
    FLRectangle *   pStart      = pNext;                                // Do not factor in our Open. So start with next.
    FLRectangle *   pFinish     = pClose->getPrevious( nDirStart );     // Do not factor in our Close. So finish on its previous rectangle.
    qreal           nMax        = pNext->getPos().x();                  // Starting with 0 does not work so start with next value even if redundant.
    return getExtent( pStart, pFinish, nDirContinue, nMax );
}

/*!
 * \brief 
 *  
 * pOpen == nullptr to get last in list otherwise search starts after scanned past pOpen 
 * 
 * \author pharvey (4/3/20)
 * 
 * \param pOpen 
 * \param nDir 
 * 
 * \return FLConditionOpen* 
 */
FLConditionOpen* FLLayout::getPreviousNest( FLConditionOpen *pOpen, CBD::DirectionTypes nDir )
{
    QVectorIterator<FLNest> i( stackNesting );
    bool bFound = ( pOpen ? false : true );     // pOpen == nullptr to get last in list otherwise search starts after scanned past pOpen
    i.toBack();                                 // move to after last
    while ( i.hasPrevious() )
    {
        FLNest Nest = i.previous();
        if ( bFound && Nest.nDir == nDir ) return Nest.pOpen;
        if ( !bFound && Nest.pOpen == pOpen ) bFound = true;
    }

    return nullptr;
}

bool FLLayout::getNesting( FLRectangle *pDestination )
{
    stackNesting.clear();
    return getNesting( pDestination, pDiagram->getBegin() );
}

bool FLLayout::getNesting( FLRectangle *pDestination, FLRectangle *pCurrent )
{
    if ( !pCurrent ) return false;
    if ( pCurrent == pDestination ) return true;

    if ( pCurrent->inherits( "FLConditionOpen" ) )
    {
        FLConditionOpen *pOpen = (FLConditionOpen*)pCurrent;
        FLConditionClose *pClose = pOpen->getClose();
        
        stackNesting.push( FLNest( pOpen, CBD::DirectionW ) );

        pClose->setIgnore();
        if ( getNesting( pDestination, pOpen->getNext( CBD::DirectionW ) ) )
        {
            pClose->setIgnore( false );
            return true;
        }
        pClose->setIgnore( false );
        stackNesting.last().nDir = CBD::DirectionE;
        return getNesting( pDestination, pOpen->getNext( CBD::DirectionE ) );
    }

    if ( pCurrent->inherits( "FLConditionClose" ) )
    {
        FLConditionClose *pClose = (FLConditionClose*)pCurrent;
        if ( pClose->getIgnore() ) return false;

        stackNesting.pop();

        return getNesting( pDestination, pClose->getNext() );
    }

    return getNesting( pDestination, pCurrent->getNext() );
}

bool FLLayout::doLayoutH( FLConditionOpen *pOpen, CBD::DirectionTypes nDirExpand )
{
    // If we need space we move content to nDir.
    // If we have extra space we move content to opposite nDir.
    // Return false if nothing needed changing.

    FLConditionClose *pClose = pOpen->getClose();
    FLRectangle *pNext = pOpen->getNext( nDirExpand );

    qreal nDelta = 0;
    {
        qreal nSpace;
        qreal nExtentOpposite;
        qreal nExtent;
        if ( nDirExpand == CBD::DirectionE )
        {
            nExtentOpposite = getExtent( pOpen, CBD::DirectionW, CBD::DirectionE );
            nExtent         = getExtent( pOpen, CBD::DirectionE, CBD::DirectionW );
            nSpace          = nExtent - nExtentOpposite;
            if ( nSpace == 0 ) nDelta = nTolerance;
            else if ( nSpace < 0 ) nDelta = abs( nSpace ) + nTolerance;
            else if ( nSpace < nTolerance ) nDelta = nTolerance - nSpace;
            else
            {
                // compress
                nDelta = -(nSpace - nTolerance);
                if ( pNext != pClose ) 
                {
                    // do not collapse the elbows
                    if ( (pNext->getPos().x() + nDelta) < (pOpen->getPos().x() + nTolerance) )
                       nDelta = pOpen->getPos().x() - pNext->getPos().x() + nTolerance;
                }
            }
        }
        else
        {
            nExtentOpposite = getExtent( pOpen, CBD::DirectionE, CBD::DirectionW );
            nExtent         = getExtent( pOpen, CBD::DirectionW, CBD::DirectionE );
            nSpace          = nExtentOpposite - nExtent;
            if ( nSpace == 0 ) nDelta = -(nTolerance);
            else if ( nSpace < 0 ) nDelta = -(abs( nSpace ) + nTolerance);
            else if ( nSpace < nTolerance ) nDelta = -(nTolerance - nSpace);
            else 
            {
                // compress
                nDelta = nSpace - nTolerance;
                if ( pNext != pClose ) 
                {
                    // do not collapse the elbows
                    if ( (pNext->getPos().x() + nDelta) > (pOpen->getPos().x() - nTolerance) )
                       nDelta = pOpen->getPos().x() - pNext->getPos().x() - nTolerance;
                }
            }
        }
//        qDebug() << __FUNCTION__ << "] Open:" << pDiagram->mapSceneToCell( pOpen->getPos() ) << "Expand:" << CBD::DirectionTypeStrings[nDirExpand];
//        qDebug() << __FUNCTION__ << "] nExtentOpposite:" << nExtentOpposite << "nExtent:" << nExtent << "nSpace:" << nSpace << "Delta: " << nDelta;
//        qDebug() << "";
    }

    if ( !nDelta ) return false;

    // Handle case where no content.
    if ( pNext == pClose )
    { 
        // No content so adjust stretch.
        pOpen->setStretch( nDirExpand, pOpen->getStretch( nDirExpand ) + nDelta );
        return true;
    }

    doMoveBy( pOpen->getClose(), nDelta, pOpen->getNext( nDirExpand ) );

    return true;
}

/*!
 * \brief Move content of a FLConditionOpen - which includes any nested conditions.
 *  
 * Will recurse until it gets to pClose. 
 *  
 * \author pharvey (4/2/20)
 * 
 * \param pClose 
 * \param nDeltaX 
 * \param pCurrent 
 */
void FLLayout::doMoveBy( FLConditionClose *pClose, qreal nDelta, FLRectangle *pCurrent  )
{
    if ( !pCurrent ) return;
    if ( pCurrent == pClose ) return;

    if ( pCurrent->inherits( "FLConditionOpen" ) )
    {
        FLConditionOpen *pOpen = (FLConditionOpen*)pCurrent;
        FLConditionClose *p = pOpen->getClose();

        p->setIgnore();
        doMoveBy( pClose, nDelta, pOpen->getNext( CBD::DirectionW ) );

        p->setIgnore( false );
        doMoveBy( pClose, nDelta, pOpen->getNext( CBD::DirectionE ) );

        pOpen->setPos( QPointF( pOpen->getPos().x() + nDelta, pOpen->getPos().y() ) );

        return;
    }

    if ( pCurrent->inherits( "FLConditionClose" ) )
    {
        FLConditionClose *p = (FLConditionClose*)pCurrent;
        if ( p->getIgnore() ) return;
    }

    pCurrent->setPos( QPointF( pCurrent->getPos().x() + nDelta, pCurrent->getPos().y() ) );
    doMoveBy( pClose, nDelta, pCurrent->getNext() );
}

void FLLayout::doDumpNesting()
{
    qDebug() << "Nest:";
    FLNest Nest;
    foreach( Nest, stackNesting )
    {
        qDebug() << "\t" << CBD::DirectionTypeStrings[Nest.nDir] << pDiagram->mapSceneToCell( Nest.pOpen->getPos() );
    }
}


