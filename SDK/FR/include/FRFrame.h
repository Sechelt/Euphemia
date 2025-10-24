#ifndef H_FRFrame
#define H_FRFrame

#include "FRObject.h"

class FRForm;

class FRFrame : public FRObject
{
    Q_OBJECT
public:

    explicit FRFrame( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRFrame();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


