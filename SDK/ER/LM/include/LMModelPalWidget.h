#ifndef H_LMModelPalWidget
#define H_LMModelPalWidget

#include <AWPalWidget.h>

class LMModelEditorWidget;

class LMModelPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    LMModelPalWidget( LMModelEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~LMModelPalWidget();
};

#endif


