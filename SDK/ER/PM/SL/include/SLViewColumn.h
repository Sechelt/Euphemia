#ifndef H_SLViewColumn
#define H_SLViewColumn

#include <PMViewColumn.h>

class SLViewColumn : public PMViewColumn
{
    Q_OBJECT
public:
    SLViewColumn( PMView *pParent );
    virtual ~SLViewColumn();

};

#endif 


