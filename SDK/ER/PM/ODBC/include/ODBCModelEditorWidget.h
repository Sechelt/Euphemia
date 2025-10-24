#ifndef H_ODBCModelEditorWidget
#define H_ODBCModelEditorWidget

#include <DDiagramEditorWidget.h>

class ODBCModel;

class ODBCModelEditorWidget: public DDiagramEditorWidget
{
    Q_OBJECT
public:
    ODBCModelEditorWidget( ODBCModel *pModel, QWidget *pParent = nullptr );
    virtual ~ODBCModelEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


