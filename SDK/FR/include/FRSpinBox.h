#ifndef H_FRSpinBox
#define H_FRSpinBox

#include "FRObject.h"

class FRForm;

class FRSpinBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRSpinBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRSpinBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


