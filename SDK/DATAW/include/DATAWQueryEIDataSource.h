#ifndef H_DATAWQueryEIDataSource
#define H_DATAWQueryEIDataSource

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEIDataSource : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIDataSource( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIDataSource();

    virtual QString     getFriendlyClassName() { return tr( "DataSource" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;
};

#endif

