#ifndef H_AWWorkspace
#define H_AWWorkspace

#include "AWContainer.h"

/*!
 * \class AWWorkspace 
 * \brief Extends AWContainer to handle special case where we are the root object. 
 *  
 * A workspace is always a root object. 
 *  
 * \sa AWObject     - base class 
 * \sa AWContainer  - saves/loads self also saves/loads references to its children 
 * \sa AWWorkspace  - a special AWContainer... is the 'root' object
 * \sa AWFile       - saves/loads self and all of its children 
 * \sa AWFolder     - no persistence but good for organizing objects in a tree view (pass through)
 *  
 * \author pharvey (1/23/20)
 */
class AWWorkspace : public AWContainer
{
    Q_OBJECT
public:
    AWWorkspace( const QString &stringName = QString() );
    virtual ~AWWorkspace();

    virtual QPixmap getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap getIcon();
};

#endif


