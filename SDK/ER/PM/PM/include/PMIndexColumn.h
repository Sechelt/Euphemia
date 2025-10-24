#ifndef H_PMIndexColumn
#define H_PMIndexColumn

#include <AWObject.h>
#include "PMObject.h"

class PMIndex;

class PMIndexColumn : public AWObject, public PMObject
{
    Q_OBJECT
public:
    PMIndexColumn( PMIndex *pParent );
    virtual ~PMIndexColumn();

    // SETTERS
    virtual void setOrder( const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Column" ); }
    virtual QString             getOrder() { return stringOrder; }
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
    virtual void slotName( const QString &s );
    virtual void slotOrder( const QString &s );

protected:
    QString     stringOrder;     // should be; "ASC", "DESC", or isEmpty()
};

#endif 


