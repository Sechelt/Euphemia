#ifndef H_ODBCReference
#define H_ODBCReference

#include <PMReference.h>

#include "ODBCTable.h"

class ODBCModel;

class ODBCReference : public PMReference
{
    Q_OBJECT
public:
    ODBCReference( ODBCModel *pParent );
    virtual ~ODBCReference();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};


#endif


