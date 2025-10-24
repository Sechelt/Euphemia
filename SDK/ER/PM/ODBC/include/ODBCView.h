#ifndef H_ODBCView
#define H_ODBCView

#include <PMView.h>

class ODBCModel;

class ODBCView : public PMView
{
    Q_OBJECT
public:
    ODBCView( ODBCModel *pParent );
    virtual ~ODBCView();
};

#endif 

