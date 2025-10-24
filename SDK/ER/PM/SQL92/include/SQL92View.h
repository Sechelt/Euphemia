#ifndef H_SQL92View
#define H_SQL92View

#include <PMView.h>

class SQL92Model;

class SQL92View : public PMView
{
    Q_OBJECT
public:
    SQL92View( SQL92Model *pParent );
    virtual ~SQL92View();

    virtual ADObject *getObject( const QString &s, ADObject *p = nullptr );
};

#endif 

