#ifndef H_FRHorizontalSlider
#define H_FRHorizontalSlider

#include "FRObject.h"

class FRForm;

class FRHorizontalSlider : public FRObject
{
    Q_OBJECT
public:

    explicit FRHorizontalSlider( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRHorizontalSlider();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


