#ifndef H_PMTableDerived
#define H_PMTableDerived

#include "PMTableBase.h"

class PMKey;

class PMTableDerived : public PMTableBase
{
    Q_OBJECT
public:
    PMTableDerived( PMModel *pParent );
    virtual ~PMTableDerived();

    // SETTERS
    
    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Table Derived" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
//    virtual PMKey *             getKey( bool bPrimary = true );

    // DO'RS
    virtual bool doLoad( QDomElement *pdomElemTable );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
};

#endif 


