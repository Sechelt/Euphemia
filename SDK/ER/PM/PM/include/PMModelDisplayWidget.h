#ifndef H_PMModelDisplayWidget
#define H_PMModelDisplayWidget

#include "PMModel.h"

class PMModelDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    PMModelDisplayWidget( PMModel *pDiagram, QWidget *pParent );
    virtual ~PMModelDisplayWidget();

protected slots:
    virtual void slotDomain( Qt::CheckState n );
    virtual void slotDataType( Qt::CheckState n );
    virtual void slotParams( Qt::CheckState n );
    virtual void slotDefault( Qt::CheckState n );

protected:
    PMModel *pDiagram;

    QCheckBox * pcheckboxDisplayDomain;
    QCheckBox * pcheckboxDisplayDataType;
    QCheckBox * pcheckboxDisplayParams;
    QCheckBox * pcheckboxDisplayDefault;
};


#endif


