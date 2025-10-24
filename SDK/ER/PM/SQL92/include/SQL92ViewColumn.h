#ifndef H_SQL92ViewColumn
#define H_SQL92ViewColumn

#include <PMViewColumn.h>

class SQL92ViewColumn : public PMViewColumn
{
    Q_OBJECT
public:
    SQL92ViewColumn( PMView *pParent );
    virtual ~SQL92ViewColumn();

};

#endif 


