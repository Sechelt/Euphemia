#ifndef H_LMRulesWidget
#define H_LMRulesWidget

#include <AWDataTable.h>

#include "LMModel.h"

class LMRulesWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMRulesWidget( LMModel *pmodelParent, QWidget *pwidgetParent );
    virtual ~LMRulesWidget();

public slots:
    virtual void slotNew();

protected:

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif


