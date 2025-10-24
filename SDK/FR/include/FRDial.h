#ifndef H_FRDial
#define H_FRDial

#include "FRObject.h"

class FRForm;

class FRDial : public FRObject
{
    Q_OBJECT
public:

    explicit FRDial( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRDial();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


