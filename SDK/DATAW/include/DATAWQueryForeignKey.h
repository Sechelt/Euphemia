#ifndef H_DATAWQueryForeignKey
#define H_DATAWQueryForeignKey

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQueryForeignKey : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryForeignKey( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQueryForeignKey();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "ForeignKey" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

