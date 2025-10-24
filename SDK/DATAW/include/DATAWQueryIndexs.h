#ifndef H_DATAWQueryIndexs
#define H_DATAWQueryIndexs

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQueryIndexs : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryIndexs( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQueryIndexs();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Indexs" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

