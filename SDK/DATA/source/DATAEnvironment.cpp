#include "LibInfo.h"
#include "DATAEnvironment.h"

DATAEnvironment::DATAEnvironment( DATASystem *pSystem )
    : DATAHandle( Env, pSystem )
{
    setObjectName( "DATAEnvironment" );

    // echo up the object hierarchy
    connect( this, SIGNAL(signalMessage(DATAMessage *)), pSystem, SIGNAL(signalMessage(DATAMessage *)) );
    connect( this, SIGNAL(signalDiagnostic(DATADiagnostic *)), pSystem, SIGNAL(signalDiagnostic(DATADiagnostic *)) );
}

DATAEnvironment::~DATAEnvironment()
{
}

/*! 
 * \brief   Sets SQL_ATTR_CONNECTION_POOLING attribute.
 * 
 *          See \sa getAttrConnectionPooling for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \note    Connection pooling not fully implemented here - yet. This does does set
 *          this attribute but does follow up with the remaining, internal, bits 
 *          we need to do.
 *
 * \param   n   Input. One of \sa DATAEnvironment::AttrConnectionPoolingTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrConnectionPooling
 */
SQLRETURN DATAEnvironment::setAttrConnectionPooling( AttrConnectionPoolingTypes n )
{
    return setEnvAttr( SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_CP_MATCH attribute.
 * 
 *          See \sa getAttrCPMatch for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   n   Input. One of \sa DATAEnvironment::AttrCPMatchTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrCPMatch
 */
SQLRETURN DATAEnvironment::setAttrCPMatch( AttrCPMatchTypes n )
{
    return setEnvAttr( SQL_ATTR_CP_MATCH, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_ODBC_VERSION attribute.
 * 
 *          See \sa getAttrODBCVersion for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   n   Input. One of \sa DATAEnvironment::AttrODBCVersionTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrODBCVersion
 */
SQLRETURN DATAEnvironment::setAttrODBCVersion( AttrODBCVersionTypes n )
{
    return setEnvAttr( SQL_ATTR_ODBC_VERSION, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_OUTPUT_NTS attribute.
 * 
 *          See \sa getAttrOutputNTS for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   b   Input. Either true or false.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrOutputNTS
 */
SQLRETURN DATAEnvironment::setAttrOutputNTS( bool b )
{
    return setEnvAttr( SQL_ATTR_OUTPUT_NTS, (SQLPOINTER)b );
}

/*! 
 * \brief   Get SQL_ATTR_CONNECTION_POOLING attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  DATAEnvironment::AttrConnectionPoolingTypes
 *
 * \sa      setAttrConnectionPooling
 */
DATAEnvironment::AttrConnectionPoolingTypes DATAEnvironment::getAttrConnectionPooling( SQLRETURN *pnReturn )
{
    AttrConnectionPoolingTypes n = CPOff;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_CONNECTION_POOLING, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_CP_MATCH attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  DATAEnvironment::AttrCPMatchTypes
 *
 * \sa      setAttrCPMatch
 */
DATAEnvironment::AttrCPMatchTypes DATAEnvironment::getAttrCPMatch( SQLRETURN *pnReturn )
{
    AttrCPMatchTypes n = CPStrictMatch;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_CP_MATCH, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_ODBC_VERSION attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  DATAEnvironment::AttrODBCVersionTypes
 *
 * \sa      setAttrODBCVersion
 */
DATAEnvironment::AttrODBCVersionTypes DATAEnvironment::getAttrODBCVersion( SQLRETURN *pnReturn )
{
    AttrODBCVersionTypes n = OVOdbc3;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_ODBC_VERSION, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_OUTPUT_NTS attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  bool
 *
 * \sa      setAttrOutputNTS
 */
bool DATAEnvironment::getAttrOutputNTS( SQLRETURN *pnReturn )
{
    bool b = true;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_OUTPUT_NTS, &b );
    if ( pnReturn ) *pnReturn = nReturn;

    return b;
}

QStringList DATAEnvironment::getDrivers( SQLRETURN *pnReturn )
{
    QStringList     stringlistDrivers;
    SQLRETURN       nReturn;
    SQLTCHAR        szDRV[100];
    SQLSMALLINT     nLength1;
    SQLTCHAR        szAttribute[300]; // all attributes (key/value pairs) in form of "key=value;"
    SQLSMALLINT     nLength2;

    nReturn = doDrivers( SQL_FETCH_FIRST, szDRV, sizeof(szDRV) / sizeof(SQLTCHAR), &nLength1, szAttribute, sizeof(szAttribute)  / sizeof(SQLTCHAR), &nLength2 );
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        stringlistDrivers.append( QString::fromUtf16( (ushort*)szDRV ) );
        nReturn = doDrivers( SQL_FETCH_NEXT, szDRV, sizeof(szDRV) / sizeof(SQLTCHAR), &nLength1, szAttribute, sizeof(szAttribute) / sizeof(SQLTCHAR), &nLength2 );
    }

    if ( pnReturn )
        *pnReturn = nReturn;

    return stringlistDrivers;
}

QMap<QString,QString> DATAEnvironment::getDriversWithDescriptions( SQLRETURN *pnReturn )
{
    QMap<QString,QString>   mapDrivers;
    SQLRETURN               nReturn;
    SQLTCHAR                szDRV[100];
    SQLSMALLINT             nLength1;
    SQLTCHAR                szAttribute[300]; // all attributes (key/value pairs) in form of "key=value;"
    SQLSMALLINT             nLength2;

    nReturn = doDrivers( SQL_FETCH_FIRST, szDRV, sizeof(szDRV) / sizeof(SQLTCHAR), &nLength1, szAttribute, sizeof(szAttribute)  / sizeof(SQLTCHAR), &nLength2 );
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        QString stringDriver = QString::fromUtf16( (ushort*)szDRV );
        QString stringDescription;
        {
            QString stringAttributes = QString::fromUtf16( (ushort*)szAttribute );
            QStringList listKeyValues = stringAttributes.split( ";" );
            QString stringKeyValue;
            foreach( stringKeyValue, listKeyValues )
            {
                if ( stringKeyValue.left( 12 ) == "Description=" )
                {
                    stringDescription = stringKeyValue.mid( 12 );
                    break;
                }
            }
        }
        mapDrivers[stringDriver] = stringDescription;
        nReturn = doDrivers( SQL_FETCH_NEXT, szDRV, sizeof(szDRV) / sizeof(SQLTCHAR), &nLength1, szAttribute, sizeof(szAttribute) / sizeof(SQLTCHAR), &nLength2 );
    }

    if ( pnReturn ) *pnReturn = nReturn;

    return mapDrivers;
}

/*!
    Qt friendly way to get a list of DSN's. 
*/
QStringList DATAEnvironment::getDataSources( bool bUser, bool bSystem, SQLRETURN *pnReturn )
{
    QStringList     stringlistDataSources;
    SQLRETURN       nReturn;
    SQLUSMALLINT    nDirection  =   SQL_FETCH_FIRST;
    SQLTCHAR        szDSN[100];
    SQLSMALLINT     nLength1;
    SQLTCHAR        szDescription[100];
    SQLSMALLINT     nLength2;

    if ( bUser || bSystem )
    {
#ifndef Q_WS_MACX
        if ( !bUser && bSystem )
            nDirection = SQL_FETCH_FIRST_SYSTEM;
        else if ( bUser && !bSystem )
            nDirection = SQL_FETCH_FIRST_USER;
#endif

        nReturn = doDataSources( nDirection, szDSN, sizeof(szDSN) / sizeof(SQLTCHAR), &nLength1, szDescription, sizeof(szDescription) / sizeof(SQLTCHAR), &nLength2 );
        while ( SQL_SUCCEEDED( nReturn ) )
        {
            stringlistDataSources += QString::fromUtf16( (ushort*)szDSN );
            nReturn = doDataSources( SQL_FETCH_NEXT, szDSN, sizeof(szDSN) / sizeof(SQLTCHAR), &nLength1, szDescription, sizeof(szDescription) / sizeof(SQLTCHAR), &nLength2 );
        }
    }
    else
        nReturn = SQL_NO_DATA;

    if ( pnReturn )
        *pnReturn = nReturn;

    return stringlistDataSources;
}

/*! 
 * \brief   Set an environment attribute.
 *
 *          Use of this method of setting an attribute is discouraged. Please consider using
 *          the more convenient methods such as \sa setAttrConnectionPooling etc. This method
 *          can be used to set non-standard environment attributes.
 * 
 * \param   nAttribute      Input. A viable environment attribute such as; SQL_ATTR_CONNECTION_POOLING.
 * \param   pValue          Input. The value (pointer *is* the value for numerics).
 * \param   nStringLength   Input. Number of chars at pValue in case where pValue points to a string.
 * 
 * \return  SQLRETURN
 * 
 * \sa      setAttrConnectionPooling
 *          setAttrCPMatch
 *          setAttrODBCVersion
 *          setAttrOutputNTS
 */
SQLRETURN DATAEnvironment::setEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN   nReturn = SQLSetEnvAttr( hHandle, nAttribute, pValue, 0 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLSETENVATTR, g_Constants->mapAttrEnvNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLSETENVATTR, g_Constants->mapAttrEnvNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETENVATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETENVATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Get an environment attribute.
 *
 *          Use of this method of getting an environment attribute is discouraged. Please consider using
 *          the more convenient methods such as \sa getAttrConnectionPooling etc. This method
 *          can be used to get non-standard environment attributes.
 * 
 * \param   nAttribute      Input. A viable environment attribute such as; SQL_ATTR_CONNECTION_POOLING.
 * \param   pValue          Output. Pointer to a buffer in which to get the desired value.
 * \param   nBufferLength   Input. The number of bytes we can use at pValue.
 * \param   pnStringLength  Output. The number of chars available if value is a string.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrConnectionPooling
 *          getAttrCPMatch
 *          getAttrODBCVersion
 *          getAttrOutputNTS
 */
SQLRETURN DATAEnvironment::getEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLINTEGER  nBufferLength   = 0;
    SQLINTEGER  nStringLength   = 0;
    SQLRETURN   nReturn = SQLGetEnvAttr( hHandle, nAttribute, pValue, nBufferLength, &nStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLGETENVATTR, g_Constants->mapAttrEnvNames.value( nAttribute ) );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLGETENVATTR, g_Constants->mapAttrEnvNames.value( nAttribute ) );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETENVATTR", "RETURN: SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQL_API_SQLSETENVATTR", "RETURN: Unexpected value.", nReturn );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Gets driver information.
 *
 *          This call maps to SQLDriver.
 * 
 * \param   nDirection
 * \param   pszDriverDescription
 * \param   nBufferLength1
 * \param   pnDescriptionLengthPtr
 * \param   pszDriverAttributes
 * \param   nBufferLength2
 * \param   pnAttributesLengthPtr
 * 
 * \return  SQLRETURN
 */
SQLRETURN DATAEnvironment::doDrivers( SQLUSMALLINT nDirection, SQLTCHAR *pszDriverDescription, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnDescriptionLengthPtr, SQLTCHAR *pszDriverAttributes, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnAttributesLengthPtr )
{
    if ( !isAlloc() ) return SQL_ERROR;

    //
    SQLRETURN nReturn = SQLDrivers( hHandle, nDirection, pszDriverDescription, nBufferLength1, pnDescriptionLengthPtr, pszDriverAttributes, nBufferLength2, pnAttributesLengthPtr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLDRIVERS );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLDRIVERS );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, __FUNCTION__, "SQL_INVALID_HANDLE" );
            break;
        default:
            eventMessage( DATAMessage::Error, __FUNCTION__, "Unexpected SQLRETURN value.", nReturn );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Get data source information.
 *
 *          This call maps to SQLDataSources.
 * 
 * \param   nDirection
 * \param   pszServerName
 * \param   nBufferLength1
 * \param   pnNameLength1Ptr
 * \param   pszDescription
 * \param   nBufferLength2
 * \param   pnNameLength2Ptr
 * 
 * \return  SQLRETURN
 */
SQLRETURN DATAEnvironment::doDataSources( SQLUSMALLINT nDirection, SQLTCHAR *pszServerName, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnNameLength1Ptr, SQLTCHAR *pszDescription, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnNameLength2Ptr )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    //
    SQLRETURN nReturn = SQLDataSources( hHandle, nDirection, pszServerName, nBufferLength1, pnNameLength1Ptr, pszDescription, nBufferLength2, pnNameLength2Ptr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic( SQL_API_SQLDATASOURCES );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLDATASOURCES );
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLDataSources", QString("SQL_INVALID_HANDLE") );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLDataSources", QString("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}


