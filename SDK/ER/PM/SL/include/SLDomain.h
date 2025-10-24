#ifndef H_SLDomain
#define H_SLDomain

#include <PMDomain.h>

class SLModel;

class SLDomain : public PMDomain
{
    Q_OBJECT
public:
    SLDomain( SLModel *pParent );
    virtual ~SLDomain();
};

#endif 


