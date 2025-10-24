#ifndef H_PMKey
#define H_PMKey

#include <AWObject.h>
#include "PMObject.h"

class PMTableNormal;
class PMColumn;

/*!
 * \brief Primary/Alternate Key. 
 *  
 * There should be 0-1 (usually 1) primary key for each table. 
 * There can be 0-n alternate keys. 
 *  
 * Each key should have 0-n columns. This is done by creating a 
 * ref from PMKey to the PMColumn. 
 *  
 * A PMReference can create a ref PMKey. 
 *  
 * \author pharvey (5/20/20)
 */
class PMKey : public AWObject, public PMObject
{
    Q_OBJECT
public:
    PMKey( PMTableNormal *pParent );
    virtual ~PMKey();
    
    // SETTERS
    virtual void setPrimary( bool b = true );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Key" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual bool                isPrimary() { return bPrimary; }
    virtual bool                isAlternate() { return !bPrimary; }
    virtual QList<PMColumn*>    getColumns();
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getCreateAuto() { return QString(); }
    virtual QString             getDropAuto() { return QString(); }

    // DO'rs
    virtual void doAddColumn( PMColumn * );     // add ref to a column
    virtual void doRemoveColumn( PMColumn * );  // remove ref to a column

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

protected:
    bool bPrimary = true;
};


#endif 



