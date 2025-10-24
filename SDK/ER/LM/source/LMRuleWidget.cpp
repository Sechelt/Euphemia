#include "LibInfo.h"
#include "LMRuleWidget.h"

LMRuleWidget::LMRuleWidget( LMRule *pObject, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    p = pObject;

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pComboBoxRuleType = new QComboBox( this );
    pComboBoxRuleType->addItem( tr("Constraint"), LMRule::nRuleTypeConstraint );
    pComboBoxRuleType->addItem( tr("Definition"), LMRule::nRuleTypeDefinition );
    pComboBoxRuleType->addItem( tr("Fact"), LMRule::nRuleTypeFact );
    pComboBoxRuleType->addItem( tr("Formula"), LMRule::nRuleTypeFormula );
    pComboBoxRuleType->addItem( tr("Requirement"), LMRule::nRuleTypeRequirement );
    pComboBoxRuleType->addItem( tr("Validation"), LMRule::nRuleTypeValidation );
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

