#ifndef H_LMDataItemsWidget
#define H_LMDataItemsWidget

#include <AWDataTable.h>

#include "LMModel.h"

class LMDataItemsWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMDataItemsWidget( LMModel *ptableParent, QWidget *pwidgetParent );
    virtual ~LMDataItemsWidget();

public slots:
    virtual void slotNew();
    virtual void slotDelete();

protected:
    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif


