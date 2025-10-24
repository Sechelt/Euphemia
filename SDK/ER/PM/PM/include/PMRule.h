#ifndef H_PMRule
#define H_PMRule

#include <AWObject.h>
#include "PMObject.h"

class PMModel;

class PMRule : public AWObject, public PMObject
{
    Q_OBJECT
public:
    enum enumRuleTypes
    {
        RuleTypeConstraint = 0,
        RuleTypeDefinition,
        RuleTypeFact,
        RuleTypeFormula,
        RuleTypeRequirement,
        RuleTypeValidation
    };

    PMRule( PMModel *pParent );
    virtual ~PMRule();

    // SETTERS
    virtual void setRuleType( enumRuleTypes n );
    virtual void setExpressionClient( const QString &s );
    virtual void setExpressionServer( const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Rule" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual enumRuleTypes       getRuleType()     { return nRuleType;                 }
    virtual QString             getExpressionClient()   { return stringExpressionClient;    }
    virtual QString             getExpressionServer()   { return stringExpressionServer;    }
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:

protected:
    enumRuleTypes   nRuleType;
    QString         stringExpressionClient;
    QString         stringExpressionServer;

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


