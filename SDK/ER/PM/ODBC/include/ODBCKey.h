#ifndef H_ODBCKey
#define H_ODBCKey

#include <PMKey.h>

class ODBCTable;

class ODBCKey : public PMKey
{
    Q_OBJECT
public:
    ODBCKey( ODBCTable *pParent );
    virtual ~ODBCKey();
};

#endif 



