#ifndef H_DATAOutputDiagnosticsWidget
#define H_DATAOutputDiagnosticsWidget

#include <CBD.h>

#include <DATAMessage.h>
#include <DATADiagnostic.h>

class DATAOutputDiagnosticsWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DATAOutputDiagnosticsWidget( QWidget *pParent );
    ~DATAOutputDiagnosticsWidget();

public slots:
    void slotMessage( DATAMessage * );
    void slotDiagnostic( DATADiagnostic * );
    void slotClear();

protected:
    virtual void doSaveState();
    virtual void doLoadState();
};

#endif

