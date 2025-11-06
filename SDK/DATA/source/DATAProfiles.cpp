#include "LibInfo.h"
#include "DATAProfiles.h"

#include "DATAConnection.h"
#include "DATAProfile.h"

DATAProfiles::DATAProfiles()
{
    stringFileName  = "ODBCProfiles.db";
    pDatabase       = nullptr;

    listStandardProfileKeys << "ODBCMin" << "ODBCCor" << "ODBCExt" << "SQL92" << "SQLite" << "MSA";

    /* Get a file name with a complete path. 
     * The location is platform specific. 
     * Examples;
     *      Linux: ~/.local/share/
     */
    QString stringDir = QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation );
    if ( stringDir.isEmpty() ) stringDir = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
    if ( !stringDir.isEmpty() )
    { 
        QFileInfo f( QDir( stringDir ), stringFileName );
        stringFileName = f.absoluteFilePath(); 
    }

    // open the database (creating it if need be) and keep it open
    if ( existsDatabase( stringFileName ) ) 
        pDatabase = doOpenDatabase( stringFileName );
    else
        pDatabase = doCreateDatabase( stringFileName );
}

DATAProfiles::~DATAProfiles()
{
    // close the database
    if ( pDatabase ) pDatabase = doCloseDatabase( pDatabase );
    doClear();
}

/*!
 * \brief Provides our global instance.
 * 
 * \author pharvey (1/14/21)
 * 
 * \return DATAProfiles* 
 */
DATAProfiles* DATAProfiles::instance()
{
    static DATAProfiles instance;
    return &instance;
}

/*!
 * \brief Get a profile. 
 *  
 * Simply return a ref to the cached profile when exists in the cache. 
 * When not in cache - we load from the profiles database and add to cache. 
 * When we fail to get the profile we will try to load from our internal standards. 
 * When we fail this we will load from pConnection but only if it is isConnected. 
 * When we fail this we will create an empty profile but only if bCreate. 
 * We return nullptr if we fail to find/load/create a profile.  
 * 
 * \author pharvey (1/20/21)
 * 
 * \param pConnection 
 * \param bCreate 
 * 
 * \return DATAProfile* 
 */
DATAProfile *DATAProfiles::getProfile( DATAConnection *pConnection, bool bCreate )
{
    QString stringKey = pConnection->getProfileKey();

    // return from cache
    if ( mapProfiles.contains( stringKey ) ) return mapProfiles.value( stringKey );
    // load from profile database
    if ( doLoad( stringKey ) ) return mapProfiles.value( stringKey );
    // internal data for standard profiles
    if ( listStandardProfileKeys.contains( stringKey ) && doLoadStandard( stringKey ) ) return mapProfiles.value( stringKey );

    // create from connection
    if ( pConnection->isConnected() )
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, pConnection );
        pProfile->setModified();
        mapProfiles[stringKey] = pProfile;
        return pProfile;
    }

    // empty profile
    if ( bCreate )
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, stringKey );
        pProfile->setModified();
        mapProfiles[stringKey] = pProfile;
        return pProfile;
    }

    // fail
    return nullptr;
}

/*!
 * \brief Get a profile. 
 *  
 * Simply return a ref to the cached profile when exists in the cache.
 * When not in cache - we load from the profiles database and add to cache. 
 * When we fail to get the profile we will try to load from our internal standards. 
 * When we fail this we will create an empty profile but only if bCreate. 
 * We return nullptr if we fail to find/load/create a profile.  
 * 
 * \author pharvey (1/20/21)
 * 
 * \param stringKey 
 * \param bCreate 
 * 
 * \return DATAProfile* 
 */
DATAProfile *DATAProfiles::getProfile( const QString &stringKey, bool bCreate )
{
    // return from cache
    if ( mapProfiles.contains( stringKey ) ) return mapProfiles.value( stringKey );

    // load from profile database
    if ( doLoad( stringKey ) ) return mapProfiles.value( stringKey );

    // internal data for standard profiles
    if ( listStandardProfileKeys.contains( stringKey ) && doLoadStandard( stringKey ) ) return mapProfiles.value( stringKey );
    // empty profile
    if ( bCreate )
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, stringKey );
        pProfile->setModified();
        mapProfiles[stringKey] = pProfile;
        return pProfile;
    }
    // fail
    return nullptr;
}

/*!
 * \brief Saves the in-memory profiles to disk. 
 *  
 * The profiles are saved to a SQLite3 database. 
 * The database will be *replaced* each time this call is made.  
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
bool DATAProfiles::doSave()
{
    printf( "[PAH][%s][%s][%d] Not saving g_Profiles while in development.\n", __FILE__, __FUNCTION__, __LINE__ );
    return false;

    bool bReturn = true;
    int nProgress = 0;
    QProgressDialog Progress( nullptr, Qt::Popup );
    Progress.setCancelButton( nullptr );
    Progress.setMaximum( mapProfiles.count() );
    Progress.setLabelText( "Saving profiles..." );
    qApp->processEvents();
    QMapIterator<QString,DATAProfile*> i(mapProfiles);
    while ( i.hasNext() ) 
    {
        i.next();
        DATAProfile *p = i.value();
        if ( !p->isModified() ) continue; 
        Progress.setLabelText( p->stringKey );
        Progress.setValue( nProgress++ );
        qApp->processEvents();
        bReturn = p->doSave();
        if ( !bReturn ) break;
    }

    return bReturn;
}

/*!
 * \brief Clears all profiles from memory. 
 *  
 * Does *not* clear any existing profiles database. 
 * 
 * \author pharvey (1/14/21)
 */
void DATAProfiles::doClear()
{
    qDeleteAll( mapProfiles ); mapProfiles.clear();
}

/*!
 * \brief Returned true if the profiles database exists.
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
bool DATAProfiles::existsDatabase( const QString &stringFileName )
{
    return QFileInfo::exists( stringFileName ) && QFileInfo( stringFileName ).isFile();
}

/*!
 * \brief Open database. 
 *  
 * The database file is created (if it does not exist) but we should be calling \sa doCreateDatabase for that. 
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
sqlite3 *DATAProfiles::doOpenDatabase( const QString &stringFileName )
{
    sqlite3 *pDatabase = nullptr;

    if ( sqlite3_open( stringFileName.toLatin1().constData(), &pDatabase ) )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return nullptr;
    }

    return pDatabase;
}

/*!
 * \brief Close database. 
 *  
 * Simply closes the database.
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
sqlite3 *DATAProfiles::doCloseDatabase( sqlite3 *pDatabase )
{
    Q_ASSERT( pDatabase );
    sqlite3_close( pDatabase );
    return nullptr;
}

/*!
 * \brief Create database. 
 *  
 * We open the database (thereby creating the file) and then create the database tables etc. 
 * The database is then closed. 
 *  
 * \note The database must not exist.
 *  
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
sqlite3 *DATAProfiles::doCreateDatabase( const QString &stringFileName )
{
    Q_ASSERT( !existsDatabase( stringFileName ) );

    // create the database file
    sqlite3 *pDatabase = doOpenDatabase( stringFileName );

    // create the tables etc
    char *pszErrMsg = 0;
    const char *pszSQL = \
            "CREATE TABLE DataTypes                         "\
            "(                                              "\
            "  ProfileID integer NOT NULL,                  "\
            "  TYPE_NAME varchar(50) NOT NULL,              "\
            "  nDATA_TYPE integer NOT NULL,                 "\
            "  DATA_TYPE varchar(50) NOT NULL,              "\
            "  COLUMN_SIZE varchar(50) NOT NULL,            "\
            "  LITERAL_PREFIX varchar(50),                  "\
            "  LITERAL_SUFFIX varchar(50),                  "\
            "  CREATE_PARAMS varchar(255),                  "\
            "  NULLABLE varchar(50) NOT NULL,               "\
            "  CASE_SENSITIVE varchar(50) NOT NULL,         "\
            "  SEARCHABLE varchar(50) NOT NULL,             "\
            "  UNSIGNED_ATTRIBUTE varchar(50) NOT NULL,     "\
            "  FIXED_PREC_SCALE varchar(50) NOT NULL,       "\
            "  AUTO_UNIQUE_VALUE varchar(50) NOT NULL,      "\
            "  LOCAL_TYPE_NAME varchar(50),                 "\
            "  MINIMUM_SCALE varchar(50),                   "\
            "  MAXIMUM_SCALE varchar(50),                   "\
            "  SQL_DATA_TYPE varchar(50) NOT NULL,          "\
            "  SQL_DATETIME_SUB varchar(50),                "\
            "  NUM_PREC_RADIX varchar(50),                  "\
            "  INTERVAL_PRECISION varchar(50),              "\
            "  Desc longvarchar(32768),                     "\
            "  Syntax varchar(255)                          "\
            ");                                             "\
            "CREATE INDEX idxDataTypes1 ON DataTypes        "\
            "(                                              "\
            "  ProfileID                                    "\
            ");                                             "\
            "CREATE TABLE Info                              "\
            "(                                              "\
            "  ProfileID integer NOT NULL,                  "\
            "  Info varchar(50) NOT NULL,                   "\
            "  Value longvarchar(32768),                    "\
            "  PRIMARY KEY (ProfileID,Info)                 "\
            ");                                             "\
            "CREATE INDEX idxInfo2 ON Info                  "\
            "(                                              "\
            "  ProfileID                                    "\
            ");                                             "\
            "CREATE TABLE Functions                         "\
            "(                                              "\
            "  ProfileID integer NOT NULL,                  "\
            "  Function varchar(50) NOT NULL,               "\
            "  Supported bit DEFAULT 1 NOT NULL,            "\
            "  Compliance varchar(50) NOT NULL,             "\
            "  PRIMARY KEY (ProfileID,Function)             "\
            ");                                             "\
            "CREATE INDEX idxFunctions2 ON Functions        "\
            "(                                              "\
            "  ProfileID                                    "\
            ");                                             "\
            "CREATE TABLE Profiles                          "\
            "(                                              "\
            "  ID integer NOT NULL,                         "\
            "  KEY varchar(255) NOT NULL,                   "\
            "  OS varchar(50) NOT NULL,                     "\
            "  OS_VER varchar(50) NOT NULL,                 "\
            "  PRIMARY KEY (ID)                             "\
            ");                                             "\
            "CREATE UNIQUE INDEX idxProfiles2 ON Profiles   "\
            "(                                              "\
            "  KEY                                          "\
            ");                                             "\
            "CREATE TABLE AttrDbc                           "\
            "(                                              "\
            "  ProfileID integer NOT NULL,                  "\
            "  Attr varchar(50) NOT NULL,                   "\
            "  Value longvarchar(32768),                    "\
            "  PRIMARY KEY (ProfileID,Attr)                 "\
            ");                                             "\
            "CREATE INDEX idxAttrDbc2 ON AttrDbc            "\
            "(                                              "\
            "  ProfileID                                    "\
            ");                                             "\
            "CREATE TABLE HYC00                             "\
            "(                                              "\
            "  ProfileID integer NOT NULL,                  "\
            "  Function varchar(50) NOT NULL,               "\
            "  Option1 varchar(50),                         "\
            "  Option2 varchar(50),                         "\
            "  Option3 varchar(50),                         "\
            "  Context varchar(255),                        "\
            "  PRIMARY KEY (ProfileID,Function)             "\
            ");                                             "\
            "CREATE INDEX idxHYC002 ON HYC00                "\
            "(                                              "\
            "  ProfileID                                    "\
            ");";

    int nRetCode = sqlite3_exec( pDatabase, pszSQL, NULL, 0, &pszErrMsg);

    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << pszErrMsg;
        sqlite3_free( pszErrMsg );
        doCloseDatabase( pDatabase );
        return nullptr;
    }
                                               
    return pDatabase;                                
}

/*!
 * \brief Drop database. 
 *  
 * This means removing/deleteing the database file. 
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
bool DATAProfiles::doDropDatabase( const QString &stringFileName )
{
    QFile::remove( stringFileName );
    return true;
}

/*!
 * \brief Load profile from the profiles database.
 * 
 * \author pharvey (1/14/21)
 * 
 * \return bool 
 */
bool DATAProfiles::doLoad( const QString &stringKey )
{
    Q_ASSERT( !mapProfiles.contains( stringKey ) );
    Q_ASSERT( pDatabase );

    printf( "[PAH][%s][%s][%d] Not loading saved profiles while in development.\n", __FILE__, __FUNCTION__, __LINE__ );
    return false;

    QString stringSQL = QString( "SELECT * FROM Profiles WHERE KEY = '%1'" ).arg( stringKey );
    sqlite3_stmt *pStatement;
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    // process results
    nRetCode = sqlite3_step( pStatement );
    while ( nRetCode != SQLITE_DONE )
    {
        // should never be busy
        Q_ASSERT( nRetCode != SQLITE_BUSY );

        // error or simply no data?
        if ( nRetCode != SQLITE_ROW )
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
            sqlite3_finalize( pStatement );
            return false;
        }

        // get data
        int nID = sqlite3_column_int( pStatement, 0 );
        sqlite3_finalize( pStatement );
        mapProfiles.insert( stringKey, new DATAProfile( pDatabase, nID ) );
        return true;
    }
    return false;
}

bool DATAProfiles::doLoadStandard( const QString &stringKey )
{
    Q_ASSERT( !mapProfiles.contains( stringKey ) );
    Q_ASSERT( listStandardProfileKeys.contains( stringKey ) );

    if ( stringKey == "ODBCMin" ) 
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, stringKey );
        doLoadODBCMin( pProfile );
        pProfile->setModified();
        mapProfiles["ODBCMin"] = pProfile;
    }
    else if ( stringKey == "ODBCCor" )
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, stringKey );
        doLoadODBCMin( pProfile );
        doLoadODBCCor( pProfile );
        pProfile->setModified();
        mapProfiles["ODBCCor"] = pProfile;
    }
    else if ( stringKey == "ODBCExt" )
    {
        DATAProfile *pProfile = new DATAProfile( pDatabase, stringKey );
        doLoadODBCMin( pProfile );
        doLoadODBCCor( pProfile );
        doLoadODBCExt( pProfile );
        pProfile->setModified();
        mapProfiles["ODBCExt"] = pProfile;
    }
    else if ( stringKey == "SQL92" ) doLoadSQL92();
    else if ( stringKey == "SQLite" ) doLoadSQLite();
    else if ( stringKey == "MSA" ) doLoadMSA();
    else
    {
        Q_ASSERT_X( 1 == 2, __FUNCTION__, stringKey.toLatin1().constData() );
    }

    return true;
}

bool DATAProfiles::doLoadODBCMin( DATAProfile *pProfile )
{
    // configure driver/data-source info 
    // - we fake calling SQLGetInfo for a driver/data-source having this compliance level
    // - we only include items that are of interest in creating a Physical Model incl. DDL
    // - this is base data and thus - may be altered by other methods
    pProfile->mapInfo["SQL_ALTER_TABLE"]                      = new DATAInfo( SQL_ALTER_TABLE, "SQL_ALTER_TABLE" );
    pProfile->mapInfo["SQL_CATALOG_LOCATION"]                 = new DATAInfo( SQL_CATALOG_LOCATION, "SQL_CATALOG_LOCATION" );
    pProfile->mapInfo["SQL_CATALOG_NAME"]                     = new DATAInfo( SQL_CATALOG_NAME, "SQL_CATALOG_NAME", "N" );
    pProfile->mapInfo["SQL_CATALOG_NAME_SEPARATOR"]           = new DATAInfo( SQL_CATALOG_NAME_SEPARATOR, "SQL_CATALOG_NAME_SEPARATOR" );
    pProfile->mapInfo["SQL_CATALOG_TERM"]                     = new DATAInfo( SQL_CATALOG_TERM, "SQL_CATALOG_TERM" );
    pProfile->mapInfo["SQL_COLUMN_ALIAS"]                     = new DATAInfo( SQL_COLUMN_ALIAS, "SQL_COLUMN_ALIAS", "N" );
    pProfile->mapInfo["SQL_CREATE_ASSERTION"]                 = new DATAInfo( SQL_CREATE_ASSERTION, "SQL_CREATE_ASSERTION" );
    pProfile->mapInfo["SQL_CREATE_CHARACTER_SET"]             = new DATAInfo( SQL_CREATE_CHARACTER_SET, "SQL_CREATE_CHARACTER_SET" );
    pProfile->mapInfo["SQL_CREATE_COLLATION"]                 = new DATAInfo( SQL_CREATE_COLLATION, "SQL_CREATE_COLLATION" );
    pProfile->mapInfo["SQL_CREATE_DOMAIN"]                    = new DATAInfo( SQL_CREATE_DOMAIN, "SQL_CREATE_DOMAIN" );
    pProfile->mapInfo["SQL_CREATE_SCHEMA"]                    = new DATAInfo( SQL_CREATE_SCHEMA, "SQL_CREATE_SCHEMA" );
    {
        DATAInfo *pInfo = new DATAInfo(SQL_CREATE_TABLE, "SQL_CREATE_TABLE", 0 );
        pInfo->mapValues.insert( "SQL_CT_CREATE_TABLE", new DATAInfo( SQL_CT_CREATE_TABLE, "SQL_CT_CREATE_TABLE", "Y" ) );
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    pProfile->mapInfo["SQL_CREATE_TRANSLATION"]               = new DATAInfo( SQL_CREATE_TRANSLATION, "SQL_CREATE_TRANSLATION" );
    pProfile->mapInfo["SQL_CREATE_VIEW"]                      = new DATAInfo( SQL_CREATE_VIEW, "SQL_CREATE_VIEW" );
    pProfile->mapInfo["SQL_DDL_INDEX"]                        = new DATAInfo( SQL_DDL_INDEX, "SQL_DDL_INDEX" );
    pProfile->mapInfo["SQL_DROP_ASSERTION"]                   = new DATAInfo( SQL_DROP_ASSERTION, "SQL_DROP_ASSERTION" );
    pProfile->mapInfo["SQL_DROP_CHARACTER_SET"]               = new DATAInfo( SQL_DROP_CHARACTER_SET, "SQL_DROP_CHARACTER_SET" );
    pProfile->mapInfo["SQL_DROP_COLLATION"]                   = new DATAInfo( SQL_DROP_COLLATION, "SQL_DROP_COLLATION" );
    pProfile->mapInfo["SQL_DROP_DOMAIN"]                      = new DATAInfo( SQL_DROP_DOMAIN, "SQL_DROP_DOMAIN" );
    pProfile->mapInfo["SQL_DROP_SCHEMA"]                      = new DATAInfo( SQL_DROP_SCHEMA, "SQL_DROP_SCHEMA" );
    {
        DATAInfo *pInfo = new DATAInfo(SQL_DROP_TABLE, "SQL_DROP_TABLE", 0 );
        pInfo->mapValues.insert( "SQL_DT_DROP_TABLE", new DATAInfo( SQL_DT_DROP_TABLE, "SQL_DT_DROP_TABLE", "Y" ) );
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    pProfile->mapInfo["SQL_DROP_TRANSLATION"]                 = new DATAInfo( SQL_DROP_TRANSLATION, "SQL_DROP_TRANSLATION" );
    pProfile->mapInfo["SQL_DROP_VIEW"]                        = new DATAInfo( SQL_DROP_VIEW, "SQL_DROP_VIEW" );
    {
        DATAInfo *pInfo = new DATAInfo(SQL_IDENTIFIER_CASE, "SQL_IDENTIFIER_CASE", 0 );
        pInfo->mapValues.insert( "SQL_IC_MIXED", new DATAInfo( SQL_IC_MIXED, "SQL_IC_MIXED", "Y" ) );
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    pProfile->mapInfo["SQL_IDENTIFIER_QUOTE_CHAR"]            = new DATAInfo( SQL_IDENTIFIER_QUOTE_CHAR, "SQL_IDENTIFIER_QUOTE_CHAR", "\"" );
    pProfile->mapInfo["SQL_INDEX_KEYWORDS"]                   = new DATAInfo( SQL_INDEX_KEYWORDS, "SQL_INDEX_KEYWORDS" );
    pProfile->mapInfo["SQL_INTEGRITY"]                        = new DATAInfo( SQL_INTEGRITY, "SQL_INTEGRITY", "N" );
    pProfile->mapInfo["SQL_KEYWORDS"]                         = new DATAInfo( SQL_KEYWORDS, "SQL_KEYWORDS", "TABLE" );
    pProfile->mapInfo["SQL_MAX_CATALOG_NAME_LEN"]             = new DATAInfo( SQL_MAX_CATALOG_NAME_LEN, "SQL_MAX_CATALOG_NAME_LEN" );
    pProfile->mapInfo["SQL_MAX_COLUMN_NAME_LEN"]              = new DATAInfo( SQL_MAX_COLUMN_NAME_LEN, "SQL_MAX_COLUMN_NAME_LEN" );
    pProfile->mapInfo["SQL_MAX_COLUMNS_IN_INDEX"]             = new DATAInfo( SQL_MAX_COLUMNS_IN_INDEX, "SQL_MAX_COLUMNS_IN_INDEX" );
    pProfile->mapInfo["SQL_MAX_COLUMNS_IN_TABLE"]             = new DATAInfo( SQL_MAX_COLUMNS_IN_TABLE, "SQL_MAX_COLUMNS_IN_TABLE" );
    pProfile->mapInfo["SQL_MAX_IDENTIFIER_LEN"]               = new DATAInfo( SQL_MAX_IDENTIFIER_LEN, "SQL_MAX_IDENTIFIER_LEN" );
    pProfile->mapInfo["SQL_MAX_INDEX_SIZE"]                   = new DATAInfo( SQL_MAX_INDEX_SIZE, "SQL_MAX_INDEX_SIZE" );
    pProfile->mapInfo["SQL_MAX_PROCEDURE_NAME_LEN"]           = new DATAInfo( SQL_MAX_PROCEDURE_NAME_LEN, "SQL_MAX_PROCEDURE_NAME_LEN" );
    pProfile->mapInfo["SQL_MAX_ROW_SIZE"]                     = new DATAInfo( SQL_MAX_ROW_SIZE, "SQL_MAX_ROW_SIZE" );
    pProfile->mapInfo["SQL_MAX_ROW_SIZE_INCLUDES_LONG"]       = new DATAInfo( SQL_MAX_ROW_SIZE_INCLUDES_LONG, "SQL_MAX_ROW_SIZE_INCLUDES_LONG" );
    pProfile->mapInfo["SQL_MAX_SCHEMA_NAME_LEN"]              = new DATAInfo( SQL_MAX_SCHEMA_NAME_LEN, "SQL_MAX_SCHEMA_NAME_LEN" );
    pProfile->mapInfo["SQL_MAX_TABLE_NAME_LEN"]               = new DATAInfo( SQL_MAX_TABLE_NAME_LEN, "SQL_MAX_TABLE_NAME_LEN" );
    {
        DATAInfo *pInfo = new DATAInfo(SQL_NON_NULLABLE_COLUMNS, "SQL_NON_NULLABLE_COLUMNS", 0 );
        pInfo->mapValues.insert( "SQL_NNC_NON_NULL", new DATAInfo( SQL_NNC_NON_NULL, "SQL_NNC_NON_NULL", "Y" ) );
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    pProfile->mapInfo["SQL_PROCEDURE_TERM"]                   = new DATAInfo( SQL_PROCEDURE_TERM, "SQL_PROCEDURE_TERM" );
    pProfile->mapInfo["SQL_PROCEDURES"]                       = new DATAInfo( SQL_PROCEDURES, "SQL_PROCEDURES", "N" );
    {
        DATAInfo *pInfo = new DATAInfo(SQL_QUOTED_IDENTIFIER_CASE, "SQL_QUOTED_IDENTIFIER_CASE", 0 );
        pInfo->mapValues.insert( "SQL_IC_SENSITIVE", new DATAInfo( SQL_IC_SENSITIVE, "SQL_IC_SENSITIVE", "Y" ) );
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    pProfile->mapInfo["SQL_SCHEMA_TERM"]                      = new DATAInfo( SQL_SCHEMA_TERM, "SQL_SCHEMA_TERM" );
    pProfile->mapInfo["SQL_SPECIAL_CHARACTERS"]               = new DATAInfo( SQL_SPECIAL_CHARACTERS, "SQL_SPECIAL_CHARACTERS" );
    pProfile->mapInfo["SQL_SQL_CONFORMANCE"]                  = new DATAInfo( SQL_SQL_CONFORMANCE, "SQL_SQL_CONFORMANCE" );
    pProfile->mapInfo["SQL_SQL92_DATETIME_FUNCTIONS"]         = new DATAInfo( SQL_SQL92_DATETIME_FUNCTIONS, "SQL_SQL92_DATETIME_FUNCTIONS" );
    pProfile->mapInfo["SQL_SQL92_RELATIONAL_JOIN_OPERATORS"]  = new DATAInfo( SQL_SQL92_RELATIONAL_JOIN_OPERATORS, "SQL_SQL92_RELATIONAL_JOIN_OPERATORS" );
    pProfile->mapInfo["SQL_TABLE_TERM"]                       = new DATAInfo( SQL_TABLE_TERM, "SQL_TABLE_TERM", "TABLE" );

    // configure data-types
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "CHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_CHAR";
        pDataTypeSpec->COLUMN_SIZE       = "256";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>CHAR(n)</B><P>Character string of fixed string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "CHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_VARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "256";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>VARCHAR(n)</B><P>Variable-length character string with a maximum string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_LONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "32768";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>LONG VARCHAR</B><P>Variable length character data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    return true;
}

bool DATAProfiles::doLoadODBCCor( DATAProfile *pProfile )
{
    // configure driver/data-source info 
    // - add more features
    {
        DATAInfo *pInfoParent = new DATAInfo();
        pInfoParent->nInfo = SQL_CREATE_VIEW;
        pInfoParent->stringInfo = "SQL_CREATE_VIEW";

        DATAInfo *pInfoChild = new DATAInfo();
        pInfoChild->nInfo = SQL_CV_CREATE_VIEW;
        pInfoChild->stringInfo = "SQL_CV_CREATE_VIEW";
        pInfoChild->vValue = "Y";
        pInfoParent->mapValues.insert( pInfoChild->stringInfo, pInfoChild );

        pProfile->mapInfo[pInfoParent->stringInfo] = pInfoParent;
    }
    {
        DATAInfo *pInfoParent = new DATAInfo();
        pInfoParent->nInfo = SQL_DDL_INDEX;
        pInfoParent->stringInfo = "SQL_DDL_INDEX";

        DATAInfo *pInfoChild = new DATAInfo();
        pInfoChild->nInfo = SQL_DI_CREATE_INDEX;
        pInfoChild->stringInfo = "SQL_DI_CREATE_INDEX";
        pInfoChild->vValue = "Y";
        pInfoParent->mapValues.insert( pInfoChild->stringInfo, pInfoChild );

        pInfoChild->nInfo = SQL_DI_DROP_INDEX;
        pInfoChild->stringInfo = "SQL_DI_DROP_INDEX";
        pInfoChild->vValue = "Y";
        pInfoParent->mapValues.insert( pInfoChild->stringInfo, pInfoChild );

        pProfile->mapInfo[pInfoParent->stringInfo] = pInfoParent;
    }
    {
        DATAInfo *pInfoParent = new DATAInfo();
        pInfoParent->nInfo = SQL_DROP_VIEW;
        pInfoParent->stringInfo = "SQL_DROP_VIEW";

        DATAInfo *pInfoChild = new DATAInfo();
        pInfoChild->nInfo = SQL_DV_DROP_VIEW;
        pInfoChild->stringInfo = "SQL_DV_DROP_VIEW";
        pInfoChild->vValue = "Y";
        pInfoParent->mapValues.insert( pInfoChild->stringInfo, pInfoChild );

        pProfile->mapInfo[pInfoParent->stringInfo] = pInfoParent;
    }

    // configure data-types
    // - add more data-types
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "DECIMAL";
        pDataTypeSpec->DATA_TYPE         = "SQL_DECIMAL";
        pDataTypeSpec->COLUMN_SIZE       = "10";
        pDataTypeSpec->CREATE_PARAMS     = "precision,scale";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_FALSE";
        pDataTypeSpec->MINIMUM_SCALE     = "0";
        pDataTypeSpec->MAXIMUM_SCALE     = "2";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->NUM_PREC_RADIX    = "10";
        pDataTypeSpec->stringDescription = "<B>DECIMAL(p,s)</B><P>Signed, exact, numeric value with a precision of at least <B>p</B> and scale s. (The maximum precision is driver-defined.) (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "DECIMAL(p,s)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "NUMERIC";
        pDataTypeSpec->DATA_TYPE         = "SQL_DECIMAL";
        pDataTypeSpec->COLUMN_SIZE       = "10";
        pDataTypeSpec->CREATE_PARAMS     = "precision,scale";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_FALSE";
        pDataTypeSpec->MINIMUM_SCALE     = "0";
        pDataTypeSpec->MAXIMUM_SCALE     = "2";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->NUM_PREC_RADIX    = "10";
        pDataTypeSpec->stringDescription = "<B>NUMERIC(p,s)</B><P>Signed, exact, numeric value with a precision <B>p</B> and scale <B>s</B> (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "NUMERIC(p,s)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "SMALLINT";
        pDataTypeSpec->DATA_TYPE         = "SQL_SMALLINT";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>SMALLINT</B><P>Exact numeric value with precision 5 and scale 0 (signed: -32,768 &lt;= <B>n</B> &lt;= 32,767, unsigned: 0 &lt;= <B>n</B> &lt;= 65,535).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTEGER";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTEGER";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTEGER</B><P>Exact numeric value with precision 10 and scale 0 (signed: -2[31] &lt;= <B>n</B> &lt;= 2[31] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[32] - 1).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "REAL";
        pDataTypeSpec->DATA_TYPE         = "SQL_REAL";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>REAL</B><P>Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[-38] to 10[38]).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "FLOAT";
        pDataTypeSpec->DATA_TYPE         = "SQL_FLOAT";
        pDataTypeSpec->COLUMN_SIZE       = "10";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_FALSE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->NUM_PREC_RADIX    = "10";
        pDataTypeSpec->stringDescription = "<B>FLOAT(p)</B><P>Signed, approximate, numeric value with a binary precision of at least <B>p</B>. (The maximum precision is driver-defined.)";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "FLOAT(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "DOUBLE PRECISION";
        pDataTypeSpec->DATA_TYPE         = "SQL_DOUBLE";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>DOUBLE PRECISION</B><P>Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[-308] to 10[308]).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    return true;
}

bool DATAProfiles::doLoadODBCExt( DATAProfile *pProfile )
{
    // configure driver/data-source info 
    // - add more features
    {
        DATAInfo *pInfo = new DATAInfo();
        pInfo->nInfo = SQL_ALTER_TABLE;
        pInfo->stringInfo = "SQL_ALTER_TABLE";

        DATAInfo *pInfoChild = new DATAInfo();
        pInfoChild->nInfo = SQL_AT_ADD_CONSTRAINT;
        pInfoChild->stringInfo = "SQL_AT_ADD_CONSTRAINT";
        pInfoChild->vValue = "Y";
        pInfo->mapValues.insert( pInfoChild->stringInfo, pInfoChild );

        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    {
        DATAInfo *pInfo = new DATAInfo();
        pInfo->nInfo = SQL_PROCEDURE_TERM;
        pInfo->stringInfo = "SQL_PROCEDURE_TERM";
        pInfo->vValue = "PROCEDURE";
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }
    {
        DATAInfo *pInfo = new DATAInfo();
        pInfo->nInfo = SQL_PROCEDURES;
        pInfo->stringInfo = "SQL_PROCEDURES";
        pInfo->vValue = "Y";
        pProfile->mapInfo[pInfo->stringInfo] = pInfo;
    }

    // configure data-types
    // - add more data-types
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "WCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_WCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "256";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>WCHAR(n)</B><P>Unicode character string of fixed string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "WCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARWCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_WVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "256";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>VARWCHAR(n)</B><P>Unicode variable-length character string with a maximum string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARWCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARWCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_WLONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "32768";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>LONG VARWCHAR</B><P>Unicode variable-length character data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "BIT";
        pDataTypeSpec->DATA_TYPE         = "SQL_BIT";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>BIT</B><P>Single bit binary data.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "TINYINT";
        pDataTypeSpec->DATA_TYPE         = "SQL_TINYINT";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>TINYINT</B><P>Exact numeric value with precision 3 and scale 0 (signed: -128 &lt;= <B>n</B> &lt;= 127, unsigned: 0 &lt;= <B>n</B> &lt;= 255)";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "BIGINT";
        pDataTypeSpec->DATA_TYPE         = "SQL_BIGINT";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>BIGINT</B><P>Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: -2[63] &lt;= <B>n</B> &lt;= 2[63] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[64] - 1.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "BINARY";
        pDataTypeSpec->DATA_TYPE         = "SQL_BINARY";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>BINARY(n)</B><P>Binary data of fixed length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "BINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARBINARY";
        pDataTypeSpec->DATA_TYPE         = "SQL_VARBINARY";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>VARBINARY(n)</B><P>Variable length binary data of maximum length <B>n</B>. The maximum is set by the user.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARBINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARBINARY";
        pDataTypeSpec->DATA_TYPE         = "SQL_LONGVARBINARY";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>LONG VARBINARY</B><P>Variable length binary data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "DATE";
        pDataTypeSpec->DATA_TYPE         = "SQL_LONGVARBINARY";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>DATE</B><P>Year, month, and day fields, conforming to the rules of the Gregorian calendar.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "TIME";
        pDataTypeSpec->DATA_TYPE         = "SQL_TIME";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>TIME(p)</B><P>Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision <B>p</B> indicates the seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "TIME(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "TIMESTAMP";
        pDataTypeSpec->DATA_TYPE         = "SQL_TIMESTAMP";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>TIMESTAMP(p)</B><P>Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "TIMESTAMP(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "UTCDATETIME";
        pDataTypeSpec->DATA_TYPE         = "SQL_TIMESTAMP";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>UTCDATETIME</B><P>Year, month, day, hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "UTCTIME";
        pDataTypeSpec->DATA_TYPE         = "SQL_TIMESTAMP";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>UTCTIME</B><P>Hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MONTH";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MONTH";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MONTH(p)</B><P>Number of months between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MONTH(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL YEAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_YEAR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL YEAR(p)</B><P>Number of years between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL YEAR(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL YEAR TO MONTH";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_YEAR_TO_MONTH";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL YEAR(p) TO MONTH</B><P>Number of years and months between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL YEAR(p) TO MONTH" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p)</B><P>Number of days between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p)</B><P>Number of hours between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MINUTE(p)</B><P>Number of minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MINUTE(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL SECOND(p,q)</B><P>Number of seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL SECOND(p,q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO HOUR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_HOUR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO HOUR</B><P>Number of days/hours between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO HOUR" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO MINUTE</B><P>Number of days/hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO MINUTE" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO SECOND(q)</B><P>Number of days/hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR TO MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR_TO_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p) TO MINUTE</B><P>Number of hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p) TO MINUTE" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p) TO SECOND(q)</B><P>Number of hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MINUTE TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MINUTE_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MINUTE(p) TO SECOND(q)</B><P>Number of minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MINUTE(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "GUID";
        pDataTypeSpec->DATA_TYPE         = "SQL_GUID";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>GUID</B><P>Fixed length Globally Unique Identifier.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    return true;
}

bool DATAProfiles::doLoadSQL92()
{
    DATAProfile *pProfile = new DATAProfile( pDatabase, "SQL92" );

    //
    // DATA TYPES
    //
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "CHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_CHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_CHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>CHAR(n)</B><P>Character string of fixed string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "CHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_VARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_VARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>VARCHAR(n)</B><P>Variable-length character string with a maximum string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_LONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "32768";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_TRUE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>LONG VARCHAR</B><P>Variable length character data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "DECIMAL";
        pDataTypeSpec->DATA_TYPE           = "SQL_DECIMAL";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "precision,scale";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "4";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DECIMAL";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>DECIMAL(p,s)</B><P>Signed, exact, numeric value with a precision of at least <B>p</B> and scale s. (The maximum precision is driver-definepDataTypeSpec->) (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "DECIMAL(p,s)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "NUMERIC";
        pDataTypeSpec->DATA_TYPE           = "SQL_NUMERIC";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "precision,scale";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "4";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_NUMERIC";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>NUMERIC(p,s)</B><P>Signed, exact, numeric value with a precision <B>p</B> and scale <B>s</B> (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "NUMERIC(p,s)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "SMALLINT";
        pDataTypeSpec->DATA_TYPE           = "SQL_SMALLINT";
        pDataTypeSpec->COLUMN_SIZE         = "5";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_SMALLINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>SMALLINT</B><P>Exact numeric value with precision 5 and scale 0 (signed: -32,768 &lt;= <B>n</B> &lt;= 32,767, unsigned: 0 &lt;= <B>n</B> &lt;= 65,535).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "INTEGER";
        pDataTypeSpec->DATA_TYPE           = "SQL_INTEGER";
        pDataTypeSpec->COLUMN_SIZE         = "10";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NO_NULLS";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_TRUE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_INTEGER";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>INTEGER</B><P>Exact numeric value with precision 10 and scale 0 (signed: -2[31] &lt;= <B>n</B> &lt;= 2[31] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[32] - 1).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "REAL";
        pDataTypeSpec->DATA_TYPE           = "SQL_REAL";
        pDataTypeSpec->COLUMN_SIZE         = "24";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_REAL";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "2";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>REAL</B><P>Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[-38] to 10[38]).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "FLOAT";
        pDataTypeSpec->DATA_TYPE         = "SQL_FLOAT";
        pDataTypeSpec->COLUMN_SIZE       = "10";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_FALSE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->NUM_PREC_RADIX    = "10";
        pDataTypeSpec->stringDescription = "<B>FLOAT(p)</B><P>Signed, approximate, numeric value with a binary precision of at least <B>p</B>. (The maximum precision is driver-definepDataTypeSpec->)";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "FLOAT(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "DOUBLE PRECISION";
        pDataTypeSpec->DATA_TYPE           = "SQL_DOUBLE";
        pDataTypeSpec->COLUMN_SIZE         = "53";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DOUBLE";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "2";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>DOUBLE PRECISION</B><P>Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[-308] to 10[308]).</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "WCHAR";
        pDataTypeSpec->DATA_TYPE           = "SQL_WCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>WCHAR(n)</B><P>Unicode character string of fixed string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "WCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARWCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_WVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "256";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS     = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WLONGVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>VARWCHAR(n)</B><P>Unicode variable-length character string with a maximum string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARWCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARWCHAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_WLONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE       = "32768";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WLONGVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>LONG VARWCHAR</B><P>Unicode variable-length character data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->DATA_TYPE           = "SQL_BIT";
        pDataTypeSpec->COLUMN_SIZE         = "1";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NO_NULLS";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BIT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>BIT</B><P>Single bit binary data.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "TINYINT";
        pDataTypeSpec->DATA_TYPE           = "SQL_TINYINT";
        pDataTypeSpec->COLUMN_SIZE         = "3";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_TRUE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TINYINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>TINYINT</B><P>Exact numeric value with precision 3 and scale 0 (signed: -128 &lt;= <B>n</B> &lt;= 127, unsigned: 0 &lt;= <B>n</B> &lt;= 255)";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "BIGINT";
        pDataTypeSpec->DATA_TYPE         = "SQL_BIGINT";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>BIGINT</B><P>Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: -2[63] &lt;= <B>n</B> &lt;= 2[63] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[64] - 1.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "BINARY";
        pDataTypeSpec->DATA_TYPE           = "SQL_BINARY";
        pDataTypeSpec->COLUMN_SIZE         = "510";
        pDataTypeSpec->LITERAL_PREFIX      = "0x";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>BINARY(n)</B><P>Binary data of fixed length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "BINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "VARBINARY";
        pDataTypeSpec->DATA_TYPE           = "SQL_VARBINARY";
        pDataTypeSpec->COLUMN_SIZE         = "510";
        pDataTypeSpec->LITERAL_PREFIX      = "0x";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_VARBINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>VARBINARY(n)</B><P>Variable length binary data of maximum length <B>n</B>. The maximum is set by the user.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "VARBINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "LONG VARBINARY";
        pDataTypeSpec->DATA_TYPE         = "SQL_LONGVARBINARY";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>LONG VARBINARY</B><P>Variable length binary data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "DATE";
        pDataTypeSpec->DATA_TYPE         = "SQL_DATE";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>DATE</B><P>Year, month, and day fields, conforming to the rules of the Gregorian calendar.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "TIME";
        pDataTypeSpec->DATA_TYPE         = "SQL_TIME";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>TIME(p)</B><P>Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision <B>p</B> indicates the seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "TIME(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "TIMESTAMP";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "precision";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DATETIME";
        pDataTypeSpec->SQL_DATETIME_SUB    = "SQL_CODE_TIMESTAMP";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>TIMESTAMP(p)</B><P>Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types. Optional (p) is precision in seconds - 0 = default. </P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "TIMESTAMP(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "UTCDATETIME";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DATETIME";
        pDataTypeSpec->SQL_DATETIME_SUB    = "SQL_CODE_TIMESTAMP";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>UTCDATETIME</B><P>Year, month, day, hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "UTCTIME";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DATETIME";
        pDataTypeSpec->SQL_DATETIME_SUB    = "SQL_CODE_TIMESTAMP";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>UTCTIME</B><P>Hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MONTH";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MONTH";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MONTH(p)</B><P>Number of months between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MONTH(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL YEAR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_YEAR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL YEAR(p)</B><P>Number of years between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL YEAR(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL YEAR TO MONTH";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_YEAR_TO_MONTH";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL YEAR(p) TO MONTH</B><P>Number of years and months between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL YEAR(p) TO MONTH" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p)</B><P>Number of days between two dates; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p)</B><P>Number of hours between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MINUTE(p)</B><P>Number of minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MINUTE(p)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL SECOND(p,q)</B><P>Number of seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL SECOND(p,q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO HOUR";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_HOUR";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO HOUR</B><P>Number of days/hours between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO HOUR" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO MINUTE</B><P>Number of days/hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO MINUTE" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL DAY TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_DAY_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL DAY(p) TO SECOND(q)</B><P>Number of days/hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL DAY(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR TO MINUTE";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR_TO_MINUTE";
        pDataTypeSpec->CREATE_PARAMS     = "precision";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p) TO MINUTE</B><P>Number of hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p) TO MINUTE" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL HOUR TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_HOUR_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL HOUR(p) TO SECOND(q)</B><P>Number of hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL HOUR(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "INTERVAL MINUTE TO SECOND";
        pDataTypeSpec->DATA_TYPE         = "SQL_INTERVAL_MINUTE_TO_SECOND";
        pDataTypeSpec->CREATE_PARAMS     = "precision1,precision2";
        pDataTypeSpec->NULLABLE          = "SQL_NULLABLE_UNKNOWN";
        pDataTypeSpec->CASE_SENSITIVE    = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE        = "SQL_PRED_NONE";
        pDataTypeSpec->FIXED_PREC_SCALE  = "SQL_TRUE";
        pDataTypeSpec->SQL_DATA_TYPE     = pDataTypeSpec->DATA_TYPE;
        pDataTypeSpec->stringDescription = "<B>INTERVAL MINUTE(p) TO SECOND(q)</B><P>Number of minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( "INTERVAL MINUTE(p) TO SECOND(q)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME         = "GUID";
        pDataTypeSpec->DATA_TYPE           = "SQL_GUID";
        pDataTypeSpec->COLUMN_SIZE         = "36";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_GUID";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription = "<B>GUID</B><P>Fixed length Globally Unique Identifier.</P>";
        pDataTypeSpec->vectorSyntax      = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }

    mapProfiles["SQL92"] = pProfile;

    return true;
}

bool DATAProfiles::doLoadSQLite()
{
    DATAProfile *pProfile = new DATAProfile( pDatabase, "SQLite" );

    // INFO
    // Load up info. This is done by querying the driver (\sa ODBCDrvModel) but we know we are dealing
    // with SQLite so we load this information without the driver.
    // This comes from the driver (via ODBCDrvModel) and/or the specification.
    // In this way we have all the info we need without having to create a connection to a driver (or even have it
    // installed).

    //
    // DATA TYPES
    // - this is how the ODBC driver wants us to see things
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "bigint";
        pDataTypeSpec->DATA_TYPE           = "SQL_BIGINT";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "bigint";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BIGINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "bit";
        pDataTypeSpec->DATA_TYPE           = "SQL_BIT";
        pDataTypeSpec->COLUMN_SIZE         = "1";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "bit";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BIT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "char";
        pDataTypeSpec->DATA_TYPE           = "SQL_CHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "char";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_CHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "char(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "date";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_DATE";
        pDataTypeSpec->COLUMN_SIZE         = "10";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "date";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TYPE_DATE";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "double";
        pDataTypeSpec->DATA_TYPE           = "SQL_DOUBLE";
        pDataTypeSpec->COLUMN_SIZE         = "15";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "double";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DOUBLE";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "float";
        pDataTypeSpec->DATA_TYPE           = "SQL_FLOAT";
        pDataTypeSpec->COLUMN_SIZE         = "7";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "float";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_FLOAT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "integer";
        pDataTypeSpec->DATA_TYPE           = "SQL_INTEGER";
        pDataTypeSpec->COLUMN_SIZE         = "9";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "integer";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_INTEGER";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "longvarbinary";
        pDataTypeSpec->DATA_TYPE           = "SQL_LONGVARBINARY";
        pDataTypeSpec->COLUMN_SIZE         = "65536";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "longvarbinary";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_LONGVARBINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "longvarchar";
        pDataTypeSpec->DATA_TYPE           = "SQL_LONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "65536";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "longvarchar";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_LONGVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "longvarchar(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "numeric";
        pDataTypeSpec->DATA_TYPE           = "SQL_DOUBLE";
        pDataTypeSpec->COLUMN_SIZE         = "15";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "numeric";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DOUBLE";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "smallint";
        pDataTypeSpec->DATA_TYPE           = "SQL_SMALLINT";
        pDataTypeSpec->COLUMN_SIZE         = "5";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "smallint";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_SMALLINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "text";
        pDataTypeSpec->DATA_TYPE           = "SQL_LONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "65536";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "text";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_LONGVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "text(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "time";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIME";
        pDataTypeSpec->COLUMN_SIZE         = "8";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "time";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TYPE_TIME";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "timestamp";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->COLUMN_SIZE         = "32";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "timestamp";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "3";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "tinyint";
        pDataTypeSpec->DATA_TYPE           = "SQL_TINYINT";
        pDataTypeSpec->COLUMN_SIZE         = "3";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "tinyint";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TINYINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "varbinary";
        pDataTypeSpec->DATA_TYPE           = "SQL_VARBINARY";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "varbinary";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_VARBINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "varchar";
        pDataTypeSpec->DATA_TYPE           = "SQL_VARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "varchar";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_VARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "varchar(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }

    mapProfiles["SQLite"] = pProfile;

    return true;
}

bool DATAProfiles::doLoadMSA()
{
    DATAProfile *pProfile = new DATAProfile( pDatabase, "MSA" );

    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "BINARY";
        pDataTypeSpec->DATA_TYPE           = "SQL_BINARY";
        pDataTypeSpec->COLUMN_SIZE         = "510";
        pDataTypeSpec->LITERAL_PREFIX      = "0x";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>BINARY(n)</B><P>Binary data of fixed length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "BINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "BIT";
        pDataTypeSpec->DATA_TYPE           = "SQL_BIT";
        pDataTypeSpec->COLUMN_SIZE         = "1";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NO_NULLS";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_BIT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>BIT</B><P>Single bit binary data.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "BYTE";
        pDataTypeSpec->DATA_TYPE           = "SQL_TINYINT";
        pDataTypeSpec->COLUMN_SIZE         = "3";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_TRUE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_TINYINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>BYTE</B><P>Exact numeric value with precision 3 and scale 0 (signed: -128 &lt;= <B>n</B> &lt;= 127, unsigned: 0 &lt;= <B>n</B> &lt;= 255)";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "CHAR";
        pDataTypeSpec->DATA_TYPE           = "SQL_WCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>CHAR(n)</B><P>Character string of fixed string length <B>n</B> to maximum of 255.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "CHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "COUNTER";
        pDataTypeSpec->DATA_TYPE           = "SQL_INTEGER";
        pDataTypeSpec->COLUMN_SIZE         = "10";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NO_NULLS";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_TRUE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_INTEGER";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>COUNTER</B><P>Auto incremented value based upon an INTEGER.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "CURRENCY";
        pDataTypeSpec->DATA_TYPE           = "SQL_NUMERIC";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_TRUE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "4";
        pDataTypeSpec->MAXIMUM_SCALE       = "4";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_NUMERIC";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>CURRENCY</B><P>For storing a currency value. A SQL_NUMERIC with pre-determined precision and scale.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "DATETIME";
        pDataTypeSpec->DATA_TYPE           = "SQL_TYPE_TIMESTAMP";
        pDataTypeSpec->COLUMN_SIZE         = "19";
        pDataTypeSpec->LITERAL_PREFIX      = "#";
        pDataTypeSpec->LITERAL_SUFFIX      = "#";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DATETIME";
        pDataTypeSpec->SQL_DATETIME_SUB    = "SQL_CODE_TIMESTAMP";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>DATETIME</B><P>Year, month, and day fields, conforming to the rules of the Gregorian calendar. Combined with TIME.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "DOUBLE";
        pDataTypeSpec->DATA_TYPE           = "SQL_DOUBLE";
        pDataTypeSpec->COLUMN_SIZE         = "53";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_DOUBLE";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "2";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>DOUBLE</B><P>Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[-308] to 10[308]).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "GUID";
        pDataTypeSpec->DATA_TYPE           = "SQL_GUID";
        pDataTypeSpec->COLUMN_SIZE         = "36";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_GUID";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>GUID</B><P>Fixed length Globally Unique Identifier.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "INTEGER";
        pDataTypeSpec->DATA_TYPE           = "SQL_INTEGER";
        pDataTypeSpec->COLUMN_SIZE         = "10";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_INTEGER";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>INTEGER</B><P>Exact numeric value with precision 10 and scale 0 (signed: -2[31] &lt;= <B>n</B> &lt;= 2[31] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[32] - 1).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "LONGBINARY";
        pDataTypeSpec->DATA_TYPE           = "SQL_LONGVARBINARY";
        pDataTypeSpec->COLUMN_SIZE         = "1073741823";
        pDataTypeSpec->LITERAL_PREFIX      = "0x";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_LONGVARBINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>LONGBINARY</B><P>Variable length binary data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "LONGCHAR";
        pDataTypeSpec->DATA_TYPE           = "SQL_WLONGVARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "1073741823";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WLONGVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>LONGCHAR</B><P>Unicode variable-length character data. Maximum length is data source-dependent.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "REAL";
        pDataTypeSpec->DATA_TYPE           = "SQL_REAL";
        pDataTypeSpec->COLUMN_SIZE         = "24";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_REAL";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "2";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>REAL</B><P>Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[-38] to 10[38]).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "SMALLINT";
        pDataTypeSpec->DATA_TYPE           = "SQL_SMALLINT";
        pDataTypeSpec->COLUMN_SIZE         = "5";
        pDataTypeSpec->LITERAL_PREFIX      = "";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_BASIC";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "0";
        pDataTypeSpec->MAXIMUM_SCALE       = "0";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_SMALLINT";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "10";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>SMALLINT</B><P>Exact numeric value with precision 5 and scale 0 (signed: -32,768 &lt;= <B>n</B> &lt;= 32,767, unsigned: 0 &lt;= <B>n</B> &lt;= 65,535).</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( pDataTypeSpec->TYPE_NAME );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "VARBINARY";
        pDataTypeSpec->DATA_TYPE           = "SQL_VARBINARY";
        pDataTypeSpec->COLUMN_SIZE         = "510";
        pDataTypeSpec->LITERAL_PREFIX      = "0x";
        pDataTypeSpec->LITERAL_SUFFIX      = "";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_PRED_NONE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_VARBINARY";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>VARBINARY(n)</B><P>Variable length binary data of maximum length <B>n</B>. The maximum is set by the user.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "VARBINARY(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME           = "VARCHAR";
        pDataTypeSpec->DATA_TYPE           = "SQL_WVARCHAR";
        pDataTypeSpec->COLUMN_SIZE         = "255";
        pDataTypeSpec->LITERAL_PREFIX      = "'";
        pDataTypeSpec->LITERAL_SUFFIX      = "'";
        pDataTypeSpec->CREATE_PARAMS       = "length";
        pDataTypeSpec->NULLABLE            = "SQL_NULLABLE";
        pDataTypeSpec->CASE_SENSITIVE      = "SQL_FALSE";
        pDataTypeSpec->SEARCHABLE          = "SQL_SEARCHABLE";
        pDataTypeSpec->UNSIGNED_ATTRIBUTE  = "SQL_FALSE";
        pDataTypeSpec->FIXED_PREC_SCALE    = "SQL_FALSE";
        pDataTypeSpec->AUTO_UNIQUE_VALUE   = "SQL_FALSE";
        pDataTypeSpec->LOCAL_TYPE_NAME     = "";
        pDataTypeSpec->MINIMUM_SCALE       = "";
        pDataTypeSpec->MAXIMUM_SCALE       = "";
        pDataTypeSpec->SQL_DATA_TYPE       = "SQL_WVARCHAR";
        pDataTypeSpec->SQL_DATETIME_SUB    = "0";
        pDataTypeSpec->NUM_PREC_RADIX      = "";
        pDataTypeSpec->INTERVAL_PRECISION  = "";
        pDataTypeSpec->stringDescription   = "<B>VARCHAR(n)</B><P>Unicode variable-length character string with a maximum string length <B>n</B>.</P>";
        pDataTypeSpec->vectorSyntax        = DATADataTypeSpec::getSyntaxVector( "VARCHAR(n)" );
        pProfile->mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;
    }

    mapProfiles["MSA"] = pProfile;

    return true;
}

