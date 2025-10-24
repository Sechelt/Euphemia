#ifndef H_SLReference
#define H_SLReference

#include <PMReference.h>

#include "SLTable.h"

class SLModel;

class SLReference : public PMReference
{
    Q_OBJECT
public:
    SLReference( SLModel *pParent );
    virtual ~SLReference();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};


#endif


