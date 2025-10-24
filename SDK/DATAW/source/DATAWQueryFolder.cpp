#include "LibInfo.h"
#include "DATAWQueryFolder.h"

DATAWQueryFolder::DATAWQueryFolder( ADObject *pParent, const QString &stringName )
    : DATAWQuery( pParent, stringName )
{
}

DATAWQueryFolder::~DATAWQueryFolder()
{
}

QPixmap DATAWQueryFolder::getIcon( AWObject::enumIconRoles nRole )
{
    switch ( nRole )
    {
    case nIconRoleExpanded:
        return QPixmap( ":DATAW/FolderOpened" );
    case nIconRoleCollapsed:
        return QPixmap( ":DATAW/FolderClosed" );
    }
    return getIcon();
}

QPixmap DATAWQueryFolder::getIcon()
{
    return QPixmap( ":DATAW/FolderClosed" );
}


