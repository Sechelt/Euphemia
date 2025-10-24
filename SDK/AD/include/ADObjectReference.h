#ifndef ADOBJECTREFERENCE_H
#define ADOBJECTREFERENCE_H

#include <QtGui> 

class ADObject;

/*!
 * \class ADObjectReference 
 * \brief Reference data. 
 *  
 * An object can reference any other object in the object hierarchy. 
 * The reference is two-way... in other words it can be navigated in both directions. 
 *  
 * Consider a case where object A references object B. 
 *  
 * Object A will be found in B::UsedBy list.
 * Object B will be found in A::Uses list. 
 *  
 * A name can be used. Consider case where a line 
 * begin and end point connect to the same shape. This creates two identical references 
 * with no indication of which is the Begin and which is the End of the line. In such 
 * a case stringName may be used; "Begin", "End".
 *  
 * pObject + stringName must be unique within the list.
 *  
 * Both ends of a reference will have same stringName so End = End. 
 *  
 * \author pharvey (12/17/19)
 */
class ADObjectReferenceKey
{
public:
    ADObjectReferenceKey( ADObject *p = nullptr, const QString &s = QString() );
    ADObjectReferenceKey( const ADObjectReferenceKey & ) = default;

    ADObject *  pObject;        // the other object
    QString     stringName;

    ADObjectReferenceKey & operator=( const ADObjectReferenceKey &x );

    friend inline bool operator==( const ADObjectReferenceKey &lhs, const ADObjectReferenceKey &rhs )
    {
        return (lhs.pObject == rhs.pObject && lhs.stringName == rhs.stringName);
    }

    friend inline bool operator>( const ADObjectReferenceKey &lhs, const ADObjectReferenceKey &rhs )
    {
        if  ( lhs.pObject == rhs.pObject ) return (lhs.stringName > rhs.stringName);
        return ( lhs.pObject > rhs.pObject );
    }

    friend inline bool operator<( const ADObjectReferenceKey &lhs, const ADObjectReferenceKey &rhs )
    {
        if  ( lhs.pObject == rhs.pObject ) return (lhs.stringName < rhs.stringName);
        return ( lhs.pObject < rhs.pObject );
    }
};


class ADObjectReference
{
public:
    ADObjectReference();
    ADObjectReference( const ADObjectReference & ) = default;

    ADObjectReference & operator=( const ADObjectReference &x );
};

#endif


