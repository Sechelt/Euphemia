#ifndef H_MSAIndex
#define H_MSAIndex

#include <PMIndex.h>

class MSATable;

class MSAIndex : public PMIndex
{
    Q_OBJECT
public:
    MSAIndex( MSATable *pParent );
    virtual ~MSAIndex();
    
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};

#endif 


