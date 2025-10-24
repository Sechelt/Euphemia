#ifndef H_DATAWQueryTableType
#define H_DATAWQueryTableType

#include "DATAWQuery.h"

class DATAConnection;

/*!
 * \brief Table type. 
 *  
 * This represents one of the table types as specified with setType(). 
 *  
 * The children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryTable 
 *  
 * \author pharvey (1/1/21)
 */
class DATAWQueryTableType : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryTableType( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema );
    virtual ~DATAWQueryTableType();

    virtual QPixmap     getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "TableType" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *    pConnection;
    QString             stringCatalog;
    QString             stringSchema;
};

#endif

