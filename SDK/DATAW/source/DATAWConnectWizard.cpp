#include "LibInfo.h"
#include "DATAWConnectWizard.h"

#include <WFileSelectWidget.h>
#include <WPasswordWidget.h>

#include "DATAConnection.h"
#include "DATAOutputDiagnosticsWidget.h"

//
// DATAWDriverWidget
//
DATAWDriverWidget::DATAWDriverWidget( QWidget *pwidgetParent )
: QWidget( pwidgetParent )
{
}

//
// DATAWConnectSQLite3Widget
//
DATAWConnectSQLite3Widget::DATAWConnectSQLite3Widget( QWidget *pwidgetParent )
: DATAWDriverWidget( pwidgetParent )
{
    QHBoxLayout *pLayoutTop = new QHBoxLayout( this );
    pTabWidget = new QTabWidget( this );
    pLayoutTop->addWidget( pTabWidget );

    // basic
    {
        QWidget *pWidget = new QWidget( pTabWidget );
        pWidget->setToolTip( tr("name of SQLite2/3 database file") );
        QVBoxLayout *pLayoutTop = new QVBoxLayout( pWidget );
        {
            QFormLayout *pLayout  = new QFormLayout();
            pDatabase = new WFileSelectWidget( QString(), "Database File (*.db)", true, pWidget );
            pLayout->addRow( tr("Database:"), pDatabase );
            pLayoutTop->addLayout( pLayout, 10 );
        }
        {
            QFormLayout *pLayout  = new QFormLayout();
            pLayout->addRow( tr("Tip:"), new QLabel( tr("if the db does not exist it will be created"), this ) );
            pLayoutTop->addLayout( pLayout );
        }

        pTabWidget->addTab( pWidget, tr("Basic") );
    }

    // advanced
    {
        QWidget *pWidget = new QWidget( pTabWidget );

        QFormLayout *pLayout  = new QFormLayout( pWidget );

        pPWD = new QLineEdit( pWidget );
        pPWD->setEchoMode( QLineEdit::Password );
        pPWD->setToolTip( tr("password when built with SEE support; default empty") );
        pLayout->addRow( tr("PWD:"), pPWD );

        pTimeout = new QSpinBox( pWidget );
        pTimeout->setMinimum( 0 );
        pTimeout->setMaximum( 500000 );
        pTimeout->setSingleStep( 1000 );
        pTimeout->setToolTip( tr("lock time out in milliseconds; default 100000") );
        pLayout->addRow( tr("Timeout:"), pTimeout );

        pStepAPI = new QCheckBox( pWidget );
        pStepAPI->setTristate();
        pStepAPI->setToolTip( tr("if true, use sqlite[3]_step et.al.; default false") );
        pLayout->addRow( tr("StepAPI:"), pStepAPI );

        pNoTXN = new QCheckBox( pWidget );
        pNoTXN->setTristate();
        pNoTXN->setToolTip( tr("if true, only pretend transactions; default false") );
        pLayout->addRow( tr("NoTXN:"), pNoTXN );

        pNoWCHAR = new QCheckBox( pWidget );
        pNoWCHAR->setTristate();
        pNoWCHAR->setToolTip( tr("if true, don't support WCHAR types for character data; default false") );
        pLayout->addRow( tr("NoWCHAR:"), pNoWCHAR );

        pNoCreat = new QCheckBox( pWidget );
        pNoCreat->setTristate();
        pNoCreat->setToolTip( tr("if true and database file doesn't exist, don't create it automatically; default false, unsupported for SQLite2") );
        pLayout->addRow( tr("NoCreat:"), pNoCreat );

        pLongNames = new QCheckBox( pWidget );
        pLongNames->setTristate();
        pLongNames->setToolTip( tr("if true, don't shorten column names; default false") );
        pLayout->addRow( tr("LongNames:"), pLongNames );

        pShortNames = new QCheckBox( pWidget );
        pShortNames->setTristate();
        pShortNames->setToolTip( tr("if true, enforce short column names; default false") );
        pLayout->addRow( tr("ShortNames:"), pShortNames );

        pSyncPragma = new QComboBox( pWidget );
        pSyncPragma->addItem( "" );
        pSyncPragma->addItem( "OFF" );
        pSyncPragma->addItem( "NORMAL" );
        pSyncPragma->addItem( "FULL" );
        pSyncPragma->addItem( "EXTRA" );
        pSyncPragma->setToolTip( tr("value for PRAGMA SYNCHRONOUS; default empty") );
        pLayout->addRow( tr("SyncPragma:"), pSyncPragma );

        pFKSupport = new QCheckBox( pWidget );
        pFKSupport->setTristate();
        pFKSupport->setToolTip( tr("if true, support SQLite3 foreign key constraints; default false") );
        pLayout->addRow( tr("FKSupport:"), pFKSupport );

        pJournalMode = new QComboBox( pWidget );
        pJournalMode->addItem( "" );
        pJournalMode->addItem( "DELETE" );
        pJournalMode->addItem( "TRUNCATE" );
        pJournalMode->addItem( "PERSIST" );
        pJournalMode->addItem( "MEMORY" );
        pJournalMode->addItem( "WAL" );
        pJournalMode->addItem( "OFF" );
        pJournalMode->setToolTip( tr("value for PRAGMA JOURNAL_MODE; default empty") );
        pLayout->addRow( tr("JournalMode:"), pJournalMode );

    #ifdef Q_OS_WIN
        pOEMCP = new QCheckBox( pWidget );
        pOEMCP->setTristate();
        pOEMCP->setToolTip( tr("if true, translate strings from/to UTF8 to current code page; default false") );
        pLayout->addRow( tr("OEMCP:"), pOEMCP );
    #endif

        pBigInt = new QCheckBox( pWidget );
        pBigInt->setTristate();
        pBigInt->setToolTip( tr("if true, force integer columns to SQL_BIGINT; default false") );
        pLayout->addRow( tr("BigInt:"), pBigInt );

        pJDConv = new QCheckBox( pWidget );
        pJDConv->setTristate();
        pJDConv->setToolTip( tr("if true, use SQLite3 julian day representation for SQL_TIME, SQL_TIMESTAMP, SQL_DATE types; default false") );
        pLayout->addRow( tr("JDConv:"), pJDConv );

        pTraceFile = new WFileSelectWidget( QString(), "Trace File (*.trace)", true, pWidget );
        pTraceFile->setToolTip( tr("name of file to write SQLite traces to; default empty") );
        pLayout->addRow( tr("TraceFile:"), pTraceFile );

        pTabWidget->addTab( pWidget, tr("Advanced") );
    }
}

QMap<QString,QString> DATAWConnectSQLite3Widget::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    // credentials
    if ( !pPWD->text().isEmpty() ) mapKeyValues["PWD"] = pPWD->text();

    // line edits
    mapKeyValues["Database"] = pDatabase->getFileName();
    if ( pTimeout->value() ) mapKeyValues["Timeout"] = pTimeout->text();
    if ( !pTraceFile->getFileName().isEmpty() ) mapKeyValues["TraceFile"] = pTraceFile->getFileName();

    // combo boxes
    if ( !pJournalMode->currentText().isEmpty() ) mapKeyValues["JournalMode"] = pJournalMode->currentText();

    // check boxes
    if ( pStepAPI->checkState() == Qt::PartiallyChecked ) mapKeyValues["StepAPI"] = "0";
    if ( pStepAPI->checkState() == Qt::Checked ) mapKeyValues["StepAPI"] = "1";

    if ( pNoTXN->checkState() == Qt::PartiallyChecked ) mapKeyValues["NoTXN"] = "0";
    if ( pNoTXN->checkState() == Qt::Checked ) mapKeyValues["NoTXN"] = "1";

    if ( pNoWCHAR->checkState() == Qt::PartiallyChecked ) mapKeyValues["NoWCHAR"] = "0";
    if ( pNoWCHAR->checkState() == Qt::Checked ) mapKeyValues["NoWCHAR"] = "1";

    if ( pNoCreat->checkState() == Qt::PartiallyChecked ) mapKeyValues["NoCreat"] = "0";
    if ( pNoCreat->checkState() == Qt::Checked ) mapKeyValues["NoCreat"] = "1";

    if ( pLongNames->checkState() == Qt::PartiallyChecked ) mapKeyValues["LongNames"] = "0";
    if ( pLongNames->checkState() == Qt::Checked ) mapKeyValues["LongNames"] = "1";

    if ( pFKSupport->checkState() == Qt::PartiallyChecked ) mapKeyValues["FKSupport"] = "0";
    if ( pFKSupport->checkState() == Qt::Checked ) mapKeyValues["FKSupport"] = "1";

#ifdef Q_OS_WIN         
    if ( pOEMCP->checkState() == Qt::PartiallyChecked ) mapKeyValues["OEMCP"] = "0";
    if ( pOEMCP->checkState() == Qt::Checked ) mapKeyValues["OEMCP"] = "1";
#endif
    if ( pBigInt->checkState() == Qt::PartiallyChecked ) mapKeyValues["BigInt"] = "0";
    if ( pBigInt->checkState() == Qt::Checked ) mapKeyValues["BigInt"] = "1";

    if ( pJDConv->checkState() == Qt::PartiallyChecked ) mapKeyValues["JDConv"] = "0";
    if ( pJDConv->checkState() == Qt::Checked ) mapKeyValues["JDConv"] = "1";

    return mapKeyValues;
}

//
// DATAWConnectMariaWidget
//
DATAWConnectMariaWidget::DATAWConnectMariaWidget( QWidget *pParent )
    : DATAWDriverWidget( pParent )
{
    pServer = new QLineEdit( this );
    pUser = new QLineEdit( "?", this );
    pPassword = new QLineEdit( "??", this );
    pDatabase = new QLineEdit( this );

    pServer->setToolTip( tr("server/hostname\nenter a hostname or IP\nblank=localhost\nsee MariaDB documentation") );
    pUser->setToolTip( tr("User name to authenticate with.\nUse ? to indicate prompt on demand.") );
    pPassword->setToolTip( tr("Password to authenticate with.\nYou should avoid entering a password here. Instead use ?? to indicate prompt on demand and not to save it anywhere.") );
    pDatabase->setToolTip( tr("enter a database to make it current after connection\nleaving this blank means no database will be current after connection") );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Server"), pServer );
    pLayout->addRow( tr("User"), pUser );
    pLayout->addRow( tr("Password"), pPassword );
    pLayout->addRow( tr("Database"), pDatabase );
}

QMap<QString,QString> DATAWConnectMariaWidget::getKeyValues()
{
    QMap <QString,QString> mapKeyValues;

    if ( !pServer->text().isEmpty() ) mapKeyValues.insert( "HOST", pServer->text() );
    if ( !pUser->text().isEmpty() ) mapKeyValues.insert( "USER", pUser->text() );
    if ( !pPassword->text().isEmpty() ) mapKeyValues.insert( "PASSWORD", pPassword->text() );
    if ( !pDatabase->text().isEmpty() ) mapKeyValues.insert( "DATABASE", pDatabase->text() );

    return mapKeyValues;
}

//
// DATAWConnectPSQLWidget
//
DATAWConnectPSQLWidget::DATAWConnectPSQLWidget( QWidget *pwidgetParent )
: DATAWDriverWidget( pwidgetParent )
{
    QHBoxLayout *pLayoutTop = new QHBoxLayout( this );
    pTabWidget = new QTabWidget( this );
    pLayoutTop->addWidget( pTabWidget );

    // basic
    {
        QWidget *pWidget = new QWidget( pTabWidget );

        QFormLayout *pLayout  = new QFormLayout( pWidget );

        pServername = new QLineEdit( pWidget );
        pServername->setToolTip( tr("address or name of server") );
        pLayout->addRow( tr("Server:"), pServername );

        pDatabase = new QLineEdit( pWidget );
        pDatabase->setToolTip( tr("database to use") );
        pLayout->addRow( tr("Database:"), pDatabase );

        pUsername = new QLineEdit( "?", pWidget );
        pUsername->setToolTip( tr("the user to authenticate as") );
        pLayout->addRow( tr("User Name:"), pUsername );

        pPassword = new QLineEdit( "??", pWidget );
        pPassword->setEchoMode( QLineEdit::Normal );
        pPassword->setToolTip( tr("password to authenticate access") );
        pLayout->addRow( tr("Password:"), pPassword );

        pTabWidget->addTab( pWidget, tr("Basic") );
    }

    // advanced
    {
        QWidget *pWidget = new QWidget( pTabWidget );

        QFormLayout *pLayout  = new QFormLayout( pWidget );

        pPort = new QLineEdit( pWidget );
        pPort->setToolTip( tr("the port to use to access server process") );
        pLayout->addRow( tr("Port:"), pPort );

        pSSLmode = new QComboBox( pWidget );
        pSSLmode->addItem( "" );
        pSSLmode->addItem( "verify-full" );
        pSSLmode->addItem( "verify-ca" );
        pSSLmode->addItem( "require" );
        pSSLmode->addItem( "prefer" );
        pSSLmode->addItem( "allow" );
        pSSLmode->addItem( "disable" );
        pSSLmode->setToolTip( tr("degree to use SSL") );
        pLayout->addRow( tr("SSLmode:"), pSSLmode );

        pTabWidget->addTab( pWidget, tr("Advanced") );
    }
}

QMap<QString,QString> DATAWConnectPSQLWidget::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    mapKeyValues["Servername"] = pServername->text();
    if ( !pDatabase->text().isEmpty() ) mapKeyValues["Database"] = pDatabase->text();
    mapKeyValues["Username"] = pUsername->text();
    mapKeyValues["Password"] = pPassword->text();
    if ( !pPort->text().isEmpty() ) mapKeyValues["Port"] = pPort->text();
    if ( !pSSLmode->currentText().isEmpty() ) mapKeyValues["SSLmode"] = pSSLmode->currentText();

    return mapKeyValues;
}

//
// DATAWConnectGenericWidget
//
DATAWConnectGenericWidget::DATAWConnectGenericWidget( QWidget *pParent )
    : DATAWDriverWidget( pParent )
{
    QGridLayout *pLayoutTop = new QGridLayout( this );
    pLayoutTop->addWidget( new QLabel( tr("Key"), this ), 0, 0 );
    pLayoutTop->addWidget( new QLabel( tr("Value"), this ), 0, 1 );

    vectorKeys.append( new QLineEdit( "HOST", this ) );
    vectorKeys.append( new QLineEdit( "UID", this ) );
    vectorKeys.append( new QLineEdit( "PWD", this ) );
    vectorKeys.append( new QLineEdit( "DATABASE", this ) );
    vectorKeys.append( new QLineEdit( "", this ) );
    vectorKeys.append( new QLineEdit( "", this ) );

    vectorValues.append( new QLineEdit( "", this ) );
    vectorValues.append( new QLineEdit( "?", this ) );
    vectorValues.append( new QLineEdit( "??", this ) );
    vectorValues.append( new QLineEdit( "", this ) );
    vectorValues.append( new QLineEdit( "", this ) );
    vectorValues.append( new QLineEdit( "", this ) );

    for ( int n = 0; n < vectorKeys.count(); n++ )
    {
        pLayoutTop->addWidget( vectorKeys[n], n + 1, 0 );
        pLayoutTop->addWidget( vectorValues[n], n + 1, 1 );
    }
    pLayoutTop->setColumnStretch( 2, 10 );
}

QMap<QString,QString> DATAWConnectGenericWidget::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;
    for ( int n = 0; n < vectorKeys.count(); n++ )
    {
        QString stringKey = vectorKeys[n]->text().simplified();
        if ( stringKey.isEmpty() ) continue;
        if ( stringKey.contains( " " ) ) 
        {
            printf( "[%s][%s][%d] Key can not have a space [%s]. Ignored.\n", __FILE__, __FUNCTION__, __LINE__, stringKey.toUtf8().constData() );
            continue;
        }
        mapKeyValues.insert( vectorKeys[n]->text().simplified(), vectorValues[n]->text().simplified() );
    }
    return mapKeyValues;
}

//
// DATAWConnectIntroPage
//
DATAWConnectIntroPage::DATAWConnectIntroPage( QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Introduction") );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pLayoutTop->addWidget( new QTextEdit( tr("This will guide you through creating a reusable connection string.\n<b>Do not</b> put passwords into your connection string. Use '??' for passwords and '?' for other values to be prompted during connect.\nYou will be able to test it on the last page."), this ) );
}

//
// DATAWConnectMethodPage
//
DATAWConnectMethodPage::DATAWConnectMethodPage( QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Connect Method") );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        QFormLayout *pLayout = new QFormLayout();
        pMethods = new QComboBox( this );
        // important: insert in same order as enum
        pMethods->addItem( tr("String"), MethodString );
        pMethods->addItem( tr("Driver"), MethodDriver );
        pMethods->addItem( tr("Browse"), MethodBrowse );
        pMethods->addItem( tr("DataSourceName Both"), MethodDataSourceNameBoth );
        pMethods->addItem( tr("DataSourceName System"), MethodDataSourceNameSystem );
        pMethods->addItem( tr("DataSourceName User"), MethodDataSourceNameUser );
        pMethods->addItem( tr("DataSourceName File"), MethodDataSourceNameFile );
        pMethods->setCurrentIndex( 1 );
        connect( pMethods, SIGNAL(currentIndexChanged(int)), SLOT(slotMethod(int)) );
        pLayout->addRow( tr("Method"), pMethods );
        pLayoutTop->addLayout( pLayout, 10 );
    }

    {
        QFormLayout *pLayout  = new QFormLayout();
        pLayout->addRow( tr("Tip:"), pTip = new QLabel( tr("select a DRIVER and use predetermined KeyValues"), this ) );
        pLayoutTop->addLayout( pLayout );
    }

    registerField( "method.methods", pMethods );
}

int DATAWConnectMethodPage::nextId() const
{
    int nIndex = pMethods->currentIndex();
    switch ( nIndex )
    {
    case MethodString:
        return DATAWConnectWizard::PageString;
    case MethodDriver:
        return DATAWConnectWizard::PageDriver;
    case MethodBrowse:
        return DATAWConnectWizard::PageBrowse;
    case MethodDataSourceNameBoth:
        return DATAWConnectWizard::PageDataSourceNameBoth;
    case MethodDataSourceNameSystem:
        return DATAWConnectWizard::PageDataSourceNameSystem;
    case MethodDataSourceNameUser:
        return DATAWConnectWizard::PageDataSourceNameUser;
    case MethodDataSourceNameFile:
        return DATAWConnectWizard::PageDataSourceNameFile;
    }
    return -1;
}

void DATAWConnectMethodPage::slotMethod( int nIndex )
{
    pTip->clear();
    switch ( nIndex )
    {
    case MethodString:
        pTip->setText( tr("connect using a free-form connect string") );
        break;
    case MethodDriver:
        pTip->setText( tr("select a DRIVER and use predetermined KeyValues") );
        break;
    case MethodBrowse:
        pTip->setText( tr("select a DRIVER and browse for KeyValues") );
        break;
    case MethodDataSourceNameBoth:
        pTip->setText( tr("select a DSN from combined System and User list") );
        break;
    case MethodDataSourceNameSystem:
        pTip->setText( tr("select a DSN from System list") );
        break;
    case MethodDataSourceNameUser:
        pTip->setText( tr("select a DSN from User list") );
        break;
    case MethodDataSourceNameFile:
        pTip->setText( tr("select a DSN from list of file DSN's (in default, local, file system location)") );
        break;
    }
}

/*!
 * \brief DATAWConnectStringPage
 * 
 * SQL Server Express (local only)
 * 
 * Command Line Tool    : sqlcmd
 * Connect String       : Driver=SQL Server;Server=localhost\SQLEXPRESS;Database=master;Trusted_Connection=True;
 * 
 * \author pharv (2/16/2025)
 * 
 * \param pParent 
 */
DATAWConnectStringPage::DATAWConnectStringPage( QWidget *pParent )
    : QWizardPage( pParent )
{
    setTitle( tr("Use Free Form Connection String") );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        pConnectString = new QLineEdit( this );
        pConnectString->setToolTip( tr("connect string\nprobably want to start with DRIVER=YourDriver;") );
        pParse = new QToolButton( this );
        pParse->setText( tr("Parse") );

        QGridLayout *pLayout = new QGridLayout();
        pLayout->addWidget( new QLabel( tr("Connect String"), this ), 0, 0 );
        pLayout->addWidget( pConnectString, 0, 1 );
        pLayout->addWidget( pParse, 0, 3 );
        pLayout->addWidget( pConnectStringParsed = new QLabel( this ), 1, 1 );
        pLayoutTop->addLayout( pLayout, 10 );

        connect( pParse, SIGNAL(clicked()), SLOT(slotParse()) );
    }

    {
        QFormLayout *pLayout  = new QFormLayout();
        pLayout->addRow( tr("Tip:"), pTip = new QLabel( tr("check ODBC driver vendor documentation for connection string"), this ) );
        pLayoutTop->addLayout( pLayout );
    }

    // registerField( "string.string", pConnectString );
}

int DATAWConnectStringPage::nextId() const
{
    return DATAWConnectWizard::PageConnect;
}

QMap<QString,QString> DATAWConnectStringPage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    // parse - super rudimentary at the moment
    QStringList listStrings = pConnectString->text().split( ";", Qt::SkipEmptyParts );
    QString stringPair;
    foreach( stringPair, listStrings )
    {
        QStringList listPair = stringPair.split( "=", Qt::KeepEmptyParts );
        if ( listPair.count() != 2 )
        {
            printf( "[%s][%s][%d] Not a KeyValue pair [%s]. Ignoring.\n", __FILE__, __FUNCTION__, __LINE__, stringPair.toUtf8().constData() );
            continue;
        }
        QString stringKey = listPair.at( 0 ).simplified();
        QString stringValue = listPair.at( 1 ).simplified();
        if ( stringKey.contains( " " ) )
        {
            printf( "[%s][%s][%d] Key can not contain a space [%s]. Ignoring.\n", __FILE__, __FUNCTION__, __LINE__, stringKey.toUtf8().constData() );
            continue;
        }
        if ( mapKeyValues.contains( stringKey ) )
        {
            printf( "[%s][%s][%d] Duplicate Key [%s]. Ignoring.\n", __FILE__, __FUNCTION__, __LINE__, stringKey.toUtf8().constData() );
            continue;
        }
        mapKeyValues.insert( stringKey, stringValue );
    }

    return mapKeyValues;
}

void DATAWConnectStringPage::slotParse()
{
    QString stringParsed;
    QMap<QString,QString> mapKeyValues = getKeyValues();
    QMapIterator<QString,QString> i(mapKeyValues);
    while ( i.hasNext() ) 
    {
        i.next();
        stringParsed += i.key() + "=" + i.value() + ";";
    }
    pConnectStringParsed->setText( stringParsed );
}

//
// DATAWConnectDriverPage
//
DATAWConnectDriverPage::DATAWConnectDriverPage( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Use SQLDriverConnect") );

    this->pConnection = pConnection;

    Q_ASSERT( pConnection );
    Q_ASSERT( pConnection->parent() );

    DATAEnvironment *pEnvironment = (DATAEnvironment*)pConnection->parent();

    pLayoutTop = new QVBoxLayout( this );

    {
        QHBoxLayout *p = new QHBoxLayout();
        pDrivers = new QComboBox( this );
        pDrivers->addItems( pEnvironment->getDrivers() );
        pDrivers->addItem( "generic" );
        pDrivers->setCurrentText( "generic" );
        p->addWidget( new QLabel( tr("Driver:"), this ) );
        p->addWidget( pDrivers );
        p->addStretch( 10 );
        pLayoutTop->addLayout( p );

        connect( pDrivers, SIGNAL(currentIndexChanged(int)), SLOT(slotDriver(int)) );
    }

    pDriverWidget = new DATAWConnectGenericWidget( this );
    pLayoutTop->addWidget( pDriverWidget );
}

int DATAWConnectDriverPage::nextId() const
{
    return DATAWConnectWizard::PageConnect;
}

QMap<QString,QString> DATAWConnectDriverPage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    if ( pDriverWidget ) mapKeyValues = pDriverWidget->getKeyValues();
    mapKeyValues.insert( "DRIVER", pDrivers->currentText() );

    return mapKeyValues;
}

void DATAWConnectDriverPage::slotDriver( int nIndex )
{
    // remove any existing edit widget
    if ( pDriverWidget )
    {
        delete pDriverWidget;
        pDriverWidget = nullptr;
    }

    // set edit widget appropriate for driver
    QString s = pDrivers->itemText( nIndex );
    if ( s.contains( "SQLite", Qt::CaseInsensitive ) )
        pDriverWidget = new DATAWConnectSQLite3Widget( this );
    else if ( s.contains( "MariaDB", Qt::CaseInsensitive ) )
        pDriverWidget = new DATAWConnectMariaWidget( this );
    else if ( s.contains( "PostgreSQL", Qt::CaseInsensitive ) )
        pDriverWidget = new DATAWConnectPSQLWidget( this );
    else
        pDriverWidget = new DATAWConnectGenericWidget( this );
    pLayoutTop->addWidget( pDriverWidget );
}

//
// DATAWConnectBrowsePage
//
DATAWConnectBrowsePage::DATAWConnectBrowsePage( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Use SQLBrowseConnect") );

    this->pConnection = pConnection;
}

int DATAWConnectBrowsePage::nextId() const
{
    return DATAWConnectWizard::PageConnect;
}

QMap<QString,QString> DATAWConnectBrowsePage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    return mapKeyValues;
}

//
// DATAWConnectDataSourceNamePage
//
DATAWConnectDataSourceNamePage::DATAWConnectDataSourceNamePage( DATASystem::ConfigModes nDataSourceType, DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Use SQLConnect With DSN") );

    this->pConnection = pConnection;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        QFormLayout *pLayout = new QFormLayout();

        pDataSourceNames    = new QComboBox( this );
        {
            bool bSystem    = nDataSourceType == DATASystem::System || nDataSourceType == DATASystem::Both;
            bool bUser      = nDataSourceType == DATASystem::User || nDataSourceType == DATASystem::Both;
            pDataSourceNames->addItems( pConnection->getEnvironment()->getDataSources( bUser, bSystem ) );
        }
        pUserName           = new QLineEdit( tr("?"), this );
        pAuthentication     = new QLineEdit( tr("??"), this );

        pUserName->setToolTip( tr("A UserName/UserID/UID. Leave '?' to prompt. Set blank if not required (ie SQLite).") );
        pAuthentication->setToolTip( tr("Often a password. Leave '??' to prompt. Set blank if not required (ie SQLite).") );

        pLayout->addRow( tr("DSN"), pDataSourceNames );
        pLayout->addRow( tr("UserName"), pUserName );
        pLayout->addRow( tr("Authentication"), pAuthentication );
        pLayoutTop->addLayout( pLayout, 10 );
    }
    {
        QFormLayout *pLayout = new QFormLayout();
        pLayout->addRow( tr("Tip:"), new QLabel( tr("keep passwords secure - leave prompt value ('??')"), this ) );
        pLayoutTop->addLayout( pLayout );
    }
}

int DATAWConnectDataSourceNamePage::nextId() const
{
    return DATAWConnectWizard::PageConnect;
}

QMap<QString,QString> DATAWConnectDataSourceNamePage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    if ( pDataSourceNames->currentText().isEmpty() ) return mapKeyValues;
    mapKeyValues.insert( "DSN", pDataSourceNames->currentText() );
    if ( !pUserName->text().simplified().isEmpty() ) mapKeyValues.insert( "UserName", pUserName->text().simplified() );
    if ( !pAuthentication->text().simplified().isEmpty() ) mapKeyValues.insert( "Authentication", pAuthentication->text().simplified() );

    return mapKeyValues;
}

//
// DATAWConnectFilePage
//
DATAWConnectFilePage::DATAWConnectFilePage( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    setTitle( tr("Use SQLConnect With File DSN") );

    this->pConnection = pConnection;



    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        QFormLayout *pLayout = new QFormLayout();
#ifdef Q_OS_WIN
        // this will need work when testing on WIN
        pFileSelect = new WFileSelectWidget( "/Program Files/Common Files/ODBC/Data Sources folder", "Data Source (*.dsn)", false, this );
#else
        // does not work - need to look into unixODBC code and see what it does
        pFileSelect = new WFileSelectWidget( "/etc/ODBCDataSources", "Data Source (*.dsn)", false, this );
#endif
        pUserName           = new QLineEdit( tr("?"), this );
        pAuthentication     = new QLineEdit( tr("??"), this );

        pUserName->setToolTip( tr("A UserName/UserID/UID. Leave '?' to prompt. Set blank if not required (ie SQLite).") );
        pAuthentication->setToolTip( tr("Often a password. Leave '??' to prompt. Set blank if not required (ie SQLite).") );

        pLayout->addRow( tr("DSN"), pFileSelect );
        pLayout->addRow( tr("UserName"), pUserName );
        pLayout->addRow( tr("Authentication"), pAuthentication );
        pLayoutTop->addLayout( pLayout, 10 );
    }
    {
        QFormLayout *pLayout = new QFormLayout();
        pLayout->addRow( tr("Tip:"), new QLabel( tr("Keep passwords secure - leave prompt value ('??') or set blank to ignore."), this ) );
        pLayout->addRow( tr(""), new QLabel( tr("File DSN's are simple text files which can be created with text editor."), this ) );
        pLayoutTop->addLayout( pLayout );
    }
}

int DATAWConnectFilePage::nextId() const
{
    return DATAWConnectWizard::PageConnect;
}

QMap<QString,QString> DATAWConnectFilePage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    // QString stringValue = "[" + pFileSelect->getFileName() + "]";
    QString stringValue = "{" + pFileSelect->getFileName() + "}";

    mapKeyValues.insert( "FILEDSN", stringValue );
    if ( !pUserName->text().simplified().isEmpty() ) mapKeyValues.insert( "UserName", pUserName->text().simplified() );
    if ( !pAuthentication->text().simplified().isEmpty() ) mapKeyValues.insert( "Authentication", pAuthentication->text().simplified() );

    return mapKeyValues;
}

//
// DATAWConnectPage
//
DATAWConnectPage::DATAWConnectPage( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizardPage( pWidgetParent )
{
    this->pConnection = pConnection;

    QVBoxLayout *pLayoutTop = new QVBoxLayout;

    // form - see initializePage()
    pForm = new QFormLayout;
    pLayoutTop->addLayout( pForm );

    // diagnostics
    {
        // output - in case the app output dock is not shown
        pDiagnostics = new DATAOutputDiagnosticsWidget( this );
        pLayoutTop->addWidget( pDiagnostics );
        connect( pConnection, SIGNAL(signalMessage(DATAMessage*)), pDiagnostics, SLOT(slotMessage(DATAMessage*)) );
        connect( pConnection, SIGNAL(signalDiagnostic(DATADiagnostic*)), pDiagnostics, SLOT(slotDiagnostic(DATADiagnostic*)) );
    }

    // buttons
    {
        QHBoxLayout *pLayoutButtons = new QHBoxLayout();
        pButtonConnect     = new QPushButton( tr("Connect"), this );
        pButtonDisconnect  = new QPushButton( tr("Disconnect"), this );
        pLayoutButtons->addStretch( 10 );
        pLayoutButtons->addWidget( pButtonConnect );
        pLayoutButtons->addWidget( pButtonDisconnect );
        pLayoutTop->addLayout( pLayoutButtons );

        doSyncControls();

        connect( pButtonConnect, SIGNAL(clicked()), this, SLOT(slotConnect()) );
        connect( pButtonDisconnect, SIGNAL(clicked()), this, SLOT(slotDisconnect()) );
    }

    setLayout( pLayoutTop );
}

QMap<QString,QString> DATAWConnectPage::getKeyValues()
{
    QMap<QString,QString> mapKeyValues;

    int nMethod = field( "method.methods" ).toInt();
    switch ( nMethod )
    {
    case DATAWConnectMethodPage::MethodString:
        mapKeyValues = ((DATAWConnectStringPage*)wizard()->page( DATAWConnectWizard::PageString ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodDriver:
        mapKeyValues = ((DATAWConnectDriverPage*)wizard()->page( DATAWConnectWizard::PageDriver ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodBrowse:
        mapKeyValues = ((DATAWConnectBrowsePage*)wizard()->page( DATAWConnectWizard::PageBrowse ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodDataSourceNameBoth:
        mapKeyValues = ((DATAWConnectDataSourceNamePage*)wizard()->page( DATAWConnectWizard::PageDataSourceNameBoth ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodDataSourceNameSystem:
        mapKeyValues = ((DATAWConnectDataSourceNamePage*)wizard()->page( DATAWConnectWizard::PageDataSourceNameSystem ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodDataSourceNameUser:
        mapKeyValues = ((DATAWConnectDataSourceNamePage*)wizard()->page( DATAWConnectWizard::PageDataSourceNameUser ))->getKeyValues();
        break;
    case DATAWConnectMethodPage::MethodDataSourceNameFile:
        mapKeyValues = ((DATAWConnectDataSourceNamePage*)wizard()->page( DATAWConnectWizard::PageDataSourceNameFile ))->getKeyValues();
        break;
    }

    return mapKeyValues;
}

void DATAWConnectPage::initializePage()
{
    QMap<QString,QString> mapKeyValues = getKeyValues();
    QMapIterator<QString,QString> i(mapKeyValues);
    while ( i.hasNext() ) 
    {
        i.next();
        if ( i.value() == "?" )
        {
            QLineEdit *p = new QLineEdit( this );
            mapKeyEditors.insert( i.key(), p );
            pForm->addRow( i.key(), p );
        }
        else if ( i.value() == "??" )
        {
            WPasswordWidget *p = new WPasswordWidget( "", this );
            mapKeyEditors.insert( i.key(), p );
            pForm->addRow( i.key(), p );
        }
    }
}

void DATAWConnectPage::cleanupPage()
{
    QMapIterator<QString,QWidget*> i(mapKeyEditors);
    while ( i.hasNext() ) 
    {
        i.next();
        pForm->removeRow( i.value() );
        // seems like layout has taken ownership and removeRow deletes it for us
        // delete i.value();
    }
    mapKeyEditors.clear();
}

void DATAWConnectPage::doSyncControls()
{
    if ( pConnection->isConnected() )
    {
        setWindowIcon( QIcon( ":W/Connected16x16" ) );
        pButtonConnect->setEnabled( false );
        pButtonDisconnect->setEnabled( true );
    }
    else
    {
        setWindowIcon( QIcon( ":W/Disconnected16x16" ) );
        pButtonConnect->setEnabled( true );
        pButtonDisconnect->setEnabled( false );
    }
}

void DATAWConnectPage::slotConnect()
{
    if ( pConnection->isConnected() ) return;
    pDiagnostics->slotClear();
    
    // replace prompt values ('?') with values in mapKeyEditors
    QMap<QString,QString> mapKeyValues = getKeyValues();
    QMapIterator<QString,QString> i(mapKeyValues);
    while ( i.hasNext() ) 
    {
        i.next();
        if ( i.value() != "?" && i.value() != "??" ) continue;
        if ( !mapKeyEditors.contains( i.key() ) )
        {
            printf( "[%s][%s][%d] Editor for [%s] not found.\n", __FILE__, __FUNCTION__, __LINE__, i.key().toUtf8().constData() );
            continue;
        }

        QWidget *p = mapKeyEditors[i.key()];
        if ( p->inherits( "QLineEdit" ) )
        {
            mapKeyValues[i.key()] = ((QLineEdit*)p)->text();
        }
        else if ( p->inherits( "WPasswordWidget" ) )
        {
            mapKeyValues[i.key()] = ((WPasswordWidget*)p)->getPassword();
        }
        else
            printf( "[%s][%s][%d] Unknown editor.\n", __FILE__, __FUNCTION__, __LINE__ );
    }

    // do it
    if ( mapKeyValues.count() == 3 && mapKeyValues.contains( "DSN" ) && mapKeyValues.contains( "UserName" ) && mapKeyValues.contains( "Authentication" ) )
        pConnection->doConnect( mapKeyValues.value( "DSN" ), mapKeyValues.value( "UserName" ), mapKeyValues.value( "Authentication" ) );
    else
        pConnection->doDriverConnect( nullptr, mapKeyValues );

    doSyncControls();

    if ( pConnection->isConnected() )
    {
        DATAMessage Message = DATAMessage( DATAMessage::Success, tr("Connect"), tr("Connected") );
        pDiagnostics->slotMessage( &Message );
    }
    else
    {
        DATAMessage Message = DATAMessage( DATAMessage::Error, tr("Connect"), tr("Not Connected") );
        pDiagnostics->slotMessage( &Message );
    }
}

void DATAWConnectPage::slotDisconnect()
{
    if ( !pConnection->isConnected() ) return;
    pDiagnostics->slotClear(); 
    pConnection->doDisconnect();
    doSyncControls();

    if ( pConnection->isConnected() )
    {
        DATAMessage Message = DATAMessage( DATAMessage::Error, tr("Diconnect"), tr("Connected") );
        pDiagnostics->slotMessage( &Message );
    }
    else
    {
        DATAMessage Message = DATAMessage( DATAMessage::Success, tr("Disconnect"), tr("Not Connected") );
        pDiagnostics->slotMessage( &Message );
    }
}

/*!
 * \brief DATAWConnectWizard
 * 
 * 
 * 
 * \author pharv (2/16/2025)
 * 
 * \param pConnection   
 * \param pWidgetParent 
 */
DATAWConnectWizard::DATAWConnectWizard( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWizard( pWidgetParent )
{
    this->pConnection = pConnection;

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( "DATAWConnectWizard" );
    setWindowIcon( QIcon( ":W/Disconnected16x16" ) );
    setWindowTitle( "Build a reusable connect string..." );

    setPage( PageIntro, new DATAWConnectIntroPage( this ) );
    setPage( PageMethod, new DATAWConnectMethodPage( this ) );
    setPage( PageString, new DATAWConnectStringPage( this ) );
    setPage( PageDriver, new DATAWConnectDriverPage( pConnection, this ) );
    setPage( PageBrowse, new DATAWConnectBrowsePage( pConnection, this ) );
    setPage( PageDataSourceNameBoth, new DATAWConnectDataSourceNamePage( DATASystem::Both, pConnection, this ) );
    setPage( PageDataSourceNameSystem, new DATAWConnectDataSourceNamePage( DATASystem::System, pConnection, this ) );
    setPage( PageDataSourceNameUser, new DATAWConnectDataSourceNamePage( DATASystem::User, pConnection, this ) );
    setPage( PageDataSourceNameFile, new DATAWConnectFilePage( pConnection, this ) );
    setPage( PageConnect, pConnectPage = new DATAWConnectPage( pConnection, this ) );

    connect( this, SIGNAL(finished(int)), this, SLOT(slotFinished(int)) );

    doLoadState();
}

void DATAWConnectWizard::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;
    restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void DATAWConnectWizard::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );
}

void DATAWConnectWizard::slotFinished( int )
{
    doSaveState();
}


