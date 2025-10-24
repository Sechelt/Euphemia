#ifndef H_DATAWConnectWizard
#define H_DATAWConnectWizard

#include <CBD.h>

#include <DATASystem.h>

class WFileSelectWidget;
class DATAConnection;
class DATAOutputDiagnosticsWidget;

/*!
 * \brief Base class for widgets to get connect options for a driver.
 *  
 * Pass driver name as found in ODBC config just in case it varies for some reason. 
 *  
 * \author pharvey (12/12/20)
 */
class DATAWDriverWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWDriverWidget( QWidget *pWidgetParent = nullptr );

    virtual QMap<QString,QString> getKeyValues() = 0;
};

/*!
 * \brief Connection options for the SQLite3 ODBC driver.
 * 
 * This is for the free (and most common) ODBC driver from Christian Werner.
 * 
 * The following is from http://www.ch-werner.de/sqliteodbc/html/index.html 
 * 
 * BEGIN
 * 
 * Connect string parameters for DSN-less connects
 *
 * Database (string)     name of SQLite2/3 database file; default empty
 * PWD (string)          password when built with SEE support; default empty
 * Timeout (integer)     lock time out in milliseconds; default 100000
 * StepAPI (boolean)     if true, use sqlite[3]_step et.al.; default false
 * NoTXN (boolean)       if true, only pretend transactions; default false
 * NoWCHAR (boolean)     if true, don't support WCHAR types for character
 *                       data; default false
 * NoCreat (boolean)     if true and database file doesn't exist, don't
 *                       create it automatically; default false, unsupported
 *                       for SQLite2
 * LongNames (boolean)   if true, don't shorten column names; default false
 * ShortNames (boolean)  if true, enforce short column names; default false
 * SyncPragma (string)   value for PRAGMA SYNCHRONOUS; default empty
 * FKSupport (boolean)   if true, support SQLite3 foreign key constraints;
 *                       default false
 * JournalMode (string)  value for PRAGMA JOURNAL_MODE; default empty
 * OEMCP (boolean)       Win32 only: if true, translate strings from/to
 *                       UTF8 to current code page; default false
 * BigInt (boolean)      if true, force integer columns to SQL_BIGINT;
 *                       default false
 * JDConv (boolean)      if true, use SQLite3 julian day representation for
 *                       SQL_TIME, SQL_TIMESTAMP, SQL_DATE types; default false
 * TraceFile (string)    name of file to write SQLite traces to; default empty
 * 
 * END
 * 
 * Example;
 * 
 * "DRIVER=SQLite3 ODBC Driver;Database=c:\mydb.db;LongNames=0;Timeout=1000;NoTXN=0;SyncPragma=NORMAL;StepAPI=0;"
 * 
 * \author pharvey (12/12/20)
 */
class DATAWConnectSQLite3Widget : public DATAWDriverWidget
{
    Q_OBJECT
public:
    DATAWConnectSQLite3Widget( QWidget *pWidgetParent );

    virtual QMap<QString,QString> getKeyValues();

protected:
    QTabWidget *        pTabWidget;
    WFileSelectWidget * pDatabase;
    QLineEdit *         pPWD;
    QSpinBox *          pTimeout;
    QCheckBox *         pStepAPI;
    QCheckBox *         pNoTXN;
    QCheckBox *         pNoWCHAR;
    QCheckBox *         pNoCreat;
    QCheckBox *         pLongNames;
    QCheckBox *         pShortNames;
    QComboBox *         pSyncPragma;    //  0 | OFF | 1 | NORMAL | 2 | FULL | 3 | EXTRA
    QCheckBox *         pFKSupport;
    QComboBox *         pJournalMode;   // DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
#ifdef Q_OS_WIN         
    QCheckBox *         pOEMCP;
#endif                  
    QCheckBox *         pBigInt;
    QCheckBox *         pJDConv;
    WFileSelectWidget * pTraceFile;
};

/*!
 * \brief Helps create a connection string for connecting to a MariaDB data source.
 * 
 * \author pharvey (1/8/25)
 */
class DATAWConnectMariaWidget : public DATAWDriverWidget
{
    Q_OBJECT
public:
    DATAWConnectMariaWidget( QWidget *pParent );

    virtual QMap<QString,QString> getKeyValues();

    QString getServer()     { return pServer->text(); }
    QString getUser()       { return pUser->text(); }
    QString getPassword()   { return pPassword->text(); }
    QString getDatabase()   { return pDatabase->text(); }

protected:
    QLineEdit *pServer;
    QLineEdit *pUser;
    QLineEdit *pPassword;
    QLineEdit *pDatabase;
};

/*!
 * \brief Connection options for PostgreSQL ODBC driver.
 *
 *  The following is from; https://odbc.postgresql.org
 *
 *  BEGIN
 *
 *  How to specify as a connection option
 * 
 *  Keyword 	            Abbreviation    Definition                          
 *                                                                      
 *  Description 	        Nothing         Data source description             
 *  Servername 	            Nothing         Name of Server                      
 *  Port 	                Nothing         Postmaster listening port           
 *  Username 	            Nothing         User Name                           
 *  Password 	            Nothing         Password                            
 *  Debug 	                B2              Debug flag                          
 *  Fetch 	                A7              Fetch Max Count                     
 *  Socket 	                A8              Socket buffer size                  
 *  ReadOnly 	            A0              Database is read only               
 *  CommLog 	            B3              Communication to backend logging    
 *  Protocol 	            A1              PostgreSQL backend protocol         
 *  ConnSettings 	        A6              Connect Settings (commands sent to backend on connection)
 *  UniqueIndex 	        Nothing         Recognize unique indexes            
 *  UnknownSizes 	        A9              Unknown result set sizes            
 *  UseDeclareFetch 	    B6              Use Declare/Fetch cursors           
 *  TextAsLongVarchar 	    B7              Text as LongVarchar                 
 *  UnknownsAsLongVarchar 	B8              Unknowns as LongVarchar             
 *  BoolsAsChar 	        B9              Bools as Char                       
 *  MaxVarcharSize 	        B0              Max Varchar size                    
 *  MaxLongVarcharSize 	    B1              Max LongVarchar size                
 *  FakeOidIndex 	        A2              Fakes a unique index on OID         
 *  ShowOidColumn 	        A3              Includes the OID in SQLColumns      
 *  RowVersioning 	        A4              Row Versioning                      
 *  ShowSystemTables 	    A5              Show SystemTables                   
 *  Parse 	                C0              Parse Statements                    
 *  ExtraSysTablePrefixes 	C2              SysTable Prefixes                   
 *  UpdatableCursors 	    C4              Updateable Cursors                  
 *  LFConversion 	        C5              LF <-> CR/LF conversion             
 *  TrueIsMinus1 	        C6              True is -1                          
 *  BI 	                    Nothing         Datatype to report int8 columns as  
 *  ByteaAsLongVarBinary 	C7              Bytea as LO (longVarBinary)         
 *  UseServerSidePrepare 	C8              Use serverside prepare              
 *  LowerCaseIdentifier 	C9              Lower case identifier               
 *  SSLmode 	            CA              SSL mode                            
 *  AB 	                    Nothing         Extra options                       
 *  CX 	                    Nothing         Abbreviate(simple setup of a recommendation value)
 *  KeepaliveTime 	        D1              TCP KEEPALIVE setting: idle time
 *  KeepaliveInterval 	    D2              TCP KEEPALIVE setting: interval 	
 *  Pqopt 	                D5              Libpq parameters (a conninfo style string e.g. pqopt={sslcert=~/mycert sslkey=~/mykey} enclosed with braces in the case of connection strings) 	
 *  NumericAS 	            D6              Numeric As: specify the map from numeric items without precision to SQL data types. 2(SQL_NUMERIC default), 8(SQL_DOUBLE), 12(SQL_VARCHAR) or -1(SQL_LONGVARCHAR) can be specified. 	
 *  OptionalErrors 	        D7              Show optional error messages. 	
 *  BatchSize 	            D8              Chunk size when executing batches for parameterized SQL statements with arrays of parameters. 	
 *  IgnoreTimeout 	        D9              Ignore SQL_ATTR_QUERY_TIMEOUT set using SQLSetStmtAttr(). 	
 *
 *  END
 * 
 *  Described in more detail at; https://odbc.postgresql.org/docs/config.html
 * 
 *  Example;
 * 
 *      Driver={PostgreSQL ANSI};Server=IP address;Port=5432;Database=myDataBase;Uid=?;Pwd=??;sslmode=require;
 * 
 * \note The connection string should use the abbreviation when possible (or not?).
 * 
 * \author pharvey (12/13/20)
 */
class DATAWConnectPSQLWidget : public DATAWDriverWidget
{
    Q_OBJECT
public:
    DATAWConnectPSQLWidget( QWidget *pWidgetParent = nullptr );

    virtual QMap<QString,QString> getKeyValues();

protected:
    QTabWidget *        pTabWidget;
    QLineEdit *         pServername;
    QLineEdit *         pDatabase;      
    QLineEdit *         pUsername;  
    QLineEdit *         pPassword;  

    QLineEdit *         pPort;      
    QComboBox *         pSSLmode;       // verify-full | verify-ca | require | prefer | allow | disable
};

/*!
 * \brief Helps create a connection string when data source is unknown.
 * 
 * Value of "?" means prompt.
 * Value of "??" means prompt with mask (ie hide password).
 * 
 * Example;
 * 
 * "HOST=MyHost;UID=?;PWD=??;DATABASE=MyDatabase;"
 *  
 * \todo Start with common and allow User to add/remove key/value pairs similar to \sa AWDynamicDataWidget. 
 *  
 * \author pharvey (1/8/25)
 */
class DATAWConnectGenericWidget : public DATAWDriverWidget
{
    Q_OBJECT
public:
    DATAWConnectGenericWidget( QWidget *pParent );

    virtual QMap<QString,QString> getKeyValues();

protected:
    QVector<QLineEdit*> vectorKeys;
    QVector<QLineEdit*> vectorValues;
};

/*!
 * \brief Provides a description of what this wizard does.
 * 
 * \author pharvey (1/11/25)
 */
class DATAWConnectIntroPage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectIntroPage( QWidget *pWidgetParent = nullptr );
};

/*!
 * \brief Select a method to connect.
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectMethodPage : public QWizardPage
{
    Q_OBJECT
public:
    enum Methods
    {
        MethodString = 0,               // let the User enter the connect string in QLineEdit.
        MethodDriver,                   // select a DRIVER and use predetermined KeyValues
        MethodBrowse,                   // select a DRIVER and browse for KeyValues
        MethodDataSourceNameBoth,       // select a DSN from combined System and User list
        MethodDataSourceNameSystem,     // select a DSN from System list
        MethodDataSourceNameUser,       // select a DSN from User list
        MethodDataSourceNameFile        // select a DSN from list of file DSN's (in default, local, file system location)
    };
    DATAWConnectMethodPage( QWidget *pWidgetParent = nullptr );

    virtual int nextId() const;

protected:
    QComboBox * pMethods;
    QLabel *    pTip;

protected slots:
    virtual void slotMethod( int nIndex );
};

/*!
 * \brief Connect using a connect string entered in a QLineEdit.
 * 
 * This supports a call to SQLDriverConnect.
 * 
 * Fields with a '%' will be prompted for.
 * Fields with a '%%' will be prompted for and will be masked as per passwords. 
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectStringPage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectStringPage( QWidget *pParent = nullptr );

    virtual int nextId() const;
    virtual QMap<QString,QString> getKeyValues();

protected:
    QLineEdit *     pConnectString;
    QToolButton *   pParse;
    QLabel *        pConnectStringParsed;
    QLabel *        pTip;

protected slots:
    virtual void slotParse();
};

/*!
 * \brief Connect using DRIVER plus predetermined KeyValues.
 * 
 * This supports a call to SQLDriverConnect.
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectDriverPage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectDriverPage( DATAConnection *pConnection, QWidget *pWidgetParent = nullptr );

    virtual int nextId() const;
    virtual QMap<QString,QString> getKeyValues();

protected:
    DATAConnection *    pConnection;
    QComboBox *         pDrivers;
    QVBoxLayout *       pLayoutTop;
    DATAWDriverWidget * pDriverWidget = nullptr;

protected slots:
    virtual void slotDriver( int nIndex );
};

/*!
 * \brief Connect using DRIVER plus SQLBrowseConnect to develop KeyValue pairs.
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectBrowsePage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectBrowsePage( DATAConnection *pConnection, QWidget *pWidgetParent = nullptr );

    virtual int nextId() const;
    virtual QMap<QString,QString> getKeyValues();

protected:
    DATAConnection *pConnection;
};

/*!
 * \brief Connect using DSN.
 * 
 * This supports a call to SQLConnect with the ServerName being the DSN.
 * 
 * \author pharvey (1/27/21)
 */
class DATAWConnectDataSourceNamePage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectDataSourceNamePage( DATASystem::ConfigModes nDataSourceType, DATAConnection *pConnection, QWidget *pWidgetParent = nullptr );

    // SQLDataSources

    virtual int nextId() const;
    virtual QMap<QString,QString> getKeyValues();

protected:
    DATAConnection *pConnection;
    QComboBox *     pDataSourceNames;
    QLineEdit *     pUserName;
    QLineEdit *     pAuthentication;
};

/*!
 * \brief Conect using a File DSN.
 * 
 * This supports a call to SQLConnect with the ServerName being the DSN.
 * 
 * \author pharvey (1/10/25)
 */
class DATAWConnectFilePage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectFilePage( DATAConnection *pConnection, QWidget *pWidgetParent = nullptr );

    virtual int nextId() const;
    virtual QMap<QString,QString> getKeyValues();

protected:
    DATAConnection *    pConnection;
    WFileSelectWidget * pFileSelect;
    QLineEdit *         pUserName;
    QLineEdit *         pAuthentication;
};

/*!
 * \brief Final page on connection wizard which allows connect/disconnect.
 * 
 * \author pharvey (1/9/25)
 */
class DATAWConnectPage : public QWizardPage
{
    Q_OBJECT
public:
    DATAWConnectPage( DATAConnection *pConnection, QWidget *pWidgetParent = nullptr );

    virtual QMap<QString,QString> getKeyValues();

protected:
    DATAConnection *                pConnection;
    DATAOutputDiagnosticsWidget *   pDiagnostics;
    QFormLayout *                   pForm;
    QPushButton *                   pButtonConnect;    
    QPushButton *                   pButtonDisconnect;
    QMap<QString,QWidget*>          mapKeyEditors;

    virtual void initializePage();
    virtual void cleanupPage();

    virtual void doSyncControls();

protected slots:
    virtual void slotConnect();
    virtual void slotDisconnect();
};

/*!
 * \brief Guide User through process to create a connect string in the form of KeyValues.
 * 
 * The UID (or equivalent) key will have a value of '?' which means prompt.
 * The PWD (or equivalent) key will have a value of '??' which means prompt with a mask.
 * The KeyValues can be stored as prompt values not saved in KeyValues returned by getKeyValues() - just '?'.
 * The KeyValues can be used by \sa DATAWConnectDialog to connect again with only '?' values being prompted for.
 * This use knowledge of common drivers, and their connect string options, as an aid.
 * 
 * \author pharvey (1/10/25)
 */
class DATAWConnectWizard : public QWizard
{
    Q_OBJECT
public:
    enum Pages
    {
        PageIntro = 0,
        PageMethod,
        PageString,
        PageDriver,
        PageBrowse,
        PageDataSourceNameBoth,
        PageDataSourceNameSystem,
        PageDataSourceNameUser,
        PageDataSourceNameFile,
        PageConnect
    };

    DATAWConnectWizard( DATAConnection *pConnection, QWidget *pWidgetParent );

    virtual QMap<QString,QString> getKeyValues() { return pConnectPage->getKeyValues(); }

protected:
    DATAConnection *                pConnection;
    DATAWConnectPage *              pConnectPage;

    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotFinished( int );
};

#endif

