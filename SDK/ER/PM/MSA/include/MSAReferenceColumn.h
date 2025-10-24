#ifndef H_MSAReferenceColumn
#define H_MSAReferenceColumn

#include "PMReferenceColumn.h"

class MSAReference;

class MSAReferenceColumn : public PMReferenceColumn
{
    Q_OBJECT
public:
    MSAReferenceColumn( MSAReference *pParent );
    virtual ~MSAReferenceColumn();
};

#endif 


