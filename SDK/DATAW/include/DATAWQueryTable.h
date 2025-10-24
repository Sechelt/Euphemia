#ifndef H_DATAWQueryTable
#define H_DATAWQueryTable

#include "DATAWQuery.h"

class DATAConnection;

/*!
 * \brief A database table. 
 *  
 * This represents a table of the specified type.
 *  
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryColumn 
 * 1    \sa DATAWQueryPrimaryKeys
 * 1    \sa DATAWQueryIndexs 
 *  
 * \author pharvey (1/1/21)
 */
class DATAWQueryTable : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryTable( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringType ); 
    virtual ~DATAWQueryTable();

    virtual QPixmap     getIcon() override;
    virtual QString     getFriendlyClassName() override { return tr( "Table" ); }
    virtual QMenu *     getContextMenu( QWidget * ) override;
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;

    virtual void doGoTo( ADObjectPathList listObjectPathItems, uint nDepth ) override;
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth ) override;
    virtual void doGoTo() override;
    virtual void doQuery() override;

public slots:
    virtual bool slotOpenEditor() override;

private:
    DATAConnection *    pConnection;
    QString             stringCatalog;
    QString             stringSchema;
    QString             stringType;
};

#endif

