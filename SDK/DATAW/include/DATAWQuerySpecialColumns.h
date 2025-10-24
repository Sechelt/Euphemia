#ifndef H_DATAWQuerySpecialColumns
#define H_DATAWQuerySpecialColumns

#include "DATAWQuery.h"

class DATAConnection;

class DATAWQuerySpecialColumns : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQuerySpecialColumns( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    virtual ~DATAWQuerySpecialColumns();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "SpecialColumns" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
    QString         stringSchema;
    QString         stringTable;
};

#endif

