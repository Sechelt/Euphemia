#ifndef H_FRDateTimeEdit
#define H_FRDateTimeEdit

#include "FRObject.h"

class FRForm;

class FRDateTimeEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRDateTimeEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRDateTimeEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


