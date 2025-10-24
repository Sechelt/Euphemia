#ifndef H_PMColumnsWidget
#define H_PMColumnsWidget

#include <AWDataTable.h>

#include "PMTableBase.h"

class PMColumnsWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMColumnsWidget( PMTableBase *pTableBase, QWidget *pwidgetParent );
    virtual ~PMColumnsWidget();

public slots:
    virtual void slotNew();

protected:
    PMModel *       pModel;
    PMTableBase *   pTableBase;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


