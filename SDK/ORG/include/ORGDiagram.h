#ifndef H_ORGDiagram
#define H_ORGDiagram

#include <DDiagram.h>

class ORGDiagram;
class ORGPerson;
class ORGReportsTo;
class DSink;
class ORGLayout;

/*!
 * \brief Organization Chart
 *  
 * This is NOT meant to be a generic organization chart. 
 *  
 * The emphasis here is on direct reports and data that is useful by system developers when 
 * doing current situation analysis. 
 *  
 * \author pharvey (4/1/20)
 */
class ORGDiagram : public DDiagram
{
    Q_OBJECT
    friend class ORGLayout;
public:
    explicit ORGDiagram( ADObject *pParent, const QString &stringName = QString() );
    virtual ~ORGDiagram();

    virtual void setScene( DGraphicsSceneProxy *p );
    virtual void setRoot( ORGPerson *p ) { pRoot = p; }

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Organization Chart" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual ORGPerson *     getRoot() { return pRoot; }

    virtual bool doInsert( const QString &stringClass, ORGPerson *pRectanglePrevious );
    virtual bool doLoad( QDomElement *pdomElemModel );                               
    virtual void doPostLoad();                                                       

    virtual void mouseMoveObjects( QGraphicsSceneMouseEvent *pMouseEvent );
    virtual void mouseReleaseObjects( QGraphicsSceneMouseEvent *pMouseEvent );

public slots:
    virtual bool slotOpenEditor();
    virtual void slotSelectionCut();
    virtual void slotSelectionCopy();
    virtual void slotSelectionPaste();
    virtual void slotSelectionDelete();

protected:
    ORGPerson *pRoot;
    ORGLayout *pLayout;

    ORGPerson *pDragOver;   // flow when drag/drop is over it
    ORGPerson *pPasteTo;    // paste target

    virtual bool canDrop( const QString &stringClassDropped, const QPointF &pointScene );
    virtual bool doDragEnter( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragMove( const QString &stringClass, const QPointF &pointScene );
    virtual bool doDragDrop( const QStringList &stringList, const QPointF &pointScene );

    virtual bool canInsert( const QString &stringClass );
    virtual bool canInsert( const QString &stringClass, const QPointF &pointPos );
    virtual bool doInsert( const QString &stringClass, const QPointF &pointPos );
    virtual bool doInsert( ORGPerson *pTarget, const QList<ORGPerson*> &listObjects );
    virtual void doSelect( ORGPerson *p );
    virtual void doClear();
    virtual bool doSelectionValid();

    virtual bool canCut()       { return pSelectionManager->hasSelection(); }
    virtual bool canCopy()      { return pSelectionManager->hasSelection(); }
    virtual bool canPaste();
    virtual bool canDelete();

    virtual bool isSelectionValid();
};


#endif


