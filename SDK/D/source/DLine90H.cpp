#include "LibInfo.h"
#include "DLine90H.h"

DLine90H::DLine90H( ADObject *pObjectParent, const QString &stringName )
    : DLine90( Qt::Horizontal, pObjectParent, stringName )
{
}

DLine90H::~DLine90H()
{
}

QPixmap DLine90H::getIcon()
{
    return QPixmap( ":D/Line90H" );
}


