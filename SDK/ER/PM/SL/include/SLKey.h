#ifndef H_SLKey
#define H_SLKey

#include <AWObject.h>

#include <PMKey.h>

class SLTable;

class SLKey : public PMKey
{
    Q_OBJECT
public:
    SLKey( SLTable *pParent );
    virtual ~SLKey();
};


#endif 



