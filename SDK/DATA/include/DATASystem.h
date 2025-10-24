#pragma once

#include "DATAHandle.h"

//
typedef QMap<QString,QString> DATAKeyValues;    /*!< For convenience and readability. */

/*! 
 * \class   DATASystem
 * \brief   An ODBC system.
 *
 *          This class wraps some ODBC setup & installer functionality.
 *
 *          \li uses Qt data types where possible
 *          \li will emit signal when new messages become available
 *          \li inherits QObject to assist in parent/child relationships using rtti and using signals/slots
 * 
 */
class DATASystem : public DATAHandle
{
    Q_OBJECT
public:
    enum ConfigModes
    {
        User    = ODBC_USER_DSN,       /*!< User scope. DSN's which are for use for a specific User account.   */
        System  = ODBC_SYSTEM_DSN,     /*!< System scope. DSN's which apply to the entire system/machine.      */
        Both    = ODBC_BOTH_DSN        /*!< User and System scope combined.                                    */
    };

    explicit DATASystem();
    ~DATASystem();

    QStringList   getDriverNames();
    QStringList   getDataSourceNames( ConfigModes n = Both );
    DATAKeyValues getDriver( const QString &stringName );
    DATAKeyValues getDataSource( const QString &stringName, ConfigModes n = Both );
    DATAKeyValues getSystem();

    BOOL doManageDataSources( QWidget *pWidgetParent );

protected:
    void eventDiagnostic( SQLUSMALLINT nFunction, const QString &stringRequest = QString() );

    DATAMessage getInstallerError( WORD nRecord /* 1-8 */,  const QString &stringRequest = QString() );

    DATAKeyValues getKeyValues( const QString &stringFilename, const QString &stringSection );
    QStringList getKeys( const QString &stringFileName, const QString &stringSection );
    QStringList getParsedList( SQLWCHAR *psz, int nMaxChars );
};

