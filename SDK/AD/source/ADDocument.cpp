#include "LibInfo.h"
#include "ADDocument.h"

ADDocument::ADDocument( ADObject *pObject )
{
    this->pObject = pObject;

    dateCreated     = QDateTime::currentDateTime();
}

ADDocument::~ADDocument()
{
}

void ADDocument::setAuthor( const QString &s )        
{                                                     
    if ( stringAuthor == s ) return;                  
    stringAuthor = s;                                 
    if ( pObject ) pObject->setModified();                                 
}                                                     
                                                      
void ADDocument::setCreatedDate( const QDateTime &d ) 
{                                                     
    if ( dateCreated == d ) return;                   
    dateCreated = d;                                  
    if ( pObject ) pObject->setModified();                                 
}                                                     
                                                      
void ADDocument::setModifiedDate( const QDateTime &d )
{                                                     
    if ( dateModified == d ) return;                  
    dateModified = d;                                 
    if ( pObject ) pObject->setModified();                                 
}                                                     
                                                      
void ADDocument::setVer( const QString &s )       
{                                                     
    if ( stringVer == s ) return;                 
    stringVer = s;                                
    if ( pObject ) pObject->setModified();                                 
}                                                     

bool ADDocument::doSaveADDocument( QDomDocument *pdomDoc, QDomElement *pdomElemThis )
{
    Q_UNUSED(pdomDoc);

    dateModified = QDateTime::currentDateTime();                                         
    if ( !stringAuthor.isEmpty() ) pdomElemThis->setAttribute( "Author", stringAuthor );
    pdomElemThis->setAttribute( "Created", dateCreated.toString() );                       
    pdomElemThis->setAttribute( "Modified", dateModified.toString() );                     
    if ( !stringVer.isEmpty() ) pdomElemThis->setAttribute( "Version", stringVer );     

    return true;
}

bool ADDocument::doLoadADDocument( QDomElement *pdomElemThis )
{
    stringAuthor    = pdomElemThis->attribute( "Author" );                           
    dateCreated     = QDateTime::fromString( pdomElemThis->attribute( "Created" ) ); 
    dateModified    = QDateTime::fromString( pdomElemThis->attribute( "Modified" ) );
    stringVer       = pdomElemThis->attribute( "Version" );                          

    return true;
}

void ADDocument::doClear()
{
    stringAuthor    = QString();
    dateCreated     = QDateTime::currentDateTime(); 
    dateModified    = QDateTime();
    stringVer       = QString();
}




