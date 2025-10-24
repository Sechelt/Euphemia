#ifndef H_SQL92ReferenceColumn
#define H_SQL92ReferenceColumn

#include "PMReferenceColumn.h"

class SQL92Reference;

class SQL92ReferenceColumn : public PMReferenceColumn
{
    Q_OBJECT
public:
    SQL92ReferenceColumn( SQL92Reference *pParent );
    virtual ~SQL92ReferenceColumn();
};

#endif 


