#ifndef H_DATAWQueryEIDBMS
#define H_DATAWQueryEIDBMS

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEIDBMS : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIDBMS( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIDBMS();

    virtual QString     getFriendlyClassName() { return tr( "DBMS" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;
};

#endif

