#ifndef H_SQL92Index
#define H_SQL92Index

#include <PMIndex.h>

class SQL92Table;

class SQL92Index : public PMIndex
{
    Q_OBJECT
public:
    SQL92Index( SQL92Table *pParent );
    virtual ~SQL92Index();
    
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
};

#endif 


