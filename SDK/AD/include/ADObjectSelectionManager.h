#ifndef H_ADObjectSelectionManager
#define H_ADObjectSelectionManager

#include "ADObject.h"

/*!
 * \brief Selection manager. 
 *  
 * This manages all selections for a given object. For the selection manager - this becomes the root and 
 * it will manage selections for all of its descendents. 
 *  
 * The given root is typically a loaded document such as a diagram. 
 *  
 * This solves the problem of how to manage selection in a hierarchy. 
 *  
 * \note It uses object pointers so it only works for loaded objects. 
 *  
 * \warning Unselect any selected object before reparenting out of the root hierarchy. 
 *  
 * \warning Do not mix this with calling ADObject::setSelected() directly.
 * 
 * \author pharvey (8/31/20)
 */
class ADObjectSelectionManager : public QObject
{
    Q_OBJECT
public:
    ADObjectSelectionManager( ADObject *pRoot );

    virtual void setSelected( ADObject *pObject, bool bSelect = true );
    virtual QList<ADObject*> getSelected();
    virtual QList<ADObject*> getSelectedChildren();
    virtual void doSelectAll( bool bRecurse = false );
    virtual void doSelectNone();
    virtual void doDelete();
    virtual bool hasSelection() { return listSelected.count(); }
    
signals:
    void signalChangedHasSelection( bool b );
    void signalChangedSelection();

protected slots:
    virtual void slotRemove( QObject *p );     // catch case where object is being deleted (or otherwise removed from the pObject we are managing)

protected:
    ADObject *          pObject;                // object we are managing ie diagram
    QList<ADObject*>    listSelected;           // objects selected

    virtual void doSelectAll( ADObject *p, bool bRecurse );
    virtual void doDelete( ADObject *pObjectParent, ADObject *pObjectChild );
};

#endif


