#ifndef H_DATAWOutputDiagnosticsWidget
#define H_DATAWOutputDiagnosticsWidget

#include <CBD.h>

#include <DATADiagnostic.h>

#include <ADObjectPath.h>
#include <ADObject.h>

class DATAWOutputDiagnosticsWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DATAWOutputDiagnosticsWidget( QWidget *pParent );
    ~DATAWOutputDiagnosticsWidget();

public slots:
    void slotMessage( ADDataEventDiagnostic *p );
    void slotClear();

protected:
    QVector<ADObjectPathList> vectorPaths;

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif

