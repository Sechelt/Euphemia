#ifndef H_LMAttributesWidget
#define H_LMAttributesWidget

#include <AWDataTable.h>

#include "LMEntityBase.h"

class LMAttributesWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMAttributesWidget( LMEntityBase *pEntity, QWidget *pwidgetParent );
    virtual ~LMAttributesWidget();

public slots:
    virtual void slotNew();
    virtual void slotNewDataItem();

protected:
    LMModel *       pModel;
    LMEntityBase *  pEntity;
    QToolButton *   ptoolbuttonNewDataItem;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


