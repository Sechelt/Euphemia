#include "LibInfo.h"
#include "LMObject.h"

LMObject::LMObject( AWObject *pObject )
{
    this->pObject = pObject;
}

LMObject::~LMObject() 
{
}

void LMObject::setDescription( const QString &s ) 
{ 
    if ( stringDescription == s ) return; 
    stringDescription = s;
    if ( pObject ) pObject->setModified();
}

void LMObject::setAnnotation( const QString &s ) 
{ 
    if ( stringAnnotation == s ) return;
    stringAnnotation = s;
    if ( pObject ) pObject->setModified(); 
}

QString LMObject::getDescription() 
{ 
    return stringDescription; 
}

QString LMObject::getAnnotation() 
{ 
    return stringAnnotation; 
}

bool LMObject::doSaveLMObject( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED(pdomDoc);

    // Description
    if ( !getDescription().isEmpty() )
    {
        unsigned long nLen = 0;
        pdomElem->setAttribute( "Description", CBDPersistUtility::getEncodedText( getDescription(), &nLen ) );
        pdomElem->setAttribute( "DescriptionSize", QString::number( nLen ) );
    }

    // Annotation
    if ( !getAnnotation().isEmpty() )
    {
        unsigned long nLen = 0;
        pdomElem->setAttribute( "Annotation", CBDPersistUtility::getEncodedText( getAnnotation(), &nLen ) );
        pdomElem->setAttribute( "AnnotationSize", QString::number( nLen ) );
    }

    return true;
}

bool LMObject::doLoadLMObject( QDomElement *pdomElemObject )
{
    QString stringAttribute;

    // Description
    stringAttribute = pdomElemObject->attribute( "DescriptionSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemObject->attribute( "Description" );
        if ( !stringAttribute.isNull() )
            stringDescription = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    // Annotation
    stringAttribute = pdomElemObject->attribute( "AnnotationSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemObject->attribute( "Annotation" );
        if ( !stringAttribute.isNull() )
            stringAnnotation = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    return true;
}


