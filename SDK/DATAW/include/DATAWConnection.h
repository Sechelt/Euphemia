#ifndef H_DATAWConnection
#define H_DATAWConnection

#include <ADTransaction.h>
#include <AWObject.h>
#include <DATASystem.h>
#include <DATAEnvironment.h>
#include <DATAConnection.h>

class DATAWQueryCatalog;

/*!
 * \brief Data connection. 
 *  
 * This provides an AWObject based data connection. 
 * Unlike \sa DATAWQueryConnection - this is not for general browsing of ODBC objects. For example; it 
 * only connects via a driver - no Data Source Names, File Data Sources nor 'browse' connection options. 
 *  
 * This can be used to streamline adding a data connection to a \sa AWWorkspace. For example; it does not need an
 * ODBC System (\sa DATAWQuerySystem) and Environment (\sa DATAWQueryEnvironment) parent. 
 *  
 * This should make using an application easier for those that care not about the underlying (ODBC) access. 
 *  
 * \author pharvey (12/13/20)
 */
class DATAWConnection : public AWObject
{
    Q_OBJECT
public:
    explicit DATAWConnection( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DATAWConnection();

    virtual void setKeyValues( QMap<QString,QString> mapKeyValues );

    virtual QPixmap                 getIcon();
    virtual QString                 getFriendlyClassName() { return tr( "Connection" ); }
    virtual ADObject *              getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual AWDataWidget *          getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QMenu *                 getContextMenu( QWidget *pParent );
    virtual DATASystem *            getSystem() { return pSystem; }
    virtual DATAEnvironment *       getEnvironment() { return pEnvironment; }
    virtual DATAConnection *        getConnection() { return pConnection; }
    virtual QMap<QString,QString>   getKeyValues() { return mapKeyValues; }
    virtual bool                    isConnected() { return pConnection->isConnected(); }

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

signals:
    void signalConnected();
    void signalDisconnected();
    void signalMessage( DATAMessage );
    void signalDiagnostic( DATADiagnostic * );

public slots:
    virtual void slotConnectWizard();
    virtual void slotConnect();
    virtual void slotDisconnect();
    virtual void slotDelete();

protected:
    DATASystem *            pSystem;
    DATAEnvironment *       pEnvironment;
    DATAConnection *        pConnection;
    QMap<QString,QString>   mapKeyValues;       // connection 'string'

    virtual QDomElement doSaveConnectString( QDomDocument *pdomDoc );
    virtual void        doLoadConnectString( QDomElement *pdomElem );

    void doDumpKeyValues( const QMap<QString,QString> &mapKeyValues );

protected slots:
    virtual void slotConnected();       // so we can change our state as well
    virtual void slotDisconnected();    // so we can change our state as well
    virtual void slotNewDATAWSql();
    virtual void slotNewCatalogBrowser();
    virtual void slotNewCatalogsBrowser();
    virtual void slotMessage( DATAMessage * );
    virtual void slotDiagnostic( DATADiagnostic * );
};

/*!
 * \brief Connection string (read-only).
 * 
 * Could invoke the DATAWConnectWizard to edit.
 * 
 * \author pharvey (1/14/25)
 */
class DATAWConnectWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWConnectWidget( DATAWConnection *pConnection, QWidget *pWidgetParent );
};

#endif


