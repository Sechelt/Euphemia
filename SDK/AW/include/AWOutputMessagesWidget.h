#ifndef H_AWOutputMessagesWidget
#define H_AWOutputMessagesWidget

#include <CBD.h>

#include <ADObjectPath.h>
#include <ADObject.h>

class AWOutputMessagesWidget : public QTableWidget
{
    Q_OBJECT
public:
    AWOutputMessagesWidget( QWidget *pParent );
    ~AWOutputMessagesWidget();

public slots:
    void slotMessage( ADEventGeneral *p );
    void slotClear();

protected:
    QVector<ADObjectPathList> vectorPaths;

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif

