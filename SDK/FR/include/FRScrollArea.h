#ifndef H_FRScrollArea
#define H_FRScrollArea

#include "FRObject.h"

class FRForm;

class FRScrollArea : public FRObject
{
    Q_OBJECT
public:

    explicit FRScrollArea( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRScrollArea();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


