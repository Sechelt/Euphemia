#ifndef H_ODBCProcedure
#define H_ODBCProcedure

#include <PMProcedure.h>

class ODBCModel;

class ODBCProcedure : public PMProcedure
{
    Q_OBJECT
public:
    ODBCProcedure( ODBCModel *pParent );
    virtual ~ODBCProcedure();
};

#endif 


