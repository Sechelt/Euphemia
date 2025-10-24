#ifndef H_PMIndexesWidget
#define H_PMIndexesWidget

#include <AWDataTable.h>

#include "PMTableBase.h"

class PMIndexesWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMIndexesWidget( PMTableBase *pTableBase, QWidget *pwidgetParent );
    virtual ~PMIndexesWidget();

public slots:
    virtual void slotNew();

protected:
    PMTableBase *pTableBase;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


