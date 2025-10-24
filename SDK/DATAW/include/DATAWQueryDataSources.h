#ifndef H_DATAWQueryDataSources
#define H_DATAWQueryDataSources

#include <DATASystem.h>

#include "DATAWQuery.h"

class DATAEnvironment;

/*!
 * \brief A container with data sources of the specified type.
 * 
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryDataSource 
 *  
 * \note The doClear() does not delete children that have an active connection.
 *  
 * \author pharvey (1/2/21)
 */
class DATAWQueryDataSources : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryDataSources( ADObject *pParent, DATAEnvironment *pEnvironment, DATASystem::ConfigModes n );
    virtual ~DATAWQueryDataSources();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "DataSources" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();
    virtual void doClear();

private:
    DATAEnvironment *       pEnvironment;
    DATASystem::ConfigModes nDataSourceType;
};

#endif

