#ifndef H_SQL92Key
#define H_SQL92Key

#include <AWObject.h>

#include <PMKey.h>

class SQL92Table;

class SQL92Key : public PMKey
{
    Q_OBJECT
public:
    SQL92Key( SQL92Table *pParent );
    virtual ~SQL92Key();
};


#endif 



