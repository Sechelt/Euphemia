#ifndef H_DATAWQueryCatalog
#define H_DATAWQueryCatalog

#include "DATAWQuery.h"

class DATAConnection;

/*!
 * \brief Catalog
 * 
 * SQL_CATALOG_TERM will indicate how this is used. For example;
 * 
 * "database"       - is a database
 * "directory"      - is a directory
 * ""               - not supported
 *  
 * Possible children; 
 *  
 * 0-n  \sa DATAWQuerySchema 
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQueryCatalog : public DATAWQuery
{
    Q_OBJECT
 public:
    DATAWQueryCatalog( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryCatalog();

    virtual QPixmap     getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Catalog" ); }
    virtual QMenu *     getContextMenu( QWidget * ); 
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

private:
    DATAConnection *pConnection;
};

#endif

