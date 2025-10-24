#ifndef H_DObject
#define H_DObject

#include <AWObject.h>

#include "DHandle.h"

class AWDataObjectWidget;
class DGraphicsItemLine;
class DDiagram;
class DGroup;
class DLayout;

/*!
 * \brief Base class for the principle object we can place on a DDiagram.
 *
 * Parent
 *
 * Top level DIObjects have \sa DDiagram as parent while others have another DObject as its parent.
 * This is similar to QGraphicsScene and QGraphicsItem.
 *
 * Stacking Order
 *
 * Default is the order in which added to parent.
 * A DObject is moved by reparenting to get desired order.
 * A QGraphicsItem is moved by changing ZOrder value.
 * DObject will 'intercept' request to \sa ADObject for changing order. It will call ADObject
 * but also call \sa DIDiagramItemProxy to make this change. In this way they are kept in sync.
 * 
 * Proxy
 * 
 * DObject is a data store first (with some processing for convenience). This means it resides in app-data as
 * part of a hierarchial/networked data store with many objects having nothing to do with diagrams. 
 * For this reason; DObject does not derive from a QGraphicsItem or even QGraphicsObject. A DObject interacts
 * with a QGraphicsObject via \sa DGraphicsProxyItem.
 *
 * \author pharvey (8/27/20)
 */
class DObject : public AWObject
{
    Q_OBJECT
public:
    enum SelectTypes
    {
        SelectOnly,
        SelectResize,
        SelectTransformRotate,
        SelectTransformShear,
        SelectMenu
    };
    enum TextDisplayTypes
    {
        TextDisplayName,
        TextDisplayCode,
        TextDisplayComment,
        TextDisplayNone
    };

    explicit DObject( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DObject();

    virtual void setProxy( DGraphicsProxyItem *p );
    virtual void setPos( const QPointF &point );
    virtual void setPosDelta( const QPointF &pointDelta );
    virtual void setPosRaw( const QPointF &pointDelta );
    virtual void setPosDeltaRaw( const QPointF &pointDelta );
    virtual void setVisible( bool b = true );
    virtual void setSelected( bool b = true );
    virtual void setSelectType();
    virtual void setSelectType( SelectTypes n );
    virtual void setTextDisplay( TextDisplayTypes n );

    virtual ADObjectSelectionManager *  getSelectionManager();
    virtual DDiagram *                  getDiagram();
    virtual DGraphicsProxyItem *        getProxy()          { return pProxy; }
    virtual QPointF                     getPos()            { return pointPos; }
    virtual QPointF                     getPosCenter();
    virtual QMenu *                     getContextMenu( QWidget * );
    virtual TextDisplayTypes            getTextDisplay()    { return nTextDisplay; }
    virtual QPen                        getPen()            { return pen; }
    virtual QBrush                      getBrush()          { return brush; }
    virtual CBD::CBDText                getText()           { return font; }
    virtual AWDataWidget *              getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *              getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual SelectTypes                 getSelectType()     { return nSelectType; }
    virtual DGroup *                    getGroup( bool bOutter = false );
    virtual DLayout *                   getLayout();
    virtual bool                        inGroup()           { return getGroup(); }
    virtual bool                        inLayout()          { return getLayout(); }

    virtual void doToBack();
    virtual void doToFront();
    virtual void doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBefore( ADObject *pObject );
    virtual void doToAfter( ADObject *pObject );
    virtual void doToIndex( int nIndex );
    virtual void doReparent( ADObject *pObject );
    virtual void doGoTo( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doGoTo();
    virtual void doGroupSync() = 0;                                 // to support \sa DGroup and \sa DLayout move
    virtual void doGroupScale( const QSizeF &sizeFactor ) = 0;      // to support \sa DGroup::doScale()
    virtual void doGroupIn( const QPointF &pointDelta ) = 0;        // to support \sa DGroup::doScale()
    virtual void doGroupOut( const QPointF &pointDelta ) = 0;       // to support \sa DGroup::doScale()

    // save/load item/element of a file
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    virtual void doUsesAdd( ADObject *pmodel, const QString &stringName = QString() );
    virtual void doUsesRemove( ADObject *pmodel, const QString &stringName = QString() );

    virtual QRectF boundingRect() const = 0;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget ) = 0;
    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos ) = 0;
    virtual QPainterPath shape() const = 0;

    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene ) = 0;
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene ) = 0;
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene ) = 0;

    virtual void contextMenu( QGraphicsSceneContextMenuEvent *pEvent );
    virtual void contextMenu( DHandle *pHandle, QGraphicsSceneContextMenuEvent *pEvent );

signals:
    // changed signals
    void signalChanged( const QPen &pen );
    void signalChanged( const QBrush &brush );
    void signalChanged( const CBD::CBDText &font );
    void signalTextDisplay( int n );

public slots:
    virtual void slotDelete();
    virtual void slotModifiedReference( ADObject * );
    // request to change slots
    virtual void slotPen( const QPen &pen );
    virtual void slotBrush( const QBrush &brush );
    virtual void slotText( const CBD::CBDText &font );
    virtual void slotTextDisplay( int n );

protected:
    DGraphicsProxyItem *pProxy;     // we can have 0-1 proxy - we should have 1 very early on
    QPointF             pointPos;
    CBD::CBDText        font;       // font
    QPen                pen;        // line (incl. color)
    QBrush              brush;      // fill (incl. color)
    TextDisplayTypes    nTextDisplay;
    SelectTypes         nSelectType;
    bool                bMovingHandle;

    // HANDLES
    // selection/manipulation handles
    // - order will indicate their use (order is important) with static coming first and any after being dynamic
    // - created when selected and deleted when unselected - among other things it ensures handles are on top
    // - sources (and thier handles) are introduced in DLine
    // - sinks (and thier handles) are introduced in DRectangle
    QVector<DHandle*> vectorHandles;

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos ) = 0;
    virtual void doHandlesCreate() = 0;
    virtual void doHandlesDelete() = 0;
    virtual void doHandlesSync() = 0;

    virtual void doInitSelect();
    virtual void doFiniSelect();
    virtual void doSyncSelect();
};

#endif


