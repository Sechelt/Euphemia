#ifndef H_ORGDiagramPalWidget
#define H_ORGDiagramPalWidget

#include <AWPalWidget.h>

class ORGDiagramEditorWidget;

class ORGDiagramPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    ORGDiagramPalWidget( ORGDiagramEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~ORGDiagramPalWidget();
};

#endif


