#ifndef H_DATAWConnectDialog
#define H_DATAWConnectDialog

#include <CBD.h>

class DATAConnection;
class DATAOutputDiagnosticsWidget;

/*!
 * \brief View/edit a connection string.
 * 
 * The connect string is in a QMap of KeyValue pairs. 
 * New KeyValue pairs can not be added so the caller should include all that is needed. 
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectKeyValueWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWConnectKeyValueWidget( const QMap<QString,QString> &mapKeyValues, QWidget *pWidgetParent );

    QMap<QString,QString> getKeyValues();

protected:
    QMap<QString,QString>       mapKeyValues;   
    QMap<QString,QWidget*>      mapKeyEditors;
};


/*!
 * \brief Connect using KeyValue pairs.
 * 
 * The KeyValue pairs were probably built using \sa DATAWConnectWizard.
 * Any KeyValue pairs with a value of '%' will be prompted for as per UID.
 * Any KeyValue pairs with a value of '%%' will be prompted for and masked as per PWD.
 * The KeyValue pairs are turned into a connect string and used in a call to SQLDriverConnect.
 * 
 * \author pharvey (1/10/25)
 */
class DATAWConnectDialog : public QDialog
{
    Q_OBJECT
public:
    DATAWConnectDialog( DATAConnection *pConnection, const QMap<QString,QString> &mapKeyValues, QWidget *pWidgetParent );

protected:
    DATAConnection *                pConnection;
    DATAWConnectKeyValueWidget *    pKeyValueWidget;
    DATAOutputDiagnosticsWidget *   pDiagnostics;
    QPushButton *                   pConnect;

    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotConnect();
    virtual void slotFinished( int );
};

#endif

