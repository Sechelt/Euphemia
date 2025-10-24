#ifndef H_DATAWQueryEICallLevelInterface
#define H_DATAWQueryEICallLevelInterface

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEICallLevelInterface : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEICallLevelInterface( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEICallLevelInterface();

    virtual QString     getFriendlyClassName() { return tr( "CallLevelInterface" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;
};

#endif

