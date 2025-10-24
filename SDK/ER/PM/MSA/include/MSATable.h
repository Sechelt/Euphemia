#ifndef H_MSATable
#define H_MSATable

#include <PMTableNormal.h>

#include "MSAColumn.h"
#include "MSAIndex.h"

class MSAModel;

class MSATable : public PMTableNormal
{
    Q_OBJECT
public:
    MSATable( MSAModel *pParent );
    virtual ~MSATable();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual bool doLoad( QDomElement *pdomElemThis );
};

#endif 


