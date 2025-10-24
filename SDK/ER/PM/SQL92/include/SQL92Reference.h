#ifndef H_SQL92Reference
#define H_SQL92Reference

#include <PMReference.h>

#include "SQL92Table.h"

class SQL92Model;

class SQL92Reference : public PMReference
{
    Q_OBJECT
public:
    SQL92Reference( SQL92Model *pParent );
    virtual ~SQL92Reference();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};


#endif


