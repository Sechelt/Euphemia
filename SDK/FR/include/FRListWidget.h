#ifndef H_FRListWidget
#define H_FRListWidget

#include "FRObject.h"

class FRForm;

class FRListWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRListWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRListWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


