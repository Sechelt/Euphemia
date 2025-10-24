#ifndef H_DDiagramPanWidget
#define H_DDiagramPanWidget

#include <AWEditorWidget.h>
#include <AWPanWidget.h>

#include "DPanner.h"

class DDiagramPanWidget: public AWPanWidget
{
    Q_OBJECT
public:
    DDiagramPanWidget( AWEditorWidget *pEditor, DGraphicsSceneProxy *pScene, QWidget *pParent = nullptr );
    virtual ~DDiagramPanWidget();

    virtual DPanner *getPanner() { return pPanner; }

protected:
    DPanner *pPanner;
};

#endif


