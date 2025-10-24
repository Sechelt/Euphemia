#ifndef H_MSAIndexColumn
#define H_MSAIndexColumn

#include <PMIndexColumn.h>

#include "MSAIndex.h"

class MSAIndexColumn : public PMIndexColumn
{
    Q_OBJECT
public:
    MSAIndexColumn( MSAIndex *pParent );
    virtual ~MSAIndexColumn();
};

#endif 


