#ifndef H_SAOutputWidget
#define H_SAOutputWidget

#include <AWOutputWidget.h>

class DATAWOutputMessagesWidget;
class DATAWOutputDiagnosticsWidget;

class SAOutputWidget : public AWOutputWidget
{
    Q_OBJECT
public:
    SAOutputWidget( QWidget *pParent );

public slots:
    virtual void slotOutput( ADEventOutput *pEvent );
  
protected:
    DATAWOutputMessagesWidget *    pDataMessages;
    DATAWOutputDiagnosticsWidget * pDataDiagnostics;

    int nDataMessagesCount;
    int nDataDiagnosticsCount;

protected slots:
    virtual void slotCurrentChanged( int nIndex );
    virtual void slotCurrentClear();
};

#endif
