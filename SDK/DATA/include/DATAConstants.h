#ifndef H_DATAConstants
#define H_DATAConstants

#include <CBD.h>

/*!
 * \brief Constants. 
 *  
 * These constants are useful in providing more readable diagnostic and driver profile information. 
 * Also; we can use a numeric to get a string, then persist the string. In this way the actual numeric value becomes irrelevant - increasing portability. 
 * There is only one, global, instance of this. 
 *  
 * \author pharvey (1/10/21)
 */
class DATAConstants
{
public:
    // features (optional?) not supported
    enum nHYC00
    {
        nHYC00_Bookmarks,                   // SQLBindCol, SQLBindParameter
        nHYC00_FailedToConvert,             // SQLBindCol, SQLBindParameter
        nHYC00_v2Driver,                    // catch-all - attempt to use a feature only available in later drivers     
        nHYC00_BulkAdd,                     // SQLBulkOperations + SQL_ADD
        nHYC00_BulkUpdate,                  // SQLBulkOperations + SQL_UPDATE_BY_BOOKMARK
        nHYC00_BulkDelete,                  // SQLBulkOperations + SQL_DELETE_BY_BOOKMARK
        nHYC00_BulkFetch,                   // SQLBulkOperations + SQL_FETCH_BY_BOOKMARK
        nHYC00_ColAttr,
    };
   
    DATAConstants();

    // This is our single, global, instance. \sa g_Constants
    static DATAConstants *instance();

    QMap<uint,QString> mapStandardsNames;
    QMap<QString,uint> mapStandardsNums;

    QMap<uint,QString> mapFunctionNames;
    QMap<QString,uint> mapFunctionNums;

    QMap<uint,QString> mapInfoNames;
    QMap<QString,uint> mapInfoNums;

    QMap<uint,QString> mapAttrEnvNames;
    QMap<QString,uint> mapAttrEnvNums;

    QMap<uint,QString> mapAttrDbcNames;
    QMap<QString,uint> mapAttrDbcNums;

    QMap<uint,QString> mapAttrStmNames;
    QMap<QString,uint> mapAttrStmNums;

protected:
    void doLoadStandards();
    void doLoadFunctions();
    void doLoadInfo();
    void doLoadAttrEnv();
    void doLoadAttrDbc();
    void doLoadAttrStm();
};

// Simplify access to our single, global, instance.For example;
//  
//          g_Constants->mapFunctionNames 
//
#define g_Constants DATAConstants::instance()

#endif

