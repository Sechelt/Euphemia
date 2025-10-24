#ifndef H_PMProcedure
#define H_PMProcedure

#include <AWObject.h>
#include "PMObject.h"
#include "PMDDLManager.h"

class PMModel;

class PMProcedure : public AWObject, public PMObject, public PMDDLManager
{
    Q_OBJECT
public:
    PMProcedure( PMModel *pParent );
    virtual ~PMProcedure();

    // SETTERS
    virtual void                setDef( const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Procedure" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getDef() { return stringDef; }
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:

protected:
    QString stringDef;

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


