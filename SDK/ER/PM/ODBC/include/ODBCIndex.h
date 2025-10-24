#ifndef H_ODBCIndex
#define H_ODBCIndex

#include <PMIndex.h>

class ODBCTable;

class ODBCIndex : public PMIndex
{
    Q_OBJECT
public:
    ODBCIndex( ODBCTable *pParent );
    virtual ~ODBCIndex();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};

#endif 


