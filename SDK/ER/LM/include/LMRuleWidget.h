#ifndef H_LMRuleWidget
#define H_LMRuleWidget

#include "LMRule.h"

class LMRuleWidget : public QWidget
{
    Q_OBJECT
public:
    LMRuleWidget( LMRule *pObject, QWidget *pWidgetParent );

protected slots:
    void slotRuleType()         { p->setRuleType( LMRule::enumRuleTypes(pComboBoxRuleType->currentIndex()) ); }
    void slotExpressionClient() { p->setExpressionClient( pTextEditExpressionClient->toPlainText() ); }
    void slotExpressionServer() { p->setExpressionServer( pTextEditExpressionServer->toPlainText() ); }

protected:
    LMRule *    p;
    QComboBox * pComboBoxRuleType;
    QTextEdit * pTextEditExpressionClient;
    QTextEdit * pTextEditExpressionServer;
};

#endif 


