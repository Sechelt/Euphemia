#ifndef H_ODBCTable
#define H_ODBCTable

#include <PMTableNormal.h>

#include "ODBCColumn.h"
#include "ODBCIndex.h"

class ODBCModel;

class ODBCTable : public PMTableNormal
{
    Q_OBJECT
public:
    ODBCTable( ODBCModel *pParent );
    virtual ~ODBCTable();

    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};

#endif 


