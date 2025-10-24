#ifndef H_SQL92ModelPalWidget
#define H_SQL92ModelPalWidget

#include <AWPalWidget.h>

class SQL92ModelEditorWidget;

class SQL92ModelPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    SQL92ModelPalWidget( SQL92ModelEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~SQL92ModelPalWidget();
};

#endif


