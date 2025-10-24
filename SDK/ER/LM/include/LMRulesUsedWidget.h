#ifndef H_LMRulesUsedWidget
#define H_LMRulesUsedWidget

#include <AWDataTable.h>

#include "LMModel.h"

/*!
 * \brief Add/remove references to rules.
 *  
 * The pModel contains all of the LMRule objects. 
 *  
 * The pObject is the object we are adding/removing references to rules. 
 *  
 * Rules can be referenced by a wide variety of objects. This class is used by all of them.
 *  
 * \author pharvey (12/6/19)
 */
class LMRulesUsedWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMRulesUsedWidget( AWObject *pObject, QWidget *pParent );
    virtual ~LMRulesUsedWidget();

public slots:
    virtual void slotNew();
    virtual void slotNewRule();

protected:
    LMModel *  pModel;
    ADObject * pObject;

    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif


