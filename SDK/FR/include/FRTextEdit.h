#ifndef H_FRTextEdit
#define H_FRTextEdit

#include "FRObject.h"

class FRForm;

class FRTextEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRTextEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTextEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


