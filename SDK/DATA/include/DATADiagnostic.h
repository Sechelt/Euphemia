#ifndef H_DATADiagnostic
#define H_DATADiagnostic

#include "DATAMessage.h"

class DATAHandle;
class DATADiagnostic;

/*! 
 * \class   DATADiagnosticRecord
 * \brief   An ODBC diagnostic record.
 *
 *          This class is an interface to the ODBC diagnostic record information. The data is 'lazy loaded'
 *          or in other words - loaded only as requested.
 * 
 *          But it is cached.
 * 
 *          This means that calling for uncached data should be avoided IF there is a chance the state has changed.
 *
 * \sa      DATADiagnostic
 */
class DATADiagnosticRecord
{
public:
    DATADiagnosticRecord( DATADiagnostic *pDiagnostic, SQLINTEGER nRecord );

    QString     getClassOrigin( SQLRETURN *pnReturn = nullptr );
    SQLINTEGER  getColumnNumber( SQLRETURN *pnReturn = nullptr );
    QString     getConnectionName( SQLRETURN *pnReturn = nullptr );
    QString     getMessageText( SQLRETURN *pnReturn = nullptr );
    SQLINTEGER  getNative( SQLRETURN *pnReturn = nullptr );
    SQLLEN      getRowNumber( SQLRETURN *pnReturn = nullptr );
    QString     getServerName( SQLRETURN *pnReturn = nullptr );
    QString     getSqlstate( SQLRETURN *pnReturn = nullptr );
    QString     getSubclassOrigin( SQLRETURN *pnReturn = nullptr );

protected:
    enum Fields
    {
        ClassOrigin     = SQL_DIAG_CLASS_ORIGIN,
        ColumnNumber    = SQL_DIAG_COLUMN_NUMBER,
        ConnectionName  = SQL_DIAG_CONNECTION_NAME,
        MessageText     = SQL_DIAG_MESSAGE_TEXT,
        Native          = SQL_DIAG_NATIVE,
        RowNumber       = SQL_DIAG_ROW_NUMBER,
        ServerName      = SQL_DIAG_SERVER_NAME,
        Sqlstate        = SQL_DIAG_SQLSTATE,
        SubclassOrigin  = SQL_DIAG_SUBCLASS_ORIGIN
    };

    DATADiagnostic *pDiagnostic;      // parent
    SQLINTEGER      nRecord;          // the diagnostic record number we reperesent

    // cache
    // - isNull means call has not been made
    // - "" means call made - failed or no value
    QVariant vClassOrigin;
    QVariant vColumnNumber;
    QVariant vConnectionName;
    QVariant vMessageText;
    QVariant vNative;
    QVariant vRowNumber;
    QVariant vServerName;
    QVariant vSqlstate;
    QVariant vSubclassOrigin;   

    static QString  getFieldText( Fields nField );
    QVariant        getDiagField( Fields nField, SQLRETURN *pnReturn = nullptr );
};

/*! 
 * \class   DATADiagnostic
 * \brief   ODBC diagnostic information.
 *
 *          This class is an interface to the ODBC diagnostic information usually requested upon
 *          receiving a SQL_ERROR or SQL_SUCCESS_WITH_INFO return code. The data is 'lazy loaded'
 *          or in other words - loaded only as requested.
 * 
 *          But it is cached.
 * 
 *          This means that calling for uncached data should be avoided if there is a chance the state has changed.
 * 
 * \sa      DATADiagnosticRecord
 * 
 * \author  pharvey (8/15/2008)
 */
class DATADiagnostic
{
    friend class DATADiagnosticRecord;
public:
    DATADiagnostic( DATAHandle *pHandle, SQLUSMALLINT nFunction, const QString &stringRequest );
    ~DATADiagnostic();

    // \todo cache any call results incl. records

    SQLUSMALLINT    getFunction() { return nFunction; }
    QString         getRequest() { return stringRequest; }
    SQLLEN          getCursorRowCount( SQLRETURN *pnReturn = nullptr );
    QString         getDynamicFunction( SQLRETURN *pnReturn = nullptr );
    SQLINTEGER      getDynamicFunctionCode( SQLRETURN *pnReturn = nullptr );
    SQLINTEGER      getNumber( SQLRETURN *pnReturn = nullptr );
    SQLRETURN       getReturncode( SQLRETURN *pnReturn = nullptr );
    SQLLEN          getRowCount( SQLRETURN *pnReturn = nullptr );

    DATADiagnosticRecord *getRecord( SQLSMALLINT nRecord /* 1-based */  );

protected:
    enum Fields
    {
        CursorRowCount      = SQL_DIAG_CURSOR_ROW_COUNT,
        DynamicFunction     = SQL_DIAG_DYNAMIC_FUNCTION,
        DynamicFunctionCode = SQL_DIAG_DYNAMIC_FUNCTION_CODE,
        Number              = SQL_DIAG_NUMBER,
        Returncode          = SQL_DIAG_RETURNCODE,
        RowCount            = SQL_DIAG_ROW_COUNT
    };

    DATAHandle *    pHandle;
    SQLUSMALLINT    nFunction;
    QString         stringRequest;
    bool            bRecordsLoaded;

    // cache
    // - isNull means call has not been made
    // - "" || -1 means call made - failed or no value (native code can be -1)
    QVariant vCursorRowCount;
    QVariant vDynamicFunction;
    QVariant vDynamicFunctionCode;
    QVariant vNumber;
    QVariant vReturncode;
    QVariant vRowCount;   

    QVector<DATADiagnosticRecord*> vectorRecords;

    static QString          getFieldText( Fields nField );
    QVariant                getDiagField( Fields nField, SQLRETURN *pnReturn = nullptr );
    DATADiagnosticRecord *  getDiagRec( SQLSMALLINT nRecord );
};

#endif

