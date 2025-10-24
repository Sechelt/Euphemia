#ifndef H_DATAWQueryEISupported
#define H_DATAWQueryEISupported

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEISupported : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEISupported( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEISupported();

    virtual QString     getFriendlyClassName() { return tr( "Supported" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    virtual void doQueryDetails( const QString &s );
};

#endif

