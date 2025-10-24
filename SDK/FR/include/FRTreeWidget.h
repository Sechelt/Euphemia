#ifndef H_FRTreeWidget
#define H_FRTreeWidget

#include "FRObject.h"

class FRForm;

class FRTreeWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRTreeWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRTreeWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


