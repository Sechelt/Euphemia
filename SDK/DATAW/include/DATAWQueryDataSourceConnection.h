#ifndef H_DATAWQueryDataSourceConnection
#define H_DATAWQueryDataSourceConnection

#include <DATAOutputDiagnosticsWidget.h>

#include "DATAWQueryConnection.h"

/*!
 * \brief Represents a DataSourceName (System or User).
 * 
 * Allows connecting to the DataSourceName.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWQueryDataSourceConnection : public DATAWQueryConnection
{
    Q_OBJECT
public:
    DATAWQueryDataSourceConnection( ADObject *pParent, DATAEnvironment *pEnvironment, SQLUSMALLINT nDataSourceType, const QString &stringDataSourceName );
    virtual ~DATAWQueryDataSourceConnection();

protected:
    SQLUSMALLINT    nDataSourceType;
    QString         stringDataSourceName; // this should be getName() 

    virtual void doConnect();
};

/*!
 * \brief Prompts for UserName and Authentication to be used for SQLConnect.
 * 
 * Supports DATAWDataSourceConnection (DataSourceName is known) so does NOT prompt for DataSourceName.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDataSourceConnectWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWDataSourceConnectWidget( QWidget *pParent );

    QString getUserName() { return pUserName->text(); }
    QString getAuthentication() { return pAuthentication->text(); }

protected:
    QLineEdit *pUserName;
    QLineEdit *pAuthentication;
    QCheckBox *pShowAuthentication;

protected slots:
    virtual void slotShowAuthentication( Qt::CheckState );
};

/*!
 * \brief Dialog wraps \sa DATAWDataSourceConnectWidget with \sa DATAOutputDiagnosticsWidget. 
 * 
 * This exists to support \sa DATAWQueryDataSourceConnection where the DataSourceName is known
 * and the connection just needs UserName and Authentication as per SQLConnect.
 * It could be used elsewhere when the DataSourceName has already been determined.
 * 
 * Contains buttons to; Connect, Disconnect and Done.
 * 
 * Does NOT store Authentication nor allow its retrieval.
 * 
 * Caller can check pConnection->isConnected() to determine connection status after call.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWDataSourceConnectDialog : public QDialog
{
    Q_OBJECT
public:
    DATAWDataSourceConnectDialog( DATAConnection *pConnection, const QString &stringDataSourceName, QWidget *pWidgetParent );

protected:
    DATAConnection *                pConnection;
    QString                         stringDataSourceName;
    DATAWDataSourceConnectWidget *  pWidget;
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

