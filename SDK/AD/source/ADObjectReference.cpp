#include "LibInfo.h"
#include "ADObjectReference.h"

#include "ADObject.h"

ADObjectReferenceKey::ADObjectReferenceKey( ADObject *p, const QString &s )
{
    pObject = p;
    stringName = s;
}

ADObjectReferenceKey & ADObjectReferenceKey::operator=( const ADObjectReferenceKey &x )
{
    pObject = x.pObject;
    stringName = x.stringName;
    return *this;
}

//
//
//
//

ADObjectReference::ADObjectReference()
{
}

ADObjectReference & ADObjectReference::operator=( const ADObjectReference & )
{
    return *this;
}


