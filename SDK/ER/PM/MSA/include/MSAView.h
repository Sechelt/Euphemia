#ifndef H_MSAView
#define H_MSAView

#include <PMView.h>

class MSAModel;

class MSAView : public PMView
{
    Q_OBJECT
public:
    MSAView( MSAModel *pParent );
    virtual ~MSAView();

    virtual ADObject *getObject( const QString &s, ADObject *p = nullptr );
};

#endif 

