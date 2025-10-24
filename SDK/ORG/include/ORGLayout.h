#ifndef H_ORGLayout
#define H_ORGLayout

#include <ORGDiagram.h>
#include <ORGPerson.h>
#include <ORGReportsTo.h>

class ORGLayout;

// Depth,ModSum
typedef QMap<int,qreal> ORGLayoutContour;

/*!
 * \brief A simple tree. 
 *  
 * We layer this simple tree over the diagram to facilitate using a publicly available 
 * algorithm for auto-layout. 
 *  
 * \sa ORGLayout 
 *  
 * \author pharvey (4/17/20)
 */
class ORGLayoutNode
{
public:
    ORGLayoutNode( ORGPerson *pPerson, ORGLayoutNode *pParent = nullptr )
    {
        this->pPerson = pPerson; 
        this->pParent = pParent;
        nX = -1;
        nY = 0;
        nMod = 0;
        nWidth = 0;
        nHeight = 0;
    }
    ~ORGLayoutNode()
    {
        ORGLayoutNode *p;
        foreach( p, vectorChildren )
        {
            delete p;
        }
    }

    ORGPerson *pPerson;
    ORGLayoutNode *pParent;
    QVector<ORGLayoutNode*> vectorChildren;
    qreal nX;
    qreal nY;
    qreal nMod;
    qreal nWidth;
    qreal nHeight;

    bool isLeaf()
    {
        return vectorChildren.count() == 0;
    }
    bool isLeft()
    {
        if ( !pParent ) return true;
        return pParent->vectorChildren[0] == this;
    }
    bool isRight()
    {
        if ( !pParent ) return true;
        return pParent->vectorChildren[pParent->vectorChildren.count() - 1] == this;
    }
    ORGLayoutNode *getPrevious()
    {
        if ( !pParent || isLeft() ) return nullptr;
        return pParent->vectorChildren[pParent->vectorChildren.indexOf(this) - 1];
    }
    ORGLayoutNode *getNext()
    {
        if ( !pParent || isRight() ) return nullptr;
        return pParent->vectorChildren[pParent->vectorChildren.indexOf(this) + 1];
    }
    ORGLayoutNode *getLeft()
    {
        if ( !pParent ) return nullptr;
        if ( isLeft() ) return this;
        return pParent->vectorChildren[0];
    }
    ORGLayoutNode *getLeftChild()
    {
        if ( vectorChildren.count() < 1 ) return nullptr;
        return vectorChildren[0];
    }
    ORGLayoutNode *getRightChild()
    {
        if ( vectorChildren.count() < 1 ) return nullptr;
        return vectorChildren[vectorChildren.count() - 1];
    }
    qreal getLowestWidth()
    {
        if ( vectorChildren.count() == 0 ) return nWidth;
        qreal n = vectorChildren[0]->nWidth;
        ORGLayoutNode *p;
        foreach( p, vectorChildren )
        {
            if ( p->nWidth < n ) n = p->nWidth;
        }
        return n;
    }
    qreal getLowestHeight()
    {
        if ( vectorChildren.count() == 0 ) return nHeight;
        qreal n = vectorChildren[0]->nHeight;
        ORGLayoutNode *p;
        foreach( p, vectorChildren )
        {
            if ( p->nHeight < n ) n = p->nHeight;
        }
        return n;
    }
};

/*!
 * \brief Layout ORGPerson objects as per organization chart. 
 *  
 * This uses an interpretation of the Reingold-Tilford Algorithm. 
 * This is a C++ port of Rachel Lim's implementation... https://rachel53461.wordpress.com/ 
 *  
 * This is not particularly pretty nor efficient code but does seem to work fine. 
 * Some debug logic is embedded and can be activated by setting bDebug = true in the constructor. 
 *  
 * \author pharvey (4/11/20)
 */
class ORGLayout
{
public:
    explicit ORGLayout( ORGDiagram *pDiagram, qreal nHSpacing = 48, qreal nVSpacing = 48 );
    virtual ~ORGLayout() {}

    void doLayout();

protected:
    ORGDiagram *        pDiagram;
    QSize               size;           // width/height of an ORGPerson (we have a common size)
    qreal               nHSpacing;      // min H spacing between ORGPerson's in single units
    qreal               nVSpacing;      // min V spacing between ORGPerson's in single units
    qreal               nH;             // min H spacing between ORGPerson's in pixels
    qreal               nV;             // min V spacing between ORGPerson's in pixels
    qreal               nHOffset;       // used to pos root in center/top of scene
    qreal               nVOffset;       // used to pos root in center/top of scene

    void setPos( ORGLayoutNode *pNode );

    void doInit( ORGLayoutNode *pNode, int nDepth );
    void doInitX( ORGLayoutNode *pNode );
    void doFini( ORGLayoutNode *pNode, qreal nModSum );
    void doCheckForConflicts( ORGLayoutNode *pNode );
    void doCenterNodesBetween( ORGLayoutNode *pLeft, ORGLayoutNode *pRight );
    void doAllChildrenOnScreen( ORGLayoutNode *pNode );
    void doLeftContour( ORGLayoutNode *pNode, qreal nModSum, ORGLayoutContour *pContour );
    void doRightContour( ORGLayoutNode *pNode, qreal nModSum, ORGLayoutContour *pContour );


    // DEBUG
    // - help for visualizing the process for debugging purposes
    bool           bDebug;
    ORGLayoutNode *pDbgNodeCurrent;
    ORGLayoutNode *pDbgNodeConflictCheck;

    void setDbgNodeCurrent( ORGLayoutNode *pNode );
    void setDbgNodeProcessed( ORGLayoutNode *pNode );
    void setDbgNodeConflictCheck( ORGLayoutNode *pNode );
    void doDbgReset( ORGLayoutNode *pNode );

    void setDbgInfo( ORGLayoutNode *pNode );
    void doDbgPause( uint nSeconds = 1 );
};

#endif


