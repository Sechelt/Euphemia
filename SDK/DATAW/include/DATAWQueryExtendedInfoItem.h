#ifndef H_DATAWQueryExtendedInfoItem
#define H_DATAWQueryExtendedInfoItem

#include "DATAWQuery.h"

/*!
 * \brief Extended info item. 
 *  
 * This is the deepest we can go with extended info (can not produce children). 
 * This is the value of an info item and is used for all extended info items.  
 * 
 * \author pharvey (1/1/21)
 */
class DATAWQueryExtendedInfoItem : public AWObject
{
    Q_OBJECT
public:
    DATAWQueryExtendedInfoItem( ADObject *pParent );
    virtual ~DATAWQueryExtendedInfoItem();

    virtual QPixmap getIcon();
    virtual QMenu * getContextMenu( QWidget * );
};

#endif

