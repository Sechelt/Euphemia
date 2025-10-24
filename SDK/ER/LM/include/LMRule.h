#ifndef LMRULE_H
#define LMRULE_H

#include <AWObject.h>

#include "LMObject.h"

class LMModel;

class LMRule : public AWObject, public LMObject
{
    Q_OBJECT
public:
    enum enumRuleTypes
    {
        nRuleTypeConstraint = 0,
        nRuleTypeDefinition,
        nRuleTypeFact,
        nRuleTypeFormula,
        nRuleTypeRequirement,
        nRuleTypeValidation
    };

    LMRule( LMModel *pModel );
    virtual ~LMRule();

    // SETTERS
    virtual void setName( const QString &s );
    virtual void setRuleType( enumRuleTypes n )             { nRuleType = n;                }
    virtual void setExpressionClient( const QString &s )    { stringExpressionClient = s;   }
    virtual void setExpressionServer( const QString &s )    { stringExpressionServer = s;   }

    // GETTERS
    virtual QPixmap              getIcon();
    virtual QString              getFriendlyClassName() { return tr( "Rule" ); }
    virtual AWDataWidget *       getObjectWidget( QWidget *pWidgetParent = nullptr );
    static  ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual enumRuleTypes        getRuleType()           { return nRuleType;                 }
    virtual QString              getExpressionClient()   { return stringExpressionClient;    }
    virtual QString              getExpressionServer()   { return stringExpressionServer;    }

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    
public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    enumRuleTypes   nRuleType;
    QString         stringExpressionClient;
    QString         stringExpressionServer;

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


