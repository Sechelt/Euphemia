#include "LibInfo.h"
#include "DATAWQueryColumn.h"

DATAWQueryColumn::DATAWQueryColumn( ADObject *pParent )
    : AWObject( pParent )
{
}

DATAWQueryColumn::~DATAWQueryColumn()
{
}

QPixmap DATAWQueryColumn::getIcon()
{
    return QPixmap( ":DATAW/Column" );
}



