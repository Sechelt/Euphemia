#ifndef H_PMRulesUsedWidget
#define H_PMRulesUsedWidget

#include <AWDataTable.h>

#include "PMModel.h"

/*!
 * \brief Add/remove references to rules.
 *  
 * The pModel contains all of the PMRule objects. 
 *  
 * The pObject is the object we are adding/removing references to rules. 
 *  
 * Rules can be referenced by a wide variety of objects. This class is used by all of them.
 *  
 * \author pharvey (12/6/19)
 */
class PMRulesUsedWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMRulesUsedWidget( AWObject *pObject, QWidget *pParent );
    virtual ~PMRulesUsedWidget();

public slots:
    virtual void slotNew();
    virtual void slotNewRule();

protected:
    PMModel *  pModel;
    ADObject * pObject;

    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif


