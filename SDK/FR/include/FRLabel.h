#ifndef H_FRLabel
#define H_FRLabel

#include "FRObject.h"

class FRForm;

class FRLabel : public FRObject
{
    Q_OBJECT
public:

    explicit FRLabel( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRLabel();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


