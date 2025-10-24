#include "LibInfo.h"
#include "DATAHandle.h"
#include "DATADiagnostic.h"

DATAHandle::DATAHandle( Types nType, DATAHandle *phandleParent )
    : QObject( phandleParent )
{
    pParent     = phandleParent;
    this->nType = nType;
    hHandle     = SQL_NULL_HANDLE;
}

DATAHandle::~DATAHandle()
{
    if ( isAlloc( false ) ) doFree();
}

DATAHandle::Types DATAHandle::getType()
{
    return nType;
}

SQLHANDLE DATAHandle::getHandle()
{
    return hHandle;
}

/*! 
 * \brief   Allocate underlying ODBC handle.
 *
 *          This method is not usually called explicitly from an application since 
 *          it will be silently called via \sa isAlloc as needed.
 *
 *          This method is public to handle any exceptional cases such as when
 *          an app wants to regularly \sa doAlloc and \sa doFree to handle unstable
 *          drivers.
 * 
 * \return  SQLRETURN
 *
 * \sa      doFree
 *          isAlloc
 */
SQLRETURN DATAHandle::doAlloc()
{
    if ( isAlloc( false ) )
    {
        eventMessage( DATAMessage::Warning, __FUNCTION__, tr("Already allocated.") );
        return SQL_ERROR;
    }

    SQLRETURN nReturn;

    switch ( nType )
    {
        case Sys:
            // fake it for Sys
            hHandle = (void*)1;
            return SQL_SUCCESS; 
        case Env:
            // app must remember to call setAttrODBCVersion() after this alloc 
            nReturn = SQLAllocHandle( nType, SQL_NULL_HANDLE, &hHandle );
            switch ( nReturn )
            {
                case SQL_SUCCESS:
                    return nReturn;
                case SQL_SUCCESS_WITH_INFO:
                    eventDiagnostic( SQL_API_SQLALLOCHANDLE );
                    return nReturn;
                case SQL_ERROR:
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", "SQL_ERROR" );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
                case SQL_INVALID_HANDLE:
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", "SQL_INVALID_HANDLE" );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
                default:
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", tr("Unexpected SQLRETURN value."), nReturn );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
            }
        case Dbc:
        case Stm:
            if ( !pParent->getHandle() ) pParent->doAlloc(); 
            nReturn = SQLAllocHandle( nType, pParent->getHandle(), &hHandle );
            switch ( nReturn )
            {
                case SQL_SUCCESS:
                    return nReturn;
                case SQL_SUCCESS_WITH_INFO:
                    eventDiagnostic( SQL_API_SQLALLOCHANDLE );
                    return nReturn;
                case SQL_ERROR:
                    pParent->eventDiagnostic( SQL_API_SQLALLOCHANDLE );
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", "SQL_ERROR" );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
                case SQL_INVALID_HANDLE:
                    pParent->eventDiagnostic( SQL_API_SQLALLOCHANDLE );
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", "SQL_INVALID_HANDLE" );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
                default:
                    pParent->eventDiagnostic( SQL_API_SQLALLOCHANDLE );
                    eventMessage( DATAMessage::Error, "SQLAllocHandle", tr("Unexpected SQLRETURN value."), nReturn );
                    hHandle = SQL_NULL_HANDLE;
                    return nReturn;
            }
        default:
            break;
    }

    eventMessage( DATAMessage::Error, __FUNCTION__, tr("Unknown handle type.") );
    return SQL_ERROR;
}

/*! 
 * \brief   Free underlying ODBC handle.
 *
 *          Frees resources used to maintain the underlying ODBC handle. Upon success - this will
 *          invalidate any child handles but will not notify them! Errors will be caught but the 
 *          proper way to free a handle is to first ensure that any/all child handles have been
 *          freed.
 *
 *          This method will be silently called in the destructor as needed.
 *
 *          This method is public to handle any exceptional cases such as when
 *          an app wants to regularly \sa doAlloc and \sa doFree to handle unstable
 *          drivers.
 * 
 * \return  SQLRETURN
 *
 * \sa      doAlloc
 *          isAlloc
 */
SQLRETURN DATAHandle::doFree()
{
    if ( !isAlloc( false ) )
    {
        eventMessage( DATAMessage::Warning, __FUNCTION__, tr("Already free.") );
        return SQL_ERROR;
    }

    // fake it for Sys
    if ( nType == Sys )
    {
        hHandle = SQL_NULL_HANDLE;
        return SQL_SUCCESS; 
    }

    SQLRETURN nReturn = SQLFreeHandle( nType, hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            hHandle = SQL_NULL_HANDLE;
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventMessage( DATAMessage::Info, "SQLFreeHandle", "SQL_SUCCESS_WITH_INFO" );
            hHandle = SQL_NULL_HANDLE;
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( DATAMessage::Error, "SQLFreeHandle", "SQL_INVALID_HANDLE" );
            break;
        case SQL_ERROR:
            eventDiagnostic( SQL_API_SQLFREEHANDLE );
            break;
        default:
            eventMessage( DATAMessage::Error, "SQLFreeHandle", tr("Unexpected SQLRETURN value."), nReturn );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Ensure underlying environment handle is allocated.
 *
 *          We call this at the start of most methods to ensure that we have an allocated
 *          handle to work with.
 * 
 * \param   bAlloc  True if we want to silently allocate it as needed. Default is true.
 *
 * \return  bool
 * \retval  false   Handle was not allocated AND we failed to allocate it.
 * \retval  true    Handle was either allocated OR we succeeded in doing so.
 *
 * \sa      doAlloc
 *          doFree
 */
bool DATAHandle::isAlloc( bool bAlloc )
{
    if ( hHandle == SQL_NULL_HANDLE )
    {
        if ( !bAlloc ) return false;
        SQLRETURN nReturn = doAlloc();
        if ( !SQL_SUCCEEDED( nReturn ) ) return false;
    }

    return true;
}

void DATAHandle::eventMessage( DATAMessage::Types nType, const QString &stringRequest, const QString &stringText, SQLINTEGER nNumeric )
{
    DATAMessage Message( nType, stringRequest, stringText, nNumeric );
    emit signalMessage( &Message );
}

void DATAHandle::eventDiagnostic( SQLUSMALLINT nFunction, const QString &stringRequest )
{
    DATADiagnostic d( this, nFunction, stringRequest );
    emit signalDiagnostic( &d );
}


