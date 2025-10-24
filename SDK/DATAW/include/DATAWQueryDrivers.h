#ifndef H_DATAWQueryDrivers
#define H_DATAWQueryDrivers

#include "DATAWQuery.h"

class DATAEnvironment;

/*!
 * \brief A container with drivers.
 * 
 * Children it can produce are; 
 *  
 * 0-n  \sa DATAWQueryDriver 
 *  
 * \note The doClear() does not delete children that have an active connection.
 *  
 * \author pharvey (1/2/21)
 */
class DATAWQueryDrivers : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQueryDrivers( ADObject *pParent, DATAEnvironment *pEnvironment );
    virtual ~DATAWQueryDrivers();

    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Drivers" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();
    virtual void doClear();

private:
    DATAEnvironment *pEnvironment;
};

#endif

