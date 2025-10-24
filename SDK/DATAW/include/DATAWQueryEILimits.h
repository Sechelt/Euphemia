#ifndef H_DATAWQueryEILimits
#define H_DATAWQueryEILimits

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;

class DATAWQueryEILimits : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEILimits( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEILimits();

    virtual QString     getFriendlyClassName() { return tr( "Limits" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    virtual void doQueryDetails( const QString &s );
};

#endif

