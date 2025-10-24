#ifndef H_SQL92Sequence
#define H_SQL92Sequence

#include <PMSequence.h>

class SQL92Model;

class SQL92Sequence : public PMSequence
{
    Q_OBJECT
public:
    SQL92Sequence( SQL92Model *pParent );
    virtual ~SQL92Sequence();
};

#endif 


