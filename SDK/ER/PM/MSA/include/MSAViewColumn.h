#ifndef H_MSAViewColumn
#define H_MSAViewColumn

#include <PMViewColumn.h>

class PMView;

class MSAViewColumn : public PMViewColumn
{
    Q_OBJECT
public:
    MSAViewColumn( PMView *pParent );
    virtual ~MSAViewColumn();
};

#endif 


