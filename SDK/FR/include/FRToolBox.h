#ifndef H_FRToolBox
#define H_FRToolBox

#include "FRObject.h"

class FRForm;

class FRToolBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRToolBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRToolBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


