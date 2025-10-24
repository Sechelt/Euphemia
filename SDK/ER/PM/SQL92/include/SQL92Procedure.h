#ifndef H_SQL92Procedure
#define H_SQL92Procedure

#include <PMProcedure.h>

class SQL92Model;

class SQL92Procedure : public PMProcedure
{
    Q_OBJECT
public:
    SQL92Procedure( SQL92Model *pParent );
    virtual ~SQL92Procedure();
};

#endif 


