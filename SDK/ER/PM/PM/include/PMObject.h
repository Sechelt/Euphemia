#ifndef H_PMObject
#define H_PMObject

#include <ADTransaction.h>
#include <AWObject.h>

/*!
 * \brief Description and Annotation.
 *  
 * These are included in reports. 
 *  
 * The object provided in the constructor is used to call setModified() when 
 * the text in here has changed. If we have been inherited along side a 
 * PMModel - it would be the PMModel. 
 *  
 * This is inherited by PMModel and all of its descendants.
 *  
 * \sa PMModel 
 *  
 * \author pharvey (11/26/19)
 */
class PMObject
{
public:
    PMObject( AWObject *pObject );
    virtual ~PMObject();

    // SETTERS
    virtual void setDescription( const QString &s );
    virtual void setAnnotation( const QString &s );

    // GETTERS
    virtual QString getDescription();
    virtual QString getAnnotation();

    // DO'RS
    virtual bool doSavePMObject( QDomDocument *pdomDoc, QDomElement *pdomElem );
    virtual bool doLoadPMObject( QDomElement *pdomElem );

protected:
    AWObject *pObject;

    // Notes
    QString     stringDescription;
    QString     stringAnnotation;
};

#endif


