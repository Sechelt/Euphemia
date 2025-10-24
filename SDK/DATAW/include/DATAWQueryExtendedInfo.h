#ifndef H_DATAWQueryExtendedInfo
#define H_DATAWQueryExtendedInfo

#include "DATAWQueryFolder.h"

class DATAConnection;

/*!
 * \brief Extended info. 
 *  
 * This is the top item for all extended info. 
 * 
 * \author pharvey (1/1/21)
 */
class DATAWQueryExtendedInfo : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryExtendedInfo( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryExtendedInfo();

    virtual QString     getFriendlyClassName() { return tr( "ExtendedInfo" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;
};

#endif

