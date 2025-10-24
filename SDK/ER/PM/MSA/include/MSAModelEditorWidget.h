#ifndef H_MSAModelEditorWidget
#define H_MSAModelEditorWidget

#include <DDiagramEditorWidget.h>

class MSAModel;

class MSAModelEditorWidget: public DDiagramEditorWidget
{
    Q_OBJECT
public:
    MSAModelEditorWidget( MSAModel *pModel, QWidget *pParent = nullptr );
    virtual ~MSAModelEditorWidget();

    virtual AWPalWidget *getPalWidget();
};

#endif


