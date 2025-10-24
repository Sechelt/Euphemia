#ifndef H_SLView
#define H_SLView

#include <PMView.h>

class SLModel;

class SLView : public PMView
{
    Q_OBJECT
public:
    SLView( SLModel *pParent );
    virtual ~SLView();
};

#endif 

