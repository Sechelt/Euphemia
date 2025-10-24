#ifndef H_DATAOutputMessagesWidget
#define H_DATAOutputMessagesWidget

#include <CBD.h>

#include <DATAMessage.h>

/*!
 * \brief Display DATAMessage's in a table.
 * 
 * \author pharvey (12/3/20)
 */
class DATAOutputMessagesWidget : public QTableWidget
{
    Q_OBJECT
public:
    DATAOutputMessagesWidget( QWidget *pParent );
    ~DATAOutputMessagesWidget();

public slots:
    void slotMessage( DATAMessage * );
    void slotClear();

protected:
    virtual void doSaveState();
    virtual void doLoadState();
};

#endif

