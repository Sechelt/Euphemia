#ifndef H_DATAWQueryIndex
#define H_DATAWQueryIndex

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQueryIndex : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryIndex( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQueryIndex();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Index" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

