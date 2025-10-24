#ifndef H_MSARule
#define H_MSARule

#include "PMRule.h"

class MSAModel;

class MSARule : public PMRule
{
    Q_OBJECT
public:
    MSARule( MSAModel *pParent );
    virtual ~MSARule();
};

#endif 


