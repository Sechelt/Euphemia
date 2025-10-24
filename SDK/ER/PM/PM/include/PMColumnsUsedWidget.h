#ifndef H_PMColumnsUsedWidget
#define H_PMColumnsUsedWidget

#include <AWDataTable.h>

#include "PMKey.h"

class PMTableBase;

/*!
 * \class PMColumnsUsedWidget 
 * \brief Add/remove references to columns. 
 *  
 * This is for adding/removing columns (references to columns) for a PMKey. 
 *  
 * Keys can reference 0-n columns. A useful Key references at least one column. 
 * 
 * \author pharvey (5/17/20)
 */
class PMColumnsUsedWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMColumnsUsedWidget( PMKey *pKey, QWidget *pParent );
    virtual ~PMColumnsUsedWidget();

public slots:
    virtual void slotNew();
    virtual void slotDelete();

protected:
    PMTableBase *  pTable;
    PMKey *  pKey;

    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


