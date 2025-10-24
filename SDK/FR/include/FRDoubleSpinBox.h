#ifndef H_FRDoubleSpinBox
#define H_FRDoubleSpinBox

#include "FRObject.h"

class FRForm;

class FRDoubleSpinBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRDoubleSpinBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRDoubleSpinBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


