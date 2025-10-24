#ifndef H_DATAWQueryConnection
#define H_DATAWQueryConnection

#include <DATA.h>

#include "DATAWQuery.h"

class DATAEnvironment;
class DATAConnection;

/*!
 * \brief Database connection. 
 *  
 * This can be based upon a data source name or a driver. 
 * This may be in a connected or disconnected state. 
 * 
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryCatalog 
 * 0-1  \sa DATAWQueryExtendedInfo 
 *  
 * \note doQuery() is a NOOP unless this is connected to the data source. 
 *  
 * \author pharvey (1/2/21)
 */
class DATAWQueryConnection : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryConnection( ADObject *pParent, DATAEnvironment *pEnvironment );
    virtual ~DATAWQueryConnection();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Connection" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual DATAConnection *getConnection() { return pConnection;   }
    virtual QMenu *         getContextMenu( QWidget * );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QStringList     getAutoCompletions();
    virtual QStringList     getAutoCompletionsFlat();
    virtual bool            isConnected();
    virtual bool            canLoadChildren() { return !bQueried && isConnected(); }

    virtual void doQuery();
   
protected slots:
    // Connection
    virtual void slotConnect();         // ask DATAConnection to connect
    virtual void slotDisconnect();      // ask DATAConnection to disconnect
    virtual void slotConnected();       // DATAConnection connected
    virtual void slotDisconnected();    // DATAConnection disconnected
    // Other
    virtual void slotAllCatalogs( bool );
    virtual void slotGenerateProfile();

protected:
    DATAEnvironment *   pEnvironment;
    DATAConnection *    pConnection;
    bool                bAllCatalogs;

    virtual void doConnect() = 0;
    virtual void doDisconnect();
};

#endif

