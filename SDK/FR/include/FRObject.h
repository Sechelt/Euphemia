#ifndef H_FRObject
#define H_FRObject

#include <DRectangle.h>

class FRGraphicsProxyObject;
class FRForm;

class FRObject : public DRectangle
{
    Q_OBJECT
public:

    explicit FRObject( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRObject();

    virtual void setSize( const QSizeF &size );

    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual const QMetaObject * getMetaObject();

//protected slots:
//    virtual void slotSize( const QSizeF &size );

protected:
    virtual void doResize( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitResize();                                            
    virtual void doSyncResize();                                            
};

class FRObjectWidget : public QWidget
{
    Q_OBJECT
public:
    FRObjectWidget( FRObject *pObject, QWidget *pParent );
    virtual ~FRObjectWidget();

protected:
    FRObject *          pObject;
};

#endif


