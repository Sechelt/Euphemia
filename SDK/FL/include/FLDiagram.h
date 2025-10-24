#ifndef H_FLDiagram
#define H_FLDiagram

#include <DDiagram.h>

class FLDiagram;
class FLRectangle;
class FLBegin;
class FLConditionOpen;
class FLConditionClose;
class FLEnd;
class FLFlow;
class FLLoopClose;
class FLLoopOpen;
class FLStep;
class DSink;

/*!
 * \brief Flow Chart. 
 *  
 * This NOT meant to be a generic Flow Chart. 
 *  
 * Some use cases; 
 *  
 * - record processing logic during a current situation analysis - perhaps during employee interviews/workshops 
 * - flesh out logic supporting conceptual and logical models 
 * - 
 * 
 * \author pharvey (4/1/20)
 */
class FLDiagram : public DDiagram
{
    Q_OBJECT
public:
    explicit FLDiagram( ADObject *pParent, const QString &stringName = QString() );
    virtual ~FLDiagram();

    virtual void setScene( DGraphicsSceneProxy *p );
    virtual void setBegin( FLBegin *p ) { pBegin = p; }
    virtual void setEnd( FLEnd *p ) { pEnd = p; }

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Flow Chart" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual FLBegin *       getBegin() { return pBegin; }
    virtual FLEnd *         getEnd() { return pEnd; }

    virtual bool doInsert( const QString &stringClass, FLRectangle *pRectanglePrevious, DSink *pSink );
    virtual bool doLoad( QDomElement *pdomElemModel );                               
    virtual void doPostLoad();                                                       

    virtual void mouseMoveObjects( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseReleaseObjects( QGraphicsSceneMouseEvent *pMouseEvent );

public slots:
    virtual bool slotOpenEditor();
    virtual void slotSelectionCopy();
    virtual void slotSelectionDelete();

protected:
    FLBegin *       pBegin;
    FLEnd *         pEnd;

    // these are set when isSelectionValid() returns true
    FLRectangle *   pSelectionStart;
    FLRectangle *   pSelectionFinish;

    FLFlow *pFlowDragOver;  // flow when drag/drop is over it
    FLFlow *pFlowPaste;     // paste target

    virtual FLFlow *        getFlow( const QPoint &pointCell );
    virtual FLRectangle *   getPrevious( const QPoint &pointCell );
    virtual FLRectangle *   getNext( const QPoint &pointCell );

    virtual bool canDrop( const QString &stringClassDropped, const QPointF &pointScene );
    virtual bool doDragEnter( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragMove( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragDrop( const QStringList &stringList, const QPointF &pointScene );

    virtual void doMoveBy( FLRectangle *pStart, FLRectangle *pFinish, const QPointF &pointDelta );

    virtual bool canInsert( const QString &stringClass );
    virtual bool canInsert( const QString &stringClass, const QPoint &pointCell );

    virtual bool doInsert( const QString &stringClass, const QPoint &pointCell );
    virtual bool doInsert( FLFlow *pFlow, FLRectangle *pFirst, FLRectangle *pLast );

    virtual void doClear();

    virtual bool canPaste();
    virtual bool canCopy() { return isSelectionValid(); }
    virtual bool canDelete()  { return isSelectionValid(); }

    virtual bool isSelectionValid();
};


#endif


