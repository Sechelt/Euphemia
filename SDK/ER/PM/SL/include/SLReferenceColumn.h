#ifndef H_SLReferenceColumn
#define H_SLReferenceColumn

#include "PMReferenceColumn.h"

class SLReference;

class SLReferenceColumn : public PMReferenceColumn
{
    Q_OBJECT
public:
    SLReferenceColumn( SLReference *pParent );
    virtual ~SLReferenceColumn();
};

#endif 


