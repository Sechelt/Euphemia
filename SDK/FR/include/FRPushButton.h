#ifndef H_FRPushButton
#define H_FRPushButton

#include "FRObject.h"

class FRForm;

class FRPushButton : public FRObject
{
    Q_OBJECT
public:

    explicit FRPushButton( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRPushButton();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


