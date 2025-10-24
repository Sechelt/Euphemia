#ifndef H_PMIndex
#define H_PMIndex

#include <AWObject.h>
#include "PMObject.h"
#include "PMDDLManager.h"

class PMTableBase;
class PMIndexColumn;

class PMIndex : public AWObject, public PMObject, public PMDDLManager
{
    Q_OBJECT
public:
    PMIndex( PMTableBase *pParent );
    virtual ~PMIndex();
    
    // SETTERS
    virtual void setUnique( bool bUnique ) { this->bUnique = bUnique;  }

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Index" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QString             getDefaultName();
    virtual bool                getUnique() { return bUnique; }
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:

protected:
    bool bUnique;
};


#endif 


