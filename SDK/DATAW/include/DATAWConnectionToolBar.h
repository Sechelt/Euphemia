#ifndef H_DATAWConnectionToolBar
#define H_DATAWConnectionToolBar

#include <QtWidgets>

#include <AWEditorWidget.h>

#include <DATASqlWidget.h>
#include <DATAResultSetWidget.h>

class DATAWConnection;
class DATAWSqlWidget;
class DATAWSql;
class ADObject;

class DATAWConnectionComboBox : public QComboBox
{
    Q_OBJECT
public:
    DATAWConnectionComboBox( QWidget *pParent );

protected:
    virtual void showPopup();
};

/*!
 * \brief ToolBar widget for data connection context. 
 *  
 * Provides; 
 *      - a combo box to select a connection
 *      - a button to create a new connection
 *      - a button to toggle connect/disconnect
 *  
 * The combo box of connections is; 
 *      - loaded by calling \sa doAdd()
 *      - is kept up-to-date by calling doAdd() and \sa doRemove()
 *      - changes to a connection name is caught by \sa DATAWConnection::signalModified()
 *  
 * The connection toggle button state is updated by \sa DATAWConnection::signalConnected() and \sa DATAWConnection::signalDisconnected().
 *  
 * A referene to this can be passed to \sa DATAWEditorToolBar so it can determine the active/current connection.  
 *  
 * \note    It is important that connections are removed from the combo box *before* they become invalid/deleted. 
 *          The app can intercept \sa ADObject::signalDestroyed() at g_Root.
 *          We can NOT work directly with g_Root here as it is not valid through the full spectrum of our existence.
 *  
 * \author pharvey (12/23/20)
 */
class DATAWConnectionToolBar : public QToolBar
{
    Q_OBJECT
public:
    DATAWConnectionToolBar( QWidget *pParent );
    ~DATAWConnectionToolBar();

    void setConnection( DATAWConnection *pConnection = nullptr );
    void setActive( bool b = true );

    DATAWConnection *getConnection();

    void doAdd( DATAWConnection * );
    void doRemove( DATAWConnection * );
    void doClear();

signals:
    void signalConnection( DATAWConnection * );     // connection context has changed
    void signalNewConnection();                     // requesting a new connection be created

protected slots:
    void slotConnection( int nIndex );              // translate value and emit signalConnection
    void slotToggleConnect( bool );                 // call DATAWConnection::slotConnect or DATAWConnection::slotDisconnect               
    void slotConnected();                           // catch DATAConnection::signalConnected so we can update pToggleConnect if needed
    void slotDisconnected();                        // catch DATAConnection::signalDisconnected so we can update pToggleConnect if needed
    void slotRemove( QObject *p );                  // catch ADObject::signalDestroyed and translate to doRemove call
    void slotUpdate( ADObject *p );                 // DATAWConnection has been modified so update name

protected:
    bool                        isActive;
    DATAWConnection *           pCurrent;
    DATAWConnectionComboBox *   pConnections;
    QAction *                   pNewConnection;         // connect directly to signalNewConnection
    QAction *                   pToggleConnect;         // connect to slotToggleConnect

    void setCurrent( DATAWConnection * );
    void setCurrent();
};

#endif

