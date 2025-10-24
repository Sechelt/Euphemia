#include "LibInfo.h"
#include "ADObjectPath.h"

ADObjectPathItem::ADObjectPathItem( const enumValueTypes nValueType, QObject *pobject, const QString &stringClassName, const QString &stringObjectName, int nValue )
{
    this->pobject           = pobject;
    this->nValueType        = nValueType;
    this->stringClassName   = stringClassName;
    this->stringObjectName  = stringObjectName;
    this->nValue            = nValue;
}


