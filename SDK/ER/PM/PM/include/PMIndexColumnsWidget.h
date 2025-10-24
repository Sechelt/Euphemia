#ifndef H_PMIndexColumnsWidget
#define H_PMIndexColumnsWidget

#include <AWDataTable.h>

#include "PMIndex.h"

class PMIndexColumnsWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMIndexColumnsWidget( PMIndex *pIndex, QWidget *pwidgetParent );
    virtual ~PMIndexColumnsWidget();

public slots:
    virtual void slotNew();

protected:
    PMIndex *pIndex;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


