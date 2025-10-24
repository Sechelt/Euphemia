#ifndef H_ADDocument
#define H_ADDocument

#include "ADObject.h"

/*!
 * \class ADDocument 
 * \brief To provide additional data for objects which are considered 'documents'. 
 *  
 * A 'document' is any object where things like; Authour, Create date, Version etc 
 * may be needed. 
 *  
 * The most obvious time that this may be desired is an object which is to be 
 * saved/loaded to a file. 
 *  
 * The object provided in the constructor is used to call setModified() when 
 * the text in here has changed.
 *  
 * \author pharvey (12/26/19)
 */
class ADDocument
{
public:
    friend class ADObject;
    ADDocument( ADObject *pObject );
    virtual ~ADDocument();

    virtual void setAuthor( const QString &s );                 
    virtual void setCreatedDate( const QDateTime &d );          
    virtual void setModifiedDate( const QDateTime &d );         
    virtual void setVer( const QString &s );                

    virtual QString     getAuthor() { return stringAuthor; }      
    virtual QDateTime   getModifiedDate() { return dateModified; }
    virtual QDateTime   getCreatedDate() { return dateCreated; }  
    virtual QString     getVer() { return stringVer; }    
    virtual ADObject *  getObject() { return pObject; }

    virtual bool doSaveADDocument( QDomDocument *pdomDoc, QDomElement *pdomElem );
    virtual bool doLoadADDocument( QDomElement *pdomElem );
    virtual void doClear();

protected:
    ADObject *pObject;

    QString     stringVer;
    QString     stringAuthor;
    QDateTime   dateCreated;
    QDateTime   dateModified;
};

#endif


