#ifndef H_FRFormPalWidget
#define H_FRFormPalWidget

#include <DDiagramPalWidget.h>

class DDiagramEditorWidget;

class FRFormPalWidget: public DDiagramPalWidget
{
    Q_OBJECT
public:
    FRFormPalWidget( DDiagramEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~FRFormPalWidget();
};

#endif


