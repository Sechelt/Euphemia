#include "LibInfo.h"
#include "ADObjectSelectionManager.h"

ADObjectSelectionManager::ADObjectSelectionManager( ADObject *p )
{
    pObject = p;
}

/*!
 * \brief Select/unselect the given object. 
 *  
 * This is a noop if object is not selectable. 
 *  
 * \note Does NOT verify that object is in the hierarchy we are managing. 
 * 
 * \author pharvey (8/31/20)
 * 
 * \param p 
 * \param b 
 */
void ADObjectSelectionManager::setSelected( ADObject *p, bool bSelect )
{
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, p );
    Q_ASSERT( p );

    bool bHasSelection = hasSelection();

printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    if ( bSelect )
    {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        if ( !p->isSelectable() ) 
        {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
            return;
        }
        if ( listSelected.contains( p ) ) 
        {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
            return;
        }
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        p->setSelected();
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        listSelected.append( p );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        connect( p, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemove(QObject*)) );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    }
    else
    {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        if ( !listSelected.contains( p ) ) return;
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        p->setSelected( false );
        listSelected.removeAll( (ADObject*)p );
        disconnect( p, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemove(QObject*)) );
    }

printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    if ( bHasSelection != hasSelection() ) emit signalChangedHasSelection( hasSelection() );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    emit signalChangedSelection();
}

/*!
 * \brief Get selected objects. 
 *  
 * The objects can be from anywhere in the object hierarchy. 
 * 
 * \author pharvey (8/9/21)
 * 
 * \return QList&lt;ADObject*&gt; 
 */
QList<ADObject*> ADObjectSelectionManager::getSelected() 
{ 
    return listSelected; 
}

/*!
 * \brief Get selected objects. 
 *  
 * The objects are only from the immediate children list.
 * 
 * \author pharvey (8/9/21)
 * 
 * \return QList&lt;ADObject*&gt; 
 */
QList<ADObject*> ADObjectSelectionManager::getSelectedChildren() 
{ 
    QList<ADObject*> listSelected;
    QList<ADObject*> listChildren = pObject->getObjects();

    QListIterator<ADObject*> iteratorChildren( listChildren );

    while ( iteratorChildren.hasNext() )
    {
        ADObject *p = iteratorChildren.next();
        if ( p->isSelected() ) listSelected.append( p );
    } // while

    return listSelected;
}

/*!
 * \brief Select all descendents. 
 *  
 * Recursion ends if a descendent is not selectable. 
 * 
 * \author pharvey (8/31/20)
 */
void ADObjectSelectionManager::doSelectAll( bool bRecurse )
{
    int n = listSelected.count();
    doSelectAll( pObject, bRecurse );
    if ( n < 1 && listSelected.count() ) emit signalChangedHasSelection( hasSelection() );
    if ( n != listSelected.count() ) emit signalChangedSelection();
}

/*!
 * \brief Unselect any selected objects.
 * 
 * \author pharvey (8/31/20)
 */
void ADObjectSelectionManager::doSelectNone()
{
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    if ( !hasSelection() ) 
    {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    ADObject *p;
    foreach( p, listSelected )
    {
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        p->setSelected( false );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
        disconnect( p, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemove(QObject*)) );
    }
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

    listSelected.clear();

    emit signalChangedHasSelection( hasSelection() );
    emit signalChangedSelection();
}

/*!
 * \brief Delete all of the selected objects. 
 *  
 * The obvious side affect is that the selection will become empty. This is done 
 * via QObject::destroyed -> ADObjectSelectionManager::slotRemove.
 *  
 * This will Assert if a selected object does not have an ADObject parent. In otherwords; 
 * the selection manager is not made to support selection of a root object. 
 *  
 * The call to slotDelete is a request to delete. Processing ends if a request fails. 
 *  
 * \author pharvey (9/2/20)
 */
void ADObjectSelectionManager::doDelete()
{
    // We can not just scan the list because calling slotDelete can result in more than one object in the 
    // list to be deleted (delete parent and all its descendents will get deleted) - and we would get a double-delete.
    ADObject *pObjectParent;                                         
    ADObject *pObjectChild;                                         
    while ( listSelected.count() )
    {
        //
        int n = listSelected.count();
        //
        pObjectChild    = listSelected.last();
        pObjectParent   = pObjectChild->getParent( "ADObject" );
        Q_ASSERT( pObjectParent );
        doDelete( pObjectParent, pObjectChild );
        // catch case where request to delete did not result in a delete
        if ( n == listSelected.count() ) return;
    }
}

/*!
 * \brief Remove the object from selection. 
 *  
 * This is typically called when a selected object is deleted but may also 
 * be called when an object is reparented out of the hierarchy we are managing. 
 * 
 * \author pharvey (8/31/20)
 * 
 * \param p 
 */
void ADObjectSelectionManager::slotRemove( QObject *p )
{
    if ( !listSelected.contains( (ADObject*)p ) ) return;

    listSelected.removeAll( (ADObject*)p );
    disconnect( p, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemove(QObject*)) );
    emit signalChangedHasSelection( hasSelection() );
    emit signalChangedSelection();
}

/*!
 * \brief Select all descendents.
 *  
 * Recursion ends if a descendent is not selectable. 
 *  
 * \author pharvey (8/31/20)
 * 
 * \param p 
 */
void ADObjectSelectionManager::doSelectAll( ADObject *p, bool bRecurse )
{
    QList<ADObject*> listObjects = p->getObjects();
    ADObject *pObject;
    foreach( pObject, listObjects )
    {
        if ( !pObject->isSelectable() || pObject->isSelected() ) continue;

        setSelected( pObject );
        if ( !bRecurse ) continue;
        doSelectAll(pObject);
    }
}

/*!
 * \brief Delete child descendents then child. 
 *  
 * \author pharvey (9/7/20)
 * 
 * \param p 
 */
void ADObjectSelectionManager::doDelete( ADObject *pObjectParent, ADObject *pObjectChild )
{
    QList<ADObject*> listObjects = pObjectChild->getObjects();
    ADObject *pObject;
    foreach( pObject, listObjects )
    {
        doDelete( pObjectChild, pObject );
    }
    // one or more child objects did not delete so short-circuit
    if ( pObjectChild->getObjects().count() ) return;

    // all descendents are deleted so delete 
    pObjectParent->slotDelete( pObjectChild );
}


