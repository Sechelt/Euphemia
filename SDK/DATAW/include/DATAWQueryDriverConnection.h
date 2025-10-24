#ifndef H_DATAWQueryDriverConnection
#define H_DATAWQueryDriverConnection

#include <DATAOutputDiagnosticsWidget.h>

#include "DATAWQueryConnection.h"

class DATAWQueryDriverConnection : public DATAWQueryConnection
{
    Q_OBJECT
public:
    DATAWQueryDriverConnection( ADObject *pParent, DATAEnvironment *pEnvironment, const QString &stringDriverName );
    virtual ~DATAWQueryDriverConnection();

protected:
    QString stringDriverName;

    virtual void doConnect();

};

/*!
 * \brief Base class for a widget which provides a connection string suitable for a SQLDriverConnect.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDriverConnectWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWDriverConnectWidget( QWidget *pParent );

    virtual QString getConnectString() = 0;
};

/*!
 * \brief Helps create a connection string for connecting to an unknown data source.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDriverOtherWidget : public DATAWDriverConnectWidget
{
    Q_OBJECT
public:
    DATAWDriverOtherWidget( QWidget *pParent );

    virtual QString getConnectString();

protected:
    QLineEdit *pConnectString;
};

/*!
 * \brief Helps create a connection string for connecting to a SQLite data source.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDriverSQLiteWidget : public DATAWDriverConnectWidget
{
    Q_OBJECT
public:
    DATAWDriverSQLiteWidget( QWidget *pParent );

    virtual QString getConnectString();

protected:
    QLineEdit *     pFile;
    QToolButton *   pSelectFile;

protected slots:
    virtual void slotSelectFile();
};

/*!
 * \brief Helps create a connection string for connecting to a MariaDB data source.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDriverMariaWidget : public DATAWDriverConnectWidget
{
    Q_OBJECT
public:
    DATAWDriverMariaWidget( QWidget *pParent );

    virtual QString getConnectString();

    QString getServer()     { return pServer->text(); }
    QString getUser()       { return pUser->text(); }
    QString getPassword()   { return pPassword->text(); }
    QString getDatabase()   { return pDatabase->text(); }

protected:
    QLineEdit *pServer;
    QLineEdit *pUser;
    QLineEdit *pPassword;
    QCheckBox *pShowPassword;
    QLineEdit *pDatabase;

protected slots:
    virtual void slotShowPassword( Qt::CheckState );
};

/*!
 * \brief Driver connect dialog which connects using SQLDriverConnect.
 * 
 * This exists to support \sa DATAWQueryDriverConnection but could be used elsewhere when only the driver name is known.
 * Will assist User in creating a connect string appropriate for the driver.
 * Always calls SQLDriverConnect with SQL_DRIVER_NOPROMPT (driver does not have to support a GUI).
 * To do this we must know the key/value pairs supported by the driver so drivers supported will be limited.
 * In cases where unknown drivers are used a User can build a key/value list using their knowledge of the driver.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDriverConnectDialog : public QDialog
{
    Q_OBJECT
public:
    DATAWDriverConnectDialog( DATAConnection *pConnection, const QString &stringDiver, QWidget *pWidgetParent );

protected:
    DATAConnection *                pConnection;
    QString                         stringDriver;
    DATAWDriverConnectWidget *      pWidget;
    DATAOutputDiagnosticsWidget *   pDiagnostics;

    QPushButton *pButtonConnect;    
    QPushButton *pButtonDisconnect; 

    virtual void doSyncControls();
    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotConnect();
    virtual void slotDisconnect();
    virtual void slotDone();
};

#endif

