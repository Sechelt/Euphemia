#include "LibInfo.h"
#include "DATAConnection.h"

#include "DATAEnvironment.h"
#include "DATAStatement.h"
#include "DATAProfiles.h"
#include "DATAProfile.h"
#include "DATAConnectDialog.h"

DATAConnection::DATAConnection( DATAEnvironment *penvironment )
    : DATAHandle( Dbc, penvironment )
{
    setObjectName( "DATAConnection" );
    pProfile                = nullptr;
    bConnected              = false;
    bPromptAlias            = false;
    bPromptDriver           = false;
    bPromptDataSourceName   = true;
    bPromptUserID           = true;
    bPromptPassword         = true;
    // echo up the object hierarchy
    connect( this, SIGNAL(signalMessage(DATAMessage *)), penvironment, SIGNAL(signalMessage(DATAMessage *)) );
    connect( this, SIGNAL(signalDiagnostic(DATADiagnostic *)), penvironment, SIGNAL(signalDiagnostic(DATADiagnostic *)) );
}

DATAConnection::~DATAConnection()
{
    // increase the chances that cleanup will complete fully
    if ( isConnected() ) doDisconnect();

    //
    g_Profiles->doSave();
}

void DATAConnection::setDataSourceName( const QString &s )  
{ 
    if ( stringDataSourceName == s ) return;
    stringDataSourceName = s;
}

void DATAConnection::setAlias( const QString &s )           
{ 
    if ( stringAlias == s ) return;              
    stringAlias = s;              
}

SQLRETURN DATAConnection::setAttrAccessMode( AttrAccessModeTypes nAttrAccessMode )
{
    return setConnectAttr( SQL_ATTR_ACCESS_MODE, (SQLPOINTER)nAttrAccessMode );
}

SQLRETURN DATAConnection::setAttrAsyncEnable( AttrAsyncEnableTypes nAttrAsyncEnable )
{
    return setConnectAttr( SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER)nAttrAsyncEnable );
}

SQLRETURN DATAConnection::setAttrAutocommit( AttrAutocommitTypes nAttrAutocommit )
{
    return setConnectAttr( SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)nAttrAutocommit );
}

SQLRETURN DATAConnection::setAttrConnectionTimeout( SQLUINTEGER nAttrConnectionTimeout )
{
    return setConnectAttr( SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)nAttrConnectionTimeout );
}

SQLRETURN DATAConnection::setAttrCurrentCatalog( const QString &stringAttrCurrentCatalog )
{
    return setConnectAttr( SQL_ATTR_CURRENT_CATALOG, stringAttrCurrentCatalog );
}

SQLRETURN DATAConnection::setAttrLoginTimeout( SQLUINTEGER nAttrLoginTimeout )
{
    return setConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)nAttrLoginTimeout );
}

SQLRETURN DATAConnection::setAttrMetadataId( bool bAttrMetadataId )
{
    return setConnectAttr( SQL_ATTR_METADATA_ID, (SQLPOINTER)bAttrMetadataId );
}

SQLRETURN DATAConnection::setAttrOdbcCursors( AttrOdbcCursorsTypes nAttrOdbcCursors )
{
    return setConnectAttr( SQL_ATTR_ODBC_CURSORS, (SQLPOINTER)nAttrOdbcCursors );
}

SQLRETURN DATAConnection::setAttrPacketSize( SQLUINTEGER nAttrPacketSize )
{
    return setConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)nAttrPacketSize );
}

SQLRETURN DATAConnection::setAttrQuietMode( SQLHWND hWnd )
{
    return setConnectAttr( SQL_ATTR_QUIET_MODE, (SQLPOINTER)hWnd );
}

SQLRETURN DATAConnection::setAttrTrace( AttrTraceTypes nAttrTrace )
{
    return setConnectAttr( SQL_ATTR_TRACE, (SQLPOINTER)nAttrTrace );
}

SQLRETURN DATAConnection::setAttrTracefile( const QString &stringAttrTracefile )
{
    return setConnectAttr( SQL_ATTR_TRACEFILE, stringAttrTracefile );
}

SQLRETURN DATAConnection::setAttrTranslateLib( const QString &stringAttrTranslateLib )
{
    return setConnectAttr( SQL_ATTR_TRANSLATE_LIB, stringAttrTranslateLib );
}

SQLRETURN DATAConnection::setAttrTranslateOption( SQLUINTEGER nAttrTranslateOption )
{
    return setConnectAttr( SQL_ATTR_TRANSLATE_OPTION, (SQLPOINTER)nAttrTranslateOption );
}

SQLRETURN DATAConnection::setAttrTxnIsolation( SQLUINTEGER nAttrTxnIsolation )
{
    return setConnectAttr( SQL_ATTR_TXN_ISOLATION, (SQLPOINTER)nAttrTxnIsolation );
}

DATAConnection::AttrAccessModeTypes DATAConnection::getAttrAccessMode( SQLRETURN * pn )
{
    AttrAccessModeTypes nAttrAccessMode = ModeReadWrite;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ACCESS_MODE, &nAttrAccessMode );
    if ( pn ) *pn = nReturn;
    return nAttrAccessMode;
}

DATAConnection::AttrAsyncEnableTypes DATAConnection::getAttrAsyncEnable( SQLRETURN * pn )
{
    AttrAsyncEnableTypes nAttrAsyncEnable = AsyncEnableOff;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ASYNC_ENABLE, &nAttrAsyncEnable );
    if ( pn ) *pn = nReturn;
    return nAttrAsyncEnable;
}

bool DATAConnection::getAttrAutoIpd( SQLRETURN * pn )
{
    bool bAttrAutoIpd = false;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_AUTO_IPD, &bAttrAutoIpd );
    if ( pn ) *pn = nReturn;
    return bAttrAutoIpd;
}

DATAConnection::AttrAutocommitTypes DATAConnection::getAttrAutocommit( SQLRETURN * pn )
{
    AttrAutocommitTypes nAttrAutocommit = AutocommitOn;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_AUTOCOMMIT, &nAttrAutocommit );
    if ( pn ) *pn = nReturn;
    return nAttrAutocommit;
}

bool DATAConnection::getAttrConnectionDead( SQLRETURN * pn )
{
    bool bAttrConnectionDead = true;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_CONNECTION_DEAD, &bAttrConnectionDead );
    if ( pn ) *pn = nReturn;
    return bAttrConnectionDead;
}

SQLUINTEGER DATAConnection::getAttrConnectionTimeout( SQLRETURN * pn )
{
    SQLUINTEGER nAttrConnectionTimeout = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_CONNECTION_DEAD, &nAttrConnectionTimeout );
    if ( pn ) *pn = nReturn;
    return nAttrConnectionTimeout;
}

QString DATAConnection::getAttrCurrentCatalog( SQLRETURN * pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_CURRENT_CATALOG, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

SQLUINTEGER DATAConnection::getAttrLoginTimeout( SQLRETURN * pn )
{
    SQLUINTEGER nAttrLoginTimeout = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, &nAttrLoginTimeout );
    if ( pn ) *pn = nReturn;
    return nAttrLoginTimeout;
}

bool DATAConnection::getAttrMetadataId( SQLRETURN * pn )
{
    bool bAttrMetadataId = true;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_METADATA_ID, &bAttrMetadataId );
    if ( pn ) *pn = nReturn;
    return bAttrMetadataId;
}

DATAConnection::AttrOdbcCursorsTypes DATAConnection::getAttrOdbcCursors( SQLRETURN * pn )
{
    AttrOdbcCursorsTypes nAttrOdbcCursors = CurUseDriver;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ODBC_CURSORS, &nAttrOdbcCursors );
    if ( pn ) *pn = nReturn;
    return nAttrOdbcCursors;
}

SQLUINTEGER DATAConnection::getAttrPacketSize( SQLRETURN * pn )
{
    SQLUINTEGER nAttrPacketSize = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_PACKET_SIZE, &nAttrPacketSize );
    if ( pn ) *pn = nReturn;
    return nAttrPacketSize;
}

SQLHWND DATAConnection::getAttrQuietMode( SQLRETURN * pn )
{
    SQLHWND hWnd = NULL;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_QUIET_MODE, &hWnd );
    if ( pn ) *pn = nReturn;
    return hWnd;
}

DATAConnection::AttrTraceTypes DATAConnection::getAttrTrace( SQLRETURN * pn )
{
    AttrTraceTypes nAttrTrace = OptTraceOff;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TRACE, &nAttrTrace );
    if ( pn ) *pn = nReturn;
    return nAttrTrace;
}

QString DATAConnection::getAttrTracefile( SQLRETURN * pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_TRACEFILE, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

QString DATAConnection::getAttrTranslateLib( SQLRETURN * pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_TRANSLATE_LIB, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

SQLUINTEGER DATAConnection::getAttrTranslateOption( SQLRETURN * pn )
{
    SQLUINTEGER nAttrTranslateOption = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TRANSLATE_OPTION, &nAttrTranslateOption );
    if ( pn ) *pn = nReturn;
    return nAttrTranslateOption;
}

SQLUINTEGER DATAConnection::getAttrTxnIsolation( SQLRETURN * pn )
{
    SQLUINTEGER nAttrTxnIsolation = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TXN_ISOLATION, &nAttrTxnIsolation );
    if ( pn ) *pn = nReturn;
    return nAttrTxnIsolation;
}

/*!
 * \brief Get info from ODBC. 
 *  
 * Assumptions: UNICODE and 64bit. 
 *  
 * The return values can be; 
 *  
 *      1. null-terminated character string 
 *      2. SQLUSMALLINT value              
 *      3. SQLUINTEGER bitmask             
 *      4. SQLUINTEGER value               
 *      5. SQLUINTEGER binary value
 *      6. SQLULEN (used in details in doc - may be doc error)
 *      7. SQLINTEGER (used in details in doc - may be doc error)
 *  
 * This method simply returns a QVariant. This saves the caller the trouble of having to deal with 
 * creating buffers for the call and makes us more Qt friendly. 
 * The value can be retreived from QVariant as follows; 
 *  
 *      1. toString()
 *      2. toULong()
 *      3. toULong()
 *      4. toULong()
 *      5. toULong()
 *      6. toULongLong()
 *      7. toLong()
 *  
 * Was going to add another param to call to let us know what value type we are dealing with as 
 * this would save us the lengthy switch(). 
 * I decided to keep the call simple and see what info types are supported in the various 
 * ODBC SDK's and compiler directives while I am at it. 
 *  
 * \author pharvey (1/14/20)
 * 
 * \param nInfoType - The requested info.
 * \param pnReturn  - Buffer to return the SQLRETURN (optional)
 * 
 * \return QVariant The requested value or will be isNull().
 */
QVariant DATAConnection::getInfo( SQLUSMALLINT nInfoType, SQLRETURN *pnReturn )
{
    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    QVariant v;

    switch ( nInfoType )
    {
        // SQLCHAR*
        case SQL_ACCESSIBLE_PROCEDURES:
        case SQL_ACCESSIBLE_TABLES:
        case SQL_CATALOG_NAME:
        case SQL_CATALOG_NAME_SEPARATOR:
        case SQL_CATALOG_TERM:
        case SQL_COLLATION_SEQ:
        case SQL_COLUMN_ALIAS:
        case SQL_DATA_SOURCE_NAME:
        case SQL_DATA_SOURCE_READ_ONLY:
        case SQL_DATABASE_NAME:
        case SQL_DBMS_NAME:
        case SQL_DBMS_VER:
        case SQL_DESCRIBE_PARAMETER:
        case SQL_DM_VER:
        case SQL_DRIVER_NAME:
        case SQL_DRIVER_ODBC_VER:
        case SQL_DRIVER_VER:
        case SQL_EXPRESSIONS_IN_ORDERBY:
        case SQL_IDENTIFIER_QUOTE_CHAR:
        case SQL_INTEGRITY:
        case SQL_KEYWORDS:
        case SQL_LIKE_ESCAPE_CLAUSE:
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
        case SQL_MULT_RESULT_SETS:
        case SQL_MULTIPLE_ACTIVE_TXN:
        case SQL_NEED_LONG_DATA_LEN:
        case SQL_ODBC_VER:
        case SQL_ORDER_BY_COLUMNS_IN_SELECT:
        case SQL_PROCEDURE_TERM:
        case SQL_PROCEDURES:
        case SQL_ROW_UPDATES:
        case SQL_SCHEMA_TERM:
        case SQL_SEARCH_PATTERN_ESCAPE:
        case SQL_SERVER_NAME:
        case SQL_SPECIAL_CHARACTERS:
        case SQL_TABLE_TERM:
        case SQL_USER_NAME:
        case SQL_XOPEN_CLI_YEAR:
        {
            SQLSMALLINT nBufferBytes = 4096;
            ushort sz[4096]; // SQLWCHAR sz[4096];
            SQLSMALLINT nStringBytes = 0;

            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( sz ), nBufferBytes, &nStringBytes );
            if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( QString::fromUtf16( sz ) );
        }
            break;

            // SQLUSMALLINT
        case SQL_ACTIVE_ENVIRONMENTS:
        case SQL_CATALOG_LOCATION:
        case SQL_CONCAT_NULL_BEHAVIOR:
        case SQL_CORRELATION_NAME:
        case SQL_FILE_USAGE:
        case SQL_GROUP_BY:
        case SQL_IDENTIFIER_CASE:
        case SQL_MAX_CATALOG_NAME_LEN:
        case SQL_MAX_COLUMN_NAME_LEN:
        case SQL_MAX_COLUMNS_IN_GROUP_BY:
        case SQL_MAX_COLUMNS_IN_INDEX:
        case SQL_MAX_COLUMNS_IN_ORDER_BY:
        case SQL_MAX_COLUMNS_IN_SELECT:
        case SQL_MAX_COLUMNS_IN_TABLE:
        case SQL_MAX_CONCURRENT_ACTIVITIES:
        case SQL_MAX_CURSOR_NAME_LEN:
        case SQL_MAX_DRIVER_CONNECTIONS:
        case SQL_MAX_IDENTIFIER_LEN:
        case SQL_MAX_PROCEDURE_NAME_LEN:
        case SQL_MAX_SCHEMA_NAME_LEN:
        case SQL_MAX_TABLE_NAME_LEN:
        case SQL_MAX_TABLES_IN_SELECT:
        case SQL_MAX_USER_NAME_LEN:
        case SQL_NON_NULLABLE_COLUMNS:
        case SQL_NULL_COLLATION:
        case SQL_QUOTED_IDENTIFIER_CASE:
        case SQL_TXN_CAPABLE:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0, NULL );
            if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
        }
            break;

            // SQLULEN
        case SQL_DRIVER_HDBC:
        case SQL_DRIVER_HENV:
        case SQL_DRIVER_HDESC:
        case SQL_DRIVER_HLIB:
        case SQL_DRIVER_HSTMT:
        {
            SQLULEN n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), sizeof(SQLULEN), NULL );
            if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
        }
            break;

            // SQLINTEGER
        case SQL_POS_OPERATIONS:
        {
            SQLINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0, NULL );
            if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
        }
            break;

            // SQLUINTEGER
        case SQL_AGGREGATE_FUNCTIONS:
        case SQL_ALTER_DOMAIN:
        case SQL_ALTER_TABLE:
            // case SQL_ASYNC_DBC_FUNCTIONS:
        case SQL_ASYNC_MODE:
            // case SQL_ASYNC_NOTIFICATION:
        case SQL_BATCH_ROW_COUNT:
        case SQL_BATCH_SUPPORT:
        case SQL_BOOKMARK_PERSISTENCE:
        case SQL_CATALOG_USAGE:
        case SQL_CONVERT_BIGINT:
        case SQL_CONVERT_BINARY:
        case SQL_CONVERT_BIT :
        case SQL_CONVERT_CHAR:
        case SQL_CONVERT_GUID:
        case SQL_CONVERT_DATE:
        case SQL_CONVERT_DECIMAL:
        case SQL_CONVERT_DOUBLE:
        case SQL_CONVERT_FLOAT:
        case SQL_CONVERT_INTEGER:
        case SQL_CONVERT_INTERVAL_YEAR_MONTH:
        case SQL_CONVERT_INTERVAL_DAY_TIME:
        case SQL_CONVERT_LONGVARBINARY:
        case SQL_CONVERT_LONGVARCHAR:
        case SQL_CONVERT_NUMERIC:
        case SQL_CONVERT_REAL:
        case SQL_CONVERT_SMALLINT :
        case SQL_CONVERT_TIME:
        case SQL_CONVERT_TIMESTAMP:
        case SQL_CONVERT_TINYINT:
        case SQL_CONVERT_VARBINARY:
        case SQL_CONVERT_VARCHAR:
        case SQL_CONVERT_FUNCTIONS:
        case SQL_CREATE_ASSERTION:
        case SQL_CREATE_CHARACTER_SET:
        case SQL_CREATE_COLLATION:
        case SQL_CREATE_DOMAIN:
        case SQL_CREATE_SCHEMA:
        case SQL_CREATE_TABLE:
        case SQL_CREATE_TRANSLATION:
        case SQL_CREATE_VIEW:
        case SQL_CURSOR_COMMIT_BEHAVIOR:
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
        case SQL_CURSOR_SENSITIVITY:
        case SQL_DATETIME_LITERALS:
        case SQL_DDL_INDEX:
        case SQL_DEFAULT_TXN_ISOLATION:
            // Not picked up on MS
            // case SQL_DRIVER_AWARE_POOLING_SUPPORTED:
        case SQL_DROP_ASSERTION:
        case SQL_DROP_CHARACTER_SET:
        case SQL_DROP_COLLATION:
        case SQL_DROP_DOMAIN:
        case SQL_DROP_SCHEMA:
        case SQL_DROP_TABLE:
        case SQL_DROP_TRANSLATION:
        case SQL_DROP_VIEW:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
        case SQL_GETDATA_EXTENSIONS:
        case SQL_INDEX_KEYWORDS:
        case SQL_INFO_SCHEMA_VIEWS:
        case SQL_INSERT_STATEMENT:
        case SQL_KEYSET_CURSOR_ATTRIBUTES1:
        case SQL_KEYSET_CURSOR_ATTRIBUTES2:
        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
        case SQL_MAX_BINARY_LITERAL_LEN:
        case SQL_MAX_CHAR_LITERAL_LEN:
        case SQL_MAX_INDEX_SIZE:
        case SQL_MAX_ROW_SIZE:
        case SQL_MAX_STATEMENT_LEN:
        case SQL_NUMERIC_FUNCTIONS:
        case SQL_ODBC_INTERFACE_CONFORMANCE:
        case SQL_OJ_CAPABILITIES:
        case SQL_PARAM_ARRAY_ROW_COUNTS:
        case SQL_PARAM_ARRAY_SELECTS:
        case SQL_SCHEMA_USAGE:
        case SQL_SCROLL_OPTIONS:
        case SQL_SQL_CONFORMANCE:
        case SQL_SQL92_DATETIME_FUNCTIONS:
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
        case SQL_SQL92_GRANT:
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
        case SQL_SQL92_PREDICATES:
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
        case SQL_SQL92_REVOKE:
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
        case SQL_SQL92_STRING_FUNCTIONS:
        case SQL_SQL92_VALUE_EXPRESSIONS:
        case SQL_STANDARD_CLI_CONFORMANCE:
        case SQL_STATIC_CURSOR_ATTRIBUTES1:
        case SQL_STATIC_CURSOR_ATTRIBUTES2:
        case SQL_STRING_FUNCTIONS:
        case SQL_SUBQUERIES:
        case SQL_SYSTEM_FUNCTIONS:
        case SQL_TIMEDATE_ADD_INTERVALS:
        case SQL_TIMEDATE_DIFF_INTERVALS:
        case SQL_TIMEDATE_FUNCTIONS:
        case SQL_TXN_ISOLATION_OPTION:
        case SQL_UNION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0, NULL );
            if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
        }
            break;

        default:
            qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
            *pnReturn = SQL_ERROR;
            return v;
    }

    return v;
}

SQLRETURN DATAConnection::getFunctions( SQLUSMALLINT nFunctionID, SQLUSMALLINT *pnResult )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLGetFunctions( hHandle, nFunctionID, pnResult );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETFUNCTIONS );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETFUNCTIONS );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLGetFunctions", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLGetFunctions", tr( "Unexpected SQLRETURN value." ), nReturn );
            break;
    }

    return nReturn;
}

QString DATAConnection::getInfoStr( SQLUSMALLINT nInfoType, SQLRETURN *pnReturn )
{
    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    switch ( nInfoType )
    {
        // SQLCHAR*
        case SQL_ACCESSIBLE_PROCEDURES:
        case SQL_ACCESSIBLE_TABLES:
        case SQL_CATALOG_NAME:
        case SQL_CATALOG_NAME_SEPARATOR:
        case SQL_CATALOG_TERM:
        case SQL_COLLATION_SEQ:
        case SQL_COLUMN_ALIAS:
        case SQL_DATA_SOURCE_NAME:
        case SQL_DATA_SOURCE_READ_ONLY:
        case SQL_DATABASE_NAME:
        case SQL_DBMS_NAME:
        case SQL_DBMS_VER:
        case SQL_DESCRIBE_PARAMETER:
        case SQL_DM_VER:
        case SQL_DRIVER_NAME:
        case SQL_DRIVER_ODBC_VER:
        case SQL_DRIVER_VER:
        case SQL_EXPRESSIONS_IN_ORDERBY:
        case SQL_IDENTIFIER_QUOTE_CHAR:
        case SQL_INTEGRITY:
        case SQL_KEYWORDS:
        case SQL_LIKE_ESCAPE_CLAUSE:
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
        case SQL_MULT_RESULT_SETS:
        case SQL_MULTIPLE_ACTIVE_TXN:
        case SQL_NEED_LONG_DATA_LEN:
        case SQL_ODBC_VER:
        case SQL_ORDER_BY_COLUMNS_IN_SELECT:
        case SQL_PROCEDURE_TERM:
        case SQL_PROCEDURES:
        case SQL_ROW_UPDATES:
        case SQL_SCHEMA_TERM:
        case SQL_SEARCH_PATTERN_ESCAPE:
        case SQL_SERVER_NAME:
        case SQL_SPECIAL_CHARACTERS:
        case SQL_TABLE_TERM:
        case SQL_USER_NAME:
        case SQL_XOPEN_CLI_YEAR:
        {
            SQLSMALLINT nBufferBytes = 4096;
            ushort sz[4096]; // SQLWCHAR sz[4096];
            SQLSMALLINT nStringBytes = 0;

            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( sz ), nBufferBytes, &nStringBytes );
            if ( SQL_SUCCEEDED( *pnReturn ) ) return QString::fromUtf16( sz );
            else return QString();
        }
            break;

            // SQLUSMALLINT
        case SQL_ACTIVE_ENVIRONMENTS:
        case SQL_MAX_CATALOG_NAME_LEN:
        case SQL_MAX_COLUMN_NAME_LEN:
        case SQL_MAX_COLUMNS_IN_GROUP_BY:
        case SQL_MAX_COLUMNS_IN_INDEX:
        case SQL_MAX_COLUMNS_IN_ORDER_BY:
        case SQL_MAX_COLUMNS_IN_SELECT:
        case SQL_MAX_COLUMNS_IN_TABLE:
        case SQL_MAX_CONCURRENT_ACTIVITIES:
        case SQL_MAX_CURSOR_NAME_LEN:
        case SQL_MAX_DRIVER_CONNECTIONS:
        case SQL_MAX_IDENTIFIER_LEN:
        case SQL_MAX_PROCEDURE_NAME_LEN:
        case SQL_MAX_SCHEMA_NAME_LEN:
        case SQL_MAX_TABLE_NAME_LEN:
        case SQL_MAX_TABLES_IN_SELECT:
        case SQL_MAX_USER_NAME_LEN:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) ) return QString::number( n );
            else return QString();
        }
            break;

            // SQLULEN
        case SQL_DRIVER_HDBC:
        case SQL_DRIVER_HENV:
        case SQL_DRIVER_HDESC:
        case SQL_DRIVER_HLIB:
        case SQL_DRIVER_HSTMT:
        {
            SQLULEN n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*sizeof(SQLULEN)*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) ) return QString::number( n );
            else return QString();
        }
            break;

            // SQLUINTEGER
            // SQL_ASYNC_DBC_FUNCTIONS - conflict?
            // SQL_ASYNC_NOTIFICATION  - conflict?

        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
        case SQL_MAX_BINARY_LITERAL_LEN:
        case SQL_MAX_CHAR_LITERAL_LEN:
        case SQL_MAX_INDEX_SIZE:
        case SQL_MAX_ROW_SIZE:
        case SQL_MAX_STATEMENT_LEN:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) ) return QString::number( n );
            else return QString();
        }
            break;

            // SINGULAR VALUE
        case SQL_ASYNC_MODE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_AM_CONNECTION ) return QString( "SQL_AM_CONNECTION" );
                if ( n == SQL_AM_STATEMENT ) return QString( "SQL_AM_STATEMENT" );
                if ( n == SQL_AM_NONE ) return QString( "SQL_AM_NONE" );
            }
        }
            return QString();
        case SQL_CATALOG_LOCATION:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_CL_START ) return QString( "SQL_CL_START" );
                if ( n == SQL_CL_END ) return QString( "SQL_CL_END" );
            }
        }
            return QString();
        case SQL_CONCAT_NULL_BEHAVIOR:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_CB_NULL ) return QString( "SQL_CB_NULL" );
                if ( n == SQL_CB_NON_NULL ) return QString( "SQL_CB_NON_NULL" );
            }
        }
            return QString();
        case SQL_CORRELATION_NAME:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_CN_NONE ) return QString( "SQL_CN_NONE" );
                if ( n == SQL_CN_DIFFERENT ) return QString( "SQL_CN_DIFFERENT" );
                if ( n == SQL_CN_ANY ) return QString( "SQL_CN_ANY" );
            }
        }
            return QString();
        case SQL_CURSOR_COMMIT_BEHAVIOR:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /* SQL_IS_USMALLINT */, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_CB_DELETE ) return QString( "SQL_CB_DELETE" );
                if ( n == SQL_CB_CLOSE ) return QString( "SQL_CB_CLOSE" );
                if ( n == SQL_CB_PRESERVE ) return QString( "SQL_CB_PRESERVE" );
            }
        }
            return QString();
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_CB_DELETE ) return QString( "SQL_CB_DELETE" );
                if ( n == SQL_CB_CLOSE ) return QString( "SQL_CB_CLOSE" );
                if ( n == SQL_CB_PRESERVE ) return QString( "SQL_CB_PRESERVE" );
            }
        }
            return QString();
        case SQL_CURSOR_SENSITIVITY:
        {
            SQLUINTEGER n = 0; // doc says SQLUINTEGER but should probably be SQLUSMALLINT???
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_INSENSITIVE ) return QString( "SQL_INSENSITIVE" );
                if ( n == SQL_UNSPECIFIED ) return QString( "SQL_UNSPECIFIED" );
                if ( n == SQL_SENSITIVE ) return QString( "SQL_SENSITIVE" );
            }
        }
            return QString();
        case SQL_DEFAULT_TXN_ISOLATION:
        {
            SQLUINTEGER n = 0; // doc says SQLUINTEGER but should probably be SQLUSMALLINT???
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_TXN_READ_UNCOMMITTED ) return QString( "SQL_TXN_READ_UNCOMMITTED" );
                if ( n == SQL_TXN_READ_COMMITTED ) return QString( "SQL_TXN_READ_COMMITTED" );
                if ( n == SQL_TXN_REPEATABLE_READ ) return QString( "SQL_TXN_REPEATABLE_READ" );
                if ( n == SQL_TXN_SERIALIZABLE ) return QString( "SQL_TXN_SERIALIZABLE" );
            }
        }
            return QString();
            // Not picked up on MS
/*
        case SQL_DRIVER_AWARE_POOLING_SUPPORTED:                                                                              
            {                                                                                                                 
                SQLUINTEGER n = 0; // doc says SQLUINTEGER but should probably be SQLUSMALLINT???                             
                *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), SQL_IS_USMALLINT, NULL );                                 
                if ( SQL_SUCCEEDED(*pnReturn) )                                                                               
                {                                                                                                             
                    if ( n == SQL_DRIVER_AWARE_POOLING_CAPABLE ) return QString( "SQL_DRIVER_AWARE_POOLING_CAPABLE" );        
                    if ( n == SQL_DRIVER_AWARE_POOLING_NOT_CAPABLE ) return QString( "SQL_DRIVER_AWARE_POOLING_NOT_CAPABLE" );
                }                                                                                                             
            }                                                                                                                 
            return QString();                                                                                                 
*/
        case SQL_FILE_USAGE:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_FILE_NOT_SUPPORTED ) return QString( "SQL_FILE_NOT_SUPPORTED" );
                if ( n == SQL_FILE_TABLE ) return QString( "SQL_FILE_TABLE" );
                if ( n == SQL_FILE_CATALOG ) return QString( "SQL_FILE_CATALOG" );
            }
        }
            return QString();
        case SQL_GROUP_BY:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_GB_COLLATE ) return QString( "SQL_GB_COLLATE" );
                if ( n == SQL_GB_NOT_SUPPORTED ) return QString( "SQL_GB_NOT_SUPPORTED" );
                if ( n == SQL_GB_GROUP_BY_EQUALS_SELECT ) return QString( "SQL_GB_GROUP_BY_EQUALS_SELECT" );
                if ( n == SQL_GB_GROUP_BY_CONTAINS_SELECT ) return QString( "SQL_GB_GROUP_BY_CONTAINS_SELECT" );
                if ( n == SQL_GB_NO_RELATION ) return QString( "SQL_GB_NO_RELATION" );
            }
        }
            return QString();
        case SQL_IDENTIFIER_CASE:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_IC_UPPER ) return QString( "SQL_IC_UPPER" );
                if ( n == SQL_IC_LOWER ) return QString( "SQL_IC_LOWER" );
                if ( n == SQL_IC_SENSITIVE ) return QString( "SQL_IC_SENSITIVE" );
                if ( n == SQL_IC_MIXED ) return QString( "SQL_IC_MIXED" );
            }
        }
            return QString();
        case SQL_NON_NULLABLE_COLUMNS:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_NNC_NULL ) return QString( "SQL_NNC_NULL" );
                if ( n == SQL_NNC_NON_NULL ) return QString( "SQL_NNC_NON_NULL" );
            }
        }
            return QString();
        case SQL_NULL_COLLATION:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_USMALLINT*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_NC_END ) return QString( "SQL_NC_END" );
                if ( n == SQL_NC_HIGH ) return QString( "SQL_NC_HIGH" );
                if ( n == SQL_NC_LOW ) return QString( "SQL_NC_LOW" );
                if ( n == SQL_NC_START ) return QString( "SQL_NC_START" );
            }
        }
            return QString();
        case SQL_ODBC_INTERFACE_CONFORMANCE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_OIC_CORE ) return QString( "SQL_OIC_CORE" );
                if ( n == SQL_OIC_LEVEL1 ) return QString( "SQL_OIC_LEVEL1" );
                if ( n == SQL_OIC_LEVEL2 ) return QString( "SQL_OIC_LEVEL2" );
            }
        }
            return QString();
        case SQL_QUOTED_IDENTIFIER_CASE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_IC_UPPER ) return QString( "SQL_IC_UPPER" );
                if ( n == SQL_IC_LOWER ) return QString( "SQL_IC_LOWER" );
                if ( n == SQL_IC_SENSITIVE ) return QString( "SQL_IC_SENSITIVE" );
                if ( n == SQL_IC_MIXED ) return QString( "SQL_IC_MIXED" );
            }
        }
            return QString();
        case SQL_TXN_CAPABLE:
        {
            SQLUSMALLINT n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                if ( n == SQL_TC_NONE ) return QString( "SQL_TC_NONE" );
                if ( n == SQL_TC_DML ) return QString( "SQL_TC_DML" );
                if ( n == SQL_TC_DDL_COMMIT ) return QString( "SQL_TC_DDL_COMMIT" );
                if ( n == SQL_TC_DDL_IGNORE ) return QString( "SQL_TC_DDL_IGNORE" );
                if ( n == SQL_TC_ALL ) return QString( "SQL_TC_ALL" );
            }
        }
            return QString();

            // BITMASKS

        case SQL_AGGREGATE_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_AF_ALL ) l << "SQL_AF_ALL";
                if ( n & SQL_AF_AVG ) l << "SQL_AF_AVG";
                if ( n & SQL_AF_COUNT ) l << "SQL_AF_COUNT";
                if ( n & SQL_AF_DISTINCT ) l << "SQL_AF_DISTINCT";
                if ( n & SQL_AF_MIN ) l << "SQL_AF_MIN";
                if ( n & SQL_AF_SUM ) l << "SQL_AF_SUM";
                return l.join( ',' );
            }
        }
            return QString();

        case SQL_ALTER_DOMAIN:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_AD_ADD_DOMAIN_CONSTRAINT ) l << "SQL_AD_ADD_DOMAIN_CONSTRAINT";
                if ( n & SQL_AD_ADD_DOMAIN_DEFAULT ) l << "SQL_AD_ADD_DOMAIN_DEFAULT";
                if ( n & SQL_AD_CONSTRAINT_NAME_DEFINITION ) l << "SQL_AD_CONSTRAINT_NAME_DEFINITION";
                if ( n & SQL_AD_DROP_DOMAIN_CONSTRAINT ) l << "SQL_AD_DROP_DOMAIN_CONSTRAINT";
                if ( n & SQL_AD_DROP_DOMAIN_DEFAULT ) l << "SQL_AD_DROP_DOMAIN_DEFAULT";
                // the following should only be when SQL_AD_ADD_DOMAIN_CONSTRAINT set
                if ( n & SQL_AD_ADD_CONSTRAINT_DEFERRABLE ) l << "SQL_AD_ADD_CONSTRAINT_DEFERRABLE";
                if ( n & SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE ) l << "SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE";
                if ( n & SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED ) l << "SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED";
                if ( n & SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE ) l << "SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE";
                return l.join( ',' );
            }
        }
            return QString();

        case SQL_ALTER_TABLE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_AT_ADD_COLUMN_COLLATION ) l << "SQL_AT_ADD_COLUMN_COLLATION";
                if ( n & SQL_AT_ADD_COLUMN_DEFAULT ) l << "SQL_AT_ADD_COLUMN_DEFAULT";
                if ( n & SQL_AT_ADD_COLUMN_SINGLE ) l << "SQL_AT_ADD_COLUMN_SINGLE";
                if ( n & SQL_AT_ADD_CONSTRAINT ) l << "SQL_AT_ADD_CONSTRAINT";
                if ( n & SQL_AT_ADD_TABLE_CONSTRAINT ) l << "SQL_AT_ADD_TABLE_CONSTRAINT";
                if ( n & SQL_AT_CONSTRAINT_NAME_DEFINITION ) l << "SQL_AT_CONSTRAINT_NAME_DEFINITION";
                if ( n & SQL_AT_DROP_COLUMN_CASCADE ) l << "SQL_AT_DROP_COLUMN_CASCADE";
                if ( n & SQL_AT_DROP_COLUMN_DEFAULT ) l << "SQL_AT_DROP_COLUMN_DEFAULT";
                if ( n & SQL_AT_DROP_COLUMN_RESTRICT ) l << "SQL_AT_DROP_COLUMN_RESTRICT";
                if ( n & SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE ) l << "SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE";
                if ( n & SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT ) l << "SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT";
                if ( n & SQL_AT_SET_COLUMN_DEFAULT ) l << "SQL_AT_SET_COLUMN_DEFAULT";
                // the following should only be when SQL_AT_ADD_CONSTRAINT set
                if ( n & SQL_AT_CONSTRAINT_INITIALLY_DEFERRED ) l << "SQL_AT_CONSTRAINT_INITIALLY_DEFERRED";
                if ( n & SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE ) l << "SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE";
                if ( n & SQL_AT_CONSTRAINT_DEFERRABLE ) l << "SQL_AT_CONSTRAINT_DEFERRABLE";
                if ( n & SQL_AT_CONSTRAINT_NON_DEFERRABLE ) l << "SQL_AT_CONSTRAINT_NON_DEFERRABLE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_BATCH_ROW_COUNT:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_BRC_ROLLED_UP ) l << "SQL_BRC_ROLLED_UP";
                if ( n & SQL_BRC_PROCEDURES ) l << "SQL_BRC_PROCEDURES";
                if ( n & SQL_BRC_EXPLICIT ) l << "SQL_BRC_EXPLICIT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_BATCH_SUPPORT:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_BS_SELECT_EXPLICIT ) l << "SQL_BS_SELECT_EXPLICIT";
                if ( n & SQL_BS_ROW_COUNT_EXPLICIT ) l << "SQL_BS_ROW_COUNT_EXPLICIT";
                if ( n & SQL_BS_SELECT_PROC ) l << "SQL_BS_SELECT_PROC";
                if ( n & SQL_BS_ROW_COUNT_PROC ) l << "SQL_BS_ROW_COUNT_PROC";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_BOOKMARK_PERSISTENCE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_BP_CLOSE ) l << "SQL_BP_CLOSE";
                if ( n & SQL_BP_DELETE ) l << "SQL_BP_DELETE";
                if ( n & SQL_BP_DROP ) l << "SQL_BP_DROP";
                if ( n & SQL_BP_TRANSACTION ) l << "SQL_BP_TRANSACTION";
                if ( n & SQL_BP_UPDATE ) l << "SQL_BP_UPDATE";
                if ( n & SQL_BP_OTHER_HSTMT ) l << "SQL_BP_OTHER_HSTMT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CATALOG_USAGE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CU_DML_STATEMENTS ) l << "SQL_CU_DML_STATEMENTS";
                if ( n & SQL_CU_PROCEDURE_INVOCATION ) l << "SQL_CU_PROCEDURE_INVOCATION";
                if ( n & SQL_CU_TABLE_DEFINITION ) l << "SQL_CU_TABLE_DEFINITION";
                if ( n & SQL_CU_INDEX_DEFINITION ) l << "SQL_CU_INDEX_DEFINITION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CONVERT_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_CVT_CAST ) l << "SQL_FN_CVT_CAST";
                if ( n & SQL_FN_CVT_CONVERT ) l << "SQL_FN_CVT_CONVERT";
                return l.join( ',' );
            }
        }
            return QString();

        case SQL_CREATE_ASSERTION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA_CREATE_ASSERTION ) l << "SQL_CA_CREATE_ASSERTION";
                // only if we can express explicitly \sa SQL_ALTER_TABLE, SQL_CREATE_TABLE
                if ( n & SQL_CA_CONSTRAINT_INITIALLY_DEFERRED ) l << "SQL_CA_CONSTRAINT_INITIALLY_DEFERRED";
                if ( n & SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE ) l << "SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE";
                if ( n & SQL_CA_CONSTRAINT_DEFERRABLE ) l << "SQL_CA_CONSTRAINT_DEFERRABLE";
                if ( n & SQL_CA_CONSTRAINT_NON_DEFERRABLE ) l << "SQL_CA_CONSTRAINT_NON_DEFERRABLE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_CHARACTER_SET:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CCS_CREATE_CHARACTER_SET ) l << "SQL_CCS_CREATE_CHARACTER_SET";
                if ( n & SQL_CCS_COLLATE_CLAUSE ) l << "SQL_CCS_COLLATE_CLAUSE";
                if ( n & SQL_CCS_LIMITED_COLLATION ) l << "SQL_CCS_LIMITED_COLLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_COLLATION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CCOL_CREATE_COLLATION ) l << "SQL_CCOL_CREATE_COLLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_DOMAIN:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CDO_CREATE_DOMAIN ) l << "SQL_CDO_CREATE_DOMAIN";
                if ( n & SQL_CDO_CONSTRAINT_NAME_DEFINITION ) l << "SQL_CDO_CONSTRAINT_NAME_DEFINITION";
                if ( n & SQL_CDO_DEFAULT ) l << "SQL_CDO_DEFAULT";
                if ( n & SQL_CDO_CONSTRAINT ) l << "SQL_CDO_CONSTRAINT";
                if ( n & SQL_CDO_COLLATION ) l << "SQL_CDO_COLLATION";
                if ( n & SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED ) l << "SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED";
                if ( n & SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE ) l << "SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE";
                if ( n & SQL_CDO_CONSTRAINT_DEFERRABLE ) l << "SQL_CDO_CONSTRAINT_DEFERRABLE";
                if ( n & SQL_CDO_CONSTRAINT_NON_DEFERRABLE ) l << "SQL_CDO_CONSTRAINT_NON_DEFERRABLE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_SCHEMA:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CS_CREATE_SCHEMA ) l << "SQL_CS_CREATE_SCHEMA";
                if ( n & SQL_CS_AUTHORIZATION ) l << "SQL_CS_AUTHORIZATION";
                if ( n & SQL_CS_DEFAULT_CHARACTER_SET ) l << "SQL_CS_DEFAULT_CHARACTER_SET";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_TABLE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CT_CREATE_TABLE ) l << "SQL_CT_CREATE_TABLE";
                if ( n & SQL_CT_TABLE_CONSTRAINT ) l << "SQL_CT_TABLE_CONSTRAINT";
                if ( n & SQL_CT_CONSTRAINT_NAME_DEFINITION ) l << "SQL_CT_CONSTRAINT_NAME_DEFINITION";
                if ( n & SQL_CT_COMMIT_PRESERVE ) l << "SQL_CT_COMMIT_PRESERVE";
                if ( n & SQL_CT_COMMIT_DELETE ) l << "SQL_CT_COMMIT_DELETE";
                if ( n & SQL_CT_GLOBAL_TEMPORARY ) l << "SQL_CT_GLOBAL_TEMPORARY";
                if ( n & SQL_CT_LOCAL_TEMPORARY ) l << "SQL_CT_LOCAL_TEMPORARY";
                if ( n & SQL_CT_COLUMN_CONSTRAINT ) l << "SQL_CT_COLUMN_CONSTRAINT";
                if ( n & SQL_CT_COLUMN_DEFAULT ) l << "SQL_CT_COLUMN_DEFAULT";
                if ( n & SQL_CT_COLUMN_COLLATION ) l << "SQL_CT_COLUMN_COLLATION";
                if ( n & SQL_CT_CONSTRAINT_INITIALLY_DEFERRED ) l << "SQL_CT_CONSTRAINT_INITIALLY_DEFERRED";
                if ( n & SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE ) l << "SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE";
                if ( n & SQL_CT_CONSTRAINT_DEFERRABLE ) l << "SQL_CT_CONSTRAINT_DEFERRABLE";
                if ( n & SQL_CT_CONSTRAINT_NON_DEFERRABLE ) l << "SQL_CT_CONSTRAINT_NON_DEFERRABLE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_TRANSLATION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CTR_CREATE_TRANSLATION ) l << "SQL_CTR_CREATE_TRANSLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_CREATE_VIEW:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CV_CREATE_VIEW ) l << "SQL_CV_CREATE_VIEW";
                if ( n & SQL_CV_CHECK_OPTION ) l << "SQL_CV_CHECK_OPTION";
                if ( n & SQL_CV_CASCADED ) l << "SQL_CV_CASCADED";
                if ( n & SQL_CV_LOCAL ) l << "SQL_CV_LOCAL";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DATETIME_LITERALS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DL_SQL92_DATE ) l << "SQL_DL_SQL92_DATE";
                if ( n & SQL_DL_SQL92_TIME ) l << "SQL_DL_SQL92_TIME";
                if ( n & SQL_DL_SQL92_TIMESTAMP ) l << "SQL_DL_SQL92_TIMESTAMP";
                if ( n & SQL_DL_SQL92_INTERVAL_YEAR ) l << "SQL_DL_SQL92_INTERVAL_YEAR";
                if ( n & SQL_DL_SQL92_INTERVAL_MONTH ) l << "SQL_DL_SQL92_INTERVAL_MONTH";
                if ( n & SQL_DL_SQL92_INTERVAL_DAY ) l << "SQL_DL_SQL92_INTERVAL_DAY";
                if ( n & SQL_DL_SQL92_INTERVAL_HOUR ) l << "SQL_DL_SQL92_INTERVAL_HOUR";
                if ( n & SQL_DL_SQL92_INTERVAL_MINUTE ) l << "SQL_DL_SQL92_INTERVAL_MINUTE";
                if ( n & SQL_DL_SQL92_INTERVAL_SECOND ) l << "SQL_DL_SQL92_INTERVAL_SECOND";
                if ( n & SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH ) l << "SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH";
                if ( n & SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR ) l << "SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR";
                if ( n & SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE ) l << "SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE";
                if ( n & SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND ) l << "SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND";
                if ( n & SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE ) l << "SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE";
                if ( n & SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND ) l << "SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND";
                if ( n & SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND ) l << "SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DDL_INDEX:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DI_CREATE_INDEX ) l << "SQL_DI_CREATE_INDEX";
                if ( n & SQL_DI_DROP_INDEX ) l << "SQL_DI_DROP_INDEX";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_ASSERTION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DA_DROP_ASSERTION ) l << "SQL_DA_DROP_ASSERTION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_CHARACTER_SET:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DCS_DROP_CHARACTER_SET ) l << "SQL_DCS_DROP_CHARACTER_SET";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_COLLATION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DC_DROP_COLLATION ) l << "SQL_DC_DROP_COLLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_DOMAIN:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DD_DROP_DOMAIN ) l << "SQL_DD_DROP_DOMAIN";
                if ( n & SQL_DD_CASCADE ) l << "SQL_DD_CASCADE";
                if ( n & SQL_DD_RESTRICT ) l << "SQL_DD_RESTRICT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_SCHEMA:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DS_DROP_SCHEMA ) l << "SQL_DS_DROP_SCHEMA";
                if ( n & SQL_DS_CASCADE ) l << "SQL_DS_CASCADE";
                if ( n & SQL_DS_RESTRICT ) l << "SQL_DS_RESTRICT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_TABLE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DT_DROP_TABLE ) l << "SQL_DT_DROP_TABLE";
                if ( n & SQL_DT_CASCADE ) l << "SQL_DT_CASCADE";
                if ( n & SQL_DT_RESTRICT ) l << "SQL_DT_RESTRICT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_TRANSLATION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DTR_DROP_TRANSLATION ) l << "SQL_DTR_DROP_TRANSLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DROP_VIEW:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_DV_DROP_VIEW ) l << "SQL_DV_DROP_VIEW";
                if ( n & SQL_DV_CASCADE ) l << "SQL_DV_CASCADE";
                if ( n & SQL_DV_RESTRICT ) l << "SQL_DV_RESTRICT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA1_NEXT ) l << "SQL_CA1_NEXT";
                if ( n & SQL_CA1_ABSOLUTE ) l << "SQL_CA1_ABSOLUTE";
                if ( n & SQL_CA1_RELATIVE ) l << "SQL_CA1_RELATIVE";
                if ( n & SQL_CA1_BOOKMARK ) l << "SQL_CA1_BOOKMARK";
                if ( n & SQL_CA1_LOCK_EXCLUSIVE ) l << "SQL_CA1_LOCK_EXCLUSIVE";
                if ( n & SQL_CA1_LOCK_NO_CHANGE ) l << "SQL_CA1_LOCK_NO_CHANGE";
                if ( n & SQL_CA1_LOCK_UNLOCK ) l << "SQL_CA1_LOCK_UNLOCK";
                if ( n & SQL_CA1_POS_POSITION ) l << "SQL_CA1_POS_POSITION";
                if ( n & SQL_CA1_POS_UPDATE ) l << "SQL_CA1_POS_UPDATE";
                if ( n & SQL_CA1_POS_DELETE ) l << "SQL_CA1_POS_DELETE";
                if ( n & SQL_CA1_POS_REFRESH ) l << "SQL_CA1_POS_REFRESH";
                if ( n & SQL_CA1_POSITIONED_UPDATE ) l << "SQL_CA1_POSITIONED_UPDATE";
                if ( n & SQL_CA1_POSITIONED_DELETE ) l << "SQL_CA1_POSITIONED_DELETE";
                if ( n & SQL_CA1_SELECT_FOR_UPDATE ) l << "SQL_CA1_SELECT_FOR_UPDATE";
                if ( n & SQL_CA1_BULK_ADD ) l << "SQL_CA1_BULK_ADD";
                if ( n & SQL_CA1_BULK_UPDATE_BY_BOOKMARK ) l << "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_DELETE_BY_BOOKMARK ) l << "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_FETCH_BY_BOOKMARK ) l << "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA2_READ_ONLY_CONCURRENCY ) l << "SQL_CA2_READ_ONLY_CONCURRENCY";
                if ( n & SQL_CA2_LOCK_CONCURRENCY ) l << "SQL_CA2_LOCK_CONCURRENCY";
                if ( n & SQL_CA2_OPT_ROWVER_CONCURRENCY ) l << "SQL_CA2_OPT_ROWVER_CONCURRENCY";
                if ( n & SQL_CA2_OPT_VALUES_CONCURRENCY ) l << "SQL_CA2_OPT_VALUES_CONCURRENCY";
                if ( n & SQL_CA2_SENSITIVITY_ADDITIONS ) l << "SQL_CA2_SENSITIVITY_ADDITIONS";
                if ( n & SQL_CA2_SENSITIVITY_DELETIONS ) l << "SQL_CA2_SENSITIVITY_DELETIONS";
                if ( n & SQL_CA2_SENSITIVITY_UPDATES ) l << "SQL_CA2_SENSITIVITY_UPDATES";
                if ( n & SQL_CA2_MAX_ROWS_SELECT ) l << "SQL_CA2_MAX_ROWS_SELECT";
                if ( n & SQL_CA2_MAX_ROWS_INSERT ) l << "SQL_CA2_MAX_ROWS_INSERT";
                if ( n & SQL_CA2_MAX_ROWS_DELETE ) l << "SQL_CA2_MAX_ROWS_DELETE";
                if ( n & SQL_CA2_MAX_ROWS_UPDATE ) l << "SQL_CA2_MAX_ROWS_UPDATE";
                if ( n & SQL_CA2_MAX_ROWS_CATALOG ) l << "SQL_CA2_MAX_ROWS_CATALOG";
                if ( n & SQL_CA2_MAX_ROWS_AFFECTS_ALL ) l << "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
                if ( n & SQL_CA2_CRC_EXACT ) l << "SQL_CA2_CRC_EXACT";
                if ( n & SQL_CA2_CRC_APPROXIMATE ) l << "SQL_CA2_CRC_APPROXIMATE";
                if ( n & SQL_CA2_SIMULATE_NON_UNIQUE ) l << "SQL_CA2_SIMULATE_NON_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_TRY_UNIQUE ) l << "SQL_CA2_SIMULATE_TRY_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_UNIQUE ) l << "SQL_CA2_SIMULATE_UNIQUE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA1_NEXT ) l << "SQL_CA1_NEXT";
                if ( n & SQL_CA1_LOCK_EXCLUSIVE ) l << "SQL_CA1_LOCK_EXCLUSIVE";
                if ( n & SQL_CA1_LOCK_NO_CHANGE ) l << "SQL_CA1_LOCK_NO_CHANGE";
                if ( n & SQL_CA1_LOCK_UNLOCK ) l << "SQL_CA1_LOCK_UNLOCK";
                if ( n & SQL_CA1_POS_POSITION ) l << "SQL_CA1_POS_POSITION";
                if ( n & SQL_CA1_POS_UPDATE ) l << "SQL_CA1_POS_UPDATE";
                if ( n & SQL_CA1_POS_DELETE ) l << "SQL_CA1_POS_DELETE";
                if ( n & SQL_CA1_POS_REFRESH ) l << "SQL_CA1_POS_REFRESH";
                if ( n & SQL_CA1_POSITIONED_UPDATE ) l << "SQL_CA1_POSITIONED_UPDATE";
                if ( n & SQL_CA1_POSITIONED_DELETE ) l << "SQL_CA1_POSITIONED_DELETE";
                if ( n & SQL_CA1_SELECT_FOR_UPDATE ) l << "SQL_CA1_SELECT_FOR_UPDATE";
                if ( n & SQL_CA1_BULK_ADD ) l << "SQL_CA1_BULK_ADD";
                if ( n & SQL_CA1_BULK_UPDATE_BY_BOOKMARK ) l << "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_DELETE_BY_BOOKMARK ) l << "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_FETCH_BY_BOOKMARK ) l << "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA2_READ_ONLY_CONCURRENCY  ) l << "SQL_CA2_READ_ONLY_CONCURRENCY ";
                if ( n & SQL_CA2_LOCK_CONCURRENCY       ) l << "SQL_CA2_LOCK_CONCURRENCY";
                if ( n & SQL_CA2_OPT_ROWVER_CONCURRENCY ) l << "SQL_CA2_OPT_ROWVER_CONCURRENCY";
                if ( n & SQL_CA2_OPT_VALUES_CONCURRENCY ) l << "SQL_CA2_OPT_VALUES_CONCURRENCY";
                if ( n & SQL_CA2_SENSITIVITY_ADDITIONS  ) l << "SQL_CA2_SENSITIVITY_ADDITIONS";
                if ( n & SQL_CA2_SENSITIVITY_DELETIONS  ) l << "SQL_CA2_SENSITIVITY_DELETIONS";
                if ( n & SQL_CA2_SENSITIVITY_UPDATES    ) l << "SQL_CA2_SENSITIVITY_UPDATES";
                if ( n & SQL_CA2_MAX_ROWS_SELECT        ) l << "SQL_CA2_MAX_ROWS_SELECT";
                if ( n & SQL_CA2_MAX_ROWS_INSERT        ) l << "SQL_CA2_MAX_ROWS_INSERT";
                if ( n & SQL_CA2_MAX_ROWS_DELETE        ) l << "SQL_CA2_MAX_ROWS_DELETE";
                if ( n & SQL_CA2_MAX_ROWS_UPDATE        ) l << "SQL_CA2_MAX_ROWS_UPDATE";
                if ( n & SQL_CA2_MAX_ROWS_CATALOG       ) l << "SQL_CA2_MAX_ROWS_CATALOG";
                if ( n & SQL_CA2_MAX_ROWS_AFFECTS_ALL   ) l << "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
                if ( n & SQL_CA2_CRC_EXACT              ) l << "SQL_CA2_CRC_EXACT";
                if ( n & SQL_CA2_CRC_APPROXIMATE        ) l << "SQL_CA2_CRC_APPROXIMATE";
                if ( n & SQL_CA2_SIMULATE_NON_UNIQUE    ) l << "SQL_CA2_SIMULATE_NON_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_TRY_UNIQUE    ) l << "SQL_CA2_SIMULATE_TRY_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_UNIQUE        ) l << "SQL_CA2_SIMULATE_UNIQUE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_GETDATA_EXTENSIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_GD_ANY_COLUMN    ) l << "SQL_GD_ANY_COLUMN";
                if ( n & SQL_GD_ANY_ORDER     ) l << "SQL_GD_ANY_ORDER";
                if ( n & SQL_GD_BLOCK         ) l << "SQL_GD_BLOCK";
                if ( n & SQL_GD_BOUND         ) l << "SQL_GD_BOUND";
                // not on MS
                // if ( n & SQL_GD_OUTPUT_PARAMS ) l << "SQL_GD_OUTPUT_PARAMS";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_INDEX_KEYWORDS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_IK_NONE ) l << "SQL_IK_NONE";
                if ( n & SQL_IK_ASC  ) l << "SQL_IK_ASC";
                if ( n & SQL_IK_DESC ) l << "SQL_IK_DESC";
                if ( n & SQL_IK_ALL  ) l << "SQL_IK_ALL";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_INFO_SCHEMA_VIEWS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_ISV_ASSERTIONS ) l << "SQL_ISV_ASSERTIONS";
                if ( n & SQL_ISV_CHARACTER_SETS ) l << "SQL_ISV_CHARACTER_SETS";
                if ( n & SQL_ISV_CHECK_CONSTRAINTS ) l << "SQL_ISV_CHECK_CONSTRAINTS";
                if ( n & SQL_ISV_COLLATIONS ) l << "SQL_ISV_COLLATIONS";
                if ( n & SQL_ISV_COLUMN_DOMAIN_USAGE ) l << "SQL_ISV_COLUMN_DOMAIN_USAGE";
                if ( n & SQL_ISV_COLUMN_PRIVILEGES ) l << "SQL_ISV_COLUMN_PRIVILEGES";
                if ( n & SQL_ISV_COLUMNS ) l << "SQL_ISV_COLUMNS";
                if ( n & SQL_ISV_CONSTRAINT_COLUMN_USAGE ) l << "SQL_ISV_CONSTRAINT_COLUMN_USAGE";
                if ( n & SQL_ISV_CONSTRAINT_TABLE_USAGE ) l << "SQL_ISV_CONSTRAINT_TABLE_USAGE";
                if ( n & SQL_ISV_DOMAIN_CONSTRAINTS ) l << "SQL_ISV_DOMAIN_CONSTRAINTS";
                if ( n & SQL_ISV_DOMAINS ) l << "SQL_ISV_DOMAINS";
                if ( n & SQL_ISV_KEY_COLUMN_USAGE ) l << "SQL_ISV_KEY_COLUMN_USAGE";
                if ( n & SQL_ISV_REFERENTIAL_CONSTRAINTS ) l << "SQL_ISV_REFERENTIAL_CONSTRAINTS";
                if ( n & SQL_ISV_SCHEMATA ) l << "SQL_ISV_SCHEMATA";
                if ( n & SQL_ISV_SQL_LANGUAGES ) l << "SQL_ISV_SQL_LANGUAGES";
                if ( n & SQL_ISV_TABLE_CONSTRAINTS ) l << "SQL_ISV_TABLE_CONSTRAINTS";
                if ( n & SQL_ISV_TABLE_PRIVILEGES ) l << "SQL_ISV_TABLE_PRIVILEGES";
                if ( n & SQL_ISV_TABLES ) l << "SQL_ISV_TABLES";
                if ( n & SQL_ISV_TRANSLATIONS ) l << "SQL_ISV_TRANSLATIONS";
                if ( n & SQL_ISV_USAGE_PRIVILEGES ) l << "SQL_ISV_USAGE_PRIVILEGES";
                if ( n & SQL_ISV_VIEW_COLUMN_USAGE ) l << "SQL_ISV_VIEW_COLUMN_USAGE";
                if ( n & SQL_ISV_VIEW_TABLE_USAGE ) l << "SQL_ISV_VIEW_TABLE_USAGE";
                if ( n & SQL_ISV_VIEWS ) l << "SQL_ISV_VIEWS";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_INSERT_STATEMENT:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_IS_INSERT_LITERALS ) l << "SQL_IS_INSERT_LITERALS";
                if ( n & SQL_IS_INSERT_SEARCHED ) l << "SQL_IS_INSERT_SEARCHED";
                if ( n & SQL_IS_SELECT_INTO ) l << "SQL_IS_SELECT_INTO";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_KEYSET_CURSOR_ATTRIBUTES1:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA1_NEXT                    ) l << "SQL_CA1_NEXT";
                if ( n & SQL_CA1_ABSOLUTE                ) l << "SQL_CA1_ABSOLUTE";
                if ( n & SQL_CA1_RELATIVE                ) l << "SQL_CA1_RELATIVE";
                if ( n & SQL_CA1_BOOKMARK                ) l << "SQL_CA1_BOOKMARK";
                if ( n & SQL_CA1_LOCK_EXCLUSIVE          ) l << "SQL_CA1_LOCK_EXCLUSIVE";
                if ( n & SQL_CA1_LOCK_NO_CHANGE          ) l << "SQL_CA1_LOCK_NO_CHANGE";
                if ( n & SQL_CA1_LOCK_UNLOCK             ) l << "SQL_CA1_LOCK_UNLOCK";
                if ( n & SQL_CA1_POS_POSITION            ) l << "SQL_CA1_POS_POSITION";
                if ( n & SQL_CA1_POS_UPDATE              ) l << "SQL_CA1_POS_UPDATE";
                if ( n & SQL_CA1_POS_DELETE              ) l << "SQL_CA1_POS_DELETE";
                if ( n & SQL_CA1_POS_REFRESH             ) l << "SQL_CA1_POS_REFRESH";
                if ( n & SQL_CA1_POSITIONED_UPDATE       ) l << "SQL_CA1_POSITIONED_UPDATE";
                if ( n & SQL_CA1_POSITIONED_DELETE       ) l << "SQL_CA1_POSITIONED_DELETE";
                if ( n & SQL_CA1_SELECT_FOR_UPDATE       ) l << "SQL_CA1_SELECT_FOR_UPDATE";
                if ( n & SQL_CA1_BULK_ADD                ) l << "SQL_CA1_BULK_ADD";
                if ( n & SQL_CA1_BULK_UPDATE_BY_BOOKMARK ) l << "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_DELETE_BY_BOOKMARK ) l << "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_FETCH_BY_BOOKMARK  ) l << "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_KEYSET_CURSOR_ATTRIBUTES2:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA2_READ_ONLY_CONCURRENCY  ) l << "SQL_CA2_READ_ONLY_CONCURRENCY";
                if ( n & SQL_CA2_LOCK_CONCURRENCY       ) l << "SQL_CA2_LOCK_CONCURRENCY";
                if ( n & SQL_CA2_OPT_ROWVER_CONCURRENCY ) l << "SQL_CA2_OPT_ROWVER_CONCURRENCY";
                if ( n & SQL_CA2_OPT_VALUES_CONCURRENCY ) l << "SQL_CA2_OPT_VALUES_CONCURRENCY";
                if ( n & SQL_CA2_SENSITIVITY_ADDITIONS  ) l << "SQL_CA2_SENSITIVITY_ADDITIONS";
                if ( n & SQL_CA2_SENSITIVITY_DELETIONS  ) l << "SQL_CA2_SENSITIVITY_DELETIONS";
                if ( n & SQL_CA2_SENSITIVITY_UPDATES    ) l << "SQL_CA2_SENSITIVITY_UPDATES";
                if ( n & SQL_CA2_MAX_ROWS_SELECT        ) l << "SQL_CA2_MAX_ROWS_SELECT";
                if ( n & SQL_CA2_MAX_ROWS_INSERT        ) l << "SQL_CA2_MAX_ROWS_INSERT";
                if ( n & SQL_CA2_MAX_ROWS_DELETE        ) l << "SQL_CA2_MAX_ROWS_DELETE";
                if ( n & SQL_CA2_MAX_ROWS_UPDATE        ) l << "SQL_CA2_MAX_ROWS_UPDATE";
                if ( n & SQL_CA2_MAX_ROWS_CATALOG       ) l << "SQL_CA2_MAX_ROWS_CATALOG";
                if ( n & SQL_CA2_MAX_ROWS_AFFECTS_ALL   ) l << "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
                if ( n & SQL_CA2_CRC_EXACT              ) l << "SQL_CA2_CRC_EXACT";
                if ( n & SQL_CA2_CRC_APPROXIMATE        ) l << "SQL_CA2_CRC_APPROXIMATE";
                if ( n & SQL_CA2_SIMULATE_NON_UNIQUE    ) l << "SQL_CA2_SIMULATE_NON_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_TRY_UNIQUE    ) l << "SQL_CA2_SIMULATE_TRY_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_UNIQUE        ) l << "SQL_CA2_SIMULATE_UNIQUE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_NUMERIC_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_NUM_ABS      ) l << "SQL_FN_NUM_ABS";
                if ( n & SQL_FN_NUM_ACOS     ) l << "SQL_FN_NUM_ACOS";
                if ( n & SQL_FN_NUM_ASIN     ) l << "SQL_FN_NUM_ASIN";
                if ( n & SQL_FN_NUM_ATAN     ) l << "SQL_FN_NUM_ATAN";
                if ( n & SQL_FN_NUM_ATAN2    ) l << "SQL_FN_NUM_ATAN2";
                if ( n & SQL_FN_NUM_CEILING  ) l << "SQL_FN_NUM_CEILING";
                if ( n & SQL_FN_NUM_COS      ) l << "SQL_FN_NUM_COS";
                if ( n & SQL_FN_NUM_COT      ) l << "SQL_FN_NUM_COT";
                if ( n & SQL_FN_NUM_DEGREES  ) l << "SQL_FN_NUM_DEGREES";
                if ( n & SQL_FN_NUM_EXP      ) l << "SQL_FN_NUM_EXP";
                if ( n & SQL_FN_NUM_FLOOR    ) l << "SQL_FN_NUM_FLOOR";
                if ( n & SQL_FN_NUM_LOG      ) l << "SQL_FN_NUM_LOG";
                if ( n & SQL_FN_NUM_LOG10    ) l << "SQL_FN_NUM_LOG10";
                if ( n & SQL_FN_NUM_MOD      ) l << "SQL_FN_NUM_MOD";
                if ( n & SQL_FN_NUM_PI       ) l << "SQL_FN_NUM_PI";
                if ( n & SQL_FN_NUM_POWER    ) l << "SQL_FN_NUM_POWER";
                if ( n & SQL_FN_NUM_RADIANS  ) l << "SQL_FN_NUM_RADIANS";
                if ( n & SQL_FN_NUM_RAND     ) l << "SQL_FN_NUM_RAND";
                if ( n & SQL_FN_NUM_ROUND    ) l << "SQL_FN_NUM_ROUND";
                if ( n & SQL_FN_NUM_SIGN     ) l << "SQL_FN_NUM_SIGN";
                if ( n & SQL_FN_NUM_SIN      ) l << "SQL_FN_NUM_SIN";
                if ( n & SQL_FN_NUM_SQRT     ) l << "SQL_FN_NUM_SQRT";
                if ( n & SQL_FN_NUM_TAN      ) l << "SQL_FN_NUM_TAN";
                if ( n & SQL_FN_NUM_TRUNCATE ) l << "SQL_FN_NUM_TRUNCATE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_OJ_CAPABILITIES:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_OJ_LEFT ) l << "SQL_OJ_LEFT";
                if ( n & SQL_OJ_RIGHT ) l << "SQL_OJ_RIGHT";
                if ( n & SQL_OJ_FULL ) l << "SQL_OJ_FULL";
                if ( n & SQL_OJ_NESTED ) l << "SQL_OJ_NESTED";
                if ( n & SQL_OJ_NOT_ORDERED ) l << "SQL_OJ_NOT_ORDERED";
                if ( n & SQL_OJ_INNER ) l << "SQL_OJ_INNER";
                if ( n & SQL_OJ_ALL_COMPARISON_OPS ) l << "SQL_OJ_ALL_COMPARISON_OPS";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_PARAM_ARRAY_ROW_COUNTS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_PARC_BATCH ) l << "SQL_PARC_BATCH";
                if ( n & SQL_PARC_NO_BATCH ) l << "SQL_PARC_NO_BATCH";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_PARAM_ARRAY_SELECTS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_PAS_BATCH ) l << "SQL_PAS_BATCH";
                if ( n & SQL_PAS_NO_BATCH ) l << "SQL_PAS_NO_BATCH";
                if ( n & SQL_PAS_NO_SELECT ) l << "SQL_PAS_NO_SELECT";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_POS_OPERATIONS:
        {
            SQLINTEGER n = 0; // doc says SQLINTEGER but probably should be SQLUINTEGER
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_POS_POSITION ) l << "SQL_POS_POSITION";
                if ( n & SQL_POS_REFRESH  ) l << "SQL_POS_REFRESH";
                if ( n & SQL_POS_UPDATE   ) l << "SQL_POS_UPDATE";
                if ( n & SQL_POS_DELETE   ) l << "SQL_POS_DELETE";
                if ( n & SQL_POS_ADD      ) l << "SQL_POS_ADD";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SCHEMA_USAGE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SU_DML_STATEMENTS ) l << "SQL_SU_DML_STATEMENTS";
                if ( n & SQL_SU_PROCEDURE_INVOCATION ) l << "SQL_SU_PROCEDURE_INVOCATION";
                if ( n & SQL_SU_TABLE_DEFINITION ) l << "SQL_SU_TABLE_DEFINITION";
                if ( n & SQL_SU_INDEX_DEFINITION ) l << "SQL_SU_INDEX_DEFINITION";
                if ( n & SQL_SU_PRIVILEGE_DEFINITION ) l << "SQL_SU_PRIVILEGE_DEFINITION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SCROLL_OPTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SO_FORWARD_ONLY ) l << "SQL_SO_FORWARD_ONLY";
                if ( n & SQL_SO_STATIC ) l << "SQL_SO_STATIC";
                if ( n & SQL_SO_KEYSET_DRIVEN ) l << "SQL_SO_KEYSET_DRIVEN";
                if ( n & SQL_SO_DYNAMIC ) l << "SQL_SO_DYNAMIC";
                if ( n & SQL_SO_MIXED ) l << "SQL_SO_MIXED";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL_CONFORMANCE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SC_SQL92_ENTRY ) l << "SQL_SC_SQL92_ENTRY";
                if ( n & SQL_SC_FIPS127_2_TRANSITIONAL ) l << "SQL_SC_FIPS127_2_TRANSITIONAL";
                if ( n & SQL_SC_SQL92_FULL ) l << "SQL_SC_SQL92_FULL";
                if ( n & SQL_SC_SQL92_INTERMEDIATE ) l << "SQL_SC_ SQL92_INTERMEDIATE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_DATETIME_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SDF_CURRENT_DATE ) l << "SQL_SDF_CURRENT_DATE";
                if ( n & SQL_SDF_CURRENT_TIME ) l << "SQL_SDF_CURRENT_TIME";
                if ( n & SQL_SDF_CURRENT_TIMESTAMP ) l << "SQL_SDF_CURRENT_TIMESTAMP";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SFKD_CASCADE ) l << "SQL_SFKD_CASCADE";
                if ( n & SQL_SFKD_NO_ACTION ) l << "SQL_SFKD_NO_ACTION";
                if ( n & SQL_SFKD_SET_DEFAULT ) l << "SQL_SFKD_SET_DEFAULT";
                if ( n & SQL_SFKD_SET_NULL ) l << "SQL_SFKD_SET_NULL";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SFKU_CASCADE ) l << "SQL_SFKU_CASCADE";
                if ( n & SQL_SFKU_NO_ACTION ) l << "SQL_SFKU_NO_ACTION";
                if ( n & SQL_SFKU_SET_DEFAULT ) l << "SQL_SFKU_SET_DEFAULT";
                if ( n & SQL_SFKU_SET_NULL ) l << "SQL_SFKU_SET_NULL";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_GRANT:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SG_DELETE_TABLE           ) l << "SQL_SG_DELETE_TABLE";
                if ( n & SQL_SG_INSERT_COLUMN          ) l << "SQL_SG_INSERT_COLUMN";
                if ( n & SQL_SG_INSERT_TABLE           ) l << "SQL_SG_INSERT_TABLE";
                if ( n & SQL_SG_REFERENCES_TABLE       ) l << "SQL_SG_REFERENCES_TABLE";
                if ( n & SQL_SG_REFERENCES_COLUMN      ) l << "SQL_SG_REFERENCES_COLUMN";
                if ( n & SQL_SG_SELECT_TABLE           ) l << "SQL_SG_SELECT_TABLE";
                if ( n & SQL_SG_UPDATE_COLUMN          ) l << "SQL_SG_UPDATE_COLUMN";
                if ( n & SQL_SG_UPDATE_TABLE           ) l << "SQL_SG_UPDATE_TABLE";
                if ( n & SQL_SG_USAGE_ON_DOMAIN        ) l << "SQL_SG_USAGE_ON_DOMAIN";
                if ( n & SQL_SG_USAGE_ON_CHARACTER_SET ) l << "SQL_SG_USAGE_ON_CHARACTER_SET";
                if ( n & SQL_SG_USAGE_ON_COLLATION     ) l << "SQL_SG_USAGE_ON_COLLATION";
                if ( n & SQL_SG_USAGE_ON_TRANSLATION   ) l << "SQL_SG_USAGE_ON_TRANSLATION";
                if ( n & SQL_SG_WITH_GRANT_OPTION      ) l << "SQL_SG_WITH_GRANT_OPTION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SNVF_BIT_LENGTH      ) l << "SQL_SNVF_BIT_LENGTH";
                if ( n & SQL_SNVF_CHAR_LENGTH     ) l << "SQL_SNVF_CHAR_LENGTH";
                if ( n & SQL_SNVF_CHARACTER_LENGTH ) l << "SQL_SNVF_CHARACTER_LENGTH";
                if ( n & SQL_SNVF_EXTRACT         ) l << "SQL_SNVF_EXTRACT";
                if ( n & SQL_SNVF_OCTET_LENGTH    ) l << "SQL_SNVF_OCTET_LENGTH";
                if ( n & SQL_SNVF_POSITION        ) l << "SQL_SNVF_POSITION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_PREDICATES:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SP_BETWEEN               ) l << "SQL_SP_BETWEEN";
                if ( n & SQL_SP_COMPARISON            ) l << "SQL_SP_COMPARISON";
                if ( n & SQL_SP_EXISTS                ) l << "SQL_SP_EXISTS";
                if ( n & SQL_SP_IN                    ) l << "SQL_SP_IN";
                if ( n & SQL_SP_ISNOTNULL             ) l << "SQL_SP_ISNOTNULL";
                if ( n & SQL_SP_ISNULL                ) l << "SQL_SP_ISNULL";
                if ( n & SQL_SP_LIKE                  ) l << "SQL_SP_LIKE";
                if ( n & SQL_SP_MATCH_FULL            ) l << "SQL_SP_MATCH_FULL";
                if ( n & SQL_SP_MATCH_PARTIAL         ) l << "SQL_SP_MATCH_PARTIAL";
                if ( n & SQL_SP_MATCH_UNIQUE_FULL     ) l << "SQL_SP_MATCH_UNIQUE_FULL";
                if ( n & SQL_SP_MATCH_UNIQUE_PARTIAL  ) l << "SQL_SP_MATCH_UNIQUE_PARTIAL";
                if ( n & SQL_SP_OVERLAPS              ) l << "SQL_SP_OVERLAPS";
                if ( n & SQL_SP_QUANTIFIED_COMPARISON ) l << "SQL_SP_QUANTIFIED_COMPARISON";
                if ( n & SQL_SP_UNIQUE                ) l << "SQL_SP_UNIQUE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SRJO_CORRESPONDING_CLAUSE ) l << "SQL_SRJO_CORRESPONDING_CLAUSE";
                if ( n & SQL_SRJO_CROSS_JOIN           ) l << "SQL_SRJO_CROSS_JOIN";
                if ( n & SQL_SRJO_EXCEPT_JOIN          ) l << "SQL_SRJO_EXCEPT_JOIN";
                if ( n & SQL_SRJO_FULL_OUTER_JOIN      ) l << "SQL_SRJO_FULL_OUTER_JOIN";
                if ( n & SQL_SRJO_INNER_JOIN           ) l << "SQL_SRJO_INNER_JOIN";
                if ( n & SQL_SRJO_INTERSECT_JOIN       ) l << "SQL_SRJO_INTERSECT_JOIN";
                if ( n & SQL_SRJO_LEFT_OUTER_JOIN      ) l << "SQL_SRJO_LEFT_OUTER_JOIN";
                if ( n & SQL_SRJO_NATURAL_JOIN         ) l << "SQL_SRJO_NATURAL_JOIN";
                if ( n & SQL_SRJO_RIGHT_OUTER_JOIN     ) l << "SQL_SRJO_RIGHT_OUTER_JOIN";
                if ( n & SQL_SRJO_UNION_JOIN           ) l << "SQL_SRJO_UNION_JOIN";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_REVOKE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SR_CASCADE                ) l << "SQL_SR_CASCADE";
                if ( n & SQL_SR_DELETE_TABLE           ) l << "SQL_SR_DELETE_TABLE";
                if ( n & SQL_SR_GRANT_OPTION_FOR       ) l << "SQL_SR_GRANT_OPTION_FOR";
                if ( n & SQL_SR_INSERT_COLUMN          ) l << "SQL_SR_INSERT_COLUMN";
                if ( n & SQL_SR_INSERT_TABLE           ) l << "SQL_SR_INSERT_TABLE";
                if ( n & SQL_SR_REFERENCES_COLUMN      ) l << "SQL_SR_REFERENCES_COLUMN";
                if ( n & SQL_SR_REFERENCES_TABLE       ) l << "SQL_SR_REFERENCES_TABLE";
                if ( n & SQL_SR_RESTRICT               ) l << "SQL_SR_RESTRICT";
                if ( n & SQL_SR_SELECT_TABLE           ) l << "SQL_SR_SELECT_TABLE";
                if ( n & SQL_SR_UPDATE_COLUMN          ) l << "SQL_SR_UPDATE_COLUMN";
                if ( n & SQL_SR_UPDATE_TABLE           ) l << "SQL_SR_UPDATE_TABLE";
                if ( n & SQL_SR_USAGE_ON_DOMAIN        ) l << "SQL_SR_USAGE_ON_DOMAIN";
                if ( n & SQL_SR_USAGE_ON_CHARACTER_SET ) l << "SQL_SR_USAGE_ON_CHARACTER_SET";
                if ( n & SQL_SR_USAGE_ON_COLLATION     ) l << "SQL_SR_USAGE_ON_COLLATION";
                if ( n & SQL_SR_USAGE_ON_TRANSLATION   ) l << "SQL_SR_USAGE_ON_TRANSLATION";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SRVC_VALUE_EXPRESSION ) l << "SQL_SRVC_VALUE_EXPRESSION";
                if ( n & SQL_SRVC_NULL ) l << "SQL_SRVC_NULL";
                if ( n & SQL_SRVC_DEFAULT ) l << "SQL_SRVC_DEFAULT";
                if ( n & SQL_SRVC_ROW_SUBQUERY ) l << "SQL_SRVC_ROW_SUBQUERY";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_STRING_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SSF_CONVERT ) l << "SQL_SSF_CONVERT";
                if ( n & SQL_SSF_LOWER ) l << "SQL_SSF_LOWER";
                if ( n & SQL_SSF_UPPER ) l << "SQL_SSF_UPPER";
                if ( n & SQL_SSF_SUBSTRING ) l << "SQL_SSF_SUBSTRING";
                if ( n & SQL_SSF_TRANSLATE ) l << "SQL_SSF_TRANSLATE";
                if ( n & SQL_SSF_TRIM_BOTH ) l << "SQL_SSF_TRIM_BOTH";
                if ( n & SQL_SSF_TRIM_LEADING ) l << "SQL_SSF_TRIM_LEADING";
                if ( n & SQL_SSF_TRIM_TRAILING ) l << "SQL_SSF_TRIM_TRAILING";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SQL92_VALUE_EXPRESSIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SVE_CASE ) l << "SQL_SVE_CASE";
                if ( n & SQL_SVE_CAST ) l << "SQL_SVE_CAST";
                if ( n & SQL_SVE_COALESCE ) l << "SQL_SVE_COALESCE";
                if ( n & SQL_SVE_NULLIF ) l << "SQL_SVE_NULLIF";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_STANDARD_CLI_CONFORMANCE:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SCC_XOPEN_CLI_VERSION1 ) l << "SQL_SCC_XOPEN_CLI_VERSION1";
                if ( n & SQL_SCC_ISO92_CLI ) l << "SQL_SCC_ISO92_CLI";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_STATIC_CURSOR_ATTRIBUTES1:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA1_NEXT                    ) l << "SQL_CA1_NEXT";
                if ( n & SQL_CA1_ABSOLUTE                ) l << "SQL_CA1_ABSOLUTE";
                if ( n & SQL_CA1_RELATIVE                ) l << "SQL_CA1_RELATIVE";
                if ( n & SQL_CA1_BOOKMARK                ) l << "SQL_CA1_BOOKMARK";
                if ( n & SQL_CA1_LOCK_NO_CHANGE          ) l << "SQL_CA1_LOCK_NO_CHANGE";
                if ( n & SQL_CA1_LOCK_EXCLUSIVE          ) l << "SQL_CA1_LOCK_EXCLUSIVE";
                if ( n & SQL_CA1_LOCK_UNLOCK             ) l << "SQL_CA1_LOCK_UNLOCK";
                if ( n & SQL_CA1_POS_POSITION            ) l << "SQL_CA1_POS_POSITION";
                if ( n & SQL_CA1_POS_UPDATE              ) l << "SQL_CA1_POS_UPDATE";
                if ( n & SQL_CA1_POS_DELETE              ) l << "SQL_CA1_POS_DELETE";
                if ( n & SQL_CA1_POS_REFRESH             ) l << "SQL_CA1_POS_REFRESH";
                if ( n & SQL_CA1_POSITIONED_UPDATE       ) l << "SQL_CA1_POSITIONED_UPDATE";
                if ( n & SQL_CA1_POSITIONED_DELETE       ) l << "SQL_CA1_POSITIONED_DELETE";
                if ( n & SQL_CA1_SELECT_FOR_UPDATE       ) l << "SQL_CA1_SELECT_FOR_UPDATE";
                if ( n & SQL_CA1_BULK_ADD                ) l << "SQL_CA1_BULK_ADD";
                if ( n & SQL_CA1_BULK_UPDATE_BY_BOOKMARK ) l << "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_DELETE_BY_BOOKMARK ) l << "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
                if ( n & SQL_CA1_BULK_FETCH_BY_BOOKMARK  ) l << "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_STATIC_CURSOR_ATTRIBUTES2:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_CA2_READ_ONLY_CONCURRENCY  ) l << "SQL_CA2_READ_ONLY_CONCURRENCY";
                if ( n & SQL_CA2_LOCK_CONCURRENCY       ) l << "SQL_CA2_LOCK_CONCURRENCY";
                if ( n & SQL_CA2_OPT_ROWVER_CONCURRENCY ) l << "SQL_CA2_OPT_ROWVER_CONCURRENCY";
                if ( n & SQL_CA2_OPT_VALUES_CONCURRENCY ) l << "SQL_CA2_OPT_VALUES_CONCURRENCY";
                if ( n & SQL_CA2_SENSITIVITY_ADDITIONS  ) l << "SQL_CA2_SENSITIVITY_ADDITIONS";
                if ( n & SQL_CA2_SENSITIVITY_DELETIONS  ) l << "SQL_CA2_SENSITIVITY_DELETIONS";
                if ( n & SQL_CA2_SENSITIVITY_UPDATES    ) l << "SQL_CA2_SENSITIVITY_UPDATES";
                if ( n & SQL_CA2_MAX_ROWS_SELECT        ) l << "SQL_CA2_MAX_ROWS_SELECT";
                if ( n & SQL_CA2_MAX_ROWS_INSERT        ) l << "SQL_CA2_MAX_ROWS_INSERT";
                if ( n & SQL_CA2_MAX_ROWS_DELETE        ) l << "SQL_CA2_MAX_ROWS_DELETE";
                if ( n & SQL_CA2_MAX_ROWS_UPDATE        ) l << "SQL_CA2_MAX_ROWS_UPDATE";
                if ( n & SQL_CA2_MAX_ROWS_CATALOG       ) l << "SQL_CA2_MAX_ROWS_CATALOG";
                if ( n & SQL_CA2_MAX_ROWS_AFFECTS_ALL   ) l << "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
                if ( n & SQL_CA2_CRC_EXACT              ) l << "SQL_CA2_CRC_EXACT";
                if ( n & SQL_CA2_CRC_APPROXIMATE        ) l << "SQL_CA2_CRC_APPROXIMATE";
                if ( n & SQL_CA2_SIMULATE_NON_UNIQUE    ) l << "SQL_CA2_SIMULATE_NON_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_TRY_UNIQUE    ) l << "SQL_CA2_SIMULATE_TRY_UNIQUE";
                if ( n & SQL_CA2_SIMULATE_UNIQUE        ) l << "SQL_CA2_SIMULATE_UNIQUE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_STRING_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_STR_ASCII            ) l << "SQL_FN_STR_ASCII";
                if ( n & SQL_FN_STR_BIT_LENGTH       ) l << "SQL_FN_STR_BIT_LENGTH";
                if ( n & SQL_FN_STR_CHAR             ) l << "SQL_FN_STR_CHAR";
                if ( n & SQL_FN_STR_CHAR_LENGTH      ) l << "SQL_FN_STR_CHAR_LENGTH";
                if ( n & SQL_FN_STR_CHARACTER_LENGTH ) l << "SQL_FN_STR_CHARACTER_LENGTH";
                if ( n & SQL_FN_STR_CONCAT           ) l << "SQL_FN_STR_CONCAT";
                if ( n & SQL_FN_STR_DIFFERENCE       ) l << "SQL_FN_STR_DIFFERENCE";
                if ( n & SQL_FN_STR_INSERT           ) l << "SQL_FN_STR_INSERT";
                if ( n & SQL_FN_STR_LCASE            ) l << "SQL_FN_STR_LCASE";
                if ( n & SQL_FN_STR_LEFT             ) l << "SQL_FN_STR_LEFT";
                if ( n & SQL_FN_STR_LENGTH           ) l << "SQL_FN_STR_LENGTH";
                if ( n & SQL_FN_STR_LOCATE           ) l << "SQL_FN_STR_LOCATE";
                if ( n & SQL_FN_STR_LTRIM            ) l << "SQL_FN_STR_LTRIM";
                if ( n & SQL_FN_STR_OCTET_LENGTH     ) l << "SQL_FN_STR_OCTET_LENGTH";
                if ( n & SQL_FN_STR_POSITION         ) l << "SQL_FN_STR_POSITION";
                if ( n & SQL_FN_STR_REPEAT           ) l << "SQL_FN_STR_REPEAT";
                if ( n & SQL_FN_STR_REPLACE          ) l << "SQL_FN_STR_REPLACE";
                if ( n & SQL_FN_STR_RIGHT            ) l << "SQL_FN_STR_RIGHT";
                if ( n & SQL_FN_STR_RTRIM            ) l << "SQL_FN_STR_RTRIM ";
                if ( n & SQL_FN_STR_SOUNDEX          ) l << "SQL_FN_STR_SOUNDEX";
                if ( n & SQL_FN_STR_SPACE            ) l << "SQL_FN_STR_SPACE";
                if ( n & SQL_FN_STR_SUBSTRING        ) l << "SQL_FN_STR_SUBSTRING";
                if ( n & SQL_FN_STR_UCASE            ) l << "SQL_FN_STR_UCASE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SUBQUERIES:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_SQ_CORRELATED_SUBQUERIES ) l << "SQL_SQ_CORRELATED_SUBQUERIES";
                if ( n & SQL_SQ_COMPARISON ) l << "SQL_SQ_COMPARISON";
                if ( n & SQL_SQ_EXISTS ) l << "SQL_SQ_EXISTS";
                if ( n & SQL_SQ_IN ) l << "SQL_SQ_IN";
                if ( n & SQL_SQ_QUANTIFIED ) l << "SQL_SQ_QUANTIFIED";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_SYSTEM_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_SYS_DBNAME ) l << "SQL_FN_SYS_DBNAME";
                if ( n & SQL_FN_SYS_IFNULL ) l << "SQL_FN_SYS_IFNULL";
                if ( n & SQL_FN_SYS_USERNAME ) l << "SQL_FN_SYS_USERNAME";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_TIMEDATE_ADD_INTERVALS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_TSI_FRAC_SECOND ) l << "SQL_FN_TSI_FRAC_SECOND";
                if ( n & SQL_FN_TSI_SECOND      ) l << "SQL_FN_TSI_SECOND";
                if ( n & SQL_FN_TSI_MINUTE      ) l << "SQL_FN_TSI_MINUTE";
                if ( n & SQL_FN_TSI_HOUR        ) l << "SQL_FN_TSI_HOUR";
                if ( n & SQL_FN_TSI_DAY         ) l << "SQL_FN_TSI_DAY";
                if ( n & SQL_FN_TSI_WEEK        ) l << "SQL_FN_TSI_WEEK";
                if ( n & SQL_FN_TSI_MONTH       ) l << "SQL_FN_TSI_MONTH";
                if ( n & SQL_FN_TSI_QUARTER     ) l << "SQL_FN_TSI_QUARTER";
                if ( n & SQL_FN_TSI_YEAR        ) l << "SQL_FN_TSI_YEAR";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_TIMEDATE_DIFF_INTERVALS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_TSI_FRAC_SECOND ) l << "SQL_FN_TSI_FRAC_SECOND";
                if ( n & SQL_FN_TSI_SECOND      ) l << "SQL_FN_TSI_SECOND";
                if ( n & SQL_FN_TSI_MINUTE      ) l << "SQL_FN_TSI_MINUTE";
                if ( n & SQL_FN_TSI_HOUR        ) l << "SQL_FN_TSI_HOUR";
                if ( n & SQL_FN_TSI_DAY         ) l << "SQL_FN_TSI_DAY";
                if ( n & SQL_FN_TSI_WEEK        ) l << "SQL_FN_TSI_WEEK";
                if ( n & SQL_FN_TSI_MONTH       ) l << "SQL_FN_TSI_MONTH";
                if ( n & SQL_FN_TSI_QUARTER     ) l << "SQL_FN_TSI_QUARTER";
                if ( n & SQL_FN_TSI_YEAR        ) l << "SQL_FN_TSI_YEAR";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_TIMEDATE_FUNCTIONS:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_FN_TD_CURRENT_DATE      ) l << "SQL_FN_TD_CURRENT_DATE";
                if ( n & SQL_FN_TD_CURRENT_TIME      ) l << "SQL_FN_TD_CURRENT_TIME";
                if ( n & SQL_FN_TD_CURRENT_TIMESTAMP ) l << "SQL_FN_TD_CURRENT_TIMESTAMP";
                if ( n & SQL_FN_TD_CURDATE           ) l << "SQL_FN_TD_CURDATE";
                if ( n & SQL_FN_TD_CURTIME           ) l << "SQL_FN_TD_CURTIME";
                if ( n & SQL_FN_TD_DAYNAME           ) l << "SQL_FN_TD_DAYNAME";
                if ( n & SQL_FN_TD_DAYOFMONTH        ) l << "SQL_FN_TD_DAYOFMONTH";
                if ( n & SQL_FN_TD_DAYOFWEEK         ) l << "SQL_FN_TD_DAYOFWEEK";
                if ( n & SQL_FN_TD_DAYOFYEAR         ) l << "SQL_FN_TD_DAYOFYEAR";
                if ( n & SQL_FN_TD_EXTRACT           ) l << "SQL_FN_TD_EXTRACT";
                if ( n & SQL_FN_TD_HOUR              ) l << "SQL_FN_TD_HOUR";
                if ( n & SQL_FN_TD_MINUTE            ) l << "SQL_FN_TD_MINUTE";
                if ( n & SQL_FN_TD_MONTH             ) l << "SQL_FN_TD_MONTH";
                if ( n & SQL_FN_TD_MONTHNAME         ) l << "SQL_FN_TD_MONTHNAME";
                if ( n & SQL_FN_TD_NOW               ) l << "SQL_FN_TD_NOW";
                if ( n & SQL_FN_TD_QUARTER           ) l << "SQL_FN_TD_QUARTER";
                if ( n & SQL_FN_TD_SECOND            ) l << "SQL_FN_TD_SECOND";
                if ( n & SQL_FN_TD_TIMESTAMPADD      ) l << "SQL_FN_TD_TIMESTAMPADD ";
                if ( n & SQL_FN_TD_TIMESTAMPDIFF     ) l << "SQL_FN_TD_TIMESTAMPDIFF";
                if ( n & SQL_FN_TD_WEEK              ) l << "SQL_FN_TD_WEEK";
                if ( n & SQL_FN_TD_YEAR              ) l << "SQL_FN_TD_YEAR";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_TXN_ISOLATION_OPTION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_TXN_READ_UNCOMMITTED ) l << "SQL_TXN_READ_UNCOMMITTED";
                if ( n & SQL_TXN_READ_COMMITTED ) l << "SQL_TXN_READ_COMMITTED";
                if ( n & SQL_TXN_REPEATABLE_READ ) l << "SQL_TXN_REPEATABLE_READ";
                if ( n & SQL_TXN_SERIALIZABLE ) l << "SQL_TXN_SERIALIZABLE";
                return l.join( ',' );
            }
        }
            return QString();
        case SQL_UNION:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_INTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) )
            {
                QStringList l;
                if ( n & SQL_U_UNION ) l << "SQL_U_UNION";
                if ( n & SQL_U_UNION_ALL ) l << "SQL_U_UNION_ALL";
                return l.join( ',' );
            }
        }
            return QString();

            // CONVERT
        case SQL_CONVERT_BIGINT:
        case SQL_CONVERT_BINARY:
        case SQL_CONVERT_BIT :
        case SQL_CONVERT_CHAR:
        case SQL_CONVERT_GUID:
        case SQL_CONVERT_DATE:
        case SQL_CONVERT_DECIMAL:
        case SQL_CONVERT_DOUBLE:
        case SQL_CONVERT_FLOAT:
        case SQL_CONVERT_INTEGER:
        case SQL_CONVERT_INTERVAL_YEAR_MONTH:
        case SQL_CONVERT_INTERVAL_DAY_TIME:
        case SQL_CONVERT_LONGVARBINARY:
        case SQL_CONVERT_LONGVARCHAR:
        case SQL_CONVERT_NUMERIC:
        case SQL_CONVERT_REAL:
        case SQL_CONVERT_SMALLINT :
        case SQL_CONVERT_TIME:
        case SQL_CONVERT_TIMESTAMP:
        case SQL_CONVERT_TINYINT:
        case SQL_CONVERT_VARBINARY:
        case SQL_CONVERT_VARCHAR:
        {
            SQLUINTEGER n = 0;
            *pnReturn = doGetInfo( nInfoType, SQLPOINTER( &n ), 0 /*SQL_IS_UINTEGER*/, NULL );
            if ( SQL_SUCCEEDED( *pnReturn ) ) return getConvertString( n );
        }
            return QString();

        default:
            qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
            *pnReturn = SQL_ERROR;
            return QString();
    }

    return QString();
}

QStringList DATAConnection::getTableNames( const QString &stringCatalog, const QString &stringSchema )
{
    QStringList l;
    DATAStatement *pStatement = getTables( stringCatalog, stringSchema, "TABLE" );
    if ( !pStatement ) return l;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCatalog = pStatement->getData( 1 ).toString();
        QString stringSchema = pStatement->getData( 2 ).toString();
        QString stringTable = pStatement->getData( 3 ).toString();

        if ( !stringSchema.isEmpty() )
        {
            stringTable = stringSchema + "." + stringTable;
            if ( !stringCatalog.isEmpty() ) stringTable = stringCatalog + "." + stringTable;
        }
        l.append( stringTable );
        n = pStatement->doFetch();
    }
    delete pStatement;
    return l;
}

QStringList DATAConnection::getTableTypes()
{
    QStringList l;

    DATAStatement *pStatement = nullptr;
    SQLRETURN      nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return l;
    }

    pStatement = new DATAStatement( this );
/* EXAMPLE PROPER CALL....
                        retCode = SQLTables( hstmt, 
                                             NULL, 0, // CatalogName, CatalogNameLength
                                             NULL, 0, // SchemaName, SchemaNameLength
                                             NULL, 0, // TableName, TableNameLength
                                             (SQLCHAR*)"%", SQL_NTS ); // TableType, TableTypeLength
*/
    nReturn = pStatement->doTables( QString(), QString(), QString(), SQL_ALL_TABLE_TYPES );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        return l;
    }

    nReturn = pStatement->doFetch();
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        QString stringTableType = pStatement->getData( 4 ).toString();
        if ( !l.contains( stringTableType ) ) l << stringTableType; // should not get duplicates but some drivers, in error, do this
        
        nReturn = pStatement->doFetch();
    }
    delete pStatement;

    return l;
}

SQLLEN DATAConnection::getRowCount( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    Q_UNUSED( stringCatalog );
    Q_UNUSED( stringSchema );

    QString stringSQL = "SELECT COUNT(*) FROM " + stringTable;

    DATAStatement *pStatement = nullptr;
    SQLRETURN      nReturn;
    SQLLEN nRowCount = -1;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return nRowCount;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        return nRowCount;
    }

    nReturn = pStatement->doFetch();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        return nRowCount;
    }

    nRowCount = pStatement->getData( 1 ).toInt();

    delete pStatement;
    return nRowCount;
}

QVariant DATAConnection::getAttr( SQLINTEGER nAttr, SQLRETURN *pnReturn )
{
    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    QVariant v;

    switch ( nAttr )
    {
        // SQLUINTEGER
        case SQL_ATTR_ACCESS_MODE:
        case SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE:
        case SQL_ATTR_AUTO_IPD:
        case SQL_ATTR_AUTOCOMMIT:
        case SQL_ATTR_CONNECTION_DEAD:
        case SQL_ATTR_CONNECTION_TIMEOUT:
        case SQL_ATTR_LOGIN_TIMEOUT:
        case SQL_ATTR_METADATA_ID:
        case SQL_ATTR_PACKET_SIZE:
        case SQL_ATTR_TRACE:
            {
                SQLUINTEGER n = 0;
                *pnReturn = doGetAttr( nAttr, SQLPOINTER( &n ), 0, NULL );
                if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
            }
            break;
        // SQLPOINTER
#ifdef Q_OS_WIN
        case SQL_ATTR_ASYNC_DBC_EVENT:
#endif
        case SQL_ATTR_ENLIST_IN_DTC:
        case SQL_ATTR_QUIET_MODE:
            {
                SQLULEN n = 0;
                *pnReturn = doGetAttr( nAttr, SQLPOINTER( &n ), sizeof(SQLULEN), NULL );
                if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
            }
            break;
        // SQLULEN
        case SQL_ATTR_ASYNC_ENABLE:
        case SQL_ATTR_ODBC_CURSORS:
            {
                SQLULEN n = 0;
                *pnReturn = doGetAttr( nAttr, SQLPOINTER( &n ), sizeof(SQLULEN), NULL );
                if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
            }
            break;
        // string
        case SQL_ATTR_CURRENT_CATALOG:
        case SQL_ATTR_TRACEFILE:
        case SQL_ATTR_TRANSLATE_LIB:
            {
                SQLINTEGER nBufferBytes = 4096;
                ushort sz[4096]; // SQLWCHAR sz[4096];
                SQLINTEGER nStringBytes = 0;

                *pnReturn = doGetAttr( nAttr, SQLPOINTER( sz ), nBufferBytes, &nStringBytes );
                if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( QString::fromUtf16( sz ) );
            }
            break;
        // 32 bit mask
        case SQL_ATTR_TRANSLATE_OPTION:
        case SQL_ATTR_TXN_ISOLATION:
            {
                SQLUINTEGER n = 0;
                *pnReturn = doGetAttr( nAttr, SQLPOINTER( &n ), 0, NULL );
                if ( SQL_SUCCEEDED( (*pnReturn) ) ) v.setValue( n );
            }
            break;
    }
    return v;
}

DATAStatement *DATAConnection::getExecute( const QString &stringStatement )
{
    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error,  __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    DATAStatement * pStatement  = new DATAStatement( this );
    SQLRETURN       nReturn     = pStatement->slotExecDirect( stringStatement );

    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getCatalogs
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getTables.
*/
DATAStatement *DATAConnection::getCatalogs()
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    /*
    SQL_CATALOG_NAME        
    SQL_CATALOG_TERM        
    SQL_ATTR_METADATA_ID    
    SQL_ATTR_CURRENT_CATALOG
    */
    pStatement = new DATAStatement( this );
    nReturn = pStatement->doTables( SQL_ALL_CATALOGS, "", "" );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getSchemas
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getTables.
*/
DATAStatement *DATAConnection::getSchemas( const QString &stringCatalog )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doTables( stringCatalog, SQL_ALL_SCHEMAS, QString() );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getTables
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getTables.
*/
DATAStatement *DATAConnection::getTables( const QString &stringCatalog, const QString &stringSchema, const QString &stringType )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doTables( stringCatalog, stringSchema, "%", stringType );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getViews
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getTables.
*/
DATAStatement *DATAConnection::getViews( const QString &stringCatalog, const QString &stringSchema, const QString &stringType )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doTables( stringCatalog, stringSchema, SQL_ALL_SCHEMAS, stringType );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getColumns
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getColumns.
*/
DATAStatement *DATAConnection::getColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringColumn )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doColumns( stringCatalog, stringSchema, stringTable, stringColumn );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getIndexs
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getStatistics.
*/
DATAStatement *DATAConnection::getIndexs( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doStatistics( stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getPrimaryKey
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getPrimaryKey.
*/
DATAStatement *DATAConnection::getPrimaryKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doPrimaryKeys( stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
 * \brief Create a statement which contains a resultset which contains foreign keys for the given table.
 * 
 * parent table: is where the key is a PK
 * child table: is where the key is a FK
 * 
 * \author pharvey (2/11/25)
 * 
 * \param stringCatalog    
 * \param stringSchema     
 * \param stringTableChild 
 * 
 * \return DATAStatement* nullptr if failed
 */
DATAStatement *DATAConnection::getForeignKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTableChild )
{
    DATAStatement *pStatement = nullptr;
    SQLRETURN      nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doForeignKeys( stringCatalog, stringSchema, stringTableChild );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = nullptr;
    }

    return pStatement;
}

/*!
    getSpecialColumns
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getSpecialColumns.
*/
DATAStatement *DATAConnection::getSpecialColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType )
{
    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    DATAStatement *pStatement      = new DATAStatement( this );
    SQLRETURN      nReturn         = pStatement->doSpecialColumns( nIdentifierType, stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getProcedures
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getProcedures.
*/
DATAStatement *DATAConnection::getProcedures( const QString &stringCatalog, const QString &stringSchema )
{
    DATAStatement *pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doProcedures( stringCatalog, stringSchema );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getProcedureColumns
        
    Creates a result set (or NULL if there was an error).
    See DATAStatement::getProcedureColumns.
*/
DATAStatement *DATAConnection::getProcedureColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure )
{
    DATAStatement *pStatement = nullptr;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return 0;
    }

    pStatement = new DATAStatement( this );
    nReturn = pStatement->doProcedureColumns( stringCatalog, stringSchema, stringProcedure );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = nullptr;
    }

    return pStatement;
}

/*!
    doBrowseConnect
    
    Connects to the data source. emits signalConnected() if things work out.
*/
SQLRETURN DATAConnection::doBrowseConnect( QString stringIn, QString *pstringOut )
{
    SQLWCHAR        szOut[4096];
    SQLSMALLINT     nCharsIn        = SQL_NTS;
    SQLSMALLINT     nCharsOutMax    = 4096;
    SQLSMALLINT     nCharsAvailable = 0;
    SQLRETURN       nReturn         = doBrowseConnect( (SQLWCHAR *)stringIn.utf16(), nCharsIn, szOut, nCharsOutMax, &nCharsAvailable );

    if ( SQL_SUCCEEDED( nReturn ) || nReturn == SQL_NEED_DATA ) pstringOut->setUtf16( (ushort *)szOut, nCharsAvailable );

    return nReturn;
}

/*!
    doConnect
    
    Connects to the data source. emits signalConnected() if things work out.
*/
SQLRETURN DATAConnection::doConnect( QString stringServerName, QString stringUserName, QString stringAuthentication )
{
    return doConnect( (SQLWCHAR *)stringServerName.utf16(), SQL_NTS, (SQLWCHAR *)stringUserName.utf16(), SQL_NTS, (SQLWCHAR *)stringAuthentication.utf16(), SQL_NTS );
}

/*!
 * \brief Driver connect. 
 *  
 * Connect by passing a connect string to the driver manager. The driver manager will load the driver as per the given "DRIVER" key and 
 * then pass the rest of the connect string on to the driver. 
 *  
 * In this case - we do not rely on the driver being able to prompt for missing key/values (particularly on all platforms). The caller should 
 * have handled any needed prompting. 
 * 
 * \author pharvey (12/15/20)
 * 
 * \param hWnd 
 * \param mapKeyValues 
 * 
 * \return SQLRETURN 
 */
SQLRETURN DATAConnection::doDriverConnect( SQLHWND hWnd, QMap<QString,QString> mapKeyValues )
{
    QString stringConnectString;

    // make a connect string from the key/value pairs
    QMapIterator<QString,QString> i( mapKeyValues );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( !stringConnectString.isEmpty() ) stringConnectString += ";";
        stringConnectString += i.key();
        stringConnectString += "=";
        stringConnectString += i.value();
    }

    // try to connect - with no prompting
    return doDriverConnect( hWnd, stringConnectString, nullptr, SQL_DRIVER_NOPROMPT );
}

/*!
 * \brief Connect using a connect string. 
 *  
 * A connection string has the following syntax:
 *
 * connection-string ::= empty-string[;] | attribute[;] | attribute; connection-string
 * 
 * empty-string ::=attribute ::= attribute-keyword=attribute-value | DRIVER=[{]attribute-value[}]
 * 
 * attribute-keyword ::= DSN | UID | PWD | driver-defined-attribute-keyword
 *
 * attribute-value ::= character-string
 *
 * driver-defined-attribute-keyword ::= identifier
 *
 * where character-string has zero or more characters; identifier has one or more characters; attribute-keyword is not case-sensitive;
 * attribute-value may be case-sensitive; and the value of the DSN keyword does not consist solely of blanks. 
 * 
 * \author pharvey (1/20/21)
 * 
 * \param hWnd 
 * \param stringIn 
 * \param pstringOut 
 * \param nPrompt 
 * 
 * \return SQLRETURN 
 */
SQLRETURN DATAConnection::doDriverConnect( SQLHWND hWnd, QString stringIn, QString *pstringOut, SQLUSMALLINT nPrompt )
{
    SQLWCHAR        szOut[4096];
    SQLSMALLINT     nCharsIn        = SQL_NTS;
    SQLSMALLINT     nCharsOutMax    = 4096;
    SQLSMALLINT     nCharsAvailable = 0;
    SQLRETURN       nReturn         = doDriverConnect( hWnd, (SQLWCHAR *)stringIn.utf16(), nCharsIn, szOut, nCharsOutMax, &nCharsAvailable, nPrompt );

    if ( SQL_SUCCEEDED( nReturn ) && pstringOut ) pstringOut->setUtf16( (ushort *)szOut, nCharsAvailable );

    return nReturn;
}

/*!
 * \brief Connect using SQLDriverConnect with SQL_DRIVER_NOPROMPT. 
 * 
 * For when you feel you have the complete connection string - perhaps you do not want to take a chance on the driver gui (if any).
 * 
 * \author pharvey (1/8/25)
 * 
 * \param stringIn 
 * 
 * \return SQLRETURN 
 */
SQLRETURN DATAConnection::doDriverConnect( QString stringIn )
{
    SQLWCHAR        szOut[4096];
    SQLSMALLINT     nCharsIn        = SQL_NTS;
    SQLSMALLINT     nCharsOutMax    = 4096;
    SQLSMALLINT     nCharsAvailable = 0;
    SQLRETURN       nReturn         = doDriverConnect( NULL, (SQLWCHAR *)stringIn.utf16(), nCharsIn, szOut, nCharsOutMax, &nCharsAvailable, SQL_DRIVER_NOPROMPT );

    return nReturn;
}

/*!
    doDisconnect
    
    Disconnects from data source.
*/
SQLRETURN DATAConnection::doDisconnect()
{
    if ( !isAlloc( false ) )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not allocated." ) );
        return SQL_ERROR;
    }
    if ( !isConnected() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr( "Not connected." ) );
        return SQL_ERROR;
    }

    SQLRETURN nReturn = SQLDisconnect( hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = false;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = false;
            eventDiagnostic( SQL_API_SQLDISCONNECT );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLDISCONNECT );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLDisconnect", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLDisconnect", tr( "Unexpected SQLRETURN value." ), nReturn );
            break;
    }

    if ( !isConnected() )
    {
        pProfile = nullptr;
        emit signalDisconnected();
    }

    return nReturn;
}

/*!
 * \brief Determine of there is a connection to the data source.
 *  
 * This is determined, by the return code, at connection time. 
 *  
 * It may be useful to check SQL_ATTR_CONNECTION_DEAD but it is not used at this time 
 * because SQLite driver appears to always return SQL_CD_FALSE. 
 *  
 * \author pharvey (1/9/20)
 * 
 * \return bool 
 */
bool DATAConnection::isConnected( bool bVerify )
{
    if ( !bVerify ) return bConnected;

    // not allocated? then definately not connected
    if ( !isAlloc() ) return false;

    //
    SQLUINTEGER nValue      = SQL_CD_FALSE;
    SQLINTEGER  nRetSize    = 0;

    SQLRETURN nReturn = SQLGetConnectAttr( hHandle, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER)(&nValue), SQL_IS_UINTEGER, &nRetSize  );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            if ( nValue == SQL_CD_FALSE ) return true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            if ( nValue == SQL_CD_FALSE ) return true;
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, "SQL_ATTR_CONNECTION_DEAD" );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, "SQL_ATTR_CONNECTION_DEAD" );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLGetConnectAttr", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLGetConnectAttr", tr( "Unexpected SQLRETURN value." ), nReturn );
            break;
    }

    // not connected (or error)
    return false;
}

void DATAConnection::slotConnect( QWidget * pWidgetParent )
{
    DATAConnectDialog dialog( this, pWidgetParent );
    if ( dialog.exec() == QDialog::Accepted )
    {
        // we updated ourself in doConnect so no need to do it here
        setDataSourceName( dialog.getDataSource() );
        setAlias( dialog.getAlias() );         
        setUserID( dialog.getUser() );        
    }

    // caller can check isConnected()
}

void DATAConnection::slotDisconnect()
{
    doDisconnect();
}


SQLRETURN DATAConnection::setConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLSetConnectAttr( hHandle, nAttribute, pValue, 0 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLSETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLSETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAConnection::setConnectAttr( SQLINTEGER nAttribute, const QString &stringValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLSetConnectAttr( hHandle, nAttribute, (SQLWCHAR *)stringValue.utf16(), SQL_NTS );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLSETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLSETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAConnection::getConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLGetConnectAttr( hHandle, nAttribute, pValue, 0, NULL  );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAConnection::getConnectAttr( SQLINTEGER nAttribute, QString *pValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLWCHAR    szValue[500];
    SQLINTEGER  nBytes = 1000;
    SQLINTEGER  nBytesReturned= 0;
    SQLRETURN   nReturn = SQLGetConnectAttr( hHandle, nAttribute, szValue, nBytes, &nBytesReturned  );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            pValue->setUtf16( (ushort *)szValue, nBytesReturned / 2 );
            break;
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pValue->setUtf16( (ushort *)szValue, nBytesReturned / 2 );
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETCONNECTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAConnection::doBrowseConnect( SQLWCHAR * pszInConnectionString, SQLSMALLINT nStringLength1, SQLWCHAR *pszOutConnectionString, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength2Ptr )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLBrowseConnect( hHandle, pszInConnectionString, nStringLength1, pszOutConnectionString, nBufferLength, pnStringLength2Ptr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = true;
            eventDiagnostic( SQL_API_SQLBROWSECONNECT );
            break;
        case SQL_NEED_DATA:
            return nReturn;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLBROWSECONNECT );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLBROWSECONNECT", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLBROWSECONNECT", "RETURN: Unexpected value.", nReturn );
            break;
    }

    if ( isConnected() ) 
    {
        stringDriver            = getValueConnectString( "DRIVER", pszInConnectionString, nStringLength1 );
        stringDataSourceName    = getValueConnectString( "DSN", pszInConnectionString, nStringLength1 );
        stringProfileKey        = getProfileKey( pszInConnectionString, nStringLength1 );
        pProfile                = g_Profiles->getProfile( this );
        emit signalConnected();
    }

    return nReturn;
}

SQLRETURN DATAConnection::doConnect( SQLWCHAR * pszDSN, SQLSMALLINT nLength1, SQLWCHAR *pszUID, SQLSMALLINT nLength2, SQLWCHAR *pszPWD, SQLSMALLINT nLength3 )
{
    if ( !isAlloc() ) return SQL_ERROR;

    // DO IT
    SQLRETURN nReturn = SQLConnect( hHandle, pszDSN, nLength1, pszUID, nLength2, pszPWD, nLength3 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = true;
            eventDiagnostic( SQL_API_SQLCONNECT );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLCONNECT );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLCONNECT", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLCONNECT", tr( "RETURN: Unexpected value." ), nReturn );
            break;
    }

    if ( isConnected() ) 
    {
        stringDriver            = QString();
        stringDataSourceName    = QString::fromUtf16( (ushort*)pszDSN );
        stringProfileKey        = stringDataSourceName;
        pProfile                = g_Profiles->getProfile( this );
        emit signalConnected();
    }

    return nReturn;
}

SQLRETURN DATAConnection::doDriverConnect( SQLHWND hWnd, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn, SQLWCHAR *pszOut, SQLSMALLINT nLengthOut, SQLSMALLINT *pnLengthOut, SQLUSMALLINT nPrompt )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLDriverConnect( hHandle, hWnd, pszIn, nLengthIn, pszOut, nLengthOut, pnLengthOut, nPrompt );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = true;
            eventDiagnostic( SQL_API_SQLDRIVERCONNECT );
            break;
        case SQL_NEED_DATA:
            return nReturn;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLDRIVERCONNECT );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLDRIVERCONNECT", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLDRIVERCONNECT", "RETURN: Unexpected value.", nReturn );
            break;
    }

    if ( isConnected() ) 
    {
        stringDriver            = getValueConnectString( "DRIVER", pszIn, nLengthIn );
        stringDataSourceName    = getValueConnectString( "DSN", pszIn, nLengthIn );
        stringProfileKey        = getProfileKey( pszIn, nLengthIn );
        pProfile                = g_Profiles->getProfile( this );
        emit signalConnected();
    }

    return nReturn;
}

SQLRETURN DATAConnection::doGetInfo( SQLUSMALLINT nInfoType, SQLPOINTER pInfoValue, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLGetInfo( hHandle, nInfoType, pInfoValue, nBufferLength, pnStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETINFO, g_Constants->mapInfoNames.value( nInfoType ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETINFO, g_Constants->mapInfoNames.value( nInfoType ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "FUNCTION: SQL_API_SQLGETINFO", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "FUNCTION: SQL_API_SQLGETINFO", tr( "RETURN: Unexpected value." ), nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAConnection::doGetAttr( SQLINTEGER nAttr, SQLPOINTER pAttr, SQLINTEGER nBufferLength, SQLINTEGER *pnStringLength )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLGetConnectAttr( hHandle, nAttr, pAttr, nBufferLength, pnStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttr ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETCONNECTATTR, g_Constants->mapAttrDbcNames.value( nAttr ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLGETCONNECTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLGETCONNECTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

QString DATAConnection::getValueConnectString( const QString &stringKey, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn )
{
    // \todo parse properly
    QString stringConnect = QString::fromUtf16( (ushort*)pszIn );

    QStringList listKeyValues = stringConnect.split( ';' );
    QString stringKeyValue;
    foreach( stringKeyValue, listKeyValues )
    {
        QStringList l = stringKeyValue.split( '=' );
        if ( l.at( 0 ).trimmed() == stringKey )
        {
            if ( l.count() > 1 ) return l.at( 1 ).trimmed();
            else return QString( "" ); // empty=found key but has no value
        }
    }
    return QString(); // null=did not find key
}

QString DATAConnection::getProfileKey( SQLWCHAR *pszIn, SQLSMALLINT nLengthIn )
{
    QString s;

    s = getValueConnectString( "DSN", pszIn, nLengthIn );
    if ( !s.isEmpty() ) return s;

    s = getValueConnectString( "DRIVER", pszIn, nLengthIn );
    if ( !s.isEmpty() ) return s;

    // \todo not likley to get here but if we do we should sanitize (we do not want auth info)
    return QString::fromUtf16( (ushort*)pszIn );
}

QString DATAConnection::getConvertString( SQLUINTEGER n )
{
    QStringList l;
    if ( n & SQL_CVT_BIGINT ) l << "SQL_CVT_BIGINT";
    if ( n & SQL_CVT_BINARY ) l << "SQL_CVT_BINARY";
    if ( n & SQL_CVT_BIT ) l << "SQL_CVT_BIT";
    if ( n & SQL_CVT_GUID ) l << "SQL_CVT_GUID";
    if ( n & SQL_CVT_CHAR ) l << "SQL_CVT_CHAR";
    if ( n & SQL_CVT_DATE ) l << "SQL_CVT_DATE";
    if ( n & SQL_CVT_DECIMAL ) l << "SQL_CVT_DECIMAL";
    if ( n & SQL_CVT_DOUBLE ) l << "SQL_CVT_DOUBLE";
    if ( n & SQL_CVT_FLOAT ) l << "SQL_CVT_FLOAT";
    if ( n & SQL_CVT_INTEGER ) l << "SQL_CVT_INTEGER";
    if ( n & SQL_CVT_INTERVAL_YEAR_MONTH ) l << "SQL_CVT_INTERVAL_YEAR_MONTH";
    if ( n & SQL_CVT_INTERVAL_DAY_TIME ) l << "SQL_CVT_INTERVAL_DAY_TIME";
    if ( n & SQL_CVT_LONGVARBINARY ) l << "SQL_CVT_LONGVARBINARY";
    if ( n & SQL_CVT_LONGVARCHAR ) l << "SQL_CVT_LONGVARCHAR";
    if ( n & SQL_CVT_NUMERIC ) l << "SQL_CVT_NUMERIC";
    if ( n & SQL_CVT_REAL ) l << "SQL_CVT_REAL";
    if ( n & SQL_CVT_SMALLINT ) l << "SQL_CVT_SMALLINT";
    if ( n & SQL_CVT_TIME ) l << "SQL_CVT_TIME";
    if ( n & SQL_CVT_TIMESTAMP ) l << "SQL_CVT_TIMESTAMP";
    if ( n & SQL_CVT_TINYINT ) l << "SQL_CVT_TINYINT";
    if ( n & SQL_CVT_VARBINARY ) l << "SQL_CVT_VARBINARY";
    if ( n & SQL_CVT_VARCHAR ) l << "SQL_CVT_VARCHAR";
    return l.join( ',' );
}

