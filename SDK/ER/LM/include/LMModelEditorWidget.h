#ifndef H_LMModelEditorWidget
#define H_LMModelEditorWidget

#include <DDiagramEditorWidget.h>

class LMModel;

class LMModelEditorWidget: public DDiagramEditorWidget
{
    Q_OBJECT
public:
    LMModelEditorWidget( LMModel *pModel, QWidget *pParent = nullptr );
    virtual ~LMModelEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


