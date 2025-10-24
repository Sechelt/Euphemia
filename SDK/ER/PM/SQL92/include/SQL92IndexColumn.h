#ifndef H_SQL92IndexColumn
#define H_SQL92IndexColumn

#include <PMIndexColumn.h>

#include "SQL92Index.h"

class SQL92IndexColumn : public PMIndexColumn
{
    Q_OBJECT
public:
    SQL92IndexColumn( SQL92Index *pParent );
    virtual ~SQL92IndexColumn();
};

#endif 


