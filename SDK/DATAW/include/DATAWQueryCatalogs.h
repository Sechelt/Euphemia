#ifndef H_DATAWQueryCatalogs
#define H_DATAWQueryCatalogs

#include "DATAWQuery.h"

class DATAConnection;

/*!
 * \brief Catalogs. 
 *  
 * Possible children; 
 *  
 * 0-n  \sa DATAWQueryCatalog 
 * 
 * \author pharvey (1/2/21)
 */
class DATAWQueryCatalogs : public DATAWQuery
{
    Q_OBJECT
 public:
    DATAWQueryCatalogs( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryCatalogs();

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

