#ifndef H_DDiagramPalWidget
#define H_DDiagramPalWidget

#include <AWPalWidget.h>
#include <WStackedWidget.h>

class DDiagramEditorWidget;

class DDiagramPalWidget: public AWPalWidget
{
    Q_OBJECT
public:
    DDiagramPalWidget( DDiagramEditorWidget *pEditor, QWidget *pParent = nullptr );
    virtual ~DDiagramPalWidget();

protected:
    WStackedWidget *pStackedWidget;
};

#endif


