#ifndef H_SLIndexColumn
#define H_SLIndexColumn

#include <PMIndexColumn.h>

#include "SLIndex.h"

class SLIndexColumn : public PMIndexColumn
{
    Q_OBJECT
public:
    SLIndexColumn( SLIndex *pParent );
    virtual ~SLIndexColumn();
};

#endif 


