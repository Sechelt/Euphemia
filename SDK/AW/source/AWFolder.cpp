#include "LibInfo.h"
#include "AWFolder.h"

AWFolder::AWFolder( AWObject *pParent,  const QString &stringName )
    : AWObject( pParent, stringName )
{
}

AWFolder::~AWFolder()
{
}

QPixmap AWFolder::getIcon( AWObject::enumIconRoles nRole )
{
    if ( nRole == AWObject::nIconRoleExpanded )
        return QPixmap( ":AW/FolderOpen" );
    
    return QPixmap( ":AW/FolderClosed" );
}

QPixmap AWFolder::getIcon()
{   
    return QPixmap( ":AW/FolderClosed" );
}


