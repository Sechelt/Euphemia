#include "LibInfo.h"
#include "AWWorkspace.h"

AWWorkspace::AWWorkspace( const QString &stringName )
    : AWContainer( nullptr, stringName )
{
}

AWWorkspace::~AWWorkspace()
{
}

QPixmap AWWorkspace::getIcon( AWObject::enumIconRoles nRole )
{
    if ( nRole == AWObject::nIconRoleExpanded )
        return QPixmap( ":W/CabinetOpen48x48" );

    return QPixmap( ":W/CabinetClosed48x48" );
}

QPixmap AWWorkspace::getIcon()
{
    return QPixmap( ":W/CabinetClosed48x48" );
}


