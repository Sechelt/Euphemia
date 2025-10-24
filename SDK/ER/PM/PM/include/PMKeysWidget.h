#ifndef H_PMKeysWidget
#define H_PMKeysWidget

#include <AWDataTable.h>

#include "PMTableBase.h"

/*!
 * \class PMKeysWidget
 * \brief Add/remove Identifiers.
 *  
 * \author pharvey (5/17/20)
 */
class PMKeysWidget : public AWDataTable
{
    Q_OBJECT
public:
    PMKeysWidget( PMTableNormal *pKey, QWidget *pParent );
    virtual ~PMKeysWidget();

public slots:
    virtual void slotNew();

protected:
    PMTableNormal *pTable;

    virtual void doRowGet( int nRow, AWObject *pObject );
    virtual void doRowSet( int nRow, AWObject *pObject );
};


#endif 


