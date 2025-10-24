#include "LibInfo.h"
#include "DATAConnection.h"
#include "DATAStatement.h"

DATAStatement::DATAStatement( DATAConnection *pconnection )
    : DATAHandle( Stm, pconnection )
{
    setObjectName( "DATAStatement" );

    nElapsedSeconds = 0;

    // echo up the object hierarchy
    connect( this, SIGNAL(signalMessage(DATAMessage *)), pconnection, SIGNAL(signalMessage(DATAMessage *)) );
    connect( this, SIGNAL(signalDiagnostic(DATADiagnostic *)), pconnection, SIGNAL(signalDiagnostic(DATADiagnostic *)) );
}

DATAStatement::~DATAStatement()
{
}

SQLRETURN DATAStatement::setAttrAsyncEnable( AttrAsyncEnableTypes n )
{
    return setStmtAttr( SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER)n );
}

SQLRETURN DATAStatement::setAttrConcurrency( AttrConcurrencyTypes n )
{
    return setStmtAttr( SQL_ATTR_CONCURRENCY, (SQLPOINTER)n );
}

SQLRETURN DATAStatement::setAttrCursorType( AttrCursorTypes n )
{
    return setStmtAttr( SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)n );
}

SQLRETURN DATAStatement::setAttrMaxLength( SQLUINTEGER n )
{
    return setStmtAttr( SQL_ATTR_MAX_LENGTH, (SQLPOINTER)n );
}

SQLRETURN DATAStatement::setAttrRowArraySize( SQLUINTEGER n )
{
    return setStmtAttr( SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)n );
}


SQLRETURN DATAStatement::getAttrAsyncEnable( AttrAsyncEnableTypes *pn )
{
    return getStmtAttr( SQL_ATTR_ASYNC_ENABLE, pn );
}

SQLRETURN DATAStatement::getAttrConcurrency( AttrConcurrencyTypes *pn )
{
    return getStmtAttr( SQL_ATTR_CONCURRENCY, pn );
}

SQLLEN DATAStatement::getRowCount()
{
    SQLLEN n = -1;
    doRowCount( &n );
    return n;
}

SQLSMALLINT DATAStatement::getColumnCount()
{
    SQLSMALLINT n = -1;
    doNumResultCols( &n );
    return n;
}

QVariant DATAStatement::getColAttribute( SQLUSMALLINT nColumnNumber, SQLUSMALLINT nFieldIdentifier )
{
    QVariant v;

    switch ( nFieldIdentifier )
    {
        // bool (SQL_TRUE || SQL_FALSE)
        case SQL_DESC_AUTO_UNIQUE_VALUE:
        case SQL_DESC_CASE_SENSITIVE:
        case SQL_DESC_FIXED_PREC_SCALE:
        case SQL_DESC_UNSIGNED:
        {
            SQLLEN n = 0;
            SQLRETURN nReturn = doColAttribute( nColumnNumber, nFieldIdentifier, 0, 0, 0, &n );
            if ( SQL_SUCCEEDED( nReturn ) )
            {
                if ( n == SQL_TRUE ) v = 1;
                else v = 0;
            }
            break;
        }
        // string
        case SQL_DESC_BASE_COLUMN_NAME:
        case SQL_DESC_BASE_TABLE_NAME:
        case SQL_DESC_CATALOG_NAME:
        case SQL_DESC_LABEL:
        case SQL_DESC_LITERAL_PREFIX:
        case SQL_DESC_LITERAL_SUFFIX:
        case SQL_DESC_LOCAL_TYPE_NAME:
        case SQL_DESC_NAME:
        case SQL_DESC_SCHEMA_NAME:
        case SQL_DESC_TABLE_NAME:
        case SQL_DESC_TYPE_NAME:
        {
            SQLSMALLINT nStringLength           = 0; // bytes available
            // get buffer size needed
            SQLRETURN nReturn = doColAttribute( nColumnNumber, nFieldIdentifier, 0, 0, &nStringLength, 0 );
            if ( SQL_SUCCEEDED( nReturn ) )
            {
                // create buffer and get char data
                SQLSMALLINT nBufferLength = nStringLength + 2; // bytes we can take - must be even
                QString     s( nBufferLength, '\0' );
                nReturn = doColAttribute( nColumnNumber, nFieldIdentifier, s.data(), nBufferLength, &nStringLength, 0 );
                if ( SQL_SUCCEEDED( nReturn ) ) v = s;
            }
            break;
        }
        // int
        case SQL_DESC_CONCISE_TYPE:
        case SQL_DESC_COUNT:
        case SQL_DESC_DISPLAY_SIZE:
        case SQL_DESC_LENGTH:
        case SQL_DESC_NULLABLE:
        case SQL_DESC_NUM_PREC_RADIX:
        case SQL_DESC_OCTET_LENGTH:
        case SQL_DESC_PRECISION:
        case SQL_DESC_SCALE:
        case SQL_DESC_SEARCHABLE:
        case SQL_DESC_TYPE:
        case SQL_DESC_UNNAMED:
        case SQL_DESC_UPDATABLE:
        {
            SQLLEN n = 0;
            SQLRETURN nReturn = doColAttribute( nColumnNumber, nFieldIdentifier, 0, 0, 0, &n );
            if ( SQL_SUCCEEDED( nReturn ) ) v.fromValue( n );
            break;
        }
        default:
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Unknown field identifier: " << nFieldIdentifier;
            break;
    }

    return v;
}

QVariant DATAStatement::getData( SQLUSMALLINT nColumnNumber, SQLRETURN *pnReturn )
{
    QVariant v( QVariant::String );

    SQLCHAR     szValue[1024];
    SQLSMALLINT nType           = SQL_C_CHAR;   // SQL_C_TCHAR
    SQLLEN      nChars          = 1024;
    SQLLEN      nBytes          = nChars;
    SQLLEN      nBytesAvailable = 0;

    bool        bCancelled  = false;
    SQLRETURN   nReturn;

    if ( !pnReturn ) pnReturn = &nReturn;

    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    szValue[0] = '\0';
    *pnReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
       *pnReturn = SQLGetData( getHandle(), nColumnNumber, nType, szValue, nBytes, &nBytesAvailable );
       switch ( *pnReturn )
        {
            case SQL_SUCCESS:
               v.setValue( QString( (const char*)szValue ) );
                break;
            case SQL_SUCCESS_WITH_INFO:
               szValue[nChars - 1] = '\0';
                v.setValue( QString( (const char*)szValue ) );
                eventDiagnostic( SQL_API_SQLGETDATA );
                break;
            case SQL_STILL_EXECUTING:
               if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
               return v;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLGETDATA );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLGetData", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLGetData", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

   return v;
}

double DATAStatement::getElapsedSeconds() 
{
    return nElapsedSeconds; 
}

SQLRETURN DATAStatement::doBindCol( SQLUSMALLINT nColumnNumber, SQLSMALLINT nTargetType, SQLPOINTER pTargetValuePtr, SQLLEN nBufferLength, SQLLEN *pnStrLen_or_Ind )
{
    SQLRETURN nReturn = SQL_ERROR;

    if ( !isAlloc() ) return SQL_ERROR;

    nReturn = SQLBindCol( getHandle(), nColumnNumber, nTargetType, pTargetValuePtr, nBufferLength, pnStrLen_or_Ind );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLBINDCOL );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLBINDCOL );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, __FUNCTION__, "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, __FUNCTION__, tr("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAStatement::doBulkOperations( SQLUSMALLINT nOperation )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;

    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLBulkOperations( getHandle(), nOperation );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLCOLATTRIBUTE );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NEED_DATA:
                eventDiagnostic( SQL_API_SQLSETPOS );
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLSETPOS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLColAttribute", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLColAttribute", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doCancel()
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLCancel( hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLCANCEL );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLCANCEL );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLCancel", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLCancel", tr("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAStatement::doCatalogs( const QString &stringCatalogName )
{
    return doTables( stringCatalogName );
}

SQLRETURN DATAStatement::doCloseCursor()
{
    if ( !isAlloc() ) return SQL_ERROR;

    // DO IT
    SQLRETURN nReturn;

    nReturn = SQLCloseCursor( getHandle() );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLCLOSECURSOR );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLCLOSECURSOR );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLCloseCursor", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLCloseCursor", tr("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAStatement::doColAttribute( SQLUSMALLINT nColumnNumber, SQLUSMALLINT nFieldIdentifier, SQLPOINTER nCharacterAttributePtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr, SQLLEN *pnNumericAttributePtr )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLColAttribute( getHandle(), nColumnNumber, nFieldIdentifier, nCharacterAttributePtr, nBufferLength, pnStringLengthPtr, pnNumericAttributePtr );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLCOLATTRIBUTE );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLCOLATTRIBUTE );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLColAttribute", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLColAttribute", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringColumn )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*) ( stringTable.isNull() ? NULL : stringTable.utf16() );
    SQLWCHAR *pszColumn  = (SQLWCHAR*) ( stringColumn.isNull() ? NULL : stringColumn.utf16() );
    int nCatalogLength   = pszCatalog ? SQL_NTS : 0;
    int nSchemaLength    = pszSchema ? SQL_NTS : 0; 
    int nTableLength     = pszTable ? SQL_NTS : 0;
    int nColumnLength    = pszColumn ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLColumnsW( getHandle(), 
                              pszCatalog, nCatalogLength, 
                              pszSchema, nSchemaLength, 
                              pszTable, nTableLength, 
                              pszColumn, nColumnLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLCOLUMNS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLCOLUMNS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLColumns", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLColumns", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doExecute()
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLExecute( getHandle() );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLEXECUTE );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NEED_DATA:
            case SQL_NO_DATA:
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLEXECUTE );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLExecute", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLExecute", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    } // while

    if ( SQL_SUCCEEDED( nReturn ) && isCursor() )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doExecDirect( const QString &stringStatement )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszStatement = (SQLWCHAR*) ( stringStatement.isNull() ? NULL : stringStatement.utf16() );
    int nStatementLength   = pszStatement ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLExecDirectW( getHandle(), pszStatement, nStatementLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLEXECDIRECT );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
            case SQL_NEED_DATA:
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLEXECDIRECT );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLExecDirect", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLExecDirect", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doExecDirect( const QString &stringStatement, QVector<QByteArray> vectorParameters )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    //
    // BIND PARMETERS
    // - its all about a binary destination - for now
    // - check SQL_NEED_LONG_DATA_LEN to determine if we need to chunk the data to the driver
    // - for now - lets assume driver can handle all data in one go... so no need for 'data at execution' (SQL_DATA_AT_EXEC/SQL_LEN_DATA_AT_EXEC/SQLPutData)
    // https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlbindparameter-function?view=sql-server-ver15
    // https://docs.microsoft.com/en-us/sql/odbc/reference/develop-app/setting-parameter-values?view=sql-server-ver15
    //
    for ( SQLLEN nParam = 0; nParam < vectorParameters.count(); nParam++ )
    {
        nReturn = SQLBindParameter( getHandle(), 
                                    nParam + 1,                         // Parameter number, ordered sequentially in increasing parameter order, starting at 1.
                                    SQL_PARAM_INPUT,                    // The type of the parameter.
                                    SQL_C_BINARY,                       // The C data type of the parameter. https://docs.microsoft.com/en-us/sql/odbc/reference/appendixes/c-data-types?view=sql-server-ver15
                                    SQL_LONGVARBINARY,                  // The SQL data type of the parameter. https://docs.microsoft.com/en-us/sql/odbc/reference/appendixes/sql-data-types?view=sql-server-ver15
                                    0,                                  // The size of the column or expression of the corresponding parameter marker. https://docs.microsoft.com/en-us/sql/odbc/reference/appendixes/column-size-decimal-digits-transfer-octet-length-and-display-size?view=sql-server-ver15
                                    0,                                  // The decimal digits of the column or expression of the corresponding parameter marker. 
                                    vectorParameters[nParam].data(),    // A pointer to a buffer for the parameter's data.
                                    vectorParameters[nParam].count(),   // Length of the ParameterValuePtr buffer in bytes.
                                    NULL );                             // A pointer to a buffer for the parameter's length.
        if ( !SQL_SUCCEEDED(nReturn) ) return nReturn; 
    }

    //
    // SUBMIT THE SQL
    //
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLExecDirectW( getHandle(), (SQLWCHAR*)stringStatement.utf16(), SQL_NTS );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLEXECDIRECT );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
            case SQL_NEED_DATA:
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLEXECDIRECT );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, __FUNCTION__, "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, __FUNCTION__, tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    //
    // PUT DATA
    // - send the big stuff (and/or binary data) down the pipe in chunks
    // https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlputdata-function?view=sql-server-ver15
    // https://docs.microsoft.com/en-us/sql/odbc/reference/develop-app/sending-long-data?view=sql-server-ver15
    //
/*
    SQLPOINTER pParam;     // whatever we provided for the POINTER TO DATA in SQLBindParameter                                                                                    
    nReturn = SQLParamData( getHandle(), &pParam );                                                            
    while ( nReturn == SQL_NEED_DATA )                                                                         
    {                                                                                                          
        int nParam = *((int**)pParam);                                                                         
        // send all of the data in one go - reconsider as needed                                               
        nReturn = SQLPutData( getHandle(), vectorParameters[nParam].data(), vectorParameters[nParam].count() );
        if ( !SQL_SUCCEEDED(nReturn) ) eventDiagnostic();                                                      
        nReturn = SQLParamData( getHandle(), &pParam );                                                        
    }                                                                                                          
                                                                                                               
*/

    // tell driver to free the parameter bindings - we do not need them anymore
    nReturn = SQLFreeStmt( getHandle(), SQL_RESET_PARAMS );

    emit signalElapsedSeconds( nElapsedSeconds );
    emit signalResults( this );

    return SQL_SUCCESS;
}

SQLRETURN DATAStatement::doFetch()
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLFetch( getHandle() );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLFETCH );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
                return nReturn;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLFETCH );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLFetch", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLFetch", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doFetchScroll( SQLSMALLINT nFetchOrientation, SQLLEN nFetchOffset )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;

    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLFetchScroll( getHandle(), nFetchOrientation, nFetchOffset );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLFETCH );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
                return nReturn;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLFETCH );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLFetchScroll", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLFetchScroll", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

/*!
 * \brief Create a resultset - containing FK's.
 * 
 * parent table: is where the key is a PK
 * child table: is where the key is a FK 
 *
 * \author pharvey (2/11/25)
 * 
 * \param stringFKCatalogName 
 * \param stringFKSchemaName  
 * \param stringFKTableName   - child table
 * 
 * \return SQLRETURN 
 */
SQLRETURN DATAStatement::doForeignKeys( const QString &stringFKCatalogName, const QString &stringFKSchemaName, const QString &stringFKTableName )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszPKCatalogName = NULL;
    SQLWCHAR *pszPKSchemaName  = NULL;
    SQLWCHAR *pszPKTableName   = NULL;

    SQLWCHAR *pszFKCatalogName = (SQLWCHAR*)( stringFKCatalogName.isNull() ? NULL : stringFKCatalogName.utf16() );
    SQLWCHAR *pszFKSchemaName  = (SQLWCHAR*)( stringFKSchemaName.isNull() ? NULL : stringFKSchemaName.utf16() );
    SQLWCHAR *pszFKTableName   = (SQLWCHAR*)( stringFKTableName.isNull() ? NULL : stringFKTableName.utf16() );

    int nPKCatalogName = pszPKCatalogName ? SQL_NTS : 0;
    int nPKSchemaName  = pszPKSchemaName ? SQL_NTS : 0;
    int nPKTableName   = pszPKTableName ? SQL_NTS : 0;

    int nFKCatalogName = pszFKCatalogName ? SQL_NTS : 0;
    int nFKSchemaName  = pszFKSchemaName ? SQL_NTS : 0;
    int nFKTableName   = pszFKTableName ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLForeignKeysW( getHandle(), 
                                  pszPKCatalogName, nPKCatalogName, 
                                  pszPKSchemaName, nPKSchemaName, 
                                  pszPKTableName, nPKTableName, 
                                  pszFKCatalogName, nFKCatalogName, 
                                  pszFKSchemaName, nFKSchemaName, 
                                  pszFKTableName, nFKTableName );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLFOREIGNKEYS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLFOREIGNKEYS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLForeignKeys", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLForeignKeys", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doNumResultCols( SQLSMALLINT *pnColumnCountPtr )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLNumResultCols( getHandle(), pnColumnCountPtr );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLNUMRESULTCOLS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLNUMRESULTCOLS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLNumResultCols", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLNumResultCols", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doPrepare( const QString &stringStatementText )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() ) return SQL_ERROR;

    SQLWCHAR *pszStatementText = (SQLWCHAR*) ( stringStatementText.isNull() ? NULL : stringStatementText.utf16() );
    int nStatementTextLength   = pszStatementText ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLPrepareW( getHandle(), pszStatementText, nStatementTextLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLPREPARE );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLPREPARE );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLPrepare", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLPrepare", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doPrimaryKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*) ( stringTable.isNull() ? NULL : stringTable.utf16() );
    int nCatalogLength   = pszCatalog ? SQL_NTS : 0;
    int nSchemaLength    = pszSchema ? SQL_NTS : 0; 
    int nTableLength     = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLPrimaryKeysW( getHandle(), pszCatalog, nCatalogLength, pszSchema, nSchemaLength, pszTable, nTableLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLPRIMARYKEYS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLPRIMARYKEYS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLPrimaryKeys", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLPrimaryKeys", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doProcedureColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure, const QString &stringColumn )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;
    SQLWCHAR *pszCatalog    = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema     = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszProcedure  = (SQLWCHAR*) ( stringProcedure.isNull() ? NULL : stringProcedure.utf16() );
    SQLWCHAR *pszColumn     = (SQLWCHAR*) ( stringColumn.isNull() ? NULL : stringColumn.utf16() );
    int         nCatalog       = pszCatalog ? SQL_NTS : 0;
    int         nSchema        = pszSchema ? SQL_NTS : 0; 
    int         nProcedure     = pszProcedure ? SQL_NTS : 0;
    int         nColumn        = pszColumn ? SQL_NTS : 0; 

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLProcedureColumnsW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszProcedure, nProcedure, pszColumn, nColumn );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLPROCEDURECOLUMNS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLPROCEDURECOLUMNS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLProcedureColumns", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLProcedureColumns", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doProcedures( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog    = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema     = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszProcedure  = (SQLWCHAR*) ( stringProcedure.isNull() ? NULL : stringProcedure.utf16() );
    int         nCatalog        = pszCatalog ? SQL_NTS : 0;
    int         nSchema         = pszSchema ? SQL_NTS : 0; 
    int         nProcedure      = pszProcedure ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLProceduresW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszProcedure, nProcedure );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLPROCEDURES );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLPROCEDURES );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLProcedures", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLProcedures", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doRowCount( SQLLEN *pnRowCountPtr )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLRowCount( getHandle(), pnRowCountPtr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLROWCOUNT );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLROWCOUNT );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLRowCount", "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLRowCount", tr("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAStatement::doSchemas( const QString &stringCatalog, const QString &stringSchema )
{
    return doTables( stringCatalog, stringSchema );
}

SQLRETURN DATAStatement::doSetPos( SQLSETPOSIROW nRowNumber, SQLUSMALLINT nOperation, SQLUSMALLINT nLockType )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;

    if ( !isAlloc() ) return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLSetPos( getHandle(), nRowNumber, nOperation, nLockType );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic( SQL_API_SQLCOLATTRIBUTE );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NEED_DATA:
                eventDiagnostic( SQL_API_SQLSETPOS );
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLSETPOS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLColAttribute", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLColAttribute", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    return nReturn;
}

SQLRETURN DATAStatement::doSpecialColumns( SQLSMALLINT nIdentifierType, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nScope, SQLSMALLINT nNullable )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*) ( stringTable.isNull() ? NULL : stringTable.utf16() );
    int         nCatalog        = pszCatalog ? SQL_NTS : 0;
    int         nSchema         = pszSchema ? SQL_NTS : 0; 
    int         nTable          = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLSpecialColumnsW( getHandle(), nIdentifierType, pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable, nScope, nNullable );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLSPECIALCOLUMNS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLSPECIALCOLUMNS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLSpecialColumns", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLSpecialColumns", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doStatistics( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLUSMALLINT nUnique, SQLUSMALLINT nReserved )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*) ( stringCatalog.isNull() ? NULL : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*) ( stringSchema.isNull() ? NULL : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*) ( stringTable.isNull() ? NULL : stringTable.utf16() );
    int         nCatalog   = pszCatalog ? SQL_NTS : 0;
    int         nSchema    = pszSchema ? SQL_NTS : 0; 
    int         nTable     = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLStatisticsW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable, nUnique, nReserved );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLSTATISTICS );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLSTATISTICS );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLStatistics", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLStatistics", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN DATAStatement::doTables( const QString &stringCatalogName, const QString &stringSchemaName, const QString &stringTableName, const QString &stringTableType )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalogName = (SQLWCHAR*) ( stringCatalogName.isNull() ? NULL : stringCatalogName.utf16() );
    SQLWCHAR *pszSchemaName  = (SQLWCHAR*) ( stringSchemaName.isNull() ? NULL : stringSchemaName.utf16() );  
    SQLWCHAR *pszTableName   = (SQLWCHAR*) ( stringTableName.isNull() ? NULL : stringTableName.utf16() );    
    SQLWCHAR *pszTableType   = (SQLWCHAR*) ( stringTableType.isNull() ? NULL : stringTableType.utf16() );    
    int nCatalogNameLength   = pszCatalogName ? SQL_NTS : 0;
    int nSchemaNameLength    = pszSchemaName ? SQL_NTS : 0; 
    int nTableNameLength     = pszTableName ? SQL_NTS : 0;
    int nTableTypeLength     = pszTableType ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLTablesW( getHandle(), pszCatalogName, nCatalogNameLength, pszSchemaName, nSchemaNameLength, pszTableName, nTableNameLength, pszTableType, nTableTypeLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLTABLES );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLTABLES );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQLTables", "SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQLTables", tr("Unexpected SQLRETURN value."), nReturn );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

/*!
 * \brief Get a result of supported data types.
 * 
 * Get the supported DDL data types for the given SQL data type.
 * 
 * The resulting data types are intended for use in Data Definition Language (DDL) statements.
 * 
 * \author pharvey (5/29/25)
 * 
 * \param nDataType The SQL data type. Default: SQL_ALL_TYPES
 * 
 * \return SQLRETURN 
 */
SQLRETURN DATAStatement::doTypeInfo( SQLSMALLINT nDataType )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() ) return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLGetTypeInfo( getHandle(), nDataType );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic( SQL_API_SQLGETTYPEINFO );
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic( SQL_API_SQLGETTYPEINFO );
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( DATAMessage::Error, "SQL_API_SQLGETTYPEINFO", "RETURN: SQL_INVALID_HANDLE" );
                break;
            default:
                eventMessage( DATAMessage::Error, "SQL_API_SQLGETTYPEINFO", "RETURN: Unexpected value.", nReturn );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

/*!
    doWaiting
    
    This will give time to the main Qt event loop during any asynch operation such
    that the application gets some time to do things like; repaint the screen or update
    a progress dialog.
*/
bool DATAStatement::doWaiting()
{
    qApp->processEvents();
    // i want to know if a driver actually supports asynch these days...
    qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " Looks like asynch option supported by this driver.\n";

    // just slow the polling down a bit
#ifdef _WINDOWS_
    Sleep( 1000 );
#else
    sleep( 1000 ); 
#endif
    return true;
}

bool DATAStatement::isCursor()
{
    SQLSMALLINT nColumns    = -1;
    SQLRETURN   nReturn     = doNumResultCols( &nColumns );

    if ( SQL_SUCCEEDED( nReturn ) && nColumns > 0 )
        return true;

    return false;
}

SQLRETURN DATAStatement::slotExecDirect( const QString &stringStatement )
{
    return doExecDirect( stringStatement );
}

SQLRETURN DATAStatement::slotExecute()
{
    return doExecute();
}

SQLRETURN DATAStatement::setStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLSetStmtAttr( getHandle(), nAttribute, pValue, 0 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLSETSTMTATTR, g_Constants->mapAttrStmNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLSETSTMTATTR, g_Constants->mapAttrStmNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETSTMTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETSTMTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

SQLRETURN DATAStatement::getStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() ) return SQL_ERROR;

    SQLRETURN nReturn = SQLGetStmtAttr( getHandle(), nAttribute, pValue, 0, NULL );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETSTMTATTR, g_Constants->mapAttrStmNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETSTMTATTR, g_Constants->mapAttrStmNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETSTMTATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETSTMTATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}



