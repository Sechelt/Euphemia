#ifndef H_FRVerticalSlider
#define H_FRVerticalSlider

#include "FRObject.h"

class FRForm;

class FRVerticalSlider : public FRObject
{
    Q_OBJECT
public:

    explicit FRVerticalSlider( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRVerticalSlider();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


