#ifndef H_FROpenGLWidget
#define H_FROpenGLWidget

#include <QOpenGLWidget>

#include "FRObject.h"

class FRForm;

class FROpenGLWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FROpenGLWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FROpenGLWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


