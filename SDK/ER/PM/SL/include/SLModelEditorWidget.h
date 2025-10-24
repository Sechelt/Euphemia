#ifndef H_SLModelEditorWidget
#define H_SLModelEditorWidget

#include <DDiagramEditorWidget.h>

class SLModel;

class SLModelEditorWidget: public DDiagramEditorWidget
{
    Q_OBJECT
public:
    SLModelEditorWidget( SLModel *pModel, QWidget *pParent = nullptr );
    virtual ~SLModelEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


