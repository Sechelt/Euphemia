#ifndef H_MSASequence
#define H_MSASequence

#include <PMSequence.h>

class MSAModel;

class MSASequence : public PMSequence
{
    Q_OBJECT
public:
    MSASequence( MSAModel *pParent );
    virtual ~MSASequence();
};

#endif 


