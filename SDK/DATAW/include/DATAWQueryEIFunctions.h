#ifndef H_DATAWQueryEIFunctions
#define H_DATAWQueryEIFunctions

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEIFunctions : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIFunctions( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIFunctions();

    virtual QString     getFriendlyClassName() { return tr( "Functions" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    virtual void doQueryDetails( const QString &s );
};

#endif

