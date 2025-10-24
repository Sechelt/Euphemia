#ifndef H_FRTableWidget
#define H_FRTableWidget

#include "FRObject.h"

class FRForm;

class FRTableWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRTableWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTableWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


