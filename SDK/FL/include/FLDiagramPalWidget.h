#ifndef H_FLDiagramPalWidget
#define H_FLDiagramPalWidget

#include <AWPalWidget.h>

class FLDiagramEditorWidget;

class FLDiagramPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    FLDiagramPalWidget( FLDiagramEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~FLDiagramPalWidget();
};

#endif


