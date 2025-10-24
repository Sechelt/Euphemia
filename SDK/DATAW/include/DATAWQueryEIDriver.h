#ifndef H_DATAWQueryEIDriver
#define H_DATAWQueryEIDriver

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEIDriver : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIDriver( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIDriver();

    virtual QString     getFriendlyClassName() { return tr( "Driver" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    virtual void doQueryDetails( const QString &s );
};

#endif

