#include "LibInfo.h"
#include "DATADiagnostic.h"

#include "DATAHandle.h"

DATADiagnosticRecord::DATADiagnosticRecord( DATADiagnostic *pDiagnostic, SQLINTEGER nRecord )
{
    this->pDiagnostic   = pDiagnostic;
    this->nRecord       = nRecord;
}

QString DATADiagnosticRecord::getClassOrigin( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vClassOrigin.isNull() )
    { 
        vClassOrigin = getDiagField( ClassOrigin, pnReturn );
        if ( vClassOrigin.isNull() ) vClassOrigin = QVariant( "" );
    }
    else if ( vClassOrigin.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vClassOrigin.toString();
}

SQLINTEGER DATADiagnosticRecord::getColumnNumber( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vColumnNumber.isNull() )
    { 
        vColumnNumber = getDiagField( ColumnNumber, pnReturn );
        if ( vColumnNumber.isNull() ) vColumnNumber.setValue( -1 );
    }
    else if ( vColumnNumber.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vClassOrigin.toInt();
}

QString DATADiagnosticRecord::getConnectionName( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vConnectionName.isNull() )
    { 
        vConnectionName = getDiagField( ConnectionName, pnReturn );
        if ( vConnectionName.isNull() ) vConnectionName = QVariant( "" );
    }
    else if ( vConnectionName.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vConnectionName.toString();
}

QString DATADiagnosticRecord::getMessageText( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vMessageText.isNull() )
    { 
        vMessageText = getDiagField( MessageText, pnReturn );
        if ( vMessageText.isNull() ) vMessageText = QVariant( "" );
    }
    else if ( vMessageText.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vMessageText.toString();
}

SQLINTEGER DATADiagnosticRecord::getNative( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vNative.isNull() )
    { 
        vNative = getDiagField( Native, pnReturn );
        if ( vNative.isNull() ) vNative.setValue( -1 );
    }
    else if ( vNative.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vNative.toInt();
}

SQLLEN DATADiagnosticRecord::getRowNumber( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vRowNumber.isNull() )
    { 
        vRowNumber = getDiagField( RowNumber, pnReturn );
        if ( vRowNumber.isNull() ) vRowNumber.setValue( -1 );
    }
    else if ( vRowNumber.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vRowNumber.toInt();
}

QString DATADiagnosticRecord::getServerName( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vServerName.isNull() )
    { 
        vServerName = getDiagField( ServerName, pnReturn );
        if ( vServerName.isNull() ) vServerName = QVariant( "" );
    }
    else if ( vServerName.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vServerName.toString();
}

QString DATADiagnosticRecord::getSqlstate( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vSqlstate.isNull() )
    { 
        vSqlstate = getDiagField( Sqlstate, pnReturn );
        if ( vSqlstate.isNull() ) vSqlstate = QVariant( "" );
    }
    else if ( vSqlstate.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vSqlstate.toString();
}

QString DATADiagnosticRecord::getSubclassOrigin( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vSubclassOrigin.isNull() )
    { 
        vSubclassOrigin = getDiagField( SubclassOrigin, pnReturn );
        if ( vSubclassOrigin.isNull() ) vSubclassOrigin = QVariant( "" );
    }
    else if ( vSubclassOrigin.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vSubclassOrigin.toString();
}

QString DATADiagnosticRecord::getFieldText( Fields nField )
{
    switch ( nField )
    {
        case ClassOrigin:   
            return QString( "SQL_DIAG_CLASS_ORIGIN" );
        case ConnectionName:
            return QString( "SQL_DIAG_CONNECTION_NAME" );
        case MessageText:   
            return QString( "SQL_DIAG_MESSAGE_TEXT" );
        case ServerName:
            return QString( "SQL_DIAG_SERVER_NAME" );
        case SubclassOrigin:
            return QString( "SQL_DIAG_SUBCLASS_ORIGIN" );
        case Sqlstate:
            return QString( "SQL_DIAG_SQLSTATE" );
        case ColumnNumber:
            return QString( "SQL_DIAG_COLUMN_NUMBER" );
        case Native:
            return QString( "SQL_DIAG_NATIVE" );
        case RowNumber:
            return QString( "SQL_DIAG_ROW_NUMBER" );
    }
    return QString( "Unknown field" );
}

QVariant DATADiagnosticRecord::getDiagField( Fields nField, SQLRETURN *pnReturn )
{
    QVariant    vReturn;
    SQLRETURN   nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;
    *pnReturn = SQL_NO_DATA;

    if ( !pDiagnostic->pHandle->isAlloc() ) return QVariant();

    SQLPOINTER  pnDiagInfoPtr    = nullptr;
    SQLSMALLINT nBufferLength    = 0;
    SQLSMALLINT nStringLength    = 0;

    SQLWCHAR    szValue[SQL_MAX_MESSAGE_LENGTH + 1]; 
    SQLINTEGER  nValue = 0;

    switch ( nField )
    {
        case ClassOrigin:   
        case ConnectionName:
        case MessageText:   
        case ServerName:
        case Sqlstate:
        case SubclassOrigin:
            pnDiagInfoPtr    = szValue;
            nBufferLength    = sizeof(szValue);  // bytes
            break;

        case ColumnNumber:
        case Native:
        case RowNumber:
            pnDiagInfoPtr    = &nValue;
            nBufferLength    = SQL_IS_INTEGER;
            break;
    }

    *pnReturn = SQLGetDiagField( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, nField, pnDiagInfoPtr, nBufferLength, &nStringLength );
    switch ( *pnReturn )
    {
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            switch ( nField )
            {
                case ClassOrigin:   
                case ConnectionName:
                case MessageText:   
                case ServerName:
                case SubclassOrigin:
                    vReturn = QString::fromUtf16( (ushort*)szValue, (nStringLength/2>nBufferLength/2 ? nBufferLength/2 : -1) );
                    break;
                case Sqlstate:
                    vReturn = QString::fromUtf16( (ushort*)szValue, 5 );
                    break;
                case ColumnNumber:
                case Native:
                case RowNumber:
                    vReturn.setValue( nValue );
                    break;
            }
            if ( *pnReturn == SQL_SUCCESS_WITH_INFO ) emit pDiagnostic->pHandle->eventMessage( DATAMessage::Info, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD RECORD: %1 FIELD: %2" ).arg( nRecord ).arg( getFieldText( nField ) ), "RETURN: SQL_SUCCESS_WITH_INFO" );
            break;
        case SQL_NO_DATA:
        break;
        case SQL_ERROR:
            emit pDiagnostic->pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD RECORD: %1 FIELD: %2" ).arg( nRecord ).arg( getFieldText( nField ) ), "RETURN: SQL_ERROR" );
            break;
        case SQL_INVALID_HANDLE:
            emit pDiagnostic->pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD RECORD: %1 FIELD: %2" ).arg( nRecord ).arg( getFieldText( nField ) ), "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            emit pDiagnostic->pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD RECORD: %1 FIELD: %2" ).arg( nRecord ).arg( getFieldText( nField ) ), "RETURN: Unexpected value.", *pnReturn );
            break;
    }

    return vReturn;
}

//
//
//
//

DATADiagnostic::DATADiagnostic( DATAHandle *pHandle, SQLUSMALLINT nFunction, const QString &stringRequest )
{
    this->pHandle       = pHandle;
    this->nFunction     = nFunction;
    this->stringRequest = stringRequest;
    bRecordsLoaded = false;
}

DATADiagnostic::~DATADiagnostic()
{
    qDeleteAll( vectorRecords );
}

SQLLEN DATADiagnostic::getCursorRowCount( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vCursorRowCount.isNull() )
    { 
        vCursorRowCount = getDiagField( CursorRowCount, pnReturn );
        if ( vCursorRowCount.isNull() ) vCursorRowCount.setValue( -1 );
    }
    else if ( vCursorRowCount.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vCursorRowCount.toInt();
}

QString DATADiagnostic::getDynamicFunction( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vDynamicFunction.isNull() )
    { 
        vDynamicFunction = getDiagField( DynamicFunction, pnReturn );
        if ( vDynamicFunction.isNull() ) vDynamicFunction = QVariant( "" );
    }
    else if ( vDynamicFunction.toString().isEmpty() )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vDynamicFunction.toString();
}

SQLINTEGER DATADiagnostic::getDynamicFunctionCode( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vDynamicFunctionCode.isNull() )
    { 
        vDynamicFunctionCode = getDiagField( DynamicFunctionCode, pnReturn );
        if ( vDynamicFunctionCode.isNull() ) vDynamicFunctionCode.setValue( -1 );
    }
    else if ( vDynamicFunctionCode.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vDynamicFunctionCode.toInt();
}

SQLINTEGER DATADiagnostic::getNumber( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vNumber.isNull() )
    { 
        vNumber = getDiagField( Number, pnReturn );
        if ( vNumber.isNull() ) vNumber.setValue( -1 );
    }
    else if ( vNumber.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vNumber.toInt();
}

SQLRETURN DATADiagnostic::getReturncode( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vReturncode.isNull() )
    { 
        vReturncode = getDiagField( Returncode, pnReturn );
        if ( vReturncode.isNull() ) vReturncode.setValue( -1 );
    }
    else if ( vReturncode.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vReturncode.toInt();
}

SQLLEN DATADiagnostic::getRowCount( SQLRETURN *pnReturn )
{
    SQLRETURN n;
    if ( !pnReturn ) pnReturn = &n;

    if ( vRowCount.isNull() )
    { 
        vRowCount = getDiagField( RowCount, pnReturn );
        if ( vRowCount.isNull() ) vRowCount.setValue( -1 );
    }
    else if ( vRowCount.toInt() < 0 )
        *pnReturn = SQL_NO_DATA;
    else 
        *pnReturn = SQL_SUCCESS;

    return vRowCount.toInt();
}

DATADiagnosticRecord *DATADiagnostic::getRecord( SQLSMALLINT nRecord )
{
   if ( nRecord < 1 ) return nullptr;                                               // always out of range
   if ( vectorRecords.size() && nRecord > vectorRecords.size() ) return nullptr;    // have cache but out of range
   if ( vectorRecords.size() ) return vectorRecords.at( nRecord - 1 );              // in cache

    if ( bRecordsLoaded ) return nullptr;   // tried to load cache but nothing was cached
    bRecordsLoaded = true;

    SQLINTEGER nRecords = getNumber();
    for ( SQLINTEGER n = 1; n <= nRecords; n++ ) 
    {
        DATADiagnosticRecord *p = getDiagRec( n );
        if ( !p ) break;
        vectorRecords.append( p );
    }

    // cache loaded so call self to try again
    return getRecord( nRecord );
}

QString DATADiagnostic::getFieldText( Fields nField )
{
    switch ( nField )
    {
        case CursorRowCount:   
            return QString( "SQL_DIAG_CURSOR_ROW_COUNT" );
        case DynamicFunction:
            return QString( "SQL_DIAG_DYNAMIC_FUNCTION" );
        case DynamicFunctionCode:   
            return QString( "SQL_DIAG_DYNAMIC_FUNCTION_CODE" );
        case Number:
            return QString( "SQL_DIAG_NUMBER" );
        case Returncode:
            return QString( "SQL_DIAG_RETURNCODE" );
        case RowCount:
            return QString( "SQL_DIAG_ROW_COUNT" );
    }
    return QString( "Unknown field" );
}

QVariant DATADiagnostic::getDiagField( Fields nField, SQLRETURN *pnReturn )
{
    QVariant    vReturn;
    SQLRETURN   nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;
    *pnReturn = SQL_ERROR;

    if ( !pHandle->isAlloc() ) return vReturn;

    SQLPOINTER  pValue           = nullptr;
    SQLSMALLINT nBufferLength    = 0;
    SQLSMALLINT nStringLength    = 0;

    SQLWCHAR    szValue[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLINTEGER  nValue = 0;

    if ( nField == DynamicFunction )
    {
        pValue = szValue;
        nBufferLength = sizeof(szValue); 
    }
    else
    {
        pValue = &nValue;
        nBufferLength = SQL_IS_INTEGER;
    }

    *pnReturn = SQLGetDiagField( pHandle->getType(), pHandle->getHandle(), 0, nField, pValue, nBufferLength, &nStringLength );
    switch ( *pnReturn )
    {
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            if ( nField == DynamicFunction )
            {
                vReturn = QString::fromUtf16( (ushort*)szValue, (nStringLength/2>nBufferLength/2 ? nBufferLength/2 : -1) );
            }
            else
            {
                vReturn.setValue( nValue );
            }
            if ( SQL_SUCCESS_WITH_INFO ) emit pHandle->eventMessage( DATAMessage::Info,  QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD FIELD: %1" ).arg( getFieldText( nField ) ), "RETURN: SQL_SUCCESS_WITH_INFO" );
            break;
        case SQL_ERROR:
            emit pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD FIELD: %1" ).arg( getFieldText( nField ) ), "RETURN: SQL_ERROR" );
            break;
        case SQL_INVALID_HANDLE:
            emit pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD FIELD: %1" ).arg( getFieldText( nField ) ), "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            emit pHandle->eventMessage( DATAMessage::Error, QString( "FUNCTION: SQL_API_SQLGETDIAGFIELD FIELD: %1" ).arg( getFieldText( nField ) ), "RETURN: Unexpected value.", *pnReturn );
            break;
    }

    return vReturn;
}

DATADiagnosticRecord *DATADiagnostic::getDiagRec( SQLSMALLINT nRecord )
{
    DATADiagnosticRecord *p = new DATADiagnosticRecord( this, nRecord );
    return p;
}


