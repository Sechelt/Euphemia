#ifndef H_FRLCDNumber
#define H_FRLCDNumber

#include "FRObject.h"

class FRForm;

class FRLCDNumber : public FRObject
{
    Q_OBJECT
public:

    explicit FRLCDNumber( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRLCDNumber();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


