#ifndef H_DRectangle
#define H_DRectangle

#include <W.h>

#include "DRectangleBase.h"

#include "DSink.h"
#include "DSource.h"

class DRectangle : public DRectangleBase
{
    Q_OBJECT
public:
    explicit DRectangle( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DRectangle();

    virtual void setName( const QString &sringName );      
    virtual void setCode( const QString &sringCode );      
    virtual void setComment( const QString &sringComment );
    virtual void setSize( const QSizeF &size );
    virtual void setPos( const QPointF &pointScene );      
    virtual void setPosRaw( const QPointF &pointScene );
    virtual void setProxy( DGraphicsProxyItem *p );
    virtual void setDynamicSinks( bool b ) { bDynamicSinks = b; }
    virtual void setSelectType();

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Rectangle" ); }
    virtual Qt::AlignmentFlag   getTextHAlign() { return nTextHAlign; }
    virtual Qt::AlignmentFlag   getTextVAlign() { return nTextVAlign; }
    virtual int                 getTextFlags();
    virtual QMenu *             getContextMenu( QWidget * );
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual bool                getDynamicSinks() { return bDynamicSinks; }
    virtual QVector<DSink*>     getSinks() { return vectorSinks; }
    virtual DSink *             getSink( const QString &stringName );
    virtual DLine *             getLine( const QString &stringSink );
    virtual int                 getShapeSize();
    virtual QSizeF              getSize() { return rect.size(); }
    virtual bool                isWordWrap() { return bWordWrap; }
    virtual bool                isInitTransform() { return bInitTransform; }
    virtual bool                isFixedSize() { return bFixedSize; }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

    // The scene will call these. Consider them as 'events'. 
    virtual bool doSourceEnter( DSource *p );
    virtual bool doSourceExit( DSource *p );
    virtual bool doSourceDrop( DSource *p, const QPointF &pointScene );

    // glue/unglueing of sources and sinks
    // Primary purpose is for DLine to call these in support of 'connect/disconnect.
    // \sa DLine::doConnect
    // \sa DLine::doDisconnect
    virtual bool doGlue( DSource *pSource, DSink *pSink );
    virtual bool doGlue( DSource *pSource );
    virtual bool doUnglue( DSource *pSource, DSink *pSink );
    virtual bool doUnglue( DSink *pSink );
    virtual bool doUnglue();

    virtual bool isGlued( DSink *pSink, DSource *pSource );
    virtual bool isGlued( DSource *pSource );

    virtual void doGroupSync();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

signals:
    void signalChangedTextVAlign( Qt::AlignmentFlag );
    void signalChangedTextHAlign( Qt::AlignmentFlag );
    void signalChangedShapeSize( int );
    void signalChangedPos( const QPointF &pointPos );
    void signalChangedSize( const QSizeF &size );

public slots:
    virtual void slotSize( const QSizeF &size );
    virtual void slotTextVAlign( Qt::AlignmentFlag n );
    virtual void slotTextHAlign( Qt::AlignmentFlag n );
    virtual void slotWordWrap( Qt::CheckState n );
    virtual void slotShapeSize( int );
    virtual void slotResetTransformation();
    virtual void slotOptimizeSize();

protected:
    Qt::AlignmentFlag   nTextHAlign;    // Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter
    Qt::AlignmentFlag   nTextVAlign;    // Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter
    bool                bWordWrap;
    bool                bDynamicSinks;  // create a sink, as needed, with relative pos
    bool                bFixedSize;

    // transform handle positions relative to center
    // - they need to be realtive so we can restore even if the rect has moved or changed size
    QPointF pointRotateDelta;
    QPointF pointShearHDelta;
    QPointF pointShearVDelta;
    bool  bInitTransform;   // is transform init
    // transform values
    qreal nAngle;           // rotation
    qreal nShearH;          // shear horizontal
    qreal nShearV;          // shear vertical


    virtual QSizeF getOptimalSize();

    // SINKS
    // - we have two types; static and dynamic
    // - static always exists while dynamic only exists when needed by a source
    // - can not be moved (ie like a source handle)
    // - can be hidden (and usually are)
    // - only applicable to DRectangle based objects (not DLine's)
    QVector<DSink*> vectorSinks;  
    bool bSinksVisible; // sinks are only visible when a source is over rectangle

    virtual void doSinksCreate();
    virtual void doSinksVisible( bool b = true );
    virtual void doSinksDelete();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();

    // HANDLES
    // - these are created when they are to be visible and deleted when they are no longer needed to be visible
    QVector<DGraphicsItemLine*> vectorLines;

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesDelete();
    virtual void doHandlesSync();

    virtual void doResize( DHandle *pHandle, const QPointF &pointScenePos );

    virtual void doRotate( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitRotate();
    virtual void doFiniRotate();
    virtual void doSyncRotate();

    virtual void doShear( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitShear();
    virtual void doFiniShear();
    virtual void doSyncShear();

    virtual void doTransform();
    virtual void doInitTransform();
    virtual void doFiniTransform();

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos ) { Q_UNUSED(pHandle); Q_UNUSED(pointScenePos); }
    virtual void doInitMenu() 
    {
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";

    }

    virtual void doFiniMenu() {}
    virtual void doSyncMenu() {}
};

#endif


