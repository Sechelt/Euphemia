#ifndef H_FRGroupBox
#define H_FRGroupBox

#include "FRObject.h"

class FRForm;

class FRGroupBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRGroupBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRGroupBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


