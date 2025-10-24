#ifndef H_FRToolButton
#define H_FRToolButton

#include "FRObject.h"

class FRForm;

class FRToolButton : public FRObject
{
    Q_OBJECT
public:

    explicit FRToolButton( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRToolButton();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


