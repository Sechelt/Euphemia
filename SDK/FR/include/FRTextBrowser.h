#ifndef H_FRTextBrowser
#define H_FRTextBrowser

#include "FRObject.h"

class FRForm;

class FRTextBrowser : public FRObject
{
    Q_OBJECT
public:

    explicit FRTextBrowser( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTextBrowser();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


