#ifndef LMOBJECT_H
#define LMOBJECT_H

#include <ADTransaction.h>
#include <AWObject.h>

/*!
 * \brief Description and Annotation.
 *  
 * These are included in reports. 
 *  
 * The object provided in the constructor is used to call setModified() when 
 * the text in here has changed. If we have been inherited along side a 
 * LMModel - it would be the LMModel. 
 *  
 * This is inherited by all our drawable LM objects and LMModel.
 *  
 * \sa LMModel 
 *  
 * \author pharvey (11/26/19)
 */
class LMObject
{
public:
    LMObject( AWObject *pObject );
    virtual ~LMObject();

    // SETTERS
    virtual void setDescription( const QString &s );
    virtual void setAnnotation( const QString &s );

    // GETTERS
    virtual QString getDescription();
    virtual QString getAnnotation();

    // DO'RS
    virtual bool doSaveLMObject( QDomDocument *pdomDoc, QDomElement *pdomElem );
    virtual bool doLoadLMObject( QDomElement *pdomElem );

protected:
    AWObject *  pObject;
    QString     stringDescription;
    QString     stringAnnotation;
};

#endif


