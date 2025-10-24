#include "LibInfo.h"
#include "ADObject.h"

#include "ADDocument.h"

ADObject *g_Root;

ADObject::ADObject( ADObject *pobjectParent, const QString &stringName )
    : QObject( pobjectParent )
{
    setObjectName( stringName );

    // meta for object factory (getObject())
    mapMeta.insert( "ADObject", ADMeta( QIcon(), "ADObject", "Object" ) );

    // events
    mapEvents.insert( "Init", QString() );
    mapEvents.insert( "Fini", QString() );

    // init - general
    nOID                = 0;        // default is not set ( zero )
    nNextOID            = 1;        // 1 and count up
    nNextOIDTemp        = -1;       // -1 and count downQString()
    pobjectOIDSource    = nullptr;  // we control OID's for our children by default
    this->stringName    = stringName;
    bSelected           = false;
    bModified           = false;
    bSelectable         = false;
    nState              = 0;

    // init - if we have parent
    if ( pobjectParent )
    {
        // use parents pobjectOIDSource for our children or NULL to generate our own OID's
        setOIDSource( pobjectParent->getOIDSource() );

        // create a non-persistent OID so we have something to work with
        nOID = pobjectParent->getNextOID( false );

        //
        // Echo up to root. 
        //

        // A new object has been created. Echo up the parent and child.
        connect( this, SIGNAL(signalCreated(ADObject*, ADObject*)), pobjectParent, SIGNAL(signalCreated(ADObject*, ADObject*)) );
        // An object has been selected/unselected.
        connect( this, SIGNAL(signalSelected(ADObject*, bool)), pobjectParent, SIGNAL(signalSelected(ADObject*, bool)) );
        // An object is about to be deleted. Use this instead of *Destroyed*.
        connect( this, SIGNAL(signalDeleted(ADObject*, ADObject*)), pobjectParent, SIGNAL(signalDeleted(ADObject*, ADObject*)) );
        // An object has been modified. 
        connect( this, SIGNAL(signalModified(ADObject*)), pobjectParent, SLOT(eventModified(ADObject*)) );
        // An object has generated a message. Most messages will include a path to source - some relative to root only.
        connect( this, SIGNAL(signalOutput(ADEventOutput *)), pobjectParent, SIGNAL(signalOutput(ADEventOutput *)) );

        //
        // Other
        //

        // A child object wants to be deleted. 
        // - The child never deletes itself. It emits signalDeleteRequest( this ) to its parent, application, and whatever else cares.
        // - In most cases the request results in a delete.
        // - In some cases the request does not result in a delete - as when a User cancels a file save request.
        connect( this, SIGNAL(signalDeleteRequest(ADObject*)), pobjectParent, SLOT(slotDelete(ADObject *)) );

    }
}

ADObject::~ADObject()
{
    // \todo Unsure as to where this should go
    // request referenced objects get deleted?
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    if ( g_Transaction->getDeleteMode() == ADTransaction::DeleteReferencedUnused )
    {
        QList<ADObjectReferenceKey> listKeys = mapUses.keys();
        ADObjectReferenceKey Key;
        foreach( Key, listKeys )
        {

        }
    }

    // Remove all UsedBy
    {   
        QList<ADObjectReferenceKey> listKeys = mapUsedBy.keys();
        ADObjectReferenceKey Key;
        foreach( Key, listKeys )
        {
            Key.pObject->doUsesRemove( this, Key.stringName );
        }
    }
    mapUsedBy.clear();

    // Remove all Uses
    {
        QList<ADObjectReferenceKey> listKeys = mapUses.keys();
        ADObjectReferenceKey Key;
        foreach( Key, listKeys )
        {
            Key.pObject->doUsedByRemove( this, Key.stringName );
        }
    }
    mapUses.clear();
}

void ADObject::setName( const QString &s )
{
    if ( stringName == s ) return;
    QObject::setObjectName( s );
    stringName = s;
    if ( g_Transaction->isLoad() ) return;
    setModified();
    emit signalName( s );
}

void ADObject::setCode( const QString &s )
{
    if ( stringCode == s ) return;
    stringCode = s;
    if ( g_Transaction->isLoad() ) return;
    setModified();
    emit signalCode( s );
}

void ADObject::setComment( const QString &s )
{
    if ( stringComment == s ) return;
    stringComment = s;
    if ( g_Transaction->isLoad() ) return;
    setModified();
    emit signalComment( s );
}

/*!
 * \brief Select/Unselect this object.
 *  
 * This is noop if this object is not selectable. 
 *  
 * Do not call here if using an \sa ADObjectSelectionManager (the normal case) - call into the selection manager instead. 
 *  
 * \author pharvey (8/31/20)
 * 
 * \param b 
 */
void ADObject::setSelected( bool b )
{
    if ( !isSelectable() ) return;

    // short circuit
    if ( b == bSelected ) return;

    // select/unselect self
    bSelected = b;

    // lete everyone, who may care, know
    emit signalSelected( b );
    emit signalSelected( this, b );
}

/*!
 * \brief Change modified status. 
 *  
 * Changes our modified status. 
 *  
 * TRUE 
 *  
 * Will trigger both eventModified methods. This will result in an echo up the object hierarchy. 
 *  
 * FALSE 
 *  
 * Will call setModified in all children and then trigger an eventModified.
 * 
 * \author pharvey (2/9/20)
 * 
 * \param b 
 */
void ADObject::setModified( bool b )
{
    if ( g_Transaction->isLoad() ) return;

    // 'short circuit'
//    if ( b == bModified ) return;

    // TRUE
    // - echo up
    // - if we are modified then everything above us is modified 
    if ( b )
    {
        bModified = true;
        eventModified();        // tell anything connected to us
        eventModified( this );  // tell everyone (echo up)
        return;
    }

    // FALSE
    // - propagate down
    // - if we are NOT modified then everything below us is not modified
    // - we do not echo back up the hierarchy when false
    bModified = false;
    QList<ADObject*> l = getObjects();
    ADObject *p; 
    foreach( p, l )
    {
        p->setModified( false );
    }
    eventModified(); // tell anything connected to us (no need to echo up)
}

void ADObject::setState( int n )
{
    if ( n == nState ) return;

    nState = n;
    emit signalState();
}

void ADObject::setValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation, ADValidation::Severity nSeverity )
{
    QObject *pObject = parent();
    if ( !pObject ) return;
    if ( !pObject->inherits( "ADObject" ) ) return;

    ((ADObject*)pObject)->setValidationSeverity( stringClass, nStrictness, stringValidation, nSeverity );
}

void ADObject::setScript( const QString &stringName, const QString &stringScript )
{
    mapScripts[stringName] = stringScript;
}

/*!
    getNextOID

    Return the next available nOID. We allow for all child objects to be unique while
    also allowing us to know which nOID values are to be persistent. The nNextOID should
    be saved/restored in similar fashion to the nOID of the child objects.
*/
int ADObject::getNextOID( bool bForPersist )
{
    // we are NOT source for OID
    if ( pobjectOIDSource )
        return pobjectOIDSource->getNextOID( bForPersist );

    // we are source for OID
    int nReturn;

    if ( bForPersist )
    {
        nReturn = nNextOID;
        nNextOID++;
    }
    else
    {
        nReturn = nNextOIDTemp;
        nNextOIDTemp--;
    }

    return nReturn;
}

ADObject *ADObject::getParent( const char *pszClassName )
{
    QObject *pobjectParent;
    QObject *pobjectChild   = this;

    while ( 1 )
    {
        pobjectParent = pobjectChild->parent();
        if ( !pobjectParent )
            break;
        if ( pobjectParent->inherits( pszClassName ) )
            return (ADObject*)pobjectParent;
        pobjectChild = pobjectParent;
    }

    return nullptr;
}

/*!
 * \brief Return the selection manager. 
 *  
 * This returns a nullptr by default. Some document objects may have a selection manager. 
 * In such a case; the document and all of its descendents should know how to return the 
 * document's selection manager. 
 * 
 * \author pharvey (9/2/20)
 * 
 * \return ADObjectSelectionManager* 
 */
ADObjectSelectionManager *ADObject::getSelectionManager()
{
    return nullptr;
}

/*!
    getPath

    Returns OID path as a list of ADObjectPathItem.     
*/
ADObjectPathList ADObject::getPath( ADObject *pobjectRoot )
{
    ADObjectPathList listObjectPathItems;

    if ( pobjectRoot != this )
    {
        if ( parent() )
        {
            ADObject *pobject = (ADObject*)parent();
            listObjectPathItems = pobject->getPath( pobjectRoot );
        }
    }

    listObjectPathItems += ADObjectPathItem( ADObjectPathItem::ValueOID, this, metaObject()->className(), getName(), getOID() );

    return listObjectPathItems;
}

/*!
 * \brief Get path from root to this object.
 * 
 * \author pharvey (1/20/20)
 * 
 * \return ADObjectPathList 
 */
ADObjectPathList ADObject::getPath()
{
    ADObjectPathList listObjectPathItems;

    if ( parent() )
    {
        ADObject *pobject = (ADObject*)parent();
        listObjectPathItems = pobject->getPath();
    }

    listObjectPathItems += ADObjectPathItem( ADObjectPathItem::ValueOID, this, metaObject()->className(), getName(), getOID() );

    return listObjectPathItems;
}

/*!
 * \brief Get a path from root to this object. 
 *  
 * Returns a comma delimited string of OID's. 
 * The first OID is the root and the last OID is for this object. 
 *  
 * This is not meant to be used for normal navigation... its much 
 * faster to navigate by object pointers. 
 *  
 * The result is a key that; 
 * - is unique in the entire object hierarchy 
 * - can be used to navigate to the object ie to resolve references during load/paste
 * - can be saved to persistent storage 
 * 
 * \author pharvey (12/24/19)
 * 
 * \param pRoot 
 * 
 * \return QString 
 */
QString ADObject::getPathString( ADObject *pRoot )
{
    QString stringPath;

    // we have recursed to the root so start building path with our OID
    if ( pRoot == this || !parent() ) return QString::number( getOID() );

    Q_ASSERT( parent()->inherits("ADObject") );     // parent outside of our object hiearchy? root is invalid

    // recurse to root
    stringPath = ((ADObject*)parent())->getPathString( pRoot );

    // add self - as we unwind recurse
    if ( !stringPath.isEmpty() ) stringPath += ",";
    stringPath += QString::number( getOID() );

    return stringPath;
}

/*!
 * \brief Get a duplicate object (if any) from among its peers. 
 *  
 * Returns the first child object where the Field is the same as in pObject. Presumably; 
 * the two objects are peers. 
 *  
 * This is useful during a validation process. 
 *  
 * Returns nullptr if no duplicate found. 
 *  
 * \author pharvey (11/29/19)
 */
ADObject *ADObject::getDuplicate( ADObject *pObject, ADObject::Fields nField )
{
    const QObjectList listChildren = children();
    QObject *p;

    foreach( p, listChildren )
    {
        if ( !p->inherits( "ADObject" ) ) continue;
        if ( ((ADObject*)p) == pObject ) continue;

        switch ( nField )
        {
            case FieldName:
                if ( pObject->getName() == ((ADObject*)p)->getName() )
                    return ((ADObject*)p);
                break;
            case FieldCode:
                if ( pObject->getCode() == ((ADObject*)p)->getCode() )
                    return ((ADObject*)p);
                break;
        }

    }
    return nullptr;
}

QStringList ADObject::getEvents()
{
    QList<QString> listKeys = mapEvents.keys();

    return listKeys;
}

QStringList ADObject::getScripts()
{
    QList<QString> listKeys = mapScripts.keys();

    return listKeys;
}

QString ADObject::getScript( const QString &stringName )
{
    return mapScripts[stringName];
}

/*!
 * \brief Get the child object with the provided OID. 
 *  
 * Scans the list of children. 
 * Returns nullptr if not found. 
 * 
 * \author pharvey (12/24/19)
 * 
 * \param nOID 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( int nOID )                             
{                                                                     
    const QObjectList listChildren   = children();                    
                                                                      
    QListIterator<QObject*> iteratorChildren( listChildren );         
    ADObject *              pobjectFound;                             
                                                                      
    while ( iteratorChildren.hasNext() )                              
    {                                                                 
        QObject *pobject = iteratorChildren.next();                   
        if ( !pobject->inherits( "ADObject" ) ) continue;                                                 
                                                                      
        pobjectFound = (ADObject*)pobject;                            
        if ( pobjectFound->getOID() != nOID ) continue;                                                 
                                                                      
        //                                                            
        // FOUND  IT!                                                 
        //                                                            
        return pobjectFound;                                          
                                                                      
    } // while                                                        
                                                                      
    return nullptr;                                                   
}                                                                     

/*!
 * \brief Return the object for the given path. 
 *  
 * Pass full path to root object. This will recurse along path until it determines 
 * the object else returns nullptr. 
 *  
 * This is a LIFO stack.  
 * The stack must have the root object OID last and the desired object's OID first.
 *  
 * \author pharvey (12/24/19)
 * 
 * \param stringPath 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( QStack<int> &stackPath )
{
    // is it us?
    stackPath.pop();
    if ( stackPath.count() < 1 ) return this;
    // get child
    ADObject *p = getObject( stackPath.top() );
    if ( !p ) return nullptr;
    // recurse
    return p->getObject( stackPath );
}

/*!
 * \brief Return the object for the given path. 
 *  
 * Pass full path to root object. This will recurse along path until it determines 
 * the object else returns nullptr. 
 *  
 * The list must have the root object OID first and the desired object's OID last.
 * 
 * \author pharvey (12/24/19)
 * 
 * \param l 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( const QStringList &l )
{
    if ( l.isEmpty() ) return nullptr;
    QStack<int> stackPath;
    // load stack - reverse order as it is LIFO
    for ( int n = l.count() - 1; n >= 0; n-- )
    {
        stackPath.push( l.at( n ).toInt() );
    }

    return getObject( stackPath );
}

/*!
 * \brief Return the object for the given path. 
 *  
 * Pass full path to root object. This will recurse along path until it determines 
 * the object else returns nullptr. 
 *  
 * This uses an enhanced form of a path. It can be faster and more flexible than a 
 * simple list of OID's. 
 * 
 * \author pharvey (12/24/19)
 * 
 * \param listObjectPathItems 
 * \param nDepth 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( ADObjectPathList listObjectPathItems, uint nDepth )
{
    // We are it!
    if ( (int)nDepth >= listObjectPathItems.count() )
        return this;

    // Find next ADObject.
    ADObject *          pobject          = 0;
    ADObjectPathItem   objectpathitem  = listObjectPathItems[nDepth];

    switch ( objectpathitem.nValueType )
    {
        case ADObjectPathItem::ValueSpecial:
            // Derived classes will implement something here as required.
            return 0;
        case ADObjectPathItem::ValueIndex:
            {
                QList<ADObject*> listChildren = getObjects( QString(), QString(), QString(), objectpathitem.nValue );
                pobject = listChildren.first();
                if ( !pobject || pobject->metaObject()->className() != objectpathitem.stringClassName )
                    return 0;
            }
            break;
        case ADObjectPathItem::ValuePointer:
            {
                pobject = (ADObject*)objectpathitem.pobject;

                // Verify pointer...
                //
                // NOTE: we do not use objectpathitem.stringObjectName on purpose :)
                if ( !getObject( pobject, QString(), objectpathitem.stringClassName, QString(), false ) )
                    return 0;
            }
            break;
        case ADObjectPathItem::ValueOID:
            {
                pobject = getObject( objectpathitem.nValue );
                if ( !pobject )
                    return 0;
            }
            break;
    }

    // Recurse into next ADObject.
    return pobject->getObject( listObjectPathItems, nDepth + 1 );
}

/*!
 * \brief Find an object.
 *  
 * This method exists because we need to be able to verify that an object still exists in the           
 * object hierarchy before we use it. If pobject was found, and the criteria matched, then pobject is 
 * simply returned. A null value is returned if pobject is not considered valid.                      
 *                                                                                                    
 * \note It is possible that the pointer given in pobject is found and matchs the criteria            
 * but is not the original object. This may be extremely unlikley - but caution should be taken.
 *  
 * \author pharvey (12/24/19)
 * 
 * \param pobject 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 * \param bRecurse 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( ADObject *pobject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass, bool bRecurse )
{
// Use a null pobject as a wildcard
//    if ( !pobject )
//        return 0;

    const QObjectList listChildren = children();

    QListIterator<QObject*>       iteratorChildren( listChildren );
    ADObject *          pobjectFound;

    // lets assume that a match is more likely near the top of the object hierarchy, in
    // this case want to delay recurse until after we check *all* our children
    while ( iteratorChildren.hasNext() )
    {
        QObject *p = iteratorChildren.next();
        if ( !p->inherits( "ADObject" ) )
            continue;
        pobjectFound = (ADObject*)p;
        if ( pobject && pobjectFound != pobject )
            continue;
        if ( !stringName.isNull() && pobjectFound->getName() != stringName )
            continue;
        if ( !stringClassName.isNull() && pobjectFound->metaObject()->className() != stringClassName )
            continue;
        if ( !stringInheritsClass.isNull() && !pobjectFound->inherits( stringInheritsClass.toLatin1().constData() ) )
            continue;

        //
        // FOUND  IT!
        //
        return pobjectFound;

    } // while

    // Not found yet so recurse as required
    if ( bRecurse )
    {
        iteratorChildren.toFront();
        while ( iteratorChildren.hasNext() )
        {
            QObject *p = iteratorChildren.next();
            if ( !p->inherits( "ADObject" ) )
                continue;
            pobjectFound = (ADObject*)p;
            if ( pobjectFound->getObject( pobject, stringName, stringClassName, stringInheritsClass, bRecurse ) )
                return pobject;

        } // while
    }

    return 0;
}

/*!
    getModels
    
    Returns a list of ADObject which match the given criteria.
*/
QList<ADObject*> ADObject::getObjects( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass, int nIndex )
{
    QList<ADObject*>  list;
    const QObjectList listChildren = children();

    QListIterator<QObject*>       iteratorChildren( listChildren );
    ADObject *          pobject;
    int                 nIndexCurrent = 0;

    while ( iteratorChildren.hasNext() )
    {
        QObject *p = iteratorChildren.next();
        if ( !p->inherits( "ADObject" ) )
            continue;
        pobject = (ADObject*)p;
        if ( !stringName.isNull() && pobject->getName() != stringName )
            continue;
        if ( !stringClassName.isNull() && pobject->metaObject()->className() != stringClassName )
            continue;
        if ( !stringInheritsClass.isNull() && !pobject->inherits( stringInheritsClass.toLatin1().constData() ) )
            continue;

        if ( nIndex >= 0 )
        {
            if ( nIndex == nIndexCurrent )
            {
                list.append( pobject );
                break;
            }
        }
        else
            list.append( pobject );

        nIndexCurrent++;

    } // while

    return list;
}

/*!
    Returns a list of ADObject which match the given criteria.
*/
QStringList ADObject::getObjectsStringList( const QString &stringClassName, const QString &stringInheritsClass )
{
    QStringList list;
    const QObjectList listChildren = children();
    QListIterator<QObject*> iteratorChildren( listChildren );
    while ( iteratorChildren.hasNext() )
    {
        QObject *p = iteratorChildren.next();
        if ( !p->inherits( "ADObject" ) )
            continue;
        ADObject *pobject = (ADObject*)p;
        if ( !stringClassName.isNull() && pobject->metaObject()->className() != stringClassName )
            continue;
        if ( !stringInheritsClass.isNull() && !pobject->inherits( stringInheritsClass.toLatin1().constData() ) )
            continue;

        list += pobject->getName();

    } // while

    return list;
}

/*!
    getUses
    
    Returns a list of ADObject which match the given criteria.
*/
QList<ADObject*> ADObject::getUses( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass, int nIndex )
{
    QList<ADObject*>          list;
    int                         nIndexCurrent = 0;

    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( !stringName.isNull() && Key.pObject->getName() != stringName )
            continue;
        if ( !stringClassName.isNull() && Key.pObject->metaObject()->className() != stringClassName )
            continue;
        if ( !stringInheritsClass.isNull() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) )
            continue;

        if ( nIndex >= 0 )
        {
            if ( nIndex == nIndexCurrent )
            {
                list.append( Key.pObject );
                break;
            }
        }
        else
            list.append( Key.pObject );

        nIndexCurrent++;

    } // while

    return list;
}

/*!
 * \brief Returns a list of classes we can instantiate.
 *  
 * Classes can be 'friendly' or 'normal'. 
 * The 'friendly' names are a bit more descriptive. 
 * We need the 'friendly' ones because they are used in the UI.
 *  
 * This is part of how each ADObject is an object factory. 
 *  
 * \author pharvey (3/13/20)
 * 
 * \param bFriendly 
 * 
 * \return QVector&lt;QString&gt; 
 */
QStringList ADObject::getClasses( bool bFriendly )
{
    QStringList listReturn;

    if ( bFriendly )
    {
        foreach( ADMeta t, mapMeta )
        {
            listReturn.append( t.stringFriendlyName );
        }
    }
    else 
        listReturn = mapMeta.keys();

    return listReturn;
}

/*!
 * \brief Get a 'normal' class name for the given class name. 
 *  
 * This is part of how each ADObject is an object factory. 
 * 
 * \author pharvey (3/13/20)
 * 
 * \param stringName Class name. Can be either 'friendly' or 'normal' but will usually be 'friendly'. 
 * 
 * \return QString 
 */
QString ADObject::getClass( const QString &stringName )
{
    // is it a C++ class name (key)?
    if ( mapMeta.contains( stringName ) ) return stringName;
    // must be friendly name? - scan to get result
    foreach( ADMeta t, mapMeta )
    {
        if ( t.stringFriendlyName == stringName ) return t.stringClass;
    }
    return QString();
}

/*!
 * \brief Get an object of the given class. 
 *  
 * The given class can be 'friendly' or 'normal'. 
 *  
 * This is part of how each ADObject is an object factory. 
 *  
 * Derived classes will typically initialize vectorClassFriendly and vectorClassesNormal in 
 * its constructor. It can then override this method to instantiate desired classes. 
 *  
 * A specific object can be an object factory for others. This is done by specifying pObjectParent. 
 *  
 * \author pharvey (3/13/20)
 * 
 * \param stringClass 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getObject( const QString &stringClass, ADObject *pObjectParent )
{
    QString s = getClass( stringClass );
    if ( s.isEmpty() ) return nullptr;

    ADObject *p = nullptr;

    if ( s == "ADObject" )
        p = new ADObject( pObjectParent ? pObjectParent : this );

    Q_ASSERT( p );
    if ( !p ) return nullptr;

    emit signalCreated( p );
    setModified();

    return p;
}

ADObject *ADObject::getObjectAnyParent( const QString &stringClass )
{
    QString s = getClass( stringClass );
    if ( s.isEmpty() ) 
    {
        if ( !parent() ) return nullptr;
        return ((ADObject*)parent())->getObjectAnyParent( stringClass );
    }

    return getObject( stringClass );
}

/*!
 * \brief Sets the parent to null. 
 *  
 * We do not use QObject::reparent directly because we want to emit a signalChildRemoved. 
 * This signal can be used (for example) to update the user interface (object browser).
 *  
 * \sa doChildAdd 
 *  
 * \author pharvey (9/7/20)
 * 
 * \param p 
 */
void ADObject::doChildRemove( ADObject *p )
{
    Q_ASSERT( children().contains( p ) );
    p->setParent( nullptr );
    emit signalChildRemoved( p );
}

/*!
 * \brief Sets the parent to this. 
 *  
 * We do not use QObject::reparent directly because we want to emit a signalChildAdded. 
 * This signal can be used (for example) to update the user interface (object browser).
 *  
 * \sa doChildRemove 
 *  
 * \author pharvey (9/7/20)
 * 
 * \param p 
 */
void ADObject::doChildAdd( ADObject *p )
{
    p->setParent( this );
    emit signalChildAdded( p );
}

void ADObject::doScriptRemove( const QString &stringName )
{
    mapScripts.remove( stringName );
}

/*!
 * \brief Save ourself to XML. 
 *  
 * Here we save the basic data we know about; Name, Code, Comment and, most importantly, our OID. 
 *  
 * We save all of our references (Uses not UsedBy). 
 *  
 * We call same for all of our children. 
 *  
 * Derived classes will probably override this method to save additional data but will usually 
 * start by calling here to get the QDomElement - primed with the work we do here.
 *  
 * In exceptional cases; a derived class will override this method and ignore our work here. 
 * Examples of this; 
 *  
 * - a container which holds files (completely ignores the code in here)
 * - when initiating a copy (needs to loop over selected objects - still calls here for each of those objects) 
 *  
 * The save can be done in support of a copy. In such a case we get direction from g_Transaction.
 *  
 * \author pharvey (12/14/19)
 * 
 * \param pdomDoc 
 * \param pdomElemParent 
 * \param bOnlySelected 
 * 
 * \return QDomElement 
 */
QDomElement ADObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "Class" );
    domElemThis.setAttribute( "ClassName", metaObject()->className() );

    if ( !getName().isEmpty() ) domElemThis.setAttribute( "Name", getName() );
    if ( !getCode().isEmpty() ) domElemThis.setAttribute( "Code", getCode() );
    if ( !getComment().isEmpty() ) domElemThis.setAttribute( "Comment", getComment() );
    if ( g_Transaction->canCopy() ) domElemThis.setAttribute( "Path", getPathString( g_Transaction->pRoot ) );
    domElemThis.setAttribute( "OID", getOID() );
    domElemThis.setAttribute( "NextOID", nNextOID /* DO NOT use getNextOID() for this */ );

    // references
    {
        QList<ADObjectReferenceKey> l = getUsesKeys();
        ADObjectReferenceKey        Key;
        QDomElement                 domElemReference;                                        
        foreach( Key, l )
        {
            if ( g_Transaction->canCopy() )
            {
                // copy referenced objects
                if ( g_Transaction->getCopyMode() == ADTransaction::CopyReferencedObjects )
                { 
                    domElemReference = pdomDoc->createElement( "Reference" );                    
                    domElemReference.setAttribute( "Path", Key.pObject->getPathString( g_Transaction->pRoot ) );   
                    domElemReference.setAttribute( "Name", Key.stringName );                 
                    domElemReference.setAttribute( "ReferencedObject", "Copied" );                 
                    if ( !g_Transaction->hashObjectsCopied.contains( Key.pObject ) )
                    {                                                                
                        g_Transaction->hashObjectsCopied.insert( Key.pObject );     
                        Key.pObject->doSave( pdomDoc, &domElemReference );  
                    }                                                                
                    domElemThis.appendChild( domElemReference );                                     
                }
                // copy reference only
                else if ( g_Transaction->getCopyMode() == ADTransaction::CopyReferenceOnly )
                { 
                    domElemReference = pdomDoc->createElement( "Reference" );                    
                    domElemReference.setAttribute( "Path", Key.pObject->getPathString( g_Transaction->pRoot ) );   
                    domElemReference.setAttribute( "Name", Key.stringName );                 
                    domElemReference.setAttribute( "ReferencedObject", "ReferenceOnly" );                 
                    domElemThis.appendChild( domElemReference );                                     
                }
                // copy reference only - but save class so paste can create a fresh object for reference
                else if ( g_Transaction->getCopyMode() == ADTransaction::CopyReferenceNew )
                {
                    domElemReference = pdomDoc->createElement( "Reference" );                    
                    domElemReference.setAttribute( "Path", Key.pObject->getPathString( g_Transaction->pRoot ) );   
                    domElemReference.setAttribute( "Name", Key.stringName );                 
                    domElemReference.setAttribute( "ReferencedObject", "New" );                 
                    if ( !g_Transaction->hashObjectsCopied.contains( Key.pObject ) )
                    {                                                                
                        g_Transaction->hashObjectsCopied.insert( Key.pObject );     
                        domElemReference.setAttribute( "Class", Key.pObject->metaObject()->className() );                 
                    }                                                                
                    domElemThis.appendChild( domElemReference );                                     
                }
            }
            else
            {
                // save reference only - all objects are getting saved so just reference is fine
                domElemReference = pdomDoc->createElement( "Reference" );                    
                domElemReference.setAttribute( "Path", Key.pObject->getPathString( g_Transaction->pRoot ) );   
                domElemReference.setAttribute( "Name", Key.stringName );                 
                domElemThis.appendChild( domElemReference );                                     
            }
        }
    }
    pdomElemParent->appendChild( domElemThis );

    // save the children
    QList<ADObject*> listChildren = getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {   
        // the initiator of a copy should start with a loop to save all selected objects
        // after that we save all descendents of those objects regardless of selected status
        // if ( g_Transaction->canCopy() && pChild->isSelectable() && !pChild->getSelected() )
        //    continue;

        pChild->doSave( pdomDoc, &domElemThis );
    }

   return domElemThis;
}

/*!
 * \brief Load self from XML. 
 *  
 * Loads our basic data and references. The references are stored in object 
 * properties where they can be accessed during doPostLoad. 
 *  
 * Goes on to load any children. This is done by; 
 *  
 * - picking up class names (each node has a tag which is the class name) 
 * - calling our getObject() to create the object 
 * - calling the child objects doLoad 
 *  
 * Derived classes should override the following (if it can have children) as this is the 'object factory'; 
 *  
 * - getClasses - tells us which class names we can expect to get an object for 
 * - getObject  - creates/provides the object for the given class name 
 *  
 * Derived class will probably want to override this method as well. 
 * In most cases the new method will call here, to take advantage of this work, and then 
 * load any additional data. 
 *  
 * Derived classes will want to override doLoadChildren if it plans to deviate from all node+tag 
 * being child classes. 
 *  
 * \author pharvey (12/22/19)
 * 
 * \param pdomElemThis 
 * 
 * \return bool 
 */
bool ADObject::doLoad( QDomElement *pdomElemThis )
{
    QString stringAttribute;

QApplication::processEvents();

    // where possible - avoid setters
    // some derived classes override setters and map to other objects 
    stringName      = pdomElemThis->attribute( "Name" ); setObjectName( stringName );
    stringCode      = pdomElemThis->attribute( "Code" );
    stringComment   = pdomElemThis->attribute( "Comment" );

    stringAttribute = pdomElemThis->attribute( "NextOID" );
    Q_ASSERT( !stringAttribute.isNull() );
    nNextOID = stringAttribute.toInt();

    if ( g_Transaction->canPaste() )
    {
        // PASTE LOAD
        // - we will have a special attribute called 'Path' which will contain the, original, full path for this object
        // - we use that to resolve references in doPostLoad
        // - we DO NOT want to replace the temp OID set when object created 
        QString stringPath = pdomElemThis->attribute( "Path" );
        g_Transaction->mapXRef[stringPath] = getPathString( g_Transaction->pRoot ); 
    }
    else
    {
        // NORMAL LOAD (ie from file)
        // - replace the temp OID with the one being loaded
        // - the loaded OID will be used during doPostLoad to resolve references
        stringAttribute = pdomElemThis->attribute( "OID" );
        Q_ASSERT( !stringAttribute.isNull() );
        nOID = stringAttribute.toInt();
    }

    // process child nodes...
    bool        bErrors     = false;
    QDomNode    domNode     = pdomElemThis->firstChild();
    int         nRef        = 0;

    // process all nodes
    while ( !domNode.isNull() )
    {
        if ( domNode.nodeType() != QDomNode::ElementNode )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        QDomElement domElem = domNode.toElement();
        if ( domElem.isNull() )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Class" )
        { 
            if ( !doLoadClass( &domElem ) ) bErrors = true;
        }
        else if ( domElem.tagName() == "Reference" )
        { 
            if ( !doLoadReference( &nRef, &domElem ) ) bErrors = true;
        }

        domNode = domNode.nextSibling();
    }

    return bErrors;
}

// create objects as needed and store reference into property for doPostLoad
bool ADObject::doLoadReference( int *pnRef, QDomElement *pdomElemReference )
{
    QString stringPath = pdomElemReference->attribute( "Path" );
    QString stringName = pdomElemReference->attribute( "Name" );
    QString stringReferencedObject  = pdomElemReference->attribute( "ReferencedObject" );

    if ( stringPath.isEmpty() )
    { 
        doMessage( "ERROR", tr("ADObject: Path needed to create reference. ") );
        return false;
    }

    // Just references?
    if ( !g_Transaction->canPaste() || stringReferencedObject == "ReferenceOnly" )
    {
        // just store away ref for doPostLoad()
        // - in this case; stringPath is path from root to referenced object - in source
        QString stringPropertyName = "Reference" + QString::number( *pnRef );           
        QString stringReference = stringPath + ':' + stringName;
        setProperty( stringPropertyName.toLatin1().constData(), stringReference );
        (*pnRef)++;                                                                  

        return true; 
    }

    // Is paste (and we want to create object) so...
    // - create references here, instead of doPoastLoad, because we can
    // - key thing here is that derived classes implement an intelligent version of getParentPaste()

    // Create a new object (leave empty)...
    if ( stringReferencedObject == "New" )
    { 
        QString stringClass = pdomElemReference->attribute( "Class" );
        if ( !stringClass.isEmpty() )
        {
            // get parent (object factory) for this class from derived classes
            ADObject *pParent = getParentPaste( stringClass );
            if ( !pParent ) return false;
            // get parent to create a new object of stringClass
            ADObject *pObject = nullptr;
            if ( pParent->getClasses().contains( stringClass ) ) pObject = pParent->getObject( stringClass );
            if ( !pObject ) return false;
            // we do not have to wait for doPostLoad in this case
            ADObject::doConnect( pObject, stringName );
            return true;
        }
    }

    // Create a new object and load it...
    // - there should be just one child (no need to loop)
    // - the child can have references that lead to more descendents (this can repeat for n)
    if ( stringReferencedObject == "Copied" )
    { 
        QDomNode domNode = pdomElemReference->firstChild();
        if ( domNode.isNull() ) return false;
        if ( domNode.nodeType() != QDomNode::ElementNode ) return false;
        QDomElement domElemObject = domNode.toElement();
        if ( domElemObject.isNull() ) return false;
        if ( domElemObject.tagName() != "Class" ) return false;
        // get parent (object factory) for this class from derived classes
        QString stringClass = domElemObject.attribute( "ClassName" );
        ADObject *pParent = getParentPaste( stringClass );
        if ( !pParent ) return false;
        // get parent to create a new object of stringClass
        ADObject *pObject = nullptr;
        if ( pParent->getClasses().contains( stringClass ) ) pObject = pParent->getObject( stringClass );
        if ( !pObject ) return false;
        // we do not have to wait for doPostLoad in this case
        pObject->doLoad( &domElemObject );
        ADObject::doConnect( pObject, stringName );
        return true;
    }

    return false;
}

bool ADObject::doLoadClass( QDomElement *pdomElemClass )
{
    QString stringClass = pdomElemClass->attribute( "ClassName" );
    if ( stringClass.isEmpty() ) return false;

    ADObject *pObject = getObject( stringClass );
    if ( !pObject ) return false;

    return pObject->doLoad( pdomElemClass );
}

/*!
    doLoad
    
    Load ourself from the given ADObject.
*/    
bool ADObject::doLoad( ADObject *pobject )
{
    nOID            = pobject->nOID;
    nNextOID        = pobject->nNextOID;
    nNextOIDTemp    = pobject->nNextOIDTemp;
    stringCode      = pobject->stringCode;
    stringComment   = pobject->stringComment;

    // do last
    setName( pobject->getName() );

    return true;
}

/*!
    doSave
    
    Save ourself to the given ADObject.
*/    
bool ADObject::doSave( ADObject *pobject )
{
    pobject->nOID              = nOID;
    pobject->nNextOID          = nNextOID;
    pobject->nNextOIDTemp      = nNextOIDTemp;
    pobject->stringCode        = stringCode;
    pobject->stringComment     = stringComment;

    // do last
    pobject->setName( getName() );

    return true;
}

/*!
 * \brief Prepare the hierarchy for a save.
 *  
 *  This is called just before we save.
 *                                                                         
 *  Its main purpose is to switch temp nOID's to perist nOID's             
 *  as required. It is expected that each ADObject will need a             
 *  persist nOID.                                                 
 *                                                                         
 *  By default; this will do all children in the hierarchy from where      
 *  it is initially called. So it normally just gets called at the root.   
 *  This single call will process everything.                              
 *                                                                         
 *  Any class that decides to override this method should be aware of this.
 *  
 * \author pharvey (11/15/19)
 */
void ADObject::doPreSave()
{
    // do we have a temp OID?
    if ( getOID() < 1 )
    {
        // do we have a parent?
        if ( parent() )
        {
            // get a normal OID from our parent
            ADObject *pobjectParent = (ADObject *)parent();
            setOID( pobjectParent->getNextOID() );
        }
        else
        {
            // do something when we are root?
        }
    }

    // allow our children the same opportunity
    QList<ADObject*> listChildren = getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {
        pChild->doPreSave();
    }
}

/*!
 * \brief Do post load stuff such as reconnecting references. 
 *  
 * This should get called as the last step in a (ie file) load.
 *  
 * By default; this will do all children in the hierarchy from where      
 * it is initially called. So it normally just gets called at the root.   
 * This single call will process everything.                              
 *                                                                        
 * Any class that decides to override this method should be aware of this.
 *  
 * \author pharvey (12/22/19)
 * 
 * \param pRoot 
 */
void ADObject::doPostLoad()
{
    // do children first
    QList<ADObject*> listChildren = getObjects();
    ADObject *pChild;

    foreach( pChild, listChildren )
    {
        pChild->doPostLoad();
    }

    // references
    // - doLoad has put our reference info in our object property's - lets get them
    int nRef = 0;
    do
    {
        // get reference info
        QString stringPropertyName = "Reference" + QString::number( nRef );
        QString stringReference = property( stringPropertyName.toLatin1().constData() ).toString();

        // first call that comes up empty means we are done - no more references
        if ( stringReference.isEmpty() )
        {
            break;
        }
        else
        {
            // seperate path and name
            QStringList l               = stringReference.split( ':' );
            QString stringPath          = l.first();
            QString stringName          = ( l.count() > 1 ? l.at(1) : QString() );
            if ( stringName.isEmpty() ) stringName = QString(); // ensure we are null - not just empty

            // we need to xref path (source-path to destination-path) when doing a paste
            if ( g_Transaction->canPaste() )
            {
               stringPath = g_Transaction->mapXRef[stringPath];
            }

            ADObject *pObject = g_Transaction->pRoot->getObject( stringPath.split( ',' ) );

            if ( !pObject )
            {
                doMessage( "DEBUG", QString(tr(" %1 %2 %3 Failed to resolve reference: %4")).arg( __FILE__ ).arg( __FUNCTION__ ).arg( __LINE__ ).arg( stringReference ) );
            }
            else
            {
                // connect
                // - validation etc that may have been added by derived classes should probably be turned off during a load
               ADObject::doConnect( pObject, stringName );

                // get rid of property (by passing null)
                setProperty( stringPropertyName.toLatin1().constData(), QVariant() );
            }
        }
        nRef++;
    }
    while ( 1 );

    // fake modified status change to init some connected objects (ie tree widget item)
    emit signalModified();
}

/*!
 * \brief Catch goto to set some sort of focus.
 *  
 * Drills down to desired object using path. Derived classes can replace   
 * this method so as to properly make themself 'current'. This may mean   
 * currently selected Line in an editor or it may mean currently selected 
 * object in a diagram.                                                   
 *                                                                        
 * Default behaviour is to drill down but do nothing.                     
 *  
 * This works the same as \sa doGoToView but with a slightly different result. 
 *  
 * Not to be confused with GoTo used by object browser (introduced later). 
 *  
 * \author pharvey (4/27/20)
 * 
 * \param listObjectPathItems 
 * \param nDepth 
 */
void ADObject::doGoTo( ADObjectPathList listObjectPathItems, uint nDepth )
{
    // are we the object at the end of the path?
    if ( (int)nDepth >= listObjectPathItems.count() )
    {
        // this means we are the desired source so we could set focus
        // to ourself here
        return;
    }

    // continue down path
    ADObject *          pobject          = 0;
    ADObjectPathItem   objectpathitem  = listObjectPathItems[nDepth];

    switch ( objectpathitem.nValueType )
    {
        case ADObjectPathItem::ValueSpecial:
        case ADObjectPathItem::ValueIndex:
            {
                QList<ADObject*> listChildren = getObjects( QString(), QString(), QString(), objectpathitem.nValue );
                pobject = listChildren.first();
                if ( !pobject || pobject->metaObject()->className() != objectpathitem.stringClassName )
                {
                    doMessage( "INFO", tr( "Object could not be fully resolved using path (Special/Index).\nPerhaps due to recent edits." ) );
                    return;
                }
            }
            break;
        case ADObjectPathItem::ValuePointer:
            {
                pobject = (ADObject*)objectpathitem.pobject;

                // Verify pointer...
                //
                // NOTE: we do not use objectpathitem.stringObjectName on purpose :)
                if ( !getObject( pobject, QString(), objectpathitem.stringClassName, QString(), false ) )
                {
                    doMessage( "INFO", tr( "Object could not be fully resolved using path (Pointer).\nPerhaps due to recent edits." ) );
                    return;
                }
            }
            break;
        case ADObjectPathItem::ValueOID:
            {
                pobject = getObject( objectpathitem.nValue );
                if ( !pobject )
                {
                    doMessage( "INFO", tr( "Object could not be fully resolved using path (OID).\nPerhaps due to recent edits." ) );
                    return;
                }
            }
    }

    pobject->doGoTo( listObjectPathItems, nDepth + 1 );
}

/*!
 * \brief Catch goto to set some sort of focus and invoke a view (if this makes sense).
 *  
 * Drills down to desired object using path. Derived classes can replace         
 * this method so as to properly make themself 'current' in view AND optionally 
 * create a view. A View may mean a properties dialog for the ADObject.         
 *                                                                              
 * Default behaviour is to drill down but do nothing.                           
 *  
 * This works the same as \sa doGoTo but with a slightly different result. 
 *  
 * Not to be confused with GoTo used by object browser (introduced later). 
 *  
 * \author pharvey (4/27/20)
 * 
 * \param listObjectPathItems 
 * \param nDepth 
 */
void ADObject::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    // are we the object at the end of the path?
    if ( (int)nDepth >= listObjectPathItems.count() )
    {
        // this means we are the desired source so we could bring up an
        // editor or properties dialog here
        return;
    }

    // continue down path
    ADObject *          pobject          = 0;
    ADObjectPathItem   objectpathitem  = listObjectPathItems[nDepth];

    switch ( objectpathitem.nValueType )
    {
        case ADObjectPathItem::ValueSpecial:
        case ADObjectPathItem::ValueIndex:
            {
                QList<ADObject*> listChildren = getObjects( QString(), QString(), QString(), objectpathitem.nValue );
                pobject = listChildren.first();
                if ( !pobject )
                    return;
            }
            break;
        case ADObjectPathItem::ValuePointer:
            {
                pobject = (ADObject*)objectpathitem.pobject;

                // Verify pointer...
                //
                // NOTE: we do not use objectpathitem.stringObjectName on purpose :)
                if ( !getObject( pobject, QString(), objectpathitem.stringClassName, QString(), false ) )
                    return;
            }
            break;
        case ADObjectPathItem::ValueOID:
            {
                pobject = getObject( objectpathitem.nValue );
                if ( !pobject )
                    return;
            }
    }

    pobject->doGoToView( listObjectPathItems, nDepth + 1 );
}

void ADObject::doClear() 
{
    // delete our children - giving them a chance to doClear()
    QList<ADObject*> listChildren = getObjects();
    ADObject *p;
    foreach( p, listChildren )
    {
        slotDelete( p );
    }

    // re-init our vars without sending signals
    QString s;
    QObject::setObjectName( s );
    stringName = s;
    stringCode = s;
    stringComment = s;
    bModified = false;
    
    // In some derived classes - this should have been done using setSelected( false ) BEFORE getting here
    // as this sends a signal which can be used to untangle 'this' from a UI.
    bSelected = false;
}

/*!
 * \brief Delete all of the child objects which match the given criteria.
 * 
 * \author pharvey (5/24/20)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 */
void ADObject::doObjectDelete( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    const QObjectList listChildren = children();
    QListIterator<QObject*> iteratorChildren( listChildren );
    while ( iteratorChildren.hasNext() )
    {
        QObject *p = iteratorChildren.next();
        if ( !p->inherits( "ADObject" ) )
            continue;
        ADObject *pobject = (ADObject*)p;
        if ( !stringName.isNull() && pobject->getName() != stringName )
            continue;
        if ( !stringClassName.isNull() && pobject->metaObject()->className() != stringClassName )
            continue;
        if ( !stringInheritsClass.isNull() && !pobject->inherits( stringInheritsClass.toLatin1().constData() ) )
            continue;

        // it is critical that we delete via the following call
        slotDelete( pobject );

    } // while
}

/*!
 * \brief Make this the first object.
 * 
 * We accomplish this by reparenting objects in the desired order.
 *  
 * \author pharvey (5/24/20)
 */
void ADObject::doToBack()
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();

    // Remove all children from the list.
    QObject *pObject;
    foreach( pObject, listChildren )
        pObject->setParent( 0 );

    // add desired child first
    setParent( pParent );

    // now add all else
    foreach( pObject, listChildren )
    {
       if ( pObject != this ) pObject->setParent( pParent );
    }
}

/*!
 * \brief Move the objects in the list to back.
 * 
 * \author pharvey (5/24/20)
 */
void ADObject::doToBack( QList<ADObject*> listSelected )
{
    ADObject *p;
    foreach( p, listSelected )
    {
        p->doToBack();
    }
}

/*!
 * \brief Make this the last object.
 *  
 * We accomplish this by reparenting objects in the desired order.
 *  
 * \author pharvey (5/24/20)
 */
void ADObject::doToFront()
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    setParent( 0 );         
    setParent( pParent );
}

/*!
 * \brief Move the objects in the list to the front.
 *  
 * Used by DDiagram. 
 *  
 * \author pharvey (5/24/20)
 */
void ADObject::doToFront( QList<ADObject*> listSelected )
{
    ADObject *p;
    foreach( p, listSelected )
    {
        p->doToFront();
    }
}

/*!
 * \brief Moves this to a place just before the previous object that matches the criteria.
 *  
 * Scans the children (of this) from back-to-front until it finds the object to be moved (other objects are moved to temp list). 
 * Removes the object to be moved. 
 * Continues scanning until it finds an object that matches the criteria (does nothing with other objects). 
 * Moves the object to the temp list. 
 * Appends the object to be moved. 
 * Appends all objects in the temp list (in reverse order). 
 *  
 * Used by AWDataTable. 
 *  
 * \author pharvey (5/25/20)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 */
void ADObject::doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();
    if ( listChildren.size() < 2 ) return;

    // scan in reverse
    // scan to find pobjectChild then
    // scan to find filter match then
    // remove all objects after match (saving in listToMove) 
    QObjectList listToMove;
    const int nStateFindModel = 0;
    const int nStateFindFilter = 1;
    const int nStateMove = 2;
    int nState = nStateFindModel; 

    QListIterator<QObject*> i( listChildren );
    i.toBack();
    while ( i.hasPrevious() )
    {
        QObject *pObject = i.previous();
        switch ( nState )
        {
            case nStateFindModel:
            {
                // is this the object we want to move?
                if ( pObject == this )
                {
                    nState = nStateFindFilter;
                }
                else
                {
                    // remove object and add to move list
                    pObject->setParent( 0 );
                    listToMove.append( pObject );
                }
                break;
            }
            case nStateFindFilter:
            {
                // does the object match?
                if ( !pObject->inherits( "ADObject" ) ) break;
                ADObject *pobject = (ADObject*)pObject;
                if ( !stringName.isNull() && pobject->getName() != stringName ) break;
                if ( !stringClassName.isNull() && pobject->metaObject()->className() != stringClassName ) break;
                if ( !stringInheritsClass.isNull() && !pobject->inherits( stringInheritsClass.toLatin1().constData() ) ) break;
                // yes - object matchs...
                // this is the previous object that matchs so no need to scan further
                // remove object, add to move list, and change state (we are done here)
                pObject->setParent( 0 );
                listToMove.append( pObject );
                nState = nStateMove;
                break;
            }
        }

        // no need to scan further
        if ( nState == nStateMove ) break;
    }

    // no match so do nothing
    if ( listToMove.count() < 1 ) return;

    // append self
    setParent( 0 );
    setParent( pParent );

    // append others (reverse order)                
    QListIterator<QObject*> i2( listToMove );
    i2.toBack();                               
    while ( i2.hasPrevious() )                 
    {                                          
        QObject *pObject = i2.previous();      
        pObject->setParent( pParent );               
    }                                          
}

/*!
 * \brief Moves this to a place just after the next object that matches the criteria.
 *  
 * Used by AWDataTable. 
 *  
 * \author pharvey (5/25/20)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 */
void ADObject::doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();
    if ( listChildren.size() < 2 ) return;

    // scan to find pobjectChild then
    // scan to find filter match then
    // remove all objects after match (saving in listToMove) 
    QObjectList listToMove;
    const int nStateFindModel = 0;
    const int nStateFindFilter = 1;
    const int nStateMove = 2;
    int nState = nStateFindModel; 

    QObject *pObject;
    foreach( pObject, listChildren )
    {
        switch ( nState )
        {
            case nStateFindModel:
            {
                if ( pObject == this )
                {
                    nState = nStateFindFilter;
                }
                break;
            }
            case nStateFindFilter:
            {
                if ( !pObject->inherits( "ADObject" ) ) break;
                ADObject *pobject = (ADObject*)pObject;
                if ( !stringName.isNull() && pobject->getName() != stringName ) break;
                if ( !stringClassName.isNull() && pobject->metaObject()->className() != stringClassName ) break;
                if ( !stringInheritsClass.isNull() && !pobject->inherits( stringInheritsClass.toLatin1().constData() ) ) break;
                nState = nStateMove;
                break;
            }
            case nStateMove:
            default:
            {
                pObject->setParent( 0 );
                listToMove.append( pObject );
            }
        }
    }

    // no match so do nothing
    if ( listToMove.count() < 1 ) return;

    // append self
    setParent( 0 );
    setParent( pParent );

    // append others
    foreach( pObject, listToMove )
    {
        pObject->setParent( pParent );
    }
}

void ADObject::doToBefore( ADObject *pObject )
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    // sanity checks
    Q_ASSERT( pObject );
    Q_ASSERT( pObject != this );
    Q_ASSERT( pObject->parent() == pParent );

    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();
    if ( listChildren.size() < 2 ) return;

    // scan to find pObject (exit if we find self first) then
    // scan to find self 
    QObjectList listToMove;
    const int nStateFindObject = 0;
    const int nStateFindSelf = 1;
    const int nStateMove = 2;
    int nState = nStateFindObject; 

    QObject *p;
    foreach( p, listChildren )
    {
        switch ( nState )
        {
            case nStateFindObject:
            {
                // self is already before pObject if we find self first
                if ( p == this ) return;

                if ( p == pObject )
                {
                    p->setParent( 0 );
                    listToMove.append( p );
                    nState = nStateFindSelf;
                }
                break;
            }
            case nStateFindSelf:
            {
                if ( p == this )
                {
                    p->setParent( 0 );
                    listToMove.insert( 0, p );
                    nState = nStateMove;
                }
                break;
            }
            case nStateMove:
            default:
            {
                p->setParent( 0 );
                listToMove.append( p );
            }
        }
    }

    // parent them all back in the new order
    foreach( p, listToMove )
    {
        p->setParent( pParent );
    }
}

/*!
 * \brief Move self after (on top of) another object.
 *  
 * Will do nothing if self is already at some position after (on top of) pObject. 
 *  
 * Used by DLayout. 
 *  
 * \author pharvey (8/10/20)
 * 
 * \param pObject 
 */
void ADObject::doToAfter( ADObject *pObject )
{
    QObject *pParent = parent();
    if ( !pParent ) return;

    // sanity checks
    Q_ASSERT( pObject );
    Q_ASSERT( pObject != this );
    Q_ASSERT( pObject->parent() == pParent );

    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();
    if ( listChildren.size() < 2 ) return;

    // scan to find pObject (exit if we find self first) then
    // scan to find self 
    QObjectList listToMove;
    const int nStateFindSelf = 0;
    const int nStateFindObject = 1;
    const int nStateMove = 2;
    int nState = nStateFindSelf; 

    QObject *p;
    foreach( p, listChildren )
    {
        switch ( nState )
        {
            case nStateFindSelf:
            {
                // self is already after pObject if we find pObject first
                if ( p == pObject ) return;

                if ( p == this )
                {
                    p->setParent( 0 );
                    nState = nStateFindObject;
                }
                break;
            }
            case nStateFindObject:
            {
                if ( p == pObject ) nState = nStateMove;
                break;
            }
            case nStateMove:
            default:
            {
                p->setParent( 0 );
                listToMove.append( p );
            }
        }
    }

    // append self
    setParent( pParent );

    // append others
    foreach( p, listToMove )
    {
        p->setParent( pParent );
    }
}

/*!
 * \brief Moves self to the given index position among its siblings. 
 *  
 * The index can be out of range on the high end - just means self becomes last sibling - 
 * but an index less than 0 will cause an assert.
 * 
 * \author pharvey (9/24/20)
 * 
 * \param nIndex 
 */
void ADObject::doToIndex( int nIndex )  
{                              
    Q_ASSERT( nIndex >= 0 );        
    QObject *pParent = parent();
    if ( !pParent ) return;
                                          
    // Get a read-only list - because that is all we can get.
    QObjectList listChildren = pParent->children();

    // out of range but we can deal with this - just make self last sibling
    if ( nIndex >= listChildren.count() )
    {
        setParent( 0 );                                     
        setParent( pParent );                                     
        return;
    }
                                                                                 
    // Remove all children from the list.                                        
    QObject *pObject;                                                            
    foreach( pObject, listChildren )                                             
    {                                                                            
        pObject->setParent( 0 );                                                 
    }                                                                            
                                                                                 
    // Add the children back in the order we want.                                                    
    int nCount = 0;                                                              
    foreach( pObject, listChildren )                                             
    {                                                                            
        // add self when at nIndex                                    
        if ( nCount == nIndex )                                               
        {                                                                        
            setParent( pParent );                                     
            nCount++;                                                            
        }                                                                        
        // add everything other than self                                
        if ( pObject != this )                                           
        {                                                                        
            pObject->setParent( pParent );                                          
            nCount++;                                                            
        }                                                                        
    }                                                                            
}                                                                                

/*!
 * \brief Do validation on us and our children. 
 *  
 * Derived classes may; use this method, augment this method or replace this method. The 
 * most common scenario is to replace it. 
 * 
 * \author pharvey (11/28/19)
 * 
 * \param nStrictness The degree of strictness we want to validate with.
 * \param nValueType The type to use for adding to teh path.
 * \param listObjectPathItems The path to the object. Used to navigate back to see and/or fix the problem.
 */
void ADObject::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                         
    stringValidation = tr("Name missing");                                                                                     
    nSeverity = getValidationSeverity( "ADObject", nStrictness, stringValidation );                     
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                           
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                            

    // Code...                                                                                                                         
    stringValidation = tr("Code missing");                                                                                     
    nSeverity = getValidationSeverity( "ADObject", nStrictness, stringValidation );                     
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                           
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                            

    // Comment...                                                                                                                      
    stringValidation = tr("Comment missing");                                                                                  
    nSeverity = getValidationSeverity( "ADObject", nStrictness, stringValidation );                     
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                           
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                            
                                                                                                                                       
    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           
        
    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      
                                                                                                                                       
        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

/*! 
 * \brief Reports matchs in self and then drills down to check for matchs in any children.  
 *   
 * Derived classes will want to replace if the derived class has additional properties
 * to check.
 * 
 * listNames and listValues are used to build a path to the source. Typically listNames
 * will contain the class name while listValues will contain an index to the
 * object - but this does not have to be the case. Derived classes do what ever makes 
 * sense to them. For example a text editor may put { "Line", n } and { "Column", n } in
 * these lists. 
 *  
 * Exists to supports \sa slotFind. 
 *   
 * \note This method used to put an object pointer in listValues but has been switched to 
 *       an index value because index value works when a model was only loaded
 *       for the search and is subsequently discarded (along with its pointer values).
 */
void ADObject::doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{
    // check ourself for a match
    if ( Criteria.bName && getName().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, QPixmap(), getName() );
    if ( Criteria.bCode && getCode().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, QPixmap(), getCode() );
    if ( Criteria.bComment && getComment().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, QPixmap(), getComment() );

    // check our children
    QList<ADObject*>            listChildren = getObjects( QString(), QString(), QString() );
    ADObject *                  pChild;
    int                         nIndex = -1;
    foreach( pChild, listChildren )
    {
        nIndex++;

        // append child to path (always provide pointer AND index because we happen to have both anyway)
        listObjectPathItems += ADObjectPathItem( nValueType, pChild, pChild->metaObject()->className(), pChild->getName(), nIndex );
        // do find
        pChild->doFind( nValueType, listObjectPathItems, Criteria );
        // remove child from path
        listObjectPathItems.pop_back();
    }
}

/*!
    This will list the current object hierarchy of ADObject's. Useful when debugging.
*/    
void ADObject::doDump( int nLevel )
{
    qDebug( "%*s %s %p", nLevel, "", metaObject()->className(), this );

    QList<ADObject*> listChildren = getObjects( QString(), QString(), QString() );
    QListIterator<ADObject*> iteratorChildren( listChildren );

    nLevel++;
    while ( iteratorChildren.hasNext() )
    {
        ADObject *pobject = iteratorChildren.next();
        pobject->doDump( nLevel );
    }
}

void ADObject::doMessageBox( const QString &stringSeverity, const QString &stringCaption, const QString &stringMessage )
{
    Q_UNUSED(stringCaption);
    doMessage( stringSeverity, stringMessage );
}

void ADObject::doMessage( const QString &stringSeverity, const QString &stringMessage )
{
    // may want to have an option to send "DEBUG" to console AND via signalMessageGeneral for support reasons
    // for now - "DEBUG" stuff goes to console only
    qDebug() << stringSeverity << " " << stringMessage;
    if ( stringSeverity == "DEBUG" ) return;
    eventOutputGeneral( stringSeverity, stringMessage );
}

/*!
 * \brief Connect (create a reference) to another object. 
 *  
 * The connection is two-way but is 'owned' by this object NOT pObject. 
 * Derived classes may override this method and, under certain circumstances, refuse. Hence; the bool return value. 
 * The lower level methods (used in here) can be used directly to guarantee a connection but consider them internal.
 *  
 * An entry will be created in this::Uses and pObject::UsedBy. 
 *  
 * The name is optional. It can be used to make a connection unique when an object is making 
 * more than one connection to the same object. A line may set it to "Begin" or "End" for example. 
 *  
 * Name can be null or empty but; object-pointer + name must be unique within the object. 
 *  
 * \author pharvey (12/17/19)
 * 
 * \param pObject 
 * \param stringName 
 * 
 * \return bool 
 */
bool ADObject::doConnect( ADObject *pObject, const QString &stringName )
{
    // sanity checks
    if ( !pObject ) return false;
    if ( isConnected( pObject, stringName ) ) return false;

    // connect
    doUsesAdd( pObject, stringName );
    pObject->doUsedByAdd( this, stringName );

    // let anyone who cares - know
    emit signalConnected( this, pObject, stringName );

   return true;
}

/*!
 * \brief Disconnect (remove a reference) to any that match the given filter.
 * 
 * Derived classes may override this method and, under certain circumstances, refuse. Hence; the bool return value. 
 * The lower level methods (used in here) can be used directly to guarantee a connection but consider them internal.
 *  
 * All filter values will need to match unless the filter value is null. 
 * A filter value that is null will be ignored. 
 *  
 * \author pharvey (12/17/19)
 * 
 * \param pObject Filter. The other object must be.
 * \param stringName Filter. The connection name (not the object name) must match.
 * \param stringClassName Filter. Object must be class.
 * \param stringInheritsClass Filter. Object must inherit. 
 * 
 * \return bool 
 */
bool ADObject::doDisconnect( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        // disconnect
        Key.pObject->doUsedByRemove( this, Key.stringName );
        doUsesRemove( Key.pObject, Key.stringName );
        // let anyone who cares - know
        emit signalDisconnected( this, Key.pObject, Key.stringName );
    }

    return true;
}

/*!
 * \brief Checks to see if we have any connections/references. 
 *  
 * This does NOT check connections/references made to us.... just the ones 
 * we have created to others using doConnect(). 
 *  
 * This is like getUsesCount but faster as we short out as soon as we find a match. 
 *  
 * All filter values will need to match unless the filter value is null. 
 * A filter value that is null will be ignored. 
 *  
 * \author pharvey (12/18/19)
 * 
 * \param pObject Filter. The other object must be.
 * \param stringName Filter. The connection name (not the object name) must match.
 * \param stringClassName Filter. Object must be class.
 * \param stringInheritsClass Filter. Object must inherit. 
 * 
 * \return bool True if a match found.
 */
bool ADObject::isConnected( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    if ( !pObject && stringName.isNull() && stringClassName.isEmpty() && stringInheritsClass.isEmpty() )
        return mapUses.count();

    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        return true;
    }

    return false;
}

/*!
 * \brief Get first object matching filter.
 *  
 * This will usually get called when it is known that the filter will produce 0-1 matchs. 
 *  
 * \author pharvey (12/18/19)
 * 
 * \param stringName 
 * \param stringClassName 
 * \param stringInheritsClass 
 * 
 * \return ADObject* 
 */
ADObject *ADObject::getConnected( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        return Key.pObject;
    }

    return nullptr;
}

int ADObject::getUsedByCount( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    if ( !pObject && stringName.isNull() && stringClassName.isEmpty() && stringInheritsClass.isEmpty() )
        return mapUsedBy.count();

    QList<ADObjectReferenceKey> listKeys = mapUsedBy.keys();
    ADObjectReferenceKey Key;
    int n = 0;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        n++;
    }

    return n;
}

int ADObject::getUsesCount( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{ 
    if ( !pObject && stringName.isNull() && stringClassName.isEmpty() && stringInheritsClass.isEmpty() )
        return mapUses.count();

    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    int n = 0;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        n++;
    }

    return n;
}

QList<ADObjectReferenceKey> ADObject::getUsedByKeys( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{ 
    if ( !pObject && stringName.isNull() && stringClassName.isEmpty() && stringInheritsClass.isEmpty() )
        return mapUsedBy.keys();

    QList<ADObjectReferenceKey> listResult;
    QList<ADObjectReferenceKey> listKeys = mapUsedBy.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        listResult.append( Key );
    }

    return listResult;
}

QList<ADObjectReferenceKey> ADObject::getUsesKeys( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{ 
    if ( !pObject && stringName.isNull() && stringClassName.isEmpty() && stringInheritsClass.isEmpty() )
        return mapUses.keys();

    QList<ADObjectReferenceKey> listResult;
    QList<ADObjectReferenceKey> listKeys = mapUses.keys();
    ADObjectReferenceKey Key;
    foreach( Key, listKeys )
    {
        if ( pObject && pObject != Key.pObject ) continue;
        if ( !stringName.isNull() && stringName != Key.stringName ) continue;
        if ( !stringClassName.isEmpty() && stringClassName != Key.pObject->metaObject()->className() ) continue;
        if ( !stringInheritsClass.isEmpty() && !Key.pObject->inherits( stringInheritsClass.toLatin1().constData() ) ) continue;
        // match
        listResult.append( Key );
    }

    return listResult;
}

/*!
 * \brief Start find at this point in object hierarchy.
 *  
 * We always start with an object pointer reference then recommend ValueIndex from 
 * there. This is because; 
 *  
 * 1. We have no idea of our index value at this point. 
 * 2. App can assume that the object in the first path item is root for path and 
 *    route GoTo operation accordingly (alt. is to connect single designated root directly
 *    with signal/slot).
 *  
 * \author pharvey (1/3/20)
 * 
 * \param stringText 
 */
void ADObject::slotFind( const ADFindCriteria &Criteria )
{
    // these should be connected to the output widget
    eventOutputRequest( "Search", ADEventRequest::RequestClear );
    eventOutputRequest( "Search", ADEventRequest::RequestFocus );

    // Create lists to store path to message source
    ADObjectPathList listObjectPathItems;

    // add self as entry point
    listObjectPathItems += ADObjectPathItem( ADObjectPathItem::ValuePointer, this, metaObject()->className(), getName() ); 

    // do it
    doFind( ADObjectPathItem::ValueIndex, listObjectPathItems, Criteria );
}

/*! 
 * \brief Start validation at this point in object hierarchy.
 *   
 * We always start with an object pointer reference then recommend ValueIndex from 
 * there. This is because we have no idea of our index value at this point.
 * 
 * There is probably no reason to override this method. However; derived classes
 * may want to override \sa doValidate - to do any kind of special validation.
 *  
 * The ADValidation::Severity can be configured for each ADValidation::Strictness.
 * This is used, via \sa getValidationSeverity, during validation. 
 *  
 * \author pharvey
 * 
 */
void ADObject::slotValidate( ADValidation::Strictness n )
{
    // these should be connected to the output widget
    eventOutputRequest( "Validation", ADEventRequest::RequestClear );
    eventOutputRequest( "Validation", ADEventRequest::RequestFocus );

    // Create lists to store path to message source
    ADObjectPathList listObjectPathItems;

    // add self as entry point
    listObjectPathItems += ADObjectPathItem( ADObjectPathItem::ValuePointer, this, metaObject()->className(), getName() ); 

    // do it
    doValidate( n, ADObjectPathItem::ValueIndex, listObjectPathItems );
}

/*!
    This is the typical way to delete a ADObject but an ADObject does not actually
    delete itself. Instead we emit a request to our parent (or application) to 
    remove us from its children list - thereby deleting us.
*/    
void ADObject::slotDelete()
{
    // request parent delete us
    emit signalDeleteRequest( this );
}

/*!
    This is almost certianly called because a child ADObject (pobject) wants to 
    be deleted. see slotDelete().
    
    Replace this in derived class if you want to verify for Save first. In this
    the request may be cancelled and false returned.
*/
bool ADObject::slotDelete( ADObject *pChild )
{
    // default is to tell the 'world' we are going to delete pChild
    emit signalDeleted( pChild );
    emit signalDeleted( this, pChild );
    // and then actually do it
    pChild->doClear();
    delete pChild;
    //
    setModified();

    return true;
}

/*!
 * \brief Drill down through path. Set focus as we go. 
 *  
 * This is used when the object in question may not be loaded/instantiated. 
 * An example of this is when a search finds an object in a file that is not loaded.  
 *  
 * Not to be confused with GoTo used by object browser (introduced later). 
 *  
 * This method is only called in the object that produced the path. This is usually 
 * the root object or an object near root.
 *  
 * There is probably no need to override this method. 
 *  
 * We call doGoTo for each object. Override doGoTo to implement code to set focus (when it makes sense). 
 *  
 * \author pharvey (1/3/20)
 * 
 * \param listObjectPathItems 
 */
void ADObject::slotGoTo( ADObjectPathList listObjectPathItems )
{
    if ( listObjectPathItems.count() < 1 ) return;

    ADObjectPathItem objectpathitem = listObjectPathItems[0];
    ADObject *pObject = (ADObject *)objectpathitem.pobject;

    // sanity checks
    if ( objectpathitem.nValueType != objectpathitem.ValuePointer )
    {
        doMessage( "ERROR", tr("First item in object-path not an object pointer") );
        return;
    }

    if ( !objectpathitem.pobject )
    {
        doMessage( "ERROR", tr("First item in object-path should not be a null") );
        return;
    }

    // Verify validity of pointer...
    // - Using objectpathitem.stringObjectName does NOT benefit us here.
    if ( pObject != this && !getObject( pObject, QString(), objectpathitem.stringClassName ) )
    {                                                                                                                           
        doMessage( "ERROR", tr("Could not find") + objectpathitem.stringClassName );                                            
        return;                                                                                                                 
    }                                                                                                                           

    // Pointer still appears valid so lets use it...
    pObject->doGoTo( listObjectPathItems, 1 );
}

/*!
 * \brief Drill down through path. Set focus as we go. Also invoke some form of editor (whatever may make sense).
 *  
 * This is used when the object in question may not be loaded/instantiated. 
 * An example of this is when a search finds an object in a file that is not loaded.  
 *  
 * Not to be confused with GoTo used by object browser (introduced later). 
 *  
 * This method is only called in the object that produced the path. This is usually 
 * the root object or an object near root.
 *  
 * There is probably no need to override this method. 
 *  
 * We call doGoToView for each object. Override doGoToView to implement code to set focus (when it makes sense). 
 *  
 * \author pharvey (1/3/20)
 * 
 * \param listObjectPathItems 
 */
void ADObject::slotGoToView( ADObjectPathList listObjectPathItems )
{
    if ( listObjectPathItems.count() < 1 ) return;

    ADObjectPathItem objectpathitem = listObjectPathItems[0];
    ADObject *pObject = (ADObject *)objectpathitem.pobject;

    // sanity checks
    if ( objectpathitem.nValueType != objectpathitem.ValuePointer )
    {
        doMessage( "ERROR", tr("First item in object-path not an object pointer") );
        return;
    }

    if ( !objectpathitem.pobject )
    {
        doMessage( "ERROR", tr("First item in object-path should not be a null") );
        return;
    }

    // Verify validity of pointer...
    // - Using objectpathitem.stringObjectName does NOT benefit us here.
    if ( pObject != this && !getObject( pObject, QString(), objectpathitem.stringClassName ) )
    {                                                                                                                           
        doMessage( "ERROR", tr("Could not find") + objectpathitem.stringClassName );                                            
        return;                                                                                                                 
    }                                                                                                                           

    // Pointer still appears valid so lets use it...
    pObject->doGoToView( listObjectPathItems, 1 );
}

/*!
    slotExportHTML
    
*/
void ADObject::slotExportHTML()
{
    ADExportHTML   exportHTML;
    QString         stringID;

    // Use pointer value as our ID
// #if QT_VERSION < 0x060000
//     stringID.sprintf( "%p", this );
// #else
    stringID.asprintf( "%p", this );
// #endif

    // get dir to place files
    exportHTML.stringDir = QFileDialog::getExistingDirectory( 0, "Dir", QString() );
    if ( exportHTML.stringDir.isEmpty() )
        return;

    // set header/footer for main pages
    exportHTML.stringDescHeader   = \
"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n" \
"<HTML>\n" \
"<HEAD>\n" \
"    <META http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\">\n" \
"    <META name=\"author\" content=\"CodeByDesign\">\n" \
"    <TITLE></TITLE>\n" \
"</HEAD>\n" \
"<BODY text=\"#000000\" bgcolor=\"#cccccc\" link=\"#0000ee\" vlink=\"#551a8b\" alink=\"#0000ee\">\n" \
"<TABLE border=\"0\" cellpadding=\"0\" cellspacing=\"3\" width=\"575\">\n" \
"    <TBODY>\n" \
"    <TR>\n" \
"        <TD valign=\"top\" width=\"20%\">\n" \
"            <TABLE border=\"0\" cellpadding=\"0\" cellspacing=\"3\" width=\"100%\">\n" \
"                <TBODY>\n" \
"                <TR>\n" \
"                    <TD bgcolor=\"#777777\">\n" \
"                        <TABLE border=\"0\" cellpadding=\"0\" cellspacing=\"2\" width=\"100%\" align=\"center\">\n" \
"                            <TBODY>\n" \
"                            <TR>\n" \
"                                <TD bgcolor=\"#ffffff\" colspan=\"1\" width=\"100%\">\n" \
"                                    <TABLE cellspacing=\"0\" width=\"100%\">\n" \
"                                        <TBODY>\n" \
"                                        <TR>\n" \
"                                            <TD valign=\"top\">\n";
    exportHTML.stringDescFooter   = \
"                                                </FONT>\n" \
"                                                <BR>\n"\
"                                            </TD>\n"\
"                                        </TR>\n"\
"                                        </TBODY>\n"\
"                                    </TABLE>\n"\
"                                </TD>\n"\
"                            </TR>\n"\
"                            </TBODY>\n"\
"                        </TABLE>\n"\
"                    </TD>\n"\
"                </TR>\n"\
"                </TBODY>\n"\
"            </TABLE>\n"\
"        </TD>\n"\
"    </TR>\n"\
"    </TBODY>\n"\
"</TABLE>\n"\
"<BR>\n"\
"<BR>\n"\
"<BR>\n"\
"<BR>\n"\
"<BR>\n"\
"</BODY>\n"\
"</HTML>\n";

    // create general purpose png files
    {
        QString stringFile;

        QPixmap pixmapPlus( ":AW/Plus" );
        stringFile = exportHTML.stringDir + "/Plus.png";
        if ( !pixmapPlus.save( stringFile, "PNG" ) )
        {
            doMessageBox( "ERROR", "Export...", "Failed to write " + stringFile );
            return;
        }

        QPixmap pixmapMinus( ":AW/Minus" );
        stringFile = exportHTML.stringDir + "/Minus.png";
        pixmapMinus.save( stringFile, "PNG" );

        QPixmap pixmapBlank16x16( ":AW/Blank" );
        stringFile = exportHTML.stringDir + "/Blank16x16.png";
        pixmapBlank16x16.save( stringFile, "PNG" );

        QPixmap pixmapCheckMark( ":AW/CheckMark" );
        stringFile = exportHTML.stringDir + "/CheckMark.png";
        pixmapCheckMark.save( stringFile, "PNG" );
    }

    // create index file
    QFile fileIndex( exportHTML.stringDir + "/index.html" );
    if ( fileIndex.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &fileIndex );
        stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "    <link rel=\"icon\" href=\"favicon.ico\" type=\"image/ico\" />\n";
        stream << "    <link rel=\"shortcut icon\" href=\"/favicon.ico\" type=\"image/ico\" />\n";
        stream << "    <meta NAME=\"keywords\" CONTENT=\"CodeByDesign\">\n";
        stream << "    <meta NAME=\"description\" CONTENT=\"Index page\">\n";
        stream << "    <title>" + getName() + "</title>\n";
        stream << "    <frameset COLS=\"143,*\" BORDER=\"3\"  FRAMEBORDER=\"YES\" FRAMESPACING=\"0\">\n";
        stream << "        <frame NAME=\"Browser\" SCROLLING=\"YES\" MARGINWIDTH=\"0\" MARGINHEIGHT=\"0\" SRC=\"Browser.html\">\n";
        stream << "        <frame NAME=\"Main\" SCROLLING=\"auto\" MARGINWIDTH=\"0\" MARGINHEIGHT=\"0\"  SRC=\"" + stringID + ".html\">\n";
        stream << "    </frameset>\n";
        stream << "</head>\n";
        stream << "<noframes>\n";
        stream << "<a HREF=\"http://www.netscape.com\">Please get a frames capable browser</a>\n";
        stream << "</noframes>\n";
        stream << "</html>\n";
        fileIndex.close();
    }
    else
    {
        doMessageBox( "ERROR", "Export...", "Failed to write " + fileIndex.fileName() );
        return;
    }
        
    // create browser header
    exportHTML.fileBrowser.setFileName( exportHTML.stringDir + "/Browser.html" );
    if ( exportHTML.fileBrowser.open( QIODevice::WriteOnly ) )
    {
        exportHTML.streamBrowser.setDevice( &exportHTML.fileBrowser );
        exportHTML.streamBrowser << "<HTML>\n";
        exportHTML.streamBrowser << "<HEAD>\n";
        exportHTML.streamBrowser << "    <TITLE>JavaScript Tree Control</TITLE>\n";
        exportHTML.streamBrowser << "    <STYLE>\n";
        exportHTML.streamBrowser << "    body{\n";
        exportHTML.streamBrowser << "        font: 10pt Verdana,sans-serif;\n";
        exportHTML.streamBrowser << "        color: navy;\n";
        exportHTML.streamBrowser << "    }\n";
        exportHTML.streamBrowser << "    .trigger{\n";
        exportHTML.streamBrowser << "        cursor: pointer;\n";
        exportHTML.streamBrowser << "        cursor: hand;\n";
        exportHTML.streamBrowser << "    }\n";
        exportHTML.streamBrowser << "    .branch{\n";
        exportHTML.streamBrowser << "        display: none;\n";
        exportHTML.streamBrowser << "        margin-left: 16px;\n";
        exportHTML.streamBrowser << "    }\n";
        exportHTML.streamBrowser << "    </STYLE>\n";
        exportHTML.streamBrowser << "    <SCRIPT language=\"JavaScript\">\n";
        exportHTML.streamBrowser << "    var openImg = new Image();\n";
        exportHTML.streamBrowser << "    openImg.src = \"Plus.png\";\n";
        exportHTML.streamBrowser << "    var closedImg = new Image();\n";
        exportHTML.streamBrowser << "    closedImg.src = \"Minus.png\";\n";
        exportHTML.streamBrowser << "    function showBranch(branch)\n";
        exportHTML.streamBrowser << "    {\n";
        exportHTML.streamBrowser << "        var objBranch = document.getElementById(branch).style;\n";
        exportHTML.streamBrowser << "        if (objBranch.display==\"block\")\n";
        exportHTML.streamBrowser << "            objBranch.display=\"none\";\n";
        exportHTML.streamBrowser << "        else\n";
        exportHTML.streamBrowser << "            objBranch.display=\"block\";\n";
        exportHTML.streamBrowser << "    }\n";
        exportHTML.streamBrowser << "    function swapFolder(img)\n";
        exportHTML.streamBrowser << "    {\n";
        exportHTML.streamBrowser << "        objImg = document.getElementById(img);\n";
        exportHTML.streamBrowser << "        if (objImg.src.indexOf('Minus.png')>-1)\n";
        exportHTML.streamBrowser << "            objImg.src = openImg.src;\n";
        exportHTML.streamBrowser << "        else\n";
        exportHTML.streamBrowser << "            objImg.src = closedImg.src;\n";
        exportHTML.streamBrowser << "    }\n";
        exportHTML.streamBrowser << "    </SCRIPT>\n";
        exportHTML.streamBrowser << "</HEAD>\n";
        exportHTML.streamBrowser << "<BODY>\n";
    }
    else
    {
        doMessageBox( "ERROR", "Export...", "Failed to write " + exportHTML.fileBrowser.fileName() );
        return;
    }
    
    // Start with ourself
    doExport( &exportHTML, true );

    // fini
    {
        // browser footer
        exportHTML.streamBrowser << "</BODY>\n";
        exportHTML.streamBrowser << "</HTML>\n";
        //
        exportHTML.fileBrowser.close();
        eventOutputGeneral( "INFO", "Export to HTML completed in " + exportHTML.stringDir );
    }
}

/*!
    slotExportRTF
    
*/    
void ADObject::slotExportRTF()
{
}

/*!
    doUsedByAdd

    Call here to tell this model that it is being used by another.    

    NOTE: Caller needs to ensure that pobject->doUsesAdd( this ) is called.
*/
void ADObject::doUsedByAdd( ADObject *p, const QString &s )
{
    // Translate null name to empty. Null is reserved to indicate 'do not match' in filter calls.
    ADObjectReferenceKey Key( p, ( s.isNull() ? "" : s ) );
    Q_ASSERT( !mapUsedBy.contains( Key ) );
    mapUsedBy.insert( Key, ADObjectReference() );
}

/*!
    doUsedByRemove

    Call here to tell this model that the given model(s) are no longer
    using this.
    
    NOTE: caller needs to ensure that pobject->doUsesRemove( this ) is called.
*/
void ADObject::doUsedByRemove( ADObject *p, const QString &s )
{
    // Translate null name to empty. Null is reserved to indicate 'do not match' in filter calls.
    ADObjectReferenceKey Key( p, ( s.isNull() ? "" : s ) );
    mapUsedBy.remove( Key );
}

/*!
    doUsesAdd

    This has been created so that derived classes can easily intercept 
    these requests (by replacing this method).    
    
    NOTE: Caller needs to ensure that pobject->doUsedByAdd( this ) is called.
*/
void ADObject::doUsesAdd( ADObject *p, const QString &s )
{
    // Translate null name to empty. Null is reserved to indicate 'do not match' in filter calls.
    ADObjectReferenceKey Key( p, ( s.isNull() ? "" : s ) );
    Q_ASSERT( !mapUses.contains( Key ) );
    mapUses.insert( Key, ADObjectReference() );
}

/*!
    doUsesRemove
    
    This has been created so that derived classes can easily intercept 
    these requests (by replacing this method).    
    
    NOTE: Caller needs to ensure that pobject->doUsedByRemove( this ) is called.
*/
void ADObject::doUsesRemove( ADObject *p, const QString &s )
{
    // Translate null name to empty. Null is reserved to indicate 'do not match' in filter calls.
    mapUses.remove( ADObjectReferenceKey( p, ( s.isNull() ? "" : s ) ) );
}

/*!
    getExportDescription
*/
QString ADObject::getExportDescription( ADExportHTML * )
{
    QString string;

    string = "<B>Comment</B>\n";
    string += "<BR><BR>\n";
    string += getComment() + "<BR>\n";

    return string;
}

/*!
    doExport

    Called from slotExportHTML().    
*/
void ADObject::doExport( ADExportHTML *pexport, bool bReportProgress )
{
    QString stringID;

    // Use pointer value as our ID
// #if QT_VERSION < 0x060000
//    stringID.sprintf( "%p", this );
// #else
    stringID.asprintf( "%p", this );
// #endif

    // create our icon
    {
        QFile file( pexport->stringDir + "/" + metaObject()->className() + ".png" );

        // if ( !file.exists() )
        //     getIcon().save( file.fileName(), "PNG" );
    }

    // get children
    QList<ADObject*> listChildren = getObjects( QString(), QString(), QString() );
    QListIterator<ADObject*> iteratorChildren( listChildren );

    if ( bReportProgress )
        pexport->pprogress = new QProgressDialog( "Export", QString(), 0, listChildren.count() );

    // folder
    if ( listChildren.count() > 0 )
    {
        pexport->streamBrowser << "<div class=\"trigger\" onclick=\"showBranch('branch" + stringID + "');swapFolder('folder" + stringID + "')\">\n";
        pexport->streamBrowser << "<A href=\"" + stringID + ".html\" TARGET=Main><img src=\"Plus.png\" border=\"0\" id=\"folder" + stringID + "\"><img src=\"" + metaObject()->className() + ".png\" border=0>" + getName() + "</A>\n";
        pexport->streamBrowser << "</div>\n";
        // children
        pexport->streamBrowser << "<span class=\"branch\" id=\"branch" << stringID << "\" style=\"display: none;\">\n";
        while ( iteratorChildren.hasNext() ) 
        {
            ADObject *pobject = iteratorChildren.next();
            if ( !pobject->inherits( "DGHandle" ) ) // skip DGHandle's
                pobject->doExport( pexport );

            // update progress
            if ( bReportProgress )
            {
                pexport->pprogress->setValue( pexport->pprogress->value() + 1 );
                qApp->processEvents();
                if ( pexport->pprogress->wasCanceled() )
                {
                    doMessageBox( "WARNING", "Export...", "Export halted before complete" );
                    return;
                }
            }

        }
        pexport->streamBrowser << "</span>\n";
    }
    // branch
    else
    {
        pexport->streamBrowser << "<A href=\"" + stringID + ".html\" TARGET=Main><img src=\"Blank16x16.png\" border=0><img src=\"" + metaObject()->className() + ".png\" border=0>" + getName() + "</A><br>\n";
    }

    // description
    QFile fileDesc( pexport->stringDir + "/" + stringID + ".html" );
    if ( fileDesc.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &fileDesc );
        stream << pexport->stringDescHeader;
        stream << "                                                <TABLE cellpadding=\"2\" cellspacing=\"2\" border=\"0\" width=\"100%\">\n";
        stream << "                                                    <TBODY>\n";
        stream << "                                                    <TR>\n";
        stream << "                                                        <TD valign=\"top\" bgcolor=\"#ce000d\"><FONT color=\"#ffffff\"><B><FONT face=\"Helvetica, Arial, sans-serif\">&nbsp;" + getName() + "</FONT></B></FONT><BR>\n";
        stream << "                                                        </TD>\n";
        stream << "                                                    </TR>\n";
        stream << "                                                    </TBODY>\n";
        stream << "                                                </TABLE>\n";
        stream << "                                                <FONT face=\"Helvetica, Arial, sans-serif\"><BR>\n";
        stream << getExportDescription( pexport ) + "<br>\n";
        stream << pexport->stringDescFooter;
        fileDesc.close();
    }
}

/*!
    doExport
*/    
void ADObject::doExport( ADExportRTF *, bool )
{
}

/*!
    getPathDiff
    
    Example;
    
    stringPathAbsStart      /home/pharvey
    stringPathAbsEnd        /home/pharvey/documents
                            =========================
                            documents
                            
    stringPathAbsStart      /home/pharvey/documents
    stringPathAbsEnd        /home/pharvey
                            =========================
                            ..
                            
    stringPathAbsStart      /home/pharvey
    stringPathAbsEnd        /hometown/pharvey
                            =========================
                            ../../hometown/pharvey
                            
    stringPathAbsStart      /home
    stringPathAbsEnd        /hometown/pharvey
                            =========================
                            ../hometown/pharvey
                            
    stringPathAbsStart      /hometown/pharvey
    stringPathAbsEnd        /home
                            =========================
                            ../../home
                            
    TODO: Does not take into account XP paths such as those which start with a drive
    letter or a "//".                            
*/
QString ADObject::getPathDiff( const QString &stringPathAbsStart, const QString &stringPathAbsEnd )
{
    QString stringPathDiff;

    if ( stringPathAbsStart.isEmpty() || stringPathAbsEnd.isEmpty() )
        return QString();

    if ( stringPathAbsStart == stringPathAbsEnd )
        return QString();

    // find point where paths diverge and discard common part
    // the diverge point will either be a '/' or a QChar::Null in the start path
    int nDiverge = 0;
    int nChar = 0;

    for ( nChar = 0; ; nChar++ )
    {
        // Paths are same...
        if ( nChar >= stringPathAbsStart.length() && nChar >= stringPathAbsEnd.length() )
            return QString();

        // Paths diverge *at* end of string...
        if ( nChar >= stringPathAbsStart.length() )
        {
            if ( stringPathAbsEnd[nChar] == '/' ) nDiverge = nChar;
            break;
        }

        if ( nChar >= stringPathAbsEnd.length() )
        {
            if ( stringPathAbsStart[nChar] == '/' ) nDiverge = nChar;
            break;
        }

        // Dir is changing...
        if ( stringPathAbsStart[nChar] == '/' ) nDiverge = nChar;
    
        // Paths diverge at this dir...
        if ( stringPathAbsStart[nChar] != stringPathAbsEnd[nChar] ) break;
    }

    // count number of dirs to go back (if any)
    for ( nChar = nDiverge; nChar < stringPathAbsStart.length(); nChar++ )
    {
        if ( stringPathAbsStart[nChar] == '/' )
        {
            if ( stringPathDiff.isEmpty() )
                stringPathDiff = "..";
            else
                stringPathDiff += "/..";
        }
    }

    // Now go forward to "End"
    if ( nDiverge < stringPathAbsEnd.length() )
    {
        if ( stringPathAbsEnd[nDiverge] == '/' ) nDiverge++;

        if ( stringPathDiff.isEmpty() )
            stringPathDiff = stringPathAbsEnd.mid( nDiverge );
        else
            stringPathDiff += "/" + stringPathAbsEnd.mid( nDiverge );
    }

    return stringPathDiff;
}

/*!
 * \brief Provide our validation configuration information for our class.
 *  
 * We should call our children as well; but we do not have any knowledge of possible 
 * child classes (at this point we are very generic) so we do nothing more. 
 * Derived classes should do more - where possible. 
 *  
 * \author pharvey (11/28/19)
 * 
 * \param ADValidationStrictness 
 */
ADValidationDatabase ADObject::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr("Name missing"), ADValidation::Information );
        mapValidations.insert( tr("Code missing"), ADValidation::Ignore );
        mapValidations.insert( tr("Comment missing"), ADValidation::Ignore );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr("Name missing"), ADValidation::Warning );
        mapValidations.insert( tr("Code missing"), ADValidation::Information );
        mapValidations.insert( tr("Comment missing"), ADValidation::Ignore );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr("Name missing"), ADValidation::Error );
        mapValidations.insert( tr("Code missing"), ADValidation::Warning );
        mapValidations.insert( tr("Comment missing"), ADValidation::Information );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "ADObject", mapStrictness );

    // add any child classes we may know of
    // ie. if we are DDiagram then something like...
    //      DLine::getValidationConfiguration( mapClasses );

    return mapClasses;
}

/*!
 * \brief Lookup the severity in the validation configuration. 
 *  
 * The default here is to pass the request on to the parent. 'Ignore' will be returned if the 
 * lookup fails for some reason (ie this call has not been intercepted by an ADObject that 
 * knows where to look). 
 *  
 * The idea here is that there will be a derived class which has replaced this method... somewhere 
 * in the object hierarchy. This derived class will maintain the validation configuration data and 
 * be able to lookup the severity. 
 * 
 * \author pharvey (11/29/19)
 * 
 * \param stringClass 
 * \param nStrictness 
 * \param stringValidation 
 * 
 * \return ADValidation::Severity 
 */
ADValidation::Severity ADObject::getValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation )
{
    QObject *pObject = parent();
    if ( !pObject ) return ADValidation::Ignore;
    if ( !pObject->inherits( "ADObject" ) )  return ADValidation::Ignore;

    return ((ADObject*)pObject)->getValidationSeverity( stringClass, nStrictness, stringValidation );
}

void ADObject::eventOutputGeneral( const QString &stringSeverity, const QString &stringMessage )
{
    ADEventGeneral Event( stringSeverity, stringMessage );
    Event.listObjectPath = getPath();
    emit signalOutput( &Event );
}

void ADObject::eventOutputValidate( ADObjectPathList listObjectPathItems, ADValidation::Severity nSeverity, const QString &stringMessage )
{
    ADEventValidation Event( listObjectPathItems, nSeverity, stringMessage );
    emit signalOutput( &Event );
}

void ADObject::eventOutputFind( ADObjectPathList listObjectPathItems, const QPixmap &Pixmap,  const QString &stringMatch )
{
    ADEventSearch Event( listObjectPathItems, Pixmap, stringMatch );
    emit signalOutput( &Event );
}

void ADObject::eventOutputRequest( const QString &s, int n )
{
    ADEventRequest Event( s, n );
    emit signalOutput( &Event );
}

/*!
 * \brief Event to notify object connected indirectly to us - that our modified status has changed.
 *  
 * setModified will update the modified status and then trigger this event if the modified status is true. 
 * This event is also triggered when a child emits signalModified(ADObject*) (by default anyway). 
 *  
 * In general; everything above the descendant becomes modified when a descendant is modified. So the 
 * default is to echo it up to the parent.
 *  
 * If we lack a parent - we are the root object. This is a common place for an app to connect to this 
 * signal, filter it as needed, and take some kind of action. ie update a modification indicator in the UI
 *  
 * Derived classes can override this method if they want different behaviour.
 * Others can connect to the signal(s). 
 *  
 * \sa eventModified() 
 * \sa setModified() 
 *  
 * \author pharvey (12/3/19)
 * 
 * \param pDescendant 
 */
void ADObject::eventModified( ADObject *pSource )
{
    if ( pSource == this )
    { 
        // echo up the fact that pSource was modified
        emit signalModified( pSource );
    }
    else
    {
        emit signalModified( pSource );
        setModified( true );
    }
}

/*!
 * \brief Event to notify object connected directly to us - that our modified status has changed.
 *  
 * setModified will update our modified status and then trigger this event. 
 *  
 * Here we just emit \sa signalModified() to notify any objects, connected directly to us, that 
 * our modified status has changed. ie a corresponding tree widget item
 *  
 * Derived classes can override this method if they want different behaviour. 
 * Others can connect to the signal(s). 
 *  
 * \sa setModified() 
 * \sa eventModified( ADObject * ) 
 * \sa signalModified() 
 *  
 * \author pharvey (2/9/20)
 */
void ADObject::eventModified()
{
    emit signalModified();
}


