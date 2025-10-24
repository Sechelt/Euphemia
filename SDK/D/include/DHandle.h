#ifndef H_DHandle
#define H_DHandle

#include <CBD.h>

class DObject;
class DGraphicsProxyItem;

/*!
 * \class DHandle 
 * \brief A handle for connecting and otherwise manipulating an item. 
 *  
 * Handles are the presentation layer for a FMPSink or a FMPSource. 
 * Handles can also be created for manipulating an DGraphicsProxyItem. 
 *   
 * \sa DGraphicsProxyItem 
 *  
 * \author pharvey (9/28/19)
 */
class DHandle : public QGraphicsObject
{
    Q_OBJECT
public:
    enum ConnectTypes
    {
        ConnectTypeSource,
        ConnectTypeSink,
        ConnectTypeNone
    };

    enum Functions
    {
        FunctionConnect,            // begin/end of line 
        FunctionNewLineSegment,     // new point added if moved
        FunctionRemoveLineSegment,  // allows intermediate point to be moved. point removed if causes segments to line up  
        FunctionResizeWidth,
        FunctionResizeHeight,
        FunctionResizeWidthHeight,
        FunctionResizeLayoutWidthHeight,
        FunctionSelectTopLeft,
        FunctionSelectTopRight,
        FunctionSelectBottomRight,
        FunctionSelectBottomLeft,
        FunctionFloatingTextBegin,
        FunctionFloatingTextEnd,
        FunctionFloatingTextCenter,
        FunctionBaseTransform,
        FunctionRotateIndicator,
        FunctionShearHIndicator,
        FunctionShearVIndicator,
        FunctionSlide,
        FunctionSlideBoth,
        FunctionMenu,
        FunctionManipulateOther,
        FunctionAnchor,
        FunctionMove,
        FunctionInsertLayout
    };

    explicit DHandle( DObject *pObject, Functions nFunction, ConnectTypes nType, const QPointF &pointPos );
    virtual ~DHandle();

    virtual void setFunction( Functions n ) { nFunction = n; }
    virtual void setIndex( int n ) { nIndex = n; } 
    virtual void setDelta( const QPointF &point ) { pointDelta = point; }
    virtual void setConnected( bool b = true ) { bConnected = b; }
    virtual void setPixmap( const QPixmap &pixmap );

    virtual DObject *       getObject() { return pObject; }
    virtual ConnectTypes    getConnectType() { return nConnectType; }
    virtual Functions       getFunction() { return nFunction; }
    virtual int             getIndex() { return nIndex; } // index into DLine::polygon
    virtual QPointF         getDelta() { return pointDelta; } // delta from a reference point

    virtual QRectF boundingRect() const override;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget ) override;

protected:
    int             nIndex;         // for use by DLine (reference to a point in polygon)
    QPointF         pointDelta;     // delta from a reference point (augment nIndex? reference is calc'd ie center of rect?)
    DObject *       pObject;        // the object we are supporting
    Functions       nFunction;
    ConnectTypes    nConnectType;
    QRectF          rect;           // our bounding rect
    bool            bConnected;
    QPixmap         pixmap;         // instead of drawing handle
};

#endif


