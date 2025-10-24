#ifndef H_DATAWQueryColumn
#define H_DATAWQueryColumn

#include "DATAWQuery.h"

/*!
 * \brief Generic column. Used for; index column, table column, etc
 *  
 * Does not produce children. 
 * Does not query - all of its info is provided by calling setName() and setComment().
 *  
 * \author pharvey (6/13/20)
 */
class DATAWQueryColumn : public AWObject
{
    Q_OBJECT
public:
    DATAWQueryColumn( ADObject *pParent );
    virtual ~DATAWQueryColumn();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Column" ); }
};

#endif

