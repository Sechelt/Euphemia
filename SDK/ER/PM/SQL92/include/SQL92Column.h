#ifndef H_SQL92Column
#define H_SQL92Column

#include <PMColumn.h>

class SQL92Table;

class SQL92Column : public PMColumn
{
    Q_OBJECT
public:
    SQL92Column( SQL92Table *pParent );
    virtual ~SQL92Column();
};

#endif 


