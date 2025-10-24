#ifndef H_FRPlainTextEdit
#define H_FRPlainTextEdit

#include "FRObject.h"

class FRForm;

class FRPlainTextEdit : public FRObject
{
    Q_OBJECT
public:

    explicit FRPlainTextEdit( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRPlainTextEdit();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


