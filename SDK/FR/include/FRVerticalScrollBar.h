#ifndef H_FRVerticalScrollBar
#define H_FRVerticalScrollBar

#include "FRObject.h"

class FRForm;

class FRVerticalScrollBar : public FRObject
{
    Q_OBJECT
public:

    explicit FRVerticalScrollBar( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRVerticalScrollBar();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


