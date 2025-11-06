#ifndef H_DATAProfile
#define H_DATAProfile

#include <QDomDocument>

#include <sqlite3.h> 

#include "DATA.h"

class DATAConnection;
class DATAInfo;
class DATADataTypeSpec;
class DATAFunction;

/*!
 * \brief Logged call that returned HYC00 (or otherwise indicated call is unsupported). 
 *  
 * Hopefully; we can use this information to work around making this call again. 
 *  
 * For critical calls this will indicate an alternate means is needed and for all 
 * else it will simply reduce superfluous diagnostics. 
 *  
 * \author pharvey (7/12/20)
 */
class DATAUnsupported
{
public:
    DATAUnsupported( const QStringList &listOptions, const QString &stringContext = QString() )
    {
        this->listOptions = listOptions;
        this->stringContext = stringContext;
    }
    DATAUnsupported()
    {
    }

    QStringList     listOptions;
    QString         stringContext;
};


/*!
 * \brief Used to store SQLGetFunctions() result.
 * 
 * \author pharvey (2020-06-08)
 */
class DATAFunction
{
public:
    DATAFunction( const QString &stringFunction, int nFunction, bool bSupported, CBD::Standards nCompliance )
    {
        this->stringFunction    = stringFunction;
        this->nFunction         = nFunction;
        this->bSupported        = bSupported;
        this->nCompliance       = nCompliance;
    }

    DATAFunction()
    {
    }

    QString         stringFunction;
    int             nFunction;
    bool            bSupported;
    CBD::Standards  nCompliance;
    QList<DATAUnsupported> listUnsupported;
};

/*!
 * \brief Info retreived via SQLGetInfo.
 *  
 * Some values are bit-masks. In this case DATAInfo will have a child DATAInfo/object for each of the possible   
 *  
 * \author pharvey (6/12/20)
 */
class DATAInfo
{
public:
    uint            nInfo;
    QString         stringInfo;
    QVariant        vValue;
    QString         stringDescription;

    // for when fixed possible values that may be 0-1 or 0-n
    // vValue will be a "" or a "Y"
    QMap<QString,DATAInfo*> mapValues;

    DATAInfo( uint nInfo, const QString &stringInfo, const QVariant &vValue = QVariant(), const QString &stringDescription = QString() );
    DATAInfo();

    bool                            isSupported( const QString &s );
    DATAInfo *                      getSupported();
    static QString                  getValueEncoded( DATAInfo *pInfo );
    static QMap<QString,DATAInfo*>  getValueDecoded( const QString &stringValue );
};

class DATAAttr
{
public:
    uint            nAttr;
    QString         stringAttr;
    QVariant        vValue;
    QString         stringDescription;

    // when value is a mask
    QMap<QString,DATAAttr*> mapValues;

    DATAAttr( uint nAttr, const QString &stringAttr, const QVariant &vValue = QVariant(), const QString &stringDescription = QString() );
    DATAAttr();

    static QString getValueEncoded( DATAAttr *pAttr );
};

/*!
 * \brief Profile of a Data Source.
 *  
 * The Profile is built from a connection and updated during use. 
 * This Profile is saved to Profile.db in the Users 'home' directory. 
 * The resulting Profile.db can be used by System Architect when creating a Physical Model with no 
 * need for the workstation to be configured (ODBC drivers installed and a connection made etc) for the target.  
 *  
 * Base Profile 
 *  
 * Most of this is from calling; \sa SQLGetInfo, \sa SQLGetTypeInfo, and \sa SQLGetFunctions after/during a connection. 
 * Some of it is inferred by mapInfo["SQL_SQL_CONFORMANCE"]. 
 *  
 * Updated Profile 
 *  
 * The updates come from using a connection. For example; calls that return HYC00 (unsupported) are logged and can be 
 * avoided in the future.
 *  
 * \author pharvey (2020-06-28)
 */

class DATAProfile
{
public:
    DATAProfile( sqlite3 *pDatabase, const QString &stringKey );    // empty profile
    DATAProfile( sqlite3 *pDatabase, DATAConnection *pConnection ); // load from DATAConnection
    DATAProfile( sqlite3 *pDatabase, int nID );                     // load from sqlite3
    ~DATAProfile();

    // client OS 
    // can be different when loaded from a profile database but normally same (default)
    QString                         stringOS;           // QSysInfo::productType();
    QString                         stringOS_VER;       // QSysInfo::productVersion();
    //
    QString                         stringKey;
    QMap<QString,DATAInfo*>         mapInfo;            // key = QString; ie "SQL_DATA_SOURCE_NAME"; from SQLGetInfo

    // DATA TYPES
    // Here we store supported data types as returned by SQLGetTypeInfo.
    // TYPE_NAME is unique while DATA_TYPE can be duplicated so we use TYPE_NAME as our key.
    // NOTE: Unfortunately; SQLColumns:TYPE_NAME may not always match SQLGetTypeInfo:TYPE_NAME (depends upon the driver) so a straight xref may not always be possible.
    QMap<QString,DATADataTypeSpec*> mapDataTypes;       // key = SQLGetTypeInfo:TYPE_NAME

    QMap<int,DATAFunction*>         mapFunctions;       // key = int; ie SQL_API_SQLCONNECT; from SQLGetFunctions (also includes DATAUnsupported)
    QMap<QString,DATAAttr*>         mapAttr;            // key = QString; ie "SQL_ATTR_AUTOCOMMIT"; from SQLGetConnectAttr

    void setModified( bool b = true ) { bModified = b; }
    bool isModified() { return bModified; }

    DATADataTypeSpec *getDataTypeSpec( const QString &stringTYPE_NAME ); // usually better than going after mapDataTypes directly

    // save to profiles database
    bool doSave();
    // save/load to XML
    QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    bool doLoad( QDomElement *pdomElemThis );

protected:
    bool                bModified;
    sqlite3 *           pDatabase;

    bool                getID( int *pnID );

    void doClear();

    bool doLoad( int nID );
    bool doLoadInfo( int nID );
    bool doLoadDataTypes( int nID );
    bool doLoadFunctions( int nID );
    bool doLoadAttrs( int nID );

    bool doLoadInfo( QDomElement *pdomElem );
    bool doLoadDataTypes( QDomElement *pdomElem );

    bool doInsertDataTypes( int nID );
    bool doInsertFunctions( int nID );
    bool doInsertFunction( int nID, DATAFunction * );
    bool doInsertInfos( int nID );
    bool doInsertInfo( int nID, DATAInfo * );
    bool doInsertAttrDbcs( int nID );
    bool doInsertUnsupported( int nID );
    bool doInsertUnsupported( int nID, DATAFunction * );
    bool doInsertUnsupported( int nID, const QString &stringFunction, const DATAUnsupported &Uns );

    bool doDelete();                    // Profiles
    bool doDeleteInfo( int nID );       // Info
    bool doDeleteDataTypes( int nID );  // DataTypes
    bool doDeleteFunctions( int nID );  // Functions
    bool doDeleteAttrs( int nID );      // AttrDbc
    bool doDeleteUnsupported( int nID );// HYC00

    QMap<QString,DATAInfo*>         getInfo( DATAConnection *pConnection );
    DATAInfo *                      getInfoData( DATAConnection *pConnection, SQLUSMALLINT nInfoType, SQLRETURN *pnReturn = nullptr );
    QMap<QString,DATADataTypeSpec*> getDataTypes( DATAConnection *pConnection );
    QMap<int,DATAFunction*>         getFunctions( DATAConnection *pConnection, SQLRETURN *pnReturn = nullptr );
    QMap<QString,DATAAttr*>         getAttr( DATAConnection *pConnection );
    DATAAttr *                      getAttrData( DATAConnection *pConnection, SQLUSMALLINT nAttrType, SQLRETURN *pnReturn = nullptr );
    QMap<QString,DATAInfo*>         getConvertValues( const QVariant &v );
};

#endif

