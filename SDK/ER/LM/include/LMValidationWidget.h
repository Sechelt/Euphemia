#ifndef H_LMValidationWidget
#define H_LMValidationWidget

#include "LMModel.h"

class LMValidationWidget : public QWidget
{
    Q_OBJECT
public:
    LMValidationWidget( LMModel *pmodel, QWidget *pwidgetParent );
    virtual ~LMValidationWidget();

public slots:
    virtual void slotApply();
    virtual void slotLevelChanged( const QString &stringLevel );

protected:
    LMModel *       pmodel;
    QComboBox *     pcomboboxLevels;
};

#endif


