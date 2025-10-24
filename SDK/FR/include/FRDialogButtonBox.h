#ifndef H_FRDialogButtonBox
#define H_FRDialogButtonBox

#include "FRObject.h"

class FRForm;

class FRDialogButtonBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRDialogButtonBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRDialogButtonBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


