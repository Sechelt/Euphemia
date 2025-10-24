#ifndef H_SQL92Domain
#define H_SQL92Domain

#include <PMDomain.h>

class SQL92Model;

class SQL92Domain : public PMDomain
{
    Q_OBJECT
public:
    SQL92Domain( SQL92Model *pParent );
    virtual ~SQL92Domain();
};

#endif 


