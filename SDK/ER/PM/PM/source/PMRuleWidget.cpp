#include "LibInfo.h"
#include "PMRuleWidget.h"

PMRuleWidget::PMRuleWidget( PMRule *pObject, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    p = pObject;

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pComboBoxRuleType = new QComboBox( this );
    pComboBoxRuleType->addItem( tr("Constraint"), PMRule::RuleTypeConstraint );
    pComboBoxRuleType->addItem( tr("Definition"), PMRule::RuleTypeDefinition );
    pComboBoxRuleType->addItem( tr("Fact"), PMRule::RuleTypeFact );
    pComboBoxRuleType->addItem( tr("Formula"), PMRule::RuleTypeFormula );
    pComboBoxRuleType->addItem( tr("Requirement"), PMRule::RuleTypeRequirement );
    pComboBoxRuleType->addItem( tr("Validation"), PMRule::RuleTypeValidation );
    pLayout->addWidget( pComboBoxRuleType );

    QTabWidget *pTabWidget = new QTabWidget( this );

    pTextEditExpressionClient = new QTextEdit( pTabWidget );
    pTextEditExpressionClient->setPlainText( p->getExpressionClient() );
    connect( pTextEditExpressionClient, SIGNAL(textChanged()), SLOT(slotExpressionClient()) );
    pTabWidget->addTab( pTextEditExpressionClient, tr("Client") );

    pTextEditExpressionServer = new QTextEdit( pTabWidget );
    pTextEditExpressionServer->setPlainText( p->getExpressionServer() );
    connect( pTextEditExpressionServer, SIGNAL(textChanged()), SLOT(slotExpressionServer()) );
    pTabWidget->addTab( pTextEditExpressionServer, tr("Server") );

    pLayout->addWidget( pTabWidget );
}

