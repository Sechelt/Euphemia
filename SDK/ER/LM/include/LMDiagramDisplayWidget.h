#ifndef H_LMDiagramDisplayWidget
#define H_LMDiagramDisplayWidget

#include "LMModel.h"

class LMDiagramDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    LMDiagramDisplayWidget( LMModel *pDiagram, QWidget *pParent );
    virtual ~LMDiagramDisplayWidget();

protected slots:
    virtual void slotDomain( Qt::CheckState n );
    virtual void slotDataType( Qt::CheckState n );
    virtual void slotParams( Qt::CheckState n );
    virtual void slotDefault( Qt::CheckState n );

protected:
    LMModel *pDiagram;

    QCheckBox * pcheckboxDisplayDomain;
    QCheckBox * pcheckboxDisplayDataType;
    QCheckBox * pcheckboxDisplayParams;
    QCheckBox * pcheckboxDisplayDefault;
};


#endif


