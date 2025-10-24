#ifndef H_SQL92Rule
#define H_SQL92Rule

#include "PMRule.h"

class SQL92Model;

class SQL92Rule : public PMRule
{
    Q_OBJECT
public:
    SQL92Rule( SQL92Model *pParent );
    virtual ~SQL92Rule();
};

#endif 


