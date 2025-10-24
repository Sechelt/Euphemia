#ifndef H_ORGDiagramEditorWidget
#define H_ORGDiagramEditorWidget

#include <DDiagramEditorWidget.h>

class ORGDiagram;

class ORGDiagramEditorWidget : public DDiagramEditorWidget
{
    Q_OBJECT
public:
    ORGDiagramEditorWidget( ORGDiagram *pDiagram, QWidget *pParent = nullptr );
    virtual ~ORGDiagramEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


