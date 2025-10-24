#ifndef H_FRCalendarWidget
#define H_FRCalendarWidget

#include "FRObject.h"

class FRForm;


/* 
 
    DESIGNABLE 
 
    QObject::property()
    QObject::setProperty()
 
 
 
*/
class FRCalendarWidget : public FRObject
{
    Q_OBJECT
public:

    explicit FRCalendarWidget( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRCalendarWidget();

    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap getIcon();
};


#endif


