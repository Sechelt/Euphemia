#ifndef H_ODBCModelPalWidget
#define H_ODBCModelPalWidget

#include <AWPalWidget.h>

class ODBCModelEditorWidget;

class ODBCModelPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    ODBCModelPalWidget( ODBCModelEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~ODBCModelPalWidget();
};

#endif


