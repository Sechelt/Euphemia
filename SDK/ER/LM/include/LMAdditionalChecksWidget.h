#ifndef H_LMAdditionalChecksWidget
#define H_LMAdditionalChecksWidget

#include <CBD.h>

class LMAdditionalChecksWidget : public QWidget
{
    Q_OBJECT
public:
    LMAdditionalChecksWidget( QWidget *pParent, const QString &stringClient, const QString &stringServer );

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


