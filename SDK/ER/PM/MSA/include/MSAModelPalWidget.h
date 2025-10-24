#ifndef H_MSAModelPalWidget
#define H_MSAModelPalWidget

#include <AWPalWidget.h>

class MSAModelEditorWidget;

class MSAModelPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    MSAModelPalWidget( MSAModelEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~MSAModelPalWidget();
};

#endif


