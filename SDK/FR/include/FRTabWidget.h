#ifndef H_FRTabWidget
#define H_FRTabWidget

#include "FRObject.h"

class FRForm;

class FRTabWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRTabWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTabWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


