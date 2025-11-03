#ifndef H_DATAWQuerySchema
#define H_DATAWQuerySchema

#include "DATAWQuery.h"

class DATAConnection;

/*!
 * \brief Schema
 * 
 * SQL_SCHEMA_TERM will indicate how this is used but conformant driver will always return 'schema' (supported) or '' (not supported).
 *  
 * Possible children; 
 *  
 * 0-n  \sa DATAWQueryTableType 
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQuerySchema : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQuerySchema( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog );
    virtual ~DATAWQuerySchema();

    virtual QPixmap     getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Schema" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
    QString         stringCatalog;
};

#endif

