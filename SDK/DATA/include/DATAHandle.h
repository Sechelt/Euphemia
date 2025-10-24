#pragma once

#include "DATA.h"

#include "DATADiagnostic.h"

/*! 
 * \brief   An ODBC handle.
 *
 *          This is the base class for key classes in this library. Specifically;
 *
 *          \li DATAEnvironment
 *          \li DATAConnection
 *          \li DATAStatement
 *
 * \sa DATAEnvironment
 *     DATAConnection
 *     DATAStatement
 */
class DATAHandle : public QObject
{
    Q_OBJECT
    friend class DATADiagnostic;
    friend class DATADiagnosticRecord;
public:
    enum Types
    {
        Sys = 100,
        Env = SQL_HANDLE_ENV,
        Dbc = SQL_HANDLE_DBC,
        Stm = SQL_HANDLE_STMT
    };

    explicit DATAHandle( Types nType, DATAHandle *phandleParent = NULL );
    virtual ~DATAHandle();

    virtual Types           getType();
    virtual SQLHANDLE       getHandle();

    virtual SQLRETURN doAlloc();        
    virtual SQLRETURN doFree();         

    bool isAlloc( bool bAlloc = true );

signals:
    void signalMessage( DATAMessage * );
    void signalDiagnostic( DATADiagnostic * );

protected:
    DATAHandle *    pParent;
    Types           nType;
    SQLHANDLE       hHandle;

    virtual void eventMessage( DATAMessage::Types nType, const QString &stringRequest = QString(), const QString &stringText = QString(), SQLINTEGER nNumeric = 0 );
    virtual void eventDiagnostic( SQLUSMALLINT nFunction, const QString &stringRequest = QString() );
};

