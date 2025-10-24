#ifndef H_SLModelPalWidget
#define H_SLModelPalWidget

#include <AWPalWidget.h>

class SLModelEditorWidget;

class SLModelPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    SLModelPalWidget( SLModelEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~SLModelPalWidget();
};

#endif


