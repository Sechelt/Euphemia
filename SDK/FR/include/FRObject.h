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

    virtual void setSize( const QSizeF &size ) override;
    virtual void setSizeHint( const QSizeF & ) override;
    virtual void setSizeConstraints( const QSizeF &sizeMin, const QSizeF &sizeMax ) override;

    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr ) override;
    virtual const QMetaObject * getMetaObject();
    virtual QWidget *           getWidget();
    virtual QSizeF              getSizeHint() override;
    virtual QSizeF              getSizeMinimum() override;
    virtual QSizeF              getSizeMaximum() override;

protected slots:
//    virtual void slotSize( const QSizeF &size );
    virtual void slotSizeHint( const QSizeF & ) override;
    virtual void slotSizeConstraints( const QSizeF &, const QSizeF & ) override;

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


