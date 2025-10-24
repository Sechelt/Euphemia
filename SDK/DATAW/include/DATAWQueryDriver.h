#ifndef H_DATAWQueryDriver
#define H_DATAWQueryDriver

#include "DATAWQuery.h"

class DATAEnvironment;

/*!
 * \brief An ODBC driver. 
 *  
 * This is a driver as defined in the ODBC system information. 
 *  
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryConnection 
 *  
 * \note doQuery() and doClear() are NOOPS in this case.
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQueryDriver : public AWObject
{
    Q_OBJECT
public:
    DATAWQueryDriver( ADObject *pParent, DATAEnvironment *pEnvironment );
    virtual ~DATAWQueryDriver();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Driver" ); }
    virtual QMenu *     getContextMenu( QWidget *pParent );
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual bool        hasConnection();
    // virtual bool        isConnected();

protected slots:
    virtual void slotNewConnection();

private:
    DATAEnvironment *pEnvironment;
};

#endif

