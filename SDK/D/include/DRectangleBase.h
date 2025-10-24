#ifndef H_DRectangleBase
#define H_DRectangleBase

#include <W.h>

#include "DObject.h"

class DLayout;

/*!
 * \brief Base class for any DObject that is based upon a QRectF. 
 *  
 * This was created so that the following classes could share common 
 * functionality. For example; DLayout and DSpacer do not want transformation 
 * displaying text, nor sinks - but they all share geometry. 
 *  
 * \sa DRectangle 
 * \sa DLayout 
 * \sa DSpacer 
 *  
 * \author pharvey (9/18/20)
 */
class DRectangleBase : public DObject
{
    Q_OBJECT
public:
    explicit DRectangleBase( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DRectangleBase();

    virtual void setSize( const QSizeF &size );
    virtual void setPos( const QPointF &point );      
    virtual void setPosRaw( const QPointF &point );
    virtual void setStretch( const QSizeF & );
    virtual void setSizeHint( const QSizeF & );
    virtual void setSizeConstraints( const QSizeF &, const QSizeF & );
    virtual void setSizeIncrement( const QSizeF & );
    virtual void setMargins( const QMargins & );

    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QSizeF              getSize()           { return rect.size();   }
    virtual QSizeF              getStretch()        { return sizeStretch;   }
    virtual QSizeF              getSizeHint()       { return sizeHint;      }
    virtual QSizeF              getSizeMinimum()    { return sizeMinimum;   }
    virtual QSizeF              getSizeMaximum()    { return sizeMaximum;   }
    virtual QSizeF              getSizeIncrement()  { return sizeIncrement; }
    virtual QMargins            getMargins()        { return margins;       }
    
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget );

    virtual void doGroupSync();
    virtual void doGroupScale( const QSizeF &sizeFactor );
    virtual void doGroupIn( const QPointF &pointDelta );
    virtual void doGroupOut( const QPointF &pointDelta );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

signals:
    void signalChangedPos( const QPointF &pointPos );
    void signalChangedSize( const QSizeF &size );
    void signalChangedStretch( const QSizeF & );
    void signalChangedSizeHint( const QSizeF & );
    void signalChangedSizeConstraints( const QSizeF &, const QSizeF & );
    void signalChangedSizeIncrement( const QSizeF & );
    void signalChangedMargins( const QMargins & );
    void signalChangedLayout(); // DLayout connects to this to let it know some controls have made changes to layout details

public slots:
    virtual void slotPos( const QPointF pointPos );
    virtual void slotSize( const QSizeF &size );
    virtual void slotStretch( const QSizeF & );
    virtual void slotSizeHint( const QSizeF & );
    virtual void slotSizeConstraints( const QSizeF &, const QSizeF & );
    virtual void slotSizeIncrement( const QSizeF & );
    virtual void slotMargins( const QMargins & );

protected:
    // geometry
    // rect in item coords and with top-left as origin (meaning top-left is always 0,0)
    QRectF rect; 
    // the following are used by default - derived classes may want to get these
    // from elsewehere - such as QObjectWidget
    QSizeF              sizeStretch;    // stretch factor
    QSizeF              sizeHint;       // preferred size - invalid means no preferrence - should be between min/max
    QSizeF              sizeMinimum;    // minimum width/height 
    QSizeF              sizeMaximum;    // maximum width/height
    QSizeF              sizeIncrement;  // step size
    QMargins            margins;        // space around object when in a layout

    // resize handles
    virtual void doResize( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitResize();
    virtual void doFiniResize();
    virtual void doSyncResize();
};

#endif


