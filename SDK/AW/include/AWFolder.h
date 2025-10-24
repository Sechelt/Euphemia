#ifndef H_AWFolder
#define H_AWFolder

#include "AWObject.h"

/*!
 * \brief A folder object. 
 *  
 * This helps organize objects in a tree view (to reduce clutter). 
 *  
 * To do this; 
 *  
 * 1. derive a new class from AWFolder 
 * 2. implement an object factory (getObject() etc) 
 *  
 * Only AWFile and AWContainer represent a file.
 *  
 * \sa AWObject                 - base class 
 * \sa AWContainer              - Saves/loads self including children to a file (exception below). 
 *                                Only saves a REFERENCE (filename) for any AWFile/AWContainer children.
 *                                Can be persisted and can have children like any AWObject. 
 * \sa AWWorkspace              - a special AWContainer... is the 'root' object
 * \sa AWFile                   - Saves/loads self and all of its children to a file. 
 * \sa AWFolder                 - Not much need for data other than stringName. Derived classes may add 
 *                                more data and certianly a version of getObject() to create children.
 *                                Can be persisted and can have children like any AWObject.
 *                                Example; organizing Stencil items into a Stencil then into a Stencils folder.
 * \sa AWObject::setFolder()    - Light weight folder alternative for organizing in tree view. 
 *                                Folder is not persisted and has no parent nor children.
 *                                Example; organizing Shapes and Connectors into their own folder.
 *  
 * \author pharvey (11/4/19)
 */
class AWFolder : public AWObject
{
    Q_OBJECT
public:
    explicit AWFolder( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~AWFolder();
    
    virtual QPixmap         getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Folder" ); }
};

#endif


