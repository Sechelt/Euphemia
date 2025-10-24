#ifndef H_FRProgressBar
#define H_FRProgressBar

#include "FRObject.h"

class FRForm;

class FRProgressBar : public FRObject
{
    Q_OBJECT
public:

    explicit FRProgressBar( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRProgressBar();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


