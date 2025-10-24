#ifndef H_SQL92ModelEditorWidget
#define H_SQL92ModelEditorWidget

#include <DDiagramEditorWidget.h>

class SQL92Model;

class SQL92ModelEditorWidget: public DDiagramEditorWidget
{
    Q_OBJECT
public:
    SQL92ModelEditorWidget( SQL92Model *pModel, QWidget *pParent = nullptr );
    virtual ~SQL92ModelEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


