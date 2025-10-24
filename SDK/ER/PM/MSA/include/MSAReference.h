#ifndef H_MSAReference
#define H_MSAReference

#include <PMReference.h>

#include "MSATable.h"

class MSAModel;

class MSAReference : public PMReference
{
    Q_OBJECT
public:
    MSAReference( MSAModel *pParent );
    virtual ~MSAReference();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};


#endif


