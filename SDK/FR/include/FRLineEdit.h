#ifndef H_FRLineEdit
#define H_FRLineEdit

#include "FRObject.h"

class FRForm;

class FRLineEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRLineEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRLineEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


