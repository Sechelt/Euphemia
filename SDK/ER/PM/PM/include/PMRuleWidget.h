#ifndef H_PMRuleWidget
#define H_PMRuleWidget

#include "PMRule.h"

class PMRuleWidget : public QWidget
{
    Q_OBJECT
public:
    PMRuleWidget( PMRule *pObject, QWidget *pWidgetParent );

protected slots:
    void slotRuleType()         { p->setRuleType( PMRule::enumRuleTypes(pComboBoxRuleType->currentIndex()) ); }
    void slotExpressionClient() { p->setExpressionClient( pTextEditExpressionClient->toPlainText() ); }
    void slotExpressionServer() { p->setExpressionServer( pTextEditExpressionServer->toPlainText() ); }

protected:
    PMRule *    p;
    QComboBox * pComboBoxRuleType;
    QTextEdit * pTextEditExpressionClient;
    QTextEdit * pTextEditExpressionServer;
};

#endif 


