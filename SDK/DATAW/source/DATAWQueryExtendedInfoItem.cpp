#include "LibInfo.h"
#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryExtendedInfoItem::DATAWQueryExtendedInfoItem( ADObject *pParent )
    : AWObject( pParent )
{
}

DATAWQueryExtendedInfoItem::~DATAWQueryExtendedInfoItem()
{
}

QPixmap DATAWQueryExtendedInfoItem::getIcon()
{
    // return QPixmap( ":DATAW/Information" );
    return QPixmap();
}

QMenu *DATAWQueryExtendedInfoItem::getContextMenu( QWidget *pParent )
{
    return nullptr;
}



