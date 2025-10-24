#ifndef H_FRHorizontalScrollBar
#define H_FRHorizontalScrollBar

#include "FRObject.h"

class FRForm;

class FRHorizontalScrollBar : public FRObject
{
    Q_OBJECT
public:

    explicit FRHorizontalScrollBar( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRHorizontalScrollBar();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


