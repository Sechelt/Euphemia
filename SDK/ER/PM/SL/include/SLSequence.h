#ifndef H_SLSequence
#define H_SLSequence

#include <PMSequence.h>

class SLModel;

class SLSequence : public PMSequence
{
    Q_OBJECT
public:
    SLSequence( SLModel *pParent );
    virtual ~SLSequence();
};

#endif 


