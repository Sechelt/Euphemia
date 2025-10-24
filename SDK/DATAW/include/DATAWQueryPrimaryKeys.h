#ifndef H_DATAWQueryPrimaryKeys
#define H_DATAWQueryPrimaryKeys

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQueryPrimaryKeys : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryPrimaryKeys( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQueryPrimaryKeys();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "PrimaryKeys" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

