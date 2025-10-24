#ifndef H_FRFormEditorWidget
#define H_FRFormEditorWidget

#include <DDiagramEditorWidget.h>

class FRForm;

class FRFormEditorWidget : public DDiagramEditorWidget
{
    Q_OBJECT
public:
    FRFormEditorWidget( FRForm *pForm, QWidget *pParent = nullptr );
    virtual ~FRFormEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


