#ifndef LMIDENTIFIER_H
#define LMIDENTIFIER_H

#include <AWObject.h>
#include <AWDataTable.h>

#include "LMObject.h"
#include "LMAttribute.h"

class LMModel;
class LMEntityNormal;

class LMIdentifier : public AWObject, public LMObject
{
    Q_OBJECT
public:
    LMIdentifier( LMEntityNormal *pentity );
    virtual ~LMIdentifier();

    // SETTERS
//    virtual void setName( const QString &s );
    virtual void setPrimary( bool b );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Identifier" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual LMEntityNormal *    getEntity();
    virtual bool                isPrimary() { return bPrimary; }

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    // virtual void doUsedByAdd( ADObject *pmodel ) { ADObject::doUsedByAdd( pmodel ); }

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    bool bPrimary;

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


