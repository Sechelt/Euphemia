#ifndef H_DATAWQueryDataSource
#define H_DATAWQueryDataSource

#include <DATA.h>

#include "DATAWQuery.h"

class DATAEnvironment;

/*!
 * \brief Data Source 
 *  
 * This is a data source as defined in the ODBC system information. 
 *  
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryConnection 
 *  
 * \note doQuery() and doClear() are NOOPS in this case.
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQueryDataSource : public AWObject
{
    Q_OBJECT
public:
    DATAWQueryDataSource( ADObject *pParent, DATAEnvironment *pEnvironment, SQLUSMALLINT nDataSourceType );
    virtual ~DATAWQueryDataSource();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "DataSource" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual QMenu *     getContextMenu( QWidget * );
    virtual bool        hasConnection();
    virtual bool        isConnected();
   
protected slots:
    virtual void slotNewConnection();

private:
    DATAEnvironment *   pEnvironment;
    SQLUSMALLINT        nDataSourceType;
};

#endif

