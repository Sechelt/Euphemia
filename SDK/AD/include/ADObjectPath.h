#ifndef ADOBJECTPATH_H
#define ADOBJECTPATH_H

#include <QtGui> 

/*! 
 * \class ADObjectPathItem
 *
 * \brief Contains a path to specific object in the object hierarchy.
 *
 * Contains a path to a specific place in the object hierarchy. This can be
 * used in a search result to allow navigation to the source of the result.
 * 
 * The path can be based upon object pointers, index positions in object 
 * lists, or a combination. The path may even contain name/values indicating, 
 * for example, a Line/Column position in a text file.
 * 
 * The idea is that we can shove this into the object hierarchy at the entry point
 * and the objects in question will be able to interpret to get to the source 
 * location.
 * 
 * Use ValuePointer when you know that the pointers are availible. This is most 
 * likely to result in successful navigation back to source.
 * 
 * Use ValueIndex when it is unlikely (or certian) that the object pointers will
 * not be availible for navigation back to source. This would be the case, for
 * example, if a search resulted in temporarily loading files. The pointers 
 * would be lost when the file is unloaded but the index values would remain 
 * valid.
 *
 * WARNING: An object path is never valid for very long so any use, subsequent to its
 * creation must be done with caution. The object name, pointer and index value may
 * all change and should only be used as a suggested path to the source. This is 
 * particularly important when casting values nValue to object pointers - do
 * not assume they still exist - look them up in the child object list before using.
 */
class ADObjectPathItem
{
public:
    enum enumValueTypes
    {
        ValueSpecial,                           // you should consider nValue to be a special value (i.e. not of the standard types)
        ValuePointer,                           // you should consider nValue suspect - use pobject and class name to lookup object
        ValueIndex,                             // you should consider nValue to be an index into an object list (i.e. child objects)
        ValueOID                                // you should consider nValue to be an Object ID (OID)
    };

    ADObjectPathItem( const enumValueTypes nValueType = ValueSpecial, QObject *pobject = 0, const QString &stringClassName = QString(), const QString &stringObjectName = QString(), int nValue = -1 );

    enumValueTypes      nValueType;             // way to interpret nValue
    QObject *           pobject;
    QString             stringClassName;
    QString             stringObjectName;
    int                 nValue;
};

typedef QList<ADObjectPathItem> ADObjectPathList;
typedef QStack<ADObjectPathItem> ADObjectPathStack;

#endif


