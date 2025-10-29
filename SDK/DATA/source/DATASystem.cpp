#include "LibInfo.h"
#include "DATASystem.h"

DATASystem::DATASystem()
    : DATAHandle( Sys )
{
    setObjectName( "DATASystem" );
}

DATASystem::~DATASystem()
{
}

QStringList DATASystem::getDriverNames()
{
    WORD    nCharsMax = 32768; // 32k
    WCHAR   szBuf[32768];
    WORD    nCharsOut = 0;

    BOOL b = SQLGetInstalledDriversW( szBuf, nCharsMax, &nCharsOut );
    if ( !b )
    { 
        eventDiagnostic( SQL_API_SQLGETINSTALLEDDRIVERS );
        return QStringList();
    }

    return getParsedList( szBuf, nCharsOut );
}

/*!
 * \brief   Get a list of data sources.
 *  
 *          Gets a list of data sources (DSN's) from system information using
 *          the given scope. This does NOT include file DSN's.
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   nScope 
 * 
 * \return  QStringList 
 */
QStringList DATASystem::getDataSourceNames( DATASystem::ConfigModes n )
{
    UWORD nOrig = ODBC_BOTH_DSN;

    SQLGetConfigMode( &nOrig );
    SQLSetConfigMode( n );
    // The section names in ODBC.INI are the data sources names. 
    // So lets get all section names.
    LPCWSTR pszSection       = NULL;        // null means get them all
    LPCWSTR pszEntry         = NULL;        // null (in this case) means ignore
    QString stringDefault    = "";          // can not be null so empty string
    int     nMaxChars        = 32768;       // 32k char buffer
    WCHAR   szResult[32768];                // buffer for results
    QString stringFileName   = "ODBC.INI";  // file name (standard file)

    //
    // !!! FAILS TO RETURN MORE THAN ONE DSN !!!
    //     (use DATAEnvironment::getDataSources instead)
    //

    int nCharsRead = SQLGetPrivateProfileStringW( pszSection, pszEntry, (LPCWSTR)stringDefault.utf16(), szResult, nMaxChars, (LPCWSTR)stringFileName.utf16() );
    if ( nCharsRead < 0 ) eventDiagnostic( SQL_API_SQLGETPRIVATEPROFILESTRING );
    SQLSetConfigMode( nOrig );
    if ( nCharsRead < 1 ) return QStringList();

    return getParsedList( szResult, nCharsRead );
}


/*!
 * \brief   Get driver attributes. 
 *  
 *          This gets the driver attributes from ODBCINST.INI. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringDriver 
 * \param   pnReturn 
 * 
 * \return  OQDriver 
 */
DATAKeyValues DATASystem::getDriver( const QString &stringDriver )
{
    return getKeyValues( "ODBCINST.INI", stringDriver );
}

/*!
 * \brief   Get data source information.
 *  
 *          Gets data source information using the given scope.
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringDataSourceName 
 * \param   nScope 
 * \param   pnReturn 
 * 
 * \return  OQDataSourceName 
 */
DATAKeyValues DATASystem::getDataSource( const QString &stringDataSourceName, DATASystem::ConfigModes nConfigMode )
{
    UWORD nOrig = DATASystem::Both;

    SQLGetConfigMode( &nOrig );
    SQLSetConfigMode( nConfigMode );

    DATAKeyValues KeyValues = getKeyValues( "ODBC.INI", stringDataSourceName );

    SQLSetConfigMode( nOrig );

    return KeyValues;
}

/*!
 * \brief   Get ODBC system-wide attributes. 
 *  
 *          These attributes are system-wide and include such things as connection pooling
 *          and DM tracing. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   pnReturn 
 * 
 * \return  DATAKeyValues 
 */
DATAKeyValues DATASystem::getSystem()
{
    return getKeyValues( "ODBCINST.INI", "ODBC" );
}

#if defined(Q_OS_OSX)
/*!
 * \brief Invoke system GUI to manage datasources. 
 *  
 * In this case we are using OSX - so iODBC.
 * 
 * On OSX we exec a program since SQLManageDataSources() is not likely to be supported. 
 *  
 * \author pharvey (2/1/20)
 * 
 * \return BOOL 
 */
BOOL DATASystem::doManageDataSources( QWidget *pWidgetParent )
{
    Q_UNUSED(pWidgetParent);

    QProcess  * pprocess = new QProcess( this );

    pprocess->addArgument( "open" );
    pprocess->addArgument( "/Applications/Utilities/ODBC Administrator.app" ); // OSX Jaguar now has a standard ODBC Administrator :)
    if ( pprocess->start() ) 
    {
        return true;
    }
    
    delete pprocess;

    // we could fall back to a 'custom brew' solution here

    return false;
}
#elif defined(Q_OS_WIN)
/*!
 * \brief Invoke system GUI to manage datasources. 
 *  
 * In this case we are using MS Windows - so Simba ODBC. 
 *  
 * The call is fully supported and no tricks are needed... we just pass the call along 
 * using a native window handle associated with the provided widget. 
 *  
 * \author pharvey (2/1/20)
 * 
 * \return BOOL 
 */
BOOL DATASystem::doManageDataSources( QWidget *pWidgetParent )
{
	return SQLManageDataSources( HWND(pWidgetParent->winId()) );
}
#else
/*!
 * \brief Invoke system GUI to manage datasources. 
 *  
 * In this case we are using any UNIX other than OSX - so unixODBC.
 *  
 * We could be using any number of GUI environments - or none at all - but the assumption 
 * is that the app/caller knows what they want. So it initializes a special window 
 * handle (ODBCINSTWND) provided by unixODBC. This provides a means to indicate the 
 * desired GUI implementation. 
 *  
 * In our case we know we want Qt - a version which matches us. 
 *  
 * \sa ODBCINSTWND in unixODBC source code for details 
 * \sa SQLManageDataSources in unixODBC source code for details 
 *  
 * \note The unixODBC GUI for Qt needs to be installed (its a seperate install from unixODBC).
 * \note The app may crash if the Qt majour version of the app is not the same as the one used in unixODBC. Measures have been taken to avoid this.
 *       The terminal will show that the object can not be reparented as its in a different thread.
 *  
 * \author pharvey (2/1/20)
 * 
 * \return BOOL 
 */
BOOL DATASystem::doManageDataSources( QWidget *pWidgetParent )
{
    printf( "[%s][%s][%d] Assuming unixODBC GUI is installed and was built with Qt6.\n", __FILE__, __FUNCTION__, __LINE__ );
    // Explicitly request odbcinstQ6 but if we set h.szUI[0] = '\0'...
    // - ODBCINSTUI env var would be used
    // - ODBCINSTUI in odbcinst.ini would be used (in ODBC section)
    ODBCINSTWND h;
    strcpy( h.szUI, "odbcinstQ6" ); // we are Qt6 code so we must ask for the Qt6 GUI implementation
    h.hWnd = HWND(pWidgetParent);   // we can pass a parent widget since we are requesting a matching library otherwise nullptr
    bool b = SQLManageDataSources( &h );
    if ( !b )
    {
        QMessageBox::critical( nullptr, "Message", "Call to SQLManageDataSources failed.\nDo you have libodbcinstQ6.so installed?\nThis is in the unixodbc-gui-qt package.\nAn alternative is to use the unixODBC 'odbcinst' command in a terminal." );
        eventDiagnostic( SQL_API_SQLMANAGEDATASOURCES ); 
    }
    return b;
}
#endif


/*!
 * \brief Get errors and send them out via signal. 
 *  
 * Unlike other handles - we do not work with OQDiagnostic. Instead - we gather any errors 
 * and send them out using signalMessage. 
 * 
 * \author pharvey (1/8/20)
 */
void DATASystem::eventDiagnostic( SQLUSMALLINT nFunction, const QString &stringRequest )
{
    // spec says max 8 error records
    for ( int nRecord = 1; nRecord <= 8; nRecord++ )
    {
        DATAMessage Error = getInstallerError( nRecord, stringRequest );
        if ( Error.isNull() ) break;
        emit signalMessage( &Error );
    }
}

/*!
 * \brief Get an installer error. 
 *  
 * Here to support \sa eventDiagnostic().
 *  
 * \param nRecord 
 * 
 * \return DATAMessage 
 *  
 * \sa SQLInstallerError
 * 
 */
DATAMessage DATASystem::getInstallerError( WORD nRecord, const QString &stringRequest )
{
    DWORD   nCode                   = 0;
    WORD    nMaxChars               = SQL_MAX_MESSAGE_LENGTH; // spec says this is the max (and in bytes ??)
    WORD    nCharsOut               = 0;
    WCHAR   szMsg[1024];

    RETCODE nRetCode = SQLInstallerErrorW( nRecord, &nCode, szMsg, nMaxChars, &nCharsOut );
    if ( SQL_SUCCEEDED(nRetCode) )
    {
        return DATAMessage( DATAMessage::Error, 
                            stringRequest, 
                            QString::fromUtf16( (char16_t*)szMsg, nCharsOut ),
                            nCode );
    }
    return DATAMessage();
}

/*!
 * \brief   Gets all key/values for the given 'section'. 
 *  
 *          The 'section' is usually naming and describing a driver (ODBCINST.INI) or a DSN (ODBC.INI). 
 *  
 *          The Config Mode should be set before calling - as the case may warrant (ie DSN in an ODBC.INI).
 *  
 *          Here to support;
 *              \sa getDriver()
 *              \sa getDataSource()
 *              \sa getSystem()
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringFilename - this can be standard file name (ODBC.INI, ODBCINST.INI) or file name for a DSN file. 
 * \param   stringSection 
 * 
 * \return  DATAKeyValues 
 */
DATAKeyValues DATASystem::getKeyValues( const QString &stringFileName, const QString &stringSection )
{
    DATAKeyValues KeyValues;

    if ( stringFileName.isNull() || stringSection.isNull() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr("Invalid arguments.") );
        return KeyValues; 
    }

    // get all of the keys in the section...
    QStringList listKeys = getKeys( stringFileName, stringSection );

    // for each key - get the value
    QString stringKey;
    foreach( stringKey, listKeys )
    {
        if ( stringKey.isEmpty() )
        {
            eventMessage( DATAMessage::Error, __FUNCTION__, tr("Invalid key.") );
            continue;
        }
        int     nMaxChars       = 32768;        // 32k char buffer
        WCHAR   szResult[32768];            // buffer for results

        int nCharsRead = SQLGetPrivateProfileStringW( (LPCWSTR)stringSection.utf16(), (LPCWSTR)stringKey.utf16(), (LPCWSTR)QString("").utf16(), szResult, nMaxChars, (LPCWSTR)stringFileName.utf16() );
        if ( nCharsRead )
//            KeyValues[stringKey] = QString::fromUtf16( (ushort*)szResult, nCharsRead );
            KeyValues[stringKey] = QString::fromUtf16( (char16_t*)szResult, nCharsRead );
        else
            eventDiagnostic( SQL_API_SQLGETPRIVATEPROFILESTRING );
    }


    return KeyValues;
}

/*!
 * \brief Get all of the keys (entries) for the given section. 
 *  
 * Here to support \sa getKeyValues(). 
 *  
 * \author pharvey (1/8/20)
 * 
 * \param stringFileName 
 * \param stringSection 
 * 
 * \return QStringList 
 */
QStringList DATASystem::getKeys( const QString &stringFileName, const QString &stringSection )
{
    if ( stringFileName.isNull() || stringSection.isNull() )
    {
        eventMessage( DATAMessage::Error, __FUNCTION__, tr("Invalid arguments.") );
        return QStringList(); 
    }

    int     nMaxChars           = 32768;        // 32k char buffer
    WCHAR   szResult[32768];                // buffer for results

    int nCharsRead = SQLGetPrivateProfileStringW( (LPCWSTR)stringSection.utf16(), nullptr, (LPCWSTR)QString("").utf16(), szResult, nMaxChars, (LPCWSTR)stringFileName.utf16() );
    if ( !nCharsRead )
    {
        eventDiagnostic( SQL_API_SQLGETPRIVATEPROFILESTRING );
        return QStringList();
    }

    return getParsedList( szResult, nCharsRead );
}

/*!
 * \brief Parses out a string list. 
 *  
 * An ODBC string list is an array of chars. 
 * The end of each string is a '\0'. 
 * The end of the list is indicated by an empty string - two '\0' in a row.
 * 
 * \author pharvey (1/8/20)
 * 
 * \param psz 
 * \param nMaxChars 
 * 
 * \return QStringList 
 */
QStringList DATASystem::getParsedList( SQLWCHAR *psz, int nMaxChars )
{
    QStringList l;
    QString s;
    if ( !psz ) return l;
qDebug() << __FILE__ << __LINE__ << nMaxChars;

    // scan for max but get out as soon as possible
    // - nMaxChars does not include the '\0' term - which is on end
    for ( int nChar = 0; nChar <= nMaxChars; nChar++ )
    {
qDebug() << __FILE__ << __LINE__ << "[" << psz[nChar] << "]";
        QChar c( psz[nChar] );
        if ( c.isNull() ) // end of string
        {
qDebug() << __FILE__ << __LINE__ << s.isEmpty();
            // if ( s.isEmpty() ) break; // end of list
            l << s;
            s.clear();
            if ( nChar > ( nMaxChars - 1 ) ) break;
qDebug() << __FILE__ << __LINE__;
            QChar cNext( psz[nChar + 1] );
qDebug() << __FILE__ << __LINE__;
            if ( cNext.isNull() ) break;
qDebug() << __FILE__ << __LINE__;
            continue;
        }
       s.append( c );
qDebug() << __FILE__ << __LINE__ << s;
    }

    return l;
}


/*!
 * \brief   Wrapper for SQLGetPrivateProfileString. 
 *  
 *          Get ODBC system information from INI files and/or registry. Use this when the
 *          result is expected to be a single string.
 * 
 * \author  pharvey (10/5/2009)  
 * 
 * \param   stringSection 
 * \param   stringEntry 
 * \param   stringDefault 
 * \param   pstring 
 * \param   stringFilename 
 * 
 * \return  BOOL 
 */
/*
BOOL DATASystem::doGetPrivateProfileString( const QString &stringSection, const QString &stringEntry, const QString &stringDefault, QString *pstring, const QString &stringFilename )
{                                                                                                                                                                                    
    if ( !pstring )                                                                                                                                                                  
    {                                                                                                                                                                                
        // post an error                                                                                                                                                             
        return false;                                                                                                                                                                
    }                                                                                                                                                                                
                                                                                                                                                                                     
    pstring->clear();                                                                                                                                                                
                                                                                                                                                                                     
    LPTSTR  pszSection = (LPTSTR)(SQLTCHAR*)stringSection);                                                                                                                          
    LPTSTR  pszEntry   = (LPTSTR)(SQLTCHAR*)stringEntry);                                                                                                                            
    LPTSTR  pszDefault = (LPTSTR)(SQLTCHAR*)stringDefault);                                                                                                                          
    LPTSTR  pszFilename= (LPTSTR)(SQLTCHAR*)stringFilename);                                                                                                                         
                                                                                                                                                                                     
    int     nCharsMax = 1024;                                                                                                                                                        
    TCHAR   szChars[nCharsMax] = { '\0' };                                                                                                                                           
                                                                                                                                                                                     
    int nCharsRead = SQLGetPrivateProfileString( pszSection, pszEntry, pszDefault, szChars, nCharsMax, pszFilename );                                                                
                                                                                                                                                                                     
    if ( nCharsRead < 1 )                                                                                                                                                            
        return true;                                                                                                                                                                 
                                                                                                                                                                                     
    if ( pstring )                                                                                                                                                                   
        *pstring = OQToQStringNts( szChars );                                                                                                                                        
                                                                                                                                                                                     
    return true;                                                                                                                                                                     
}                                                                                                                                                                                    
*/

/*!
 * \brief   Wrapper for SQLGetPrivateProfileString. 
 *  
 *          Get ODBC system information from INI files and/or registry. Use this when
 *          the result is expected to be a string list - when getting all sections or
 *          all keys for a specified section.
 * 
 * \author  pharvey (10/5/2009)
 * 
 * \param   stringSection 
 * \param   stringEntry 
 * \param   pvectorStrings 
 * \param   stringFilename 
 * 
 * \return  BOOL 
 */
/*
BOOL DATASystem::doGetPrivateProfileString( const QString &stringSection, QVector<QString> *pvectorStrings, const QString &stringFilename )
{                                                                                                                                          
    if ( !pvectorStrings )                                                                                                                 
    {                                                                                                                                      
        // post an error                                                                                                                   
        return false;                                                                                                                      
    }                                                                                                                                      
                                                                                                                                           
    pvectorStrings->clear();                                                                                                               
                                                                                                                                           
    LPTSTR  pszSection      = (LPTSTR)(SQLTCHAR*)stringSection);                                                                           
    LPTSTR  pszFilename     = (LPTSTR)(SQLTCHAR*)stringFilename);                                                                          
                                                                                                                                           
    int     nCharsMax = 4096;                                                                                                              
    TCHAR   szChars[nCharsMax] = { '\0' };                                                                                                 
                                                                                                                                           
    int nCharsRead = SQLGetPrivateProfileString( pszSection, NULL, NULL, szChars, nCharsMax, pszFilename );                                
                                                                                                                                           
    if ( nCharsRead < 1 )                                                                                                                  
        return true;                                                                                                                       
                                                                                                                                           
    // decode result                                                                                                                       
    pvectorStrings->clear();                                                                                                               
    for ( TCHAR *pszCursor = szChars; pszCursor[1] != '\0'; )                                                                              
    {                                                                                                                                      
        // grab the string                                                                                                                 
        pvectorStrings->append( OQToQStringNts( pszCursor ) );                                                                             
        // scan until next string                                                                                                          
        while ( *pszCursor != '\0' )                                                                                                       
            pszCursor++;                                                                                                                   
        pszCursor++;                                                                                                                       
    }                                                                                                                                      
                                                                                                                                           
    return true;                                                                                                                           
}                                                                                                                                          
                                                                                                                                           
                                                                                                                                           
*/

