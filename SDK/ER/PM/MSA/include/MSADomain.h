#ifndef H_MSADomain
#define H_MSADomain

#include <PMDomain.h>

class MSAModel;

class MSADomain : public PMDomain
{
    Q_OBJECT
public:
    MSADomain( MSAModel *pParent );
    virtual ~MSADomain();
};

#endif 


