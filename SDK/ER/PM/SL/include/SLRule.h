#ifndef H_SLRule
#define H_SLRule

#include "PMRule.h"

class SLModel;

class SLRule : public PMRule
{
    Q_OBJECT
public:
    SLRule( SLModel *pParent );
    virtual ~SLRule();
};

#endif 


