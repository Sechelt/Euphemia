#ifndef H_FLDiagramEditorWidget
#define H_FLDiagramEditorWidget

#include <DDiagramEditorWidget.h>

class FLDiagram;

class FLDiagramEditorWidget : public DDiagramEditorWidget
{
    Q_OBJECT
public:
    FLDiagramEditorWidget( FLDiagram *pDiagram, QWidget *pParent = nullptr );
    virtual ~FLDiagramEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


