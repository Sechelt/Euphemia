#include "LibInfo.h"
#include "ORGLayout.h"

#ifdef Q_OS_WIN
    #include<windows.h>
#endif

#include <DGraphicsSceneProxy.h>

ORGLayout::ORGLayout( ORGDiagram *pDiagram, qreal nHSpacing, qreal nVSpacing )
{
    this->pDiagram  = pDiagram;
    this->nHSpacing = 0;
    this->nVSpacing = 0;
    this->nH = nHSpacing;
    this->nV = nVSpacing;

    bDebug                  = true;
    pDbgNodeCurrent         = nullptr;
    pDbgNodeConflictCheck   = nullptr;
}

void ORGLayout::doLayout()
{                                                            
    ORGPerson *pPerson = pDiagram->getRoot();                

    // all rectangles are the same size            
    // work with a single unit size for now - factor in real size when we set the pos of the unerlying object
    QSizeF sizePage  = pDiagram->getCalcPageSize();
    size.setWidth( 1 );
    size.setHeight( 1 );
    nHOffset = sizePage.width() / 2;
    nVOffset = pDiagram->getRoot()->boundingRect().height() / 2 + 10;

    ORGLayoutNode Root( pPerson );
    doInit( &Root, 0 );
    doInitX( &Root );
    // doAllChildrenOnScreen( &Root );
    doFini( &Root, 0 );
    if ( bDebug ) doDbgReset( &Root );
}

/*!
 * \brief Set the position of the underlying diagram object.
 * 
 * \author pharvey (4/17/20)
 * 
 * \param pNode 
 */
void ORGLayout::setPos( ORGLayoutNode *pNode )
{
    pNode->pPerson->setPos( QPointF( pNode->nX * (pNode->pPerson->boundingRect().width() + nH) + nHOffset, 
                                     pNode->nY * (pNode->pPerson->boundingRect().height() + nV) + nVOffset ) );
    if ( bDebug ) setDbgInfo( pNode );
}

/*!
 * \brief First pass... create a tree over our diagram - recording depth (Y) as we go.
 * 
 * \author pharvey (4/17/20)
 * 
 * \param pNode 
 * \param nDepth 
 */
void ORGLayout::doInit( ORGLayoutNode *pNode, int nDepth )
{
    pNode->nX = -1;
    pNode->nY = nDepth;
    pNode->nMod = 0;

    QList<ORGPerson*> l = pNode->pPerson->getSubordinates();
    ORGPerson * pSubordinate;
    foreach( pSubordinate, l )
    {
        ORGLayoutNode *pChild = new ORGLayoutNode( pSubordinate, pNode );
        pNode->vectorChildren.append( pChild );
        doInit( pChild, nDepth + 1 );
    }
}

/*!
 * \brief Second pass... calculate X and Mod for each tree node.
 * 
 * \author pharvey (4/17/20)
 * 
 * \param pNode 
 */
void ORGLayout::doInitX( ORGLayoutNode *pNode )
{
    ORGLayoutNode *pChild;
    foreach( pChild, pNode->vectorChildren )
    {
        doInitX( pChild );
    }

    if ( pNode->isLeaf() )
    {
        if ( !pNode->isLeft() )
            pNode->nX = pNode->getPrevious()->nX + size.width() + nHSpacing;
        else
            pNode->nX = 0;
    }
    else if ( pNode->vectorChildren.count() == 1 )
    {
        if ( pNode->isLeft() )
            pNode->nX = pNode->vectorChildren[0]->nX;
        else
        {
            pNode->nX = pNode->getPrevious()->nX + size.width() + nHSpacing;
            pNode->nMod = pNode->nX - pNode->vectorChildren[0]->nX;
        }
    }
    else
    {
        ORGLayoutNode *pLeftChild = pNode->getLeftChild();
        ORGLayoutNode *pRightChild = pNode->getRightChild();
        qreal nMid = (pLeftChild->nX + pRightChild->nX) / 2;
        if ( pNode->isLeft() )
            pNode->nX = nMid;
        else
        { 
            pNode->nX = pNode->getPrevious()->nX + size.width() + nHSpacing;
            pNode->nMod = pNode->nX - nMid;
        }
    }

    if ( pNode->vectorChildren.count() > 0 && !pNode->isLeft() )
    {
        doCheckForConflicts( pNode );
    }

    //
    if ( bDebug ) setPos( pNode );
    if ( bDebug ) setDbgNodeCurrent( pNode );
    if ( bDebug ) doDbgPause();
}

/*!
 * \brief Third pass... apply Mod and setPos of underlying diagram object.
 * 
 * \author pharvey (4/17/20)
 * 
 * \param pNode 
 * \param nModSum 
 */
void ORGLayout::doFini( ORGLayoutNode *pNode, qreal nModSum )
{
    // do self
    pNode->nX += nModSum;
    setPos( pNode );
    if ( bDebug ) setDbgNodeCurrent( pNode );
    if ( bDebug ) doDbgPause();

    // do children
    nModSum += pNode->nMod;
    ORGLayoutNode *p;
    foreach( p, pNode->vectorChildren )
    {
        doFini( p, nModSum );
    }

/*
    if ( pNode->vectorChildren.count() == 0 )     
    {                                             
        pNode->nWidth = pNode->nX;                
        pNode->nHeight = pNode->nY;               
    }                                             
    else                                          
    {                                             
        pNode->nWidth = pNode->getLowestWidth();  
        pNode->nHeight = pNode->getLowestHeight();
    }                                             
*/
}

void ORGLayout::doCheckForConflicts( ORGLayoutNode *pNode )
{
    if ( bDebug ) setDbgNodeConflictCheck( pNode );
    if ( bDebug ) doDbgPause();

    qreal nMin = nHSpacing + size.width();
    qreal nShift = 0;

    ORGLayoutContour mapNodeContour;
    doLeftContour( pNode, 0, &mapNodeContour );

    if ( bDebug && pNode->pPerson->getComment() == "B3" )
    { 
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Left Contour: " << pNode->pPerson->getComment();
        QMapIterator<int,qreal> i( mapNodeContour );
        while ( i.hasNext() ) 
        {
            i.next();
            qDebug() << i.key() << ": " << i.value() << Qt::endl;
        }
    }

    ORGLayoutNode *pSibling = pNode->getLeft();
    while ( pSibling && pSibling != pNode )
    {
        ORGLayoutContour mapSiblingContour;
        doRightContour( pSibling, 0, &mapSiblingContour );

        if ( bDebug && pNode->pPerson->getComment() == "B3" )
        { 
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Right Contour: " << pSibling->pPerson->getComment();
            QMapIterator<int,qreal> i( mapSiblingContour );
            while ( i.hasNext() ) 
            {
                i.next();
                qDebug() << i.key() << ": " << i.value() << Qt::endl;
            }
        }

        for ( int nLevel = pNode->nY + 1; nLevel <= qMin( mapSiblingContour.lastKey(), mapNodeContour.lastKey() ); nLevel++ )
        {
            qreal nDistance = mapNodeContour[nLevel] - mapSiblingContour[nLevel];
            if ( nDistance + nShift < nMin )
            {
                nShift = qMax( nMin - nDistance, nShift );
                // nShift = nMin - nDistance;
            }
        }

        pSibling = pSibling->getNext();
    }

    if ( nShift > 0 )
    {
        pNode->nX += nShift;
        pNode->nMod += nShift;
        if ( bDebug ) setPos( pNode );
        if ( bDebug ) setDbgNodeCurrent( pNode );
        if ( bDebug ) doDbgPause();

        // doCenterNodesBetween( pNode, pSibling );

        nShift = 0;
    }

    if ( bDebug ) setDbgNodeConflictCheck( nullptr );
    if ( bDebug ) setDbgNodeProcessed( pNode );
    if ( bDebug ) doDbgPause();
}

void ORGLayout::doCenterNodesBetween( ORGLayoutNode *pLeft, ORGLayoutNode *pRight )
{
//    int nLeftIndex = pLeft->pParent->vectorChildren.indexOf( pRight );
//    int nRightIndex = pLeft->pParent->vectorChildren.indexOf( pLeft );
    int nLeftIndex      = pLeft->pParent->vectorChildren.indexOf( pLeft );
    int nRightIndex     = pLeft->pParent->vectorChildren.indexOf( pRight );
    int nNodesBetween   = (nRightIndex - nLeftIndex) - 1;

    if ( nNodesBetween > 0 )
    { 
//        qreal nDistanceBetweenNodes = (pLeft->nX - pRight->nX) / (nNodesBetween + 1);
        qreal nDistanceBetweenNodes = (pRight->nX - pLeft->nX) / (nNodesBetween + 1);
        int nCount = 1;
        for ( int n = nLeftIndex + 1; n < nRightIndex; n++ )
        {
            ORGLayoutNode *pMiddle = pLeft->pParent->vectorChildren[n];
            qreal nDesiredX = pRight->nX + (nDistanceBetweenNodes + nCount);
            qreal nOffset = nDesiredX - pMiddle->nX;

            pMiddle->nX += nOffset;
            pMiddle->nMod += nOffset;

            nCount++;
        }
        doCheckForConflicts( pLeft );
    }
}

void ORGLayout::doAllChildrenOnScreen( ORGLayoutNode *pNode )
{
    ORGLayoutContour mapContour;
    doLeftContour( pNode, 0, &mapContour);

    qreal nShift = 0;
    QList<int> l = mapContour.keys();
    int n;
    foreach ( n, l )
    {
        if ( mapContour[n] + nShift < 0 )
            nShift = (mapContour[n] * -1);
    }

    if (nShift > 0)
    {
        pNode->nX += nShift;
        pNode->nMod += nShift;
    }
}

void ORGLayout::doLeftContour( ORGLayoutNode *pNode, qreal nModSum, ORGLayoutContour *pContour )
{
    if ( !pContour->contains( pNode->nY ) )
        pContour->insert( pNode->nY, pNode->nX + nModSum);
    else
        (*pContour)[pNode->nY] = qMin( (*pContour)[pNode->nY], pNode->nX + nModSum );

    nModSum += pNode->nMod;
    ORGLayoutNode *pChild;
    foreach( pChild, pNode->vectorChildren )
    {
        doLeftContour( pChild, nModSum, pContour );
    }
}

void ORGLayout::doRightContour( ORGLayoutNode *pNode, qreal nModSum, ORGLayoutContour *pContour )
{
    if ( !pContour->contains( pNode->nY ) )
        pContour->insert( pNode->nY, pNode->nX + nModSum );
    else
        (*pContour)[pNode->nY] = qMax( (*pContour)[pNode->nY], pNode->nX + nModSum );

    nModSum += pNode->nMod;
    ORGLayoutNode *pChild;
    foreach( pChild, pNode->vectorChildren )
    {
        doRightContour( pChild, nModSum, pContour );
    }
}

void ORGLayout::setDbgInfo( ORGLayoutNode *pNode )
{
    pNode->pPerson->setName( QString( "X:%1 Y:%2 Mod:%3" ).arg( pNode->nX ).arg( pNode->nY ).arg( pNode->nMod ) );
}

void ORGLayout::doDbgPause( uint nSeconds )
{
    qApp->processEvents();
#ifdef Q_OS_WIN
    Sleep( nSeconds / 1000 );
#else
    sleep( nSeconds );  
#endif

}

void ORGLayout::setDbgNodeCurrent( ORGLayoutNode *pNode )
{
    if ( pDbgNodeCurrent )
    {
        QPen pen( Qt::black );
        pen.setWidth( 1 );
        pDbgNodeCurrent->pPerson->slotPen( pen );
        pDbgNodeCurrent = nullptr;
    }
    if ( pNode )
    { 
        QPen pen( Qt::green );
        pen.setWidth( 2 );
        pNode->pPerson->slotPen( pen );
        pDbgNodeCurrent = pNode;
    }
}

void ORGLayout::setDbgNodeProcessed( ORGLayoutNode *pNode )
{
    if ( !pNode ) return;
    QBrush brush( Qt::SolidPattern );
    brush.setColor( Qt::lightGray );
    pNode->pPerson->slotBrush( brush );
}

void ORGLayout::setDbgNodeConflictCheck( ORGLayoutNode *pNode )
{
    if ( pDbgNodeConflictCheck )
    {
        QBrush brush( Qt::NoBrush );
        pDbgNodeConflictCheck->pPerson->slotBrush( brush );
        pDbgNodeConflictCheck = nullptr;
    }
    if ( pNode )
    { 
        QBrush brush( Qt::SolidPattern );
        brush.setColor( Qt::red );
        pNode->pPerson->slotBrush( brush );
        pDbgNodeConflictCheck = pNode;
    }
}

void ORGLayout::doDbgReset( ORGLayoutNode *pNode )
{
    pDbgNodeCurrent         = nullptr;
    pDbgNodeConflictCheck   = nullptr;

    QBrush brush( Qt::NoBrush );
    pNode->pPerson->slotBrush( brush );
    QPen pen( Qt::black );
    pen.setWidth( 1 );
    pNode->pPerson->slotPen( pen );

    ORGLayoutNode *p;
    foreach( p, pNode->vectorChildren )
    {
        doDbgReset( p );
    }
}


