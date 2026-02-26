#ifndef H_DSpacer
#define H_DSpacer

#include "DRectangleBase.h"

/*!
 * \brief Base class for all spacers. 
 *  
 * Spacers are used by layouts to add spacing among the objects.  
 *  
 * Qt Definitions 
 *  
 * Strut - a spacer with a minimum size ie addSpacing()
 * Stretch - a spacer which stretchs based upon a weight ie addStretch()
 *  
 * \author pharvey (9/10/20)
 */
class DSpacer : public DRectangleBase
{
    Q_OBJECT
public:
    enum SizeTypes
    {
        SizeExpanding,
        SizeFixed
    };
    explicit DSpacer( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DSpacer();

    virtual void setSizeType( SizeTypes n );

    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual SizeTypes       getSizeType() { return nSizeType; }

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

signals:
    void signalChangedSizeType( DSpacer::SizeTypes );

public slots:
    virtual void slotSizeType( DSpacer::SizeTypes );

protected:
    SizeTypes nSizeType;

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesDelete();
    virtual void doHandlesSync();
};

#endif


