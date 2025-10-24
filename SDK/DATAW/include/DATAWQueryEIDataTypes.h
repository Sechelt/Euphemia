#ifndef H_DATAWQueryEIDataTypes
#define H_DATAWQueryEIDataTypes

#include "DATAWQueryFolder.h"

class DATAConnection;
class DATADataTypeSpec;

class DATAWQueryEIDataTypes : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIDataTypes( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIDataTypes();

    virtual QString     getFriendlyClassName() { return tr( "DataTypes" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    void doQueryDetails( ADObject *pParent, DATADataTypeSpec *pDataTypeSpec );
};

#endif

