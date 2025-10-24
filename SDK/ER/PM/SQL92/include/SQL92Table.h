#ifndef H_SQL92Table
#define H_SQL92Table

#include <PMTableNormal.h>

#include "SQL92Column.h"
#include "SQL92Index.h"

class SQL92Model;

class SQL92Table : public PMTableNormal
{
    Q_OBJECT
public:
    SQL92Table( SQL92Model *pParent );
    virtual ~SQL92Table();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual bool doLoad( QDomElement *pdomElemThis );
};

#endif 


