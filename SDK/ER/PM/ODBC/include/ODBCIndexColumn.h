#ifndef H_ODBCIndexColumn
#define H_ODBCIndexColumn

#include <PMIndexColumn.h>

#include "ODBCIndex.h"

class ODBCIndexColumn : public PMIndexColumn
{
    Q_OBJECT
public:
    ODBCIndexColumn( ODBCIndex *pParent );
    virtual ~ODBCIndexColumn();
};

#endif 


