#ifndef H_LMDomainsWidget
#define H_LMDomainsWidget

#include <AWDataTable.h>

#include "LMModel.h"

class LMDomainsWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMDomainsWidget( LMModel *pmodelParent, QWidget *pwidgetParent );
    virtual ~LMDomainsWidget();

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


