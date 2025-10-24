#ifndef H_PMAdditionalChecksWidget
#define H_PMAdditionalChecksWidget

#include <CBD.h>

class PMAdditionalChecksWidget : public QWidget
{
    Q_OBJECT
public:
    PMAdditionalChecksWidget( QWidget *pParent, const QString &stringClient, const QString &stringServer );

signals:
    void signalClient( const QString &s );
    void signalServer( const QString &s );

protected slots:
    virtual void slotClient();
    virtual void slotServer();

protected:
    QTextEdit *         pServer;
    QTextEdit *         pClient;
};

#endif 


