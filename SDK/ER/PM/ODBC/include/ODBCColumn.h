#ifndef H_ODBCColumn
#define H_ODBCColumn

#include <PMColumn.h>

class ODBCTable;

/*!
    ODBCColumn
    
    Many of the properties inherited by ODBCColumn are supported
    simply for documentation and validation purposes. Domain references
    are supported only to make column creation easier when the same
    data type definition is used many times.   
*/
class ODBCColumn : public PMColumn
{
    Q_OBJECT
public:
    ODBCColumn( ODBCTable *pParent );
    virtual ~ODBCColumn();
};

#endif 


