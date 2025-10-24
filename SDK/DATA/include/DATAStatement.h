#pragma once

#include "DATAHandle.h"

class DATAConnection;

/*! 
 * \class   DATAStatement
 * \brief   An ODBC statement.
 *
 *          This class wraps an ODBC Statement handle.
 *
 *          \li uses Qt data types where possible
 *          \li provides several Qt signals
 *          \li inherits QObject to assist in parent/child relationships using rtti and using signals/slots
 *          \li use of Qt threads 
 * 
 */
class DATAStatement : public DATAHandle
{
    Q_OBJECT
public:
    enum AttributeTypes
    {
        AttrAppParamDesc                = SQL_ATTR_APP_PARAM_DESC,
        AttrAppRowDesc                  = SQL_ATTR_APP_ROW_DESC,
        AttrAsyncEnable                 = SQL_ATTR_ASYNC_ENABLE,
        AttrConcurrency                 = SQL_ATTR_CONCURRENCY,
        AttrCursorScrollable            = SQL_ATTR_CURSOR_SCROLLABLE,
        AttrCursorSensitivity           = SQL_ATTR_CURSOR_SENSITIVITY,
        AttrCursorType                  = SQL_ATTR_CURSOR_TYPE,
        AttrEnableAuotIpd               = SQL_ATTR_ENABLE_AUTO_IPD,
        AttrFetchBookmarkPtr            = SQL_ATTR_FETCH_BOOKMARK_PTR,
        AttrImpParamDesc                = SQL_ATTR_IMP_PARAM_DESC,
        AttrImpRowDesc                  = SQL_ATTR_IMP_ROW_DESC,
        AttrKeysetSize                  = SQL_ATTR_KEYSET_SIZE,
        AttrMaxLength                   = SQL_ATTR_MAX_LENGTH,
        AttrMaxRows                     = SQL_ATTR_MAX_ROWS,
        AttrMetadataId                  = SQL_ATTR_METADATA_ID,
        AttrNoscan                      = SQL_ATTR_NOSCAN,
        AttrParamBindOffsetPtr          = SQL_ATTR_PARAM_BIND_OFFSET_PTR,
        AttrParamBindType               = SQL_ATTR_PARAM_BIND_TYPE,
        AttrParamOperationPtr           = SQL_ATTR_PARAM_OPERATION_PTR,
        AttrParamStatusPtr              = SQL_ATTR_PARAM_STATUS_PTR,
        AttrParamsProcessedPtr          = SQL_ATTR_PARAMS_PROCESSED_PTR,
        AttrParamsetSize                = SQL_ATTR_PARAMSET_SIZE,
        AttrQueryTimeout                = SQL_ATTR_QUERY_TIMEOUT,
        AttrRetrieveData                = SQL_ATTR_RETRIEVE_DATA,
        AttrRowArraySize                = SQL_ATTR_ROW_ARRAY_SIZE,
        AttrRowBindOffsetPtr            = SQL_ATTR_ROW_BIND_OFFSET_PTR,
        AttrRowBindType                 = SQL_ATTR_ROW_BIND_TYPE,
        AttrRowNumber                   = SQL_ATTR_ROW_NUMBER,
        AttrRowOperationPtr             = SQL_ATTR_ROW_OPERATION_PTR,
        AttrRowStatusPtr                = SQL_ATTR_ROW_STATUS_PTR,
        AttrRowsFetchedPtr              = SQL_ATTR_ROWS_FETCHED_PTR,
        AttrSimulateCursor              = SQL_ATTR_SIMULATE_CURSOR,
        AttrUseBookmarks                = SQL_ATTR_USE_BOOKMARKS
    };

    enum AttrAsyncEnableTypes
    {
        AsyncEnableOff          = SQL_ASYNC_ENABLE_OFF,
        AsyncEnableOn           = SQL_ASYNC_ENABLE_ON
    };

    enum AttrConcurrencyTypes
    {
        ConcurReadOnly      = SQL_CONCUR_READ_ONLY,
        ConcurLock          = SQL_CONCUR_LOCK,
        ConcurRowver        = SQL_CONCUR_ROWVER,
        ConcurValues        = SQL_CONCUR_VALUES
    };

    enum AttrCursorScrollableTypes
    {
        Nonscrollable   = SQL_NONSCROLLABLE,
        Scrollable      = SQL_SCROLLABLE
    };

    enum AttrCursorSensitivityTypes
    {
        Unspecified     = SQL_UNSPECIFIED,
        Insensitive     = SQL_INSENSITIVE,
        Sensitive       = SQL_SENSITIVE
    };

    enum AttrCursorTypes
    {
        CursorForwardOnly   = SQL_CURSOR_FORWARD_ONLY,
        CursorStatic        = SQL_CURSOR_STATIC,
        CursorKeysetDriven  = SQL_CURSOR_KEYSET_DRIVEN,
        CursorDynamic       = SQL_CURSOR_DYNAMIC
    };

    enum AttrNoscanTypes
    {
        NoscanOff   = SQL_NOSCAN_OFF,
        NoscanOn    = SQL_NOSCAN_ON
    };

    enum AttrRetrieveDataTypes
    {
        RdOn    = SQL_RD_ON,
        RdOff   = SQL_RD_OFF
    };

    enum AttrSimulateCursorTypes
    {
        ScNonUnique = SQL_SC_NON_UNIQUE,
        ScTryUnique = SQL_SC_TRY_UNIQUE,
        ScUnique    = SQL_SC_UNIQUE
    };

    enum AttrUseBookmarksTypes
    {
        UbOff   = SQL_UB_OFF,
        UbOn    = SQL_UB_ON
    };

    DATAStatement( DATAConnection *pconnection );
    ~DATAStatement();

    // SETTERS
    SQLRETURN setAttrAsyncEnable( AttrAsyncEnableTypes n );
    SQLRETURN setAttrConcurrency( AttrConcurrencyTypes n );
    SQLRETURN setAttrCursorType( AttrCursorTypes n );
    SQLRETURN setAttrMaxLength( SQLUINTEGER n );
    SQLRETURN setAttrRowArraySize( SQLUINTEGER n );
    SQLRETURN setAttrRowBindType( SQLINTEGER nAttribute, SQLPOINTER pValue ) { return setStmtAttr( nAttribute, pValue ); }
    SQLRETURN setAttrRowStatusPtr( SQLINTEGER nAttribute, SQLPOINTER pValue ) { return setStmtAttr( nAttribute, pValue ); }

    // GETTERS
    SQLRETURN           getAttrAsyncEnable( AttrAsyncEnableTypes *pn );
    SQLRETURN           getAttrConcurrency( AttrConcurrencyTypes *pn );
    SQLLEN              getRowCount();
    SQLSMALLINT         getColumnCount();
    QVariant            getColAttribute( SQLUSMALLINT nColumnNumber, SQLUSMALLINT nFieldIdentifier );
    QVariant            getData( SQLUSMALLINT nColumnNumber, SQLRETURN *pnReturn = 0 );
    double              getElapsedSeconds();
    DATAConnection *    getConnection() { return (DATAConnection*)pParent; }

    // DOERS
    SQLRETURN doBindCol( SQLUSMALLINT nColumnNumber, SQLSMALLINT nTargetType, SQLPOINTER pTargetValuePtr, SQLLEN nBufferLength, SQLLEN *pnStrLen_or_Ind );
    SQLRETURN doBulkOperations( SQLUSMALLINT nOperation );
    SQLRETURN doCancel();
    SQLRETURN doCatalogs( const QString &stringCatalogName );
    SQLRETURN doCloseCursor();
    SQLRETURN doColAttribute( SQLUSMALLINT nColumnNumber, SQLUSMALLINT nFieldIdentifier, SQLPOINTER nCharacterAttributePtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr, SQLLEN *pnNumericAttributePtr );
    SQLRETURN doColumns( const QString &stringCatalogName, const QString &stringSchemaName = QString(""), const QString &stringTableName = QString(""), const QString &stringColumnName = QString("%") );
    SQLRETURN doExecute();
    SQLRETURN doExecDirect( const QString &stringStatement );
    SQLRETURN doExecDirect( const QString &stringStatement, QVector<QByteArray> vectorParameters );
    SQLRETURN doFetch();
    SQLRETURN doFetchScroll( SQLSMALLINT nFetchOrientation, SQLLEN nFetchOffset );
    SQLRETURN doForeignKeys( const QString &stringFKCatalogName, const QString &stringFKSchemaName, const QString &stringFKTableName );
    SQLRETURN doNumResultCols( SQLSMALLINT *pnColumnCountPtr );
    SQLRETURN doPrepare( const QString &stringStatementText );
    SQLRETURN doPrimaryKeys( const QString &stringCatalogName, const QString &stringSchemaName = QString(), const QString &stringTableName = QString() );
    SQLRETURN doProcedureColumns( const QString &stringCatalogName = QString(), const QString &stringSchemaName = QString(), const QString &stringProcName = QString(), const QString &stringColumnName = QString() );
    SQLRETURN doProcedures( const QString &stringCatalogName, const QString &stringSchemaName = QString(), const QString &stringProcName = QString() );
    SQLRETURN doRowCount( SQLLEN *pnRowCountPtr );
    SQLRETURN doSchemas( const QString &stringCatalogName, const QString &stringSchemaName = QString() );
    SQLRETURN doSetPos( SQLSETPOSIROW nRowNumber, SQLUSMALLINT nOperation, SQLUSMALLINT nLockType );
    SQLRETURN doSpecialColumns( SQLSMALLINT nIdentifierType = SQL_BEST_ROWID, const QString &stringCatalogName = QString(), const QString &stringSchemaName = QString(), const QString &stringTableName = QString(), SQLSMALLINT nScope = SQL_SCOPE_CURROW, SQLSMALLINT nNullable = SQL_NULLABLE );
    SQLRETURN doStatistics( const QString &stringCatalogName, const QString &stringSchemaName = QString(), const QString &stringTableName = QString(), SQLUSMALLINT nUnique = SQL_INDEX_ALL, SQLUSMALLINT nReserved = SQL_QUICK );
//    SQLRETURN doTables( const QString &stringCatalogName, const QString &stringSchemaName = QString(), const QString &stringTableName = QString(), const QString &stringTableType = QString() );
    SQLRETURN doTables( const QString &stringCatalogName, const QString &stringSchemaName = "", const QString &stringTableName = "", const QString &stringTableType = "" );
    SQLRETURN doTypeInfo( SQLSMALLINT nDataType = SQL_ALL_TYPES );
    bool      doWaiting();

    bool isCursor();

signals:
    void signalElapsedSeconds( double nElapsedSeconds );
    void signalResults( DATAStatement *pstatement );

public slots:
    SQLRETURN slotExecDirect( const QString &stringStatement );
    SQLRETURN slotExecute();

protected:
    double nElapsedSeconds;    /*!< seconds it took for request to complete                                */

    SQLRETURN setStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );
    SQLRETURN getStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );
};


