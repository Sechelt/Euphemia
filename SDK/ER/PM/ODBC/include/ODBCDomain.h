#ifndef H_ODBCDomain
#define H_ODBCDomain

#include <PMDomain.h>

class ODBCModel;

/*!
    ODBCDomain
    
    In this model type a Domain is simply used to make creating columns easier. This
    model type does not support Domains ie "CREATE DOMAIN" during script creation.
    
    Any column using a domain should simply use the domains data type definition
    instead of relying on a domain existing in the database.
    
    The main reason we derive ODBCDomain is because we want the properties dialog
    to work well for the given data types supported by this model type.
*/
class ODBCDomain : public PMDomain
{
    Q_OBJECT
public:
    ODBCDomain( ODBCModel *pParent );
    virtual ~ODBCDomain();
};

#endif 


