#pragma once

/* 
 * WIDE CHARACTERS
 * --------------- 
 *  
 * WIN 
 *  
 *      MS uses 2-byte encoding (UTF-16).
 *  
 *      wchar_t is 2-bytes.
 *  
 * UNIX 
 *  
 *      UNIX's typically use 4-byte encoding (UCS-4). 
 *  
 *      wchar_t is 4-bytes.
 *  
 * MS ODBC Driver Manager & SDK (Simba)
 *  
 *      A variety of build options can be used to create code that will work for 1-byte and
 *      wide-char (ie SQLTCHAR). MS code will silently translate these to appropriate types.
 *      MS will also translate function calls to either *A() or *W() as may be needed.
 *      This translation is dependent upon UNICODE build option being specified (or not).
 *  
 *      SQLWCHAR is 2-bytes.
 *  
 * unixODBC Driver Manager & SDK
 *  
 *      Similar build translation (1-byte or wide-char) happens when building with unixODBC. 
 *      Uses a 2-byte wide char (unsigned short) - by default - even on UNIX platforms with 4-byte wchar_t.
 *      Different behaviour can be requested during a build (ie SQL_WCHART_CONVERT) but not done here.
 *  
 *      The up-shot of this is that all ODBC drivers should use 2-byte encoding (UTF-16) 
 *      for wide-char - even on UNIX.  
 *  
 *      SQLWCHAR is 2-bytes.
 *  
 * Qt 
 *  
 *      QString data consists of QChar's - which are 2-byte encoded.
 *      This is true on all platforms.
 *      This is consistent with most (if not all) ODBC installations (as noted above).
 *  
 *      QChar is 2-bytes.
 *  
 * Encoding 
 *  
 *      The UNICODE standard seems be almost universal (either UTF-16 or UCS-4).
 *      Assuming all 1-byte is UTF-8 probably does little harm and will catch cases where this is the encoding.
 *  
 * String Literal 
 *  
 *      gcc v11 introduced 'u' which means the compiler will translate the string literal to UTF-16 (char16_t).
 *      For example; u"Hello world!"
 *  
 * Summary 
 *  
 *      We try to use QString for strings as much as possible. 
 *       
 *      IF UNICODE THEN 
 *           We assume UTF-16 for Qt and ODBC - so no translation required. 
 *      ELSE 
 *           We assume UTF-16 for Qt and UTF-8 for ODBC - so we translate strings to/from ODBC.
 *       
 *      Just ensure we are building with UNICODE. UTF-8 access to/from ODBC can be implemented 
 *      later - if needed. 
 */

#include <CBD.h>

// bring in ODBC stuff...
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <tchar.h>
    #include <sqlext.h>
    #include <odbcinst.h>
#else
extern "C" {
    #include <unistd.h>
    #include <odbcinst.h> 
    #include <sqlext.h>
}
#endif

/* 
 *               +------------+       +---------------+ 
 *               | Qt QString | <---> | ODBC SQLTCHAR |
 *               +------------+       +---------------+ 
 *  
 * wide-char 
 * --------- 
 *  
 * When built for wide-char - we can often call ODBC using QString buffers directly such as; 
 *  
 *          SQLConnectW( hHandle,
 *                      (SQLWCHAR*)stringDSN.utf16(), stringDSN.count(),
 *                      (SQLWCHAR*)stringUID.utf16(), stringUID.count(),
 *                      (SQLWCHAR*)stringPWD.utf16(), stringPWD.count() );   
 *  
 * We can do the following when a simple, unitialized, buffer is required; 
 *  
 *          SQLTCHAR sz[nBufferLength] 
 *  
 *          SQLGetInfo( hHandle,
 *                      nInfoType,
 *                      sz, nBufferLength, pnStringLength );
 *  
 *          QString s.setUtf16( sz, *pnStringLength )
 *                              OR
 *          QString s = QString::fromUtf16( sz )
 *                              OR
 *          QString s = QString::fromUtf16( sz, (nChars==SQL_NTS ? -1 : nChars) )
 *  
 * 1-byte-char 
 * ----------- 
 *  
 * When built for 1-byte-char - we must translate the string such as;
 *  
 *          QByteArray baDSN = stringDSN.toUtf8();
 *          QByteArray baUID = stringUID.toUtf8();
 *          QByteArray baPWD = stringPWD.toUtf8();
 *  
 *          SQLConnect( hHandle,
 *                      (SQLCHAR*)baDSN.data(), baDSN.count(),
 *                      (SQLCHAR*)baUID.data(), baUID.count(),
 *                      (SQLCHAR*)baPWD.data(), baPWD.count() );   
 *  
 * We can do the following when a simple, unitialized, buffer is required; 
 *  
 *          SQLTCHAR sz[nBufferLength] 
 *  
 *          SQLGetInfo( hHandle,
 *                      nInfoType,
 *                      sz, nBufferLength, pnStringLength );
 *  
 *          QString s.setUtf8( sz, *pnStringLength )
 *                           OR
 *          QString s = sz
 *  
 */

// some extra functions defined for DATAConstants etc
#ifndef SQL_API_SQLGETFUNCTIONS
    #define SQL_API_SQLGETFUNCTIONS 2000
#endif
#ifndef SQL_API_SQLGETINSTALLEDDRIVERS
    #define SQL_API_SQLGETINSTALLEDDRIVERS 2001    
#endif
#ifndef SQL_API_SQLGETPRIVATEPROFILESTRING
    #define SQL_API_SQLGETPRIVATEPROFILESTRING 2002
#endif
#ifndef SQL_API_SQLMANAGEDATASOURCES
    #define SQL_API_SQLMANAGEDATASOURCES 2003      
#endif

// some more globals
#include "DATAConstants.h"

