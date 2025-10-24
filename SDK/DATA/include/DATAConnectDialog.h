#ifndef H_DATAConnectDialog
#define H_DATAConnectDialog

#include <CBD.h>

class DATAConnection;
class DATAOutputDiagnosticsWidget;

class DATADataSourcesComboBox : public QWidget
{
    Q_OBJECT
public:
    DATADataSourcesComboBox( DATAConnection *pConnection, QWidget *pParent );

    QString getDataSource() { return pDataSources->currentText(); }

protected:
    DATAConnection *pConnection;
    QComboBox *     pDataSources;
    QToolButton *   pRefresh;
    QToolButton *   pManageDataSources;

protected slots:
    void slotManageDataSources();
    void slotRefresh();
};

//
//
//
//
//
//

class DATAConnectWidget : public QWidget
{
    Q_OBJECT
public:
    DATAConnectWidget( DATAConnection *pConnection, QWidget *pWidgetParent );

    QString getDataSource(); 
    QString getAlias();       
    QString getUser();       
    QString getPassword();   

    void doClearDiagnostics();

protected:
    DATAConnection *pConnection;

    DATADataSourcesComboBox *       pDataSourceNames;
    QLineEdit *                     pAlias;
    QLineEdit *                     pUser;
    QLineEdit *                     pPassword;
    QToolButton *                   pButtonManageDataSources;
    DATAOutputDiagnosticsWidget *   pDiagnostics;

protected slots:
    void slotManageDataSources();
};

//
//
//
//
//
//

class DATAConnectDialog : public QDialog
{
    Q_OBJECT
public:
    DATAConnectDialog( DATAConnection *pConnection, QWidget *pWidgetParent );

    QString getDataSource()     { return pWidget->getDataSource();  }
    QString getAlias()          { return pWidget->getAlias();       }
    QString getUser()           { return pWidget->getUser();        }

protected:
    DATAConnectWidget * pWidget;
    DATAConnection *    pConnection;

    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotConnect();
    virtual void slotRejected();
};

#endif

