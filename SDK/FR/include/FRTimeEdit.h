#ifndef H_FRTimeEdit
#define H_FRTimeEdit

#include "FRObject.h"

class FRForm;

class FRTimeEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRTimeEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTimeEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


