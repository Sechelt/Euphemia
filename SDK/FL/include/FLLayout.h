#ifndef H_FLLayout
#define H_FLLayout

#include <FLDiagram.h>

/*!
 * \brief
 * 
 * \author pharvey (4/1/20)
 */
class FLNest
{
public:
    FLNest( FLConditionOpen *p, CBD::DirectionTypes n = CBD::DirectionUnknown )
    {
        pOpen = p; nDir = n;
    }
    FLNest()
    {
        pOpen = nullptr; nDir = CBD::DirectionUnknown;
    }
    FLConditionOpen *   pOpen;
    CBD::DirectionTypes nDir;
};

/*!
 * \brief Provides methods for doing layout for FLDiagram.
 *  
 * Create an FLLayout and call the relevant methods after; 
 *  - inserting a new object (or object pair if that was the case) 
 *  - inserting 1-n connected objects to support a paste 
 *  - deleting 1-n connected objects 
 *  
 * \todo Possibly merge doExpandVertical and doCompressVertical into a doLayoutVertical. 
 *  
 * \sa FLDiagram::doInsert (this is where we handle inserting new/pasted objects)
 * \sa FLDiagram::slotSelectionDelete (this is where we handle delete)
 * \sa FLDiagram::doPostLoad (this is where we intercept a paste)
 *  
 * \author pharvey (4/1/20)
 */
class FLLayout
{
public:
    FLLayout( FLDiagram *p );

    virtual void doExpandVertical( FLRectangle *pPrevious, FLRectangle *pNext );
    virtual void doCompressVertical( FLRectangle *pPrevious, FLRectangle *pNext );

    virtual void doLayoutHorizontal( FLRectangle *pStart );

protected:
    FLDiagram * pDiagram;
    qreal       nTolerance; // min spacing (ie 2 cell widths)
    qreal       nInc;       // amount to inc by (ie 1 cell width)

    virtual qreal getExtent( FLRectangle *pCurrent, FLRectangle *pFinish, CBD::DirectionTypes nDir, qreal nMax );
    virtual qreal getExtent( FLConditionOpen *pOpen, CBD::DirectionTypes nDirStart, CBD::DirectionTypes nDirContinue );

    QStack<FLNest> stackNesting;
    virtual bool getNesting( FLRectangle *pDestination );
    virtual bool getNesting( FLRectangle *pDestination, FLRectangle *pCurrent );
    virtual FLConditionOpen* getPreviousNest( FLConditionOpen *pOpen, CBD::DirectionTypes nDir );

    virtual bool doLayoutH( FLConditionOpen *pOpen, CBD::DirectionTypes nDir );

    virtual void doMoveBy( FLConditionClose *pClose, qreal nDelta, FLRectangle *pCurrent );           // simply moves content by delta

    virtual void doDumpNesting();
};

#endif


