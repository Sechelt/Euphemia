#ifndef H_MSAProcedure
#define H_MSAProcedure

#include <PMProcedure.h>

class MSAModel;

class MSAProcedure : public PMProcedure
{
    Q_OBJECT
public:
    MSAProcedure( MSAModel *pParent );
    virtual ~MSAProcedure();
};

#endif 


