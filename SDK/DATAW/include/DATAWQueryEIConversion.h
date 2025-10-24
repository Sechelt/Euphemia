#ifndef H_DATAWQueryEIConversion
#define H_DATAWQueryEIConversion

#include <DATA.h>

#include "DATAWQueryFolder.h"

class DATAConnection;
class DATAWQueryExtendedInfoItem;

class DATAWQueryEIConversion : public DATAWQueryFolder
{
    Q_OBJECT
public:
    DATAWQueryEIConversion( ADObject *pParent, DATAConnection *pConnection );
    virtual ~DATAWQueryEIConversion();

    virtual QString     getFriendlyClassName() { return tr( "Conversion" ); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    virtual void doQuery();

protected:
    DATAConnection *pConnection;

    void doLoadConversionDetails( SQLUINTEGER nBitMask, ADObject *pItem );
};

#endif

