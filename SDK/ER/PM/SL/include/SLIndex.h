#ifndef H_SLIndex
#define H_SLIndex

#include <PMIndex.h>

class SLTable;

class SLIndex : public PMIndex
{
    Q_OBJECT
public:
    SLIndex( SLTable *pParent );
    virtual ~SLIndex();
    
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};

#endif 


