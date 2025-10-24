#pragma once

#include "DATASystem.h"

/*! 
 * \class   DATAEnvironment
 * \brief   An ODBC environment.
 *
 *          This class wraps the ODBC Environment handle with Qt Core. It;
 *
 *          \li uses Qt data types where possible
 *          \li will emit signal when new messages become available
 *          \li inherits QObject to assist in parent/child relationships using rtti and using signals/slots
 * 
 */
class DATAEnvironment : public DATAHandle
{
    Q_OBJECT
public:
    enum AttributeTypes
    {
        AttrConnectionPooling   = SQL_ATTR_CONNECTION_POOLING,  /*!< On/off/type of connection pooling.         */
        AttrCPMatch             = SQL_ATTR_CP_MATCH,            /*!< Type of matching for connection pooling.   */
        AttrODBCVersion         = SQL_ATTR_ODBC_VERSION,        /*!< ODBC version conforming to.                */
        AttrOutputNTS           = SQL_ATTR_OUTPUT_NTS           /*!< Are strings null terminated.               */
    };

    enum AttrConnectionPoolingTypes
    {
        CPOff           = SQL_CP_OFF,               /*!< Connection pooling is turned off. This is the default.                                                                     */
        CPOnePerDriver  = SQL_CP_ONE_PER_DRIVER,    /*!< A single connection pool is supported for each driver. Every connection in a pool is associated with one driver.           */
        CPOnePerHenv    = SQL_CP_ONE_PER_HENV       /*!< A single connection pool is supported for each environment. Every connection in a pool is associated with one environment. */
    };

    enum AttrCPMatchTypes
    {
        CPStrictMatch   = SQL_CP_STRICT_MATCH,      /*!< Only connections that exactly match the connection options in the call and the connection attributes set by the application are reused. This is the default.   */
        CPRelaxedMatch  = SQL_CP_RELAXED_MATCH      /*!< Connections with matching connection string keywords can be used. Keywords must match, but not all connection attributes must match.                           */
    };

    enum AttrODBCVersionTypes                       /*!< This needs to be set right after alloc.                        */
    {
        OVOdbc3 = SQL_OV_ODBC3,                     /*!< The Driver Manager and driver exhibit some ODBC 3.x behavior.  */
        OVOdbc2 = SQL_OV_ODBC2                      /*!< The Driver Manager and driver exhibit some ODBC 2.x behavior.  */
    };

    DATAEnvironment( DATASystem *pSystem );
    ~DATAEnvironment();

    SQLRETURN setAttrConnectionPooling( AttrConnectionPoolingTypes n = CPOff );
    SQLRETURN setAttrCPMatch( AttrCPMatchTypes n = CPStrictMatch );
    SQLRETURN setAttrODBCVersion( AttrODBCVersionTypes n );
    SQLRETURN setAttrOutputNTS( bool b = SQL_TRUE );

    AttrConnectionPoolingTypes  getAttrConnectionPooling( SQLRETURN *pnReturn = NULL );
    AttrCPMatchTypes            getAttrCPMatch( SQLRETURN *pnReturn = NULL );
    AttrODBCVersionTypes        getAttrODBCVersion( SQLRETURN *pnReturn = NULL );
    bool                        getAttrOutputNTS( SQLRETURN *pnReturn = NULL );
    DATASystem *                getSystem() { return (DATASystem*)pParent; }

    QStringList getDrivers( SQLRETURN *pnReturn = NULL );
    QMap<QString,QString> getDriversWithDescriptions( SQLRETURN *pnReturn = NULL );
    QStringList getDataSources( bool bUser = true, bool bSystem = true, SQLRETURN *pnReturn = NULL );

protected:
    SQLRETURN setEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );

    SQLRETURN getEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue );

    SQLRETURN doDrivers( SQLUSMALLINT nDirection, SQLTCHAR *pszDriverDescription, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnDescriptionLengthPtr, SQLTCHAR *pszDriverAttributes, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnAttributesLengthPtr );
    SQLRETURN doDataSources( SQLUSMALLINT nDirection, SQLTCHAR *pszServerName, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnNameLength1Ptr, SQLTCHAR *pszDescription, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnNameLength2Ptr );
};


