#include "LibInfo.h"
#include "PMDDLManager.h"

PMDDLManager::PMDDLManager( AWObject *p )
{
    pObject = p;

    bIncludeCreate  = true;
    bIncludeDrop    = true;
    bCreateManual   = false;
    bDropManual     = false;
}

void PMDDLManager::setIncludeCreate( bool b )
{
    if ( b == bIncludeCreate ) return;
    bIncludeCreate = b;
    if ( pObject ) pObject->setModified();
}

void PMDDLManager::setIncludeDrop( bool b )
{
    if ( b == bIncludeDrop ) return;
    bIncludeDrop = b;
    if ( pObject ) pObject->setModified();
}

void PMDDLManager::setCreateManual( bool b )
{
    if ( b == bCreateManual ) return;
    bCreateManual = b;
    if ( pObject ) pObject->setModified();
}

void PMDDLManager::setDropManual( bool b )
{
    if ( b == bDropManual ) return;
    bDropManual = b;
    if ( pObject ) pObject->setModified();
}

void PMDDLManager::setCreateManual( const QString &s ) 
{ 
    if ( stringCreateManual == s ) return; 
    stringCreateManual = s;
    if ( pObject ) pObject->setModified();
}

void PMDDLManager::setDropManual( const QString &s ) 
{ 
    if ( stringDropManual == s ) return;
    stringDropManual = s;
    if ( pObject ) pObject->setModified(); 
}

bool PMDDLManager::doSavePMDDLManager( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED(pdomDoc);

    // Flags...
    if ( !bIncludeCreate ) pdomElem->setAttribute( "IncludeCreate", bIncludeCreate );
    if ( !bIncludeDrop ) pdomElem->setAttribute( "IncludeDrop", bIncludeDrop );
    if ( bCreateManual ) pdomElem->setAttribute( "CreateManual", bCreateManual );
    if ( bDropManual ) pdomElem->setAttribute( "DropManual", bDropManual );

    // Manual scripts...
    if ( !stringCreateManual.isEmpty() )
    {
        unsigned long nLen = 0;
        pdomElem->setAttribute( "CreateManual", CBDPersistUtility::getEncodedText( stringCreateManual, &nLen ) );
        pdomElem->setAttribute( "CreateManualSize", QString::number( nLen ) );
    }

    if ( !stringDropManual.isEmpty() )
    {
        unsigned long nLen = 0;
        pdomElem->setAttribute( "DropManual", CBDPersistUtility::getEncodedText( stringDropManual, &nLen ) );
        pdomElem->setAttribute( "DropManualSize", QString::number( nLen ) );
    }

    return true;
}

bool PMDDLManager::doLoadPMDDLManager( QDomElement *pdomElemObject )
{
    QString stringAttribute;

    // Flags...
    bIncludeCreate  = pdomElemObject->attribute( "IncludeCreate", "1" ).toInt();
    bIncludeDrop    = pdomElemObject->attribute( "IncludeDrop", "1" ).toInt();
    bCreateManual   = pdomElemObject->attribute( "CreateManual", "0" ).toInt();
    bDropManual     = pdomElemObject->attribute( "DropManual", "0" ).toInt();

    // Manual scripts...
    stringAttribute = pdomElemObject->attribute( "CreateManualSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemObject->attribute( "CreateManual" );
        if ( !stringAttribute.isNull() )
            stringCreateManual = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    stringAttribute = pdomElemObject->attribute( "DropManualSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemObject->attribute( "DropManual" );
        if ( !stringAttribute.isNull() )
            stringDropManual = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    return true;
}


