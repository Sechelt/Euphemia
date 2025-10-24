#ifndef H_DCallOut
#define H_DCallOut

#include "DLine.h"

/*!
 * \brief Call outs are special lines that are used for attaching notes to objects. 
 *  
 * The line and the note are integrated into a special line. 
 * 
 * \author pharvey (5/1/20)
 */
class DCallOut : public DLine
{
    Q_OBJECT
public:
    explicit DCallOut( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DCallOut();

    virtual void setPos( const QPointF &point );
    virtual void setPosRaw( const QPointF &point );
    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Call Out" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual Qt::AlignmentFlag   getTextHAlign() { return nTextHAlign; }
    virtual Qt::AlignmentFlag   getTextVAlign() { return nTextVAlign; }
    virtual int                 getTextFlags();
    virtual bool                isWordWrap() { return bWordWrap; }

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual void doPostLoad();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotTextVAlign( Qt::AlignmentFlag n );
    virtual void slotTextHAlign( Qt::AlignmentFlag n );
    virtual void slotWordWrap( Qt::CheckState n );

protected slots:
    virtual void slotStyleBegin( W::WLineEndStyles n );
    virtual void slotTruncateBegin( Qt::CheckState n );
    virtual void slotTruncateEnd( Qt::CheckState n );

protected:
    QRectF              rectText;           // scene coords  - just the text box (used to calc entire shape)
    QPolygonF           polygonCallOut;     // item coords  - entire shape (used for painting etc)
    QLineF              lineIntersectedEnd;
    Qt::AlignmentFlag   nTextHAlign;        // Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter
    Qt::AlignmentFlag   nTextVAlign;        // Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter
    bool                bWordWrap;

    virtual void doMoveSource( DSource *pSource, const QPointF &pointScenePos );
    virtual void doSourcesCreate();

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesSync();

    virtual void doCalculateShape();
};

#endif


