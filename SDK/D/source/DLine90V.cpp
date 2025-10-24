#include "LibInfo.h"
#include "DLine90V.h"

DLine90V::DLine90V( ADObject *pObjectParent, const QString &stringName )
    : DLine90( Qt::Vertical, pObjectParent, stringName )
{
}

DLine90V::~DLine90V()
{
}

QPixmap DLine90V::getIcon()
{
    return QPixmap( ":D/Line90V" );
}


