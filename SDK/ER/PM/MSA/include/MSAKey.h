#ifndef H_MSAKey
#define H_MSAKey

#include <AWObject.h>

#include <PMKey.h>

class MSATable;

class MSAKey : public PMKey
{
    Q_OBJECT
public:
    MSAKey( MSATable *pParent );
    virtual ~MSAKey();
};


#endif 



