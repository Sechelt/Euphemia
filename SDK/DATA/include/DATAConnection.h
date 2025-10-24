#pragma once

#include "DATAEnvironment.h"
#include "DATADataType.h"

class DATAProfile;
class DATAConnection;
class DATAStatement;

/*! 
 * \class   DATAConnection
 * \brief   An ODBC connection.
 *
 *          This class wraps an ODBC Connection handle.
 *
 *          \li uses Qt data types where possible
 *          \li provides several Qt signals
 *          \li inherits QObject to assist in parent/child relationships using rtti and using signals/slots
 *
 */
class DATAConnection : public DATAHandle
{
    Q_OBJECT
public:
    enum AutoTypes
    {
        AutoCatalog,
        AutoSchema,
        AutoTable,
        AutoView,
        AutoColumn
    };

    enum AttributeTypes
    {
        AttrAccessMode          = SQL_ATTR_ACCESS_MODE,         /*!< This can be used to optimize locking strategies, transaction management, or other areas as appropriate to the driver or data source.   */
        AttrAsyncEnable         = SQL_ATTR_ASYNC_ENABLE,        /*!< Specifies whether a function called with a statement on the specified connection is executed asynchronously.                           */
        AttrAutoIpd             = SQL_ATTR_AUTO_IPD,            /*!< ReadOnly. Specifies whether automatic population of the parameter descriptions after a call to prepare statement is supported.         */
        AttrAutocommit          = SQL_ATTR_AUTOCOMMIT,          /*!< Specifies whether to use autocommit or manual-commit mode.                                                                             */
        AttrConnectionDead      = SQL_ATTR_CONNECTION_DEAD,     /*!< ReadOnly. Indicates the state of the connection (or at least the connect/disconnect part of state).                                    */
        AttrConnectionTimeout   = SQL_ATTR_CONNECTION_TIMEOUT,  /*!< The number of seconds to wait for any request on the connection to complete before returning to the application.                       */
        AttrCurrentCatalog      = SQL_ATTR_CURRENT_CATALOG,     /*!< A character string containing the name of the catalog to be used by the data source.                                                   */
        AttrLoginTimeout        = SQL_ATTR_LOGIN_TIMEOUT,       /*!< The number of seconds to wait for a login request to complete before returning to the application.                                     */
        AttrMetadataId          = SQL_ATTR_METADATA_ID,         /*!< Determines how the string arguments of catalog functions are treated.                                                                  */
        AttrOdbcCursors         = SQL_ATTR_ODBC_CURSORS,        /*!< Specifies how the Driver Manager uses the ODBC cursor library.                                                                         */
        AttrPacketSize          = SQL_ATTR_PACKET_SIZE,         /*!< Specifying the network packet size in bytes.                                                                                           */
        AttrQuietMode           = SQL_ATTR_QUIET_MODE,          /*!< The driver uses this handle to display dialog boxes.                                                                                   */
        AttrTrace               = SQL_ATTR_TRACE,               /*!< Tells the Driver Manager whether to perform tracing.                                                                                   */
        AttrTracefile           = SQL_ATTR_TRACEFILE,           /*!< String containing the name of the trace file.                                                                                          */
        AttrTranslateLib        = SQL_ATTR_TRANSLATE_LIB,       /*!< String containing the name of a lib containing the functions that the driver accesses to perform tasks such as char set translation.   */
        AttrTranslateOption     = SQL_ATTR_TRANSLATE_OPTION,    /*!< Value that is passed to the translation DLL.                                                                                           */
        AttrTxnIsolation        = SQL_ATTR_TXN_ISOLATION        /*!< Sets the transaction isolation level for the current connection.                                                                       */
    };

    enum AttrAccessModeTypes
    {
        ModeReadOnly        = SQL_MODE_READ_ONLY,               /*!< Used by the driver/data source as an indicator that the connection is not required to support SQL that cause updates to occur.         */
        ModeReadWrite       = SQL_MODE_READ_WRITE               /*!< Default. Driver/Data Source should support SQL which cause updates to occur.                                                           */
    };

    enum AttrAsyncEnableTypes
    {
        AsyncEnableOff      = SQL_ASYNC_ENABLE_OFF,             /*!< Default. All calls are synchronous.                                                                                                    */
        AsyncEnableOn       = SQL_ASYNC_ENABLE_ON               /*!< Enables asynchronous execution for all future statement handles allocated on this connection.                                          */
    };

    enum AttrAutocommitTypes
    {
        AutocommitOff   = SQL_AUTOCOMMIT_OFF,                   /*!< Driver uses manual-commit mode, and the application must explicitly commit or roll back transactions with SQLEndTran.                  */
        AutocommitOn    = SQL_AUTOCOMMIT_ON                     /*!< Default. Driver uses autocommit mode. Each statement is committed immediately after it is executed.                                    */
    };

    enum AttrOdbcCursorsTypes
    {
#ifndef Q_OS_WIN
        CurUseIfNeeded      = SQL_CUR_USE_IF_NEEDED,            /*!< Driver Manager uses the ODBC cursor library only if it is needed.                                                                      */
        CurUseOdbc          = SQL_CUR_USE_ODBC,                 /*!< Driver Manager uses the ODBC cursor library.                                                                                           */
#endif
        CurUseDriver        = SQL_CUR_USE_DRIVER                /*!< Default. Driver Manager uses the scrolling capabilities of the driver.                                                                 */
    };

    enum AttrTraceTypes
    {
        OptTraceOff = SQL_OPT_TRACE_OFF,                        /*!< Default. Tracing off.                                                                                                                  */
        OptTraceOn  = SQL_OPT_TRACE_ON                          /*!< Tracing on. When tracing is on, the Driver Manager writes each ODBC function call to the trace file.                                   */
    };

    DATAConnection( DATAEnvironment *penvironment );
    ~DATAConnection();

    // SETTERS
    void setPromptAlias( bool b )               { bPromptAlias = b;             }
    void setPromptDriver( bool b )              { bPromptDriver = b;            }
    void setPromptDataSourceName( bool b )      { bPromptDataSourceName = b;    }
    void setPromptUserID( bool b )              { bPromptUserID = b;            }
    void setPromptPassword( bool b )            { bPromptPassword = b;          }
    void setDriver( const QString &s )          { stringDriver = s;             }
    void setDataSourceName( const QString &s );
    void setAlias( const QString &s );
    void setUserID( const QString &s )          { stringUserID = s;             }
    SQLRETURN setAttrAccessMode( AttrAccessModeTypes nAttrAccessMode = ModeReadWrite );
    SQLRETURN setAttrAsyncEnable( AttrAsyncEnableTypes nAttrAsyncEnable = AsyncEnableOff ); 
    SQLRETURN setAttrAutocommit( AttrAutocommitTypes nAttrAutocommit = AutocommitOn );
    SQLRETURN setAttrConnectionTimeout( SQLUINTEGER nAttrConnectionTimeout = 0 );
    SQLRETURN setAttrCurrentCatalog( const QString &stringAttrCurrentCatalog );
    SQLRETURN setAttrLoginTimeout( SQLUINTEGER nAttrLoginTimeout = 0 );
    SQLRETURN setAttrMetadataId( bool bAttrMetadataId = false );
    SQLRETURN setAttrOdbcCursors( AttrOdbcCursorsTypes nAttrOdbcCursors = CurUseDriver );
    SQLRETURN setAttrPacketSize( SQLUINTEGER nAttrPacketSize = 1024 );
    SQLRETURN setAttrQuietMode( SQLHWND hWnd );
    SQLRETURN setAttrTrace( AttrTraceTypes nAttrTrace = OptTraceOff );
    SQLRETURN setAttrTracefile( const QString &stringAttrTracefile );
    SQLRETURN setAttrTranslateLib( const QString &stringAttrTranslateLib );
    SQLRETURN setAttrTranslateOption( SQLUINTEGER nAttrTranslateOption );
    SQLRETURN setAttrTxnIsolation( SQLUINTEGER nAttrTxnIsolation );

    // GETTERS
    QString                 getProfileKey()             { return stringProfileKey;      }
    DATAProfile *           getProfile()                { return pProfile;              }
    bool                    getPromptAlias()            { return bPromptAlias;          }
    bool                    getPromptDriver()           { return bPromptDriver;         }
    bool                    getPromptDataSourceName()   { return bPromptDataSourceName; }
    bool                    getPromptUserID()           { return bPromptUserID;         }
    bool                    getPromptPassword()         { return bPromptPassword;       }
    QString                 getAlias()                  { return stringAlias;           }
    QString                 getDriver()                 { return stringDriver;          }
    QString                 getDataSourceName()         { return stringDataSourceName;  }
    QString                 getUserID()                 { return stringUserID;          }
    AttrAccessModeTypes     getAttrAccessMode( SQLRETURN *pn = NULL );
    AttrAsyncEnableTypes    getAttrAsyncEnable( SQLRETURN *pn = NULL ); 
    bool                    getAttrAutoIpd( SQLRETURN *pn = NULL );
    AttrAutocommitTypes     getAttrAutocommit( SQLRETURN *pn = NULL );
    bool                    getAttrConnectionDead( SQLRETURN *pn = NULL );
    SQLUINTEGER             getAttrConnectionTimeout( SQLRETURN *pn = NULL );
    QString                 getAttrCurrentCatalog( SQLRETURN *pn = NULL );
    SQLUINTEGER             getAttrLoginTimeout( SQLRETURN *pn = NULL );
    bool                    getAttrMetadataId( SQLRETURN *pn = NULL );
    AttrOdbcCursorsTypes    getAttrOdbcCursors( SQLRETURN *pn = NULL );
    SQLUINTEGER             getAttrPacketSize( SQLRETURN *pn = NULL );
    SQLHWND                 getAttrQuietMode( SQLRETURN *pn = NULL );
    AttrTraceTypes          getAttrTrace( SQLRETURN *pn = NULL );
    QString                 getAttrTracefile( SQLRETURN *pn = NULL );
    QString                 getAttrTranslateLib( SQLRETURN *pn = NULL );
    SQLUINTEGER             getAttrTranslateOption( SQLRETURN *pn = NULL );
    SQLUINTEGER             getAttrTxnIsolation( SQLRETURN *pn = NULL );
    SQLRETURN               getFunctions( SQLUSMALLINT nFunctionID, SQLUSMALLINT *pnResult );
    QString                 getInfoStr( SQLUSMALLINT nInfoType, SQLRETURN *pnReturn = nullptr );
    QVariant                getInfo( SQLUSMALLINT nInfoType, SQLRETURN *pnReturn = nullptr );
    QStringList             getTableNames( const QString &stringCatalog, const QString &stringSchema );
    QStringList             getTableTypes();
    SQLLEN                  getRowCount( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    QVariant                getAttr( SQLINTEGER nAttribute, SQLRETURN *pn = NULL );
    DATAStatement *         getExecute( const QString &stringStatement );
    DATAStatement *         getCatalogs();
    DATAStatement *         getSchemas( const QString &stringCatalog );
    DATAStatement *         getTables( const QString &stringCatalog, const QString &stringSchema, const QString &stringType = "TABLE" );
    DATAStatement *         getViews( const QString &stringCatalog, const QString &stringSchema, const QString &stringType = "VIEW" );
    DATAStatement *         getColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringColumn = "%" );
    DATAStatement *         getIndexs( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    DATAStatement *         getPrimaryKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    DATAStatement *         getForeignKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    DATAStatement *         getSpecialColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType = SQL_BEST_ROWID );
    DATAStatement *         getProcedures( const QString &stringCatalog, const QString &stringSchema );
    DATAStatement *         getProcedureColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure );
    DATAEnvironment *       getEnvironment() { return (DATAEnvironment*)pParent; }

    // DOERS
    SQLRETURN doConnect( QString stringServerName = QString(), QString stringUserName = QString(), QString stringAuthentication = QString() );
    SQLRETURN doBrowseConnect( QString stringIn, QString *pstringOut );
    SQLRETURN doDriverConnect( SQLHWND hWnd, QMap<QString,QString> mapKeyValues );
    SQLRETURN doDriverConnect( SQLHWND hWnd, QString stringIn, QString *pstringOut, SQLUSMALLINT nPrompt );
    SQLRETURN doDriverConnect( QString stringIn );
    SQLRETURN doDisconnect();

    bool isConnected( bool bVerify = false );

signals:
    void signalConnected();
    void signalDisconnected();

public slots:
    void slotConnect( QWidget *pWidgetParent = nullptr );
    void slotDisconnect();

protected:
    DATAProfile *   pProfile;
    QString         stringProfileKey;
    bool            bConnected;
    bool            bPromptDriver;
    bool            bPromptDataSourceName;
    bool            bPromptAlias;
    bool            bPromptUserID;
    bool            bPromptPassword;
    QString         stringDriver;
    QString         stringDataSourceName;
    QString         stringAlias;
    QString         stringUserID;

    SQLRETURN setConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );
    SQLRETURN setConnectAttr( SQLINTEGER nAttribute, const QString &stringValue );

    SQLRETURN getConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );
    SQLRETURN getConnectAttr( SQLINTEGER nAttribute, QString *pValue );

    SQLRETURN doConnect( SQLWCHAR *pszServerName = nullptr, SQLSMALLINT nLength1 = SQL_NTS, SQLWCHAR *pszUserName = NULL, SQLSMALLINT nLength2 = SQL_NTS, SQLWCHAR *pszAuthentication = NULL, SQLSMALLINT nLength3 = SQL_NTS );
    SQLRETURN doDriverConnect( SQLHWND hWnd, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn, SQLWCHAR *pszOut, SQLSMALLINT nLengthOut, SQLSMALLINT *pnLengthOut, SQLUSMALLINT nPrompt );
    SQLRETURN doBrowseConnect( SQLWCHAR *szInConnectionString, SQLSMALLINT nStringLength1, SQLWCHAR *szOutConnectionString, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength2Ptr );
    SQLRETURN doGetInfo( SQLUSMALLINT nInfoType, SQLPOINTER pInfoValue, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength );
    SQLRETURN doGetAttr( SQLINTEGER nAttr, SQLPOINTER pInfoValue, SQLINTEGER nBufferLength, SQLINTEGER *pnStringLength );

    QString getValueConnectString( const QString &stringKey, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn );
    QString getProfileKey( SQLWCHAR *pszIn, SQLSMALLINT nLengthIn );

    QString getConvertString( SQLUINTEGER n );
};

