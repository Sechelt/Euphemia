#ifndef H_FRCheckBox
#define H_FRCheckBox

#include "FRObject.h"

class FRForm;

class FRCheckBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRCheckBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRCheckBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


