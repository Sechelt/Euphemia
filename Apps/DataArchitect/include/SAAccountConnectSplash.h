#ifndef H_SAAccountConnectSplash
#define H_SAAccountConnectSplash

#include <QtWidgets>

class WDirSelectWidget;
class SAAccountPage;

/*!
 * \brief A login wizard. 
 *  
 * The splash screen is a login wizard. The app should exit if we fail to login using this. 
 * The login can be assisted by providing login method (ie URL for remote) via the command-line. 
 * In this way we can be a helper app for a browser? 
 *  
 * \author pharvey (1/26/21)
 */
class SAAccountConnectSplash : public QDialog
{
    Q_OBJECT
public:
    enum Pages
    {
        PageIntro = 0,
        PageLocal,
        PageODBC,
        PageRemote,
        ResultAccept,
        ResultCancel
    };

    SAAccountConnectSplash( QWidget *pParent = nullptr, const QString &stringURL = QString() );

protected slots:
    void slotNext( SAAccountConnectSplash::Pages );
    void slotCancel();

protected:
    QString stringURL; // file://... or https://...

    QStackedWidget *pStack;
    SAAccountPage * pIntro;
    SAAccountPage * pLocal;
    SAAccountPage * pODBC;
    SAAccountPage * pRemote;
};

/*!
 * \brief Base class for a page.
 * 
 * \author pharvey (1/27/21)
 */
class SAAccountPage : public QWidget
{
    Q_OBJECT
public:
    SAAccountPage( QWidget *pParent = nullptr );

signals:
    void signalNext( SAAccountConnectSplash::Pages n );
};

/*!
 * \brief Select how/where to login. 
 *  
 * Local    - use a local directory containing 0-n SQLite databases (to mimick a typical server)
 * ODBC     - connect to a data source via an ODBC driver or DSN
 * Remote   - connect to a data source via https 
 * 
 * \author pharvey (1/26/21)
 */
class SAAccountIntroPage : public SAAccountPage
{
    Q_OBJECT
public:
    SAAccountIntroPage( QWidget *pParent = nullptr );

protected slots:
    void slotNext();

protected:
    QRadioButton *pButtonLocal;
    QRadioButton *pButtonODBC;
    QRadioButton *pButtonRemote;
    QPushButton *pNext;
};

/*!
 * \brief Select a URL (https) to connect to.
 *  
 * The server must have supporting PHP files to provide access to the database server.
 *  
 * \author pharvey (1/26/21)
 */
class SAAccountRemotePage : public SAAccountPage
{
    Q_OBJECT
public:
    SAAccountRemotePage( QWidget *pParent = nullptr );

protected slots:
    void slotNext();

protected:
    QLineEdit *pURL; // https:// etc
    QLineEdit *pUID; // email
    QLineEdit *pPWD; //
};

/*!
 * \brief Select a local directory. 
 *  
 * A local account is a directory. The directory will contain 0-n (SQLite) databases.
 *  
 * \author pharvey (1/26/21)
 */
class SAAccountLocalPage : public SAAccountPage
{
    Q_OBJECT
public:
    SAAccountLocalPage( QWidget *pParent = nullptr );

protected slots:
    void slotNext();

protected:
    WDirSelectWidget *pURL; // file://...
};

/*!
 * \brief Select an ODBC connection.
 *  
 * This can be an ODBC; Driver, or Data Source Name.
 *  
 * \author pharvey (1/26/21)
 */
class SAAccountODBCPage : public SAAccountPage
{
    Q_OBJECT
public:
    SAAccountODBCPage( QWidget *pParent = nullptr );

protected:

protected slots:
    void slotNext();
};


#endif

