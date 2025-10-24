#ifndef H_MSAColumn
#define H_MSAColumn

#include <PMColumn.h>

class MSATable;

class MSAColumn : public PMColumn
{
    Q_OBJECT
public:
    MSAColumn( MSATable *pParent );
    virtual ~MSAColumn();
};

#endif 


