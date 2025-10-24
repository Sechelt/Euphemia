#ifndef H_DATAWOutputMessagesWidget
#define H_DATAWOutputMessagesWidget

#include <CBD.h>

#include <DATAMessage.h>
#include <DATADiagnostic.h>

#include <ADObject.h>

class DATAWOutputMessagesWidget : public QTableWidget
{
    Q_OBJECT
public:
    DATAWOutputMessagesWidget( QWidget *pParent );
    ~DATAWOutputMessagesWidget();

public slots:
    void slotMessage( ADDataEventGeneral *p );
    void slotClear();

protected:
    QVector<ADObjectPathList> vectorPaths;

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif

