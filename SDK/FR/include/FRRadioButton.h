#ifndef H_FRRadioButton
#define H_FRRadioButton

#include "FRObject.h"

class FRForm;

class FRRadioButton : public FRObject
{
    Q_OBJECT
public:

    explicit FRRadioButton( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRRadioButton();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


