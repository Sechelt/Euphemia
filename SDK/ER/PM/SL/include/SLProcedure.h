#ifndef H_SLProcedure
#define H_SLProcedure

#include <PMProcedure.h>

class SLModel;

class SLProcedure : public PMProcedure
{
    Q_OBJECT
public:
    SLProcedure( SLModel *pParent );
    virtual ~SLProcedure();
};

#endif 


