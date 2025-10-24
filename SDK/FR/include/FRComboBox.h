#ifndef H_FRComboBox
#define H_FRComboBox

#include "FRObject.h"

class FRForm;

class FRComboBox : public FRObject
{
    Q_OBJECT
public:

    explicit FRComboBox( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRComboBox();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


