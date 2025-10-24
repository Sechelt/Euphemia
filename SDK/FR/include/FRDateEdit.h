#ifndef H_FRDateEdit
#define H_FRDateEdit

#include "FRObject.h"

class FRForm;

class FRDateEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRDateEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRDateEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


