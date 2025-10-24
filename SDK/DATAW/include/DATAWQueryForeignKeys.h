#ifndef H_DATAWQueryForeignKeys
#define H_DATAWQueryForeignKeys

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQueryForeignKeys : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryForeignKeys( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQueryForeignKeys();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "ForeignKeys" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

