#ifndef H_FRStackedWidget
#define H_FRStackedWidget

#include "FRObject.h"

class FRForm;

class FRStackedWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRStackedWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRStackedWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


