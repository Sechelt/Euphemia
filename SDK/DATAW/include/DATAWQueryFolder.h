#ifndef H_DATAWQueryFolder
#define H_DATAWQueryFolder

#include "DATAWQuery.h"

/*!
 * \brief Folder 
 *  
 * Here we handle the icon for a folder (open/close) and nothing else. 
 * A base class - derive something from it. 
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQueryFolder : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryFolder( ADObject *pParent, const QString &stringName = QString() );
    ~DATAWQueryFolder();

    virtual QPixmap getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap getIcon();
};

#endif

