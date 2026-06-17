#include "LibInfo.h"
#include "DATAWQueryDriverConnection.h"

#include <DATAConnection.h>
// #include <DATAStatement.h>

DATAWQueryDriverConnection::DATAWQueryDriverConnection( ADObject *pParent, DATAEnvironment *pEnvironment, const QString &stringDriverName )
    : DATAWQueryConnection( pParent, pEnvironment )
{
    setObjectName( "DATAWQueryDriverConnection" );

    this->stringDriverName = stringDriverName;
}

DATAWQueryDriverConnection::~DATAWQueryDriverConnection()
{
}

void DATAWQueryDriverConnection::doConnect()
{
    if ( isConnected() ) return;

    DATAWDriverConnectDialog dialog( pConnection, stringDriverName, getEditorWidgetParent() );
    dialog.exec();
}

//
// DATAWDriverConnectWidget
//
DATAWDriverConnectWidget::DATAWDriverConnectWidget( QWidget *pParent )
    : QWidget( pParent )
{
}

//
// DATAWDriverOtherWidget
//
DATAWDriverOtherWidget::DATAWDriverOtherWidget( QWidget *pParent )
    : DATAWDriverConnectWidget( pParent )
{
    pConnectString = new QLineEdit( this );
    pConnectString->setToolTip( tr("connect string\nNOTE: do not include the DRIVER attribute.") );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Connect String"), pConnectString );
}

QString DATAWDriverOtherWidget::getConnectString()
{
    return pConnectString->text();
}

//
// DATAWDriverSQLiteWidget
//
DATAWDriverSQLiteWidget::DATAWDriverSQLiteWidget( QWidget *pParent )
    : DATAWDriverConnectWidget( pParent )
{
    pFile = new QLineEdit( this );

    pFile->setToolTip( tr("database file") );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Database"), pFile );
}

QString DATAWDriverSQLiteWidget::getConnectString()
{
    return pFile->text();
}

void DATAWDriverSQLiteWidget::slotSelectFile()
{
}

//
// DATAWDriverMariaWidget
//
DATAWDriverMariaWidget::DATAWDriverMariaWidget( QWidget *pParent )
    : DATAWDriverConnectWidget( pParent )
{
    pServer = new QLineEdit( this );
    pUser = new QLineEdit( this );
    pPassword = new QLineEdit( this );
    pShowPassword = new QCheckBox( this );
    pDatabase = new QLineEdit( this );

    pServer->setToolTip( tr("server/hostname\nenter a hostname or IP\nblank=localhost\nsee MariaDB documentation") );
    pUser->setToolTip( tr("User name to authenticate with") );
    pPassword->setToolTip( tr("Password to authenticate with") );
    pShowPassword->setToolTip( tr("toggle masking of Password") );
    pDatabase->setToolTip( tr("enter a database to make it current after connection\nleaving this blank means no database will be current after connection") );

    pPassword->setEchoMode( QLineEdit::Password );
    pShowPassword->setChecked( false );

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pShowPassword, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotShowPassword(Qt::CheckState)) );
#else
    connect( pShowPassword, SIGNAL(stateChanged(int)), SLOT(slotShowPassword(int)) );
#endif
    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Server"), pServer );
    pLayout->addRow( tr("User"), pUser );
    pLayout->addRow( tr("Password"), pPassword );
    pLayout->addRow( tr("Show Password"), pShowPassword );
    pLayout->addRow( tr("Database"), pDatabase );
}

QString DATAWDriverMariaWidget::getConnectString()
{
    QString stringConnectString;

    if ( !pServer->text().isEmpty() ) stringConnectString = "HOST=" + pServer->text() + ";";
    if ( !pUser->text().isEmpty() ) stringConnectString = "USER=" + pUser->text() + ";";
    if ( !pPassword->text().isEmpty() ) stringConnectString = "PASSWORD=" + pPassword->text() + ";";
    if ( !pDatabase->text().isEmpty() ) stringConnectString = "DATABASE=" + pDatabase->text() + ";";

    return stringConnectString;
}

void DATAWDriverMariaWidget::slotShowPassword( Qt::CheckState n )
{
    if ( n == Qt::Checked )
        pPassword->setEchoMode( QLineEdit::Normal );
    else
        pPassword->setEchoMode( QLineEdit::Password );
}

//
// DATAWDriverConnectDialog
//
DATAWDriverConnectDialog::DATAWDriverConnectDialog( DATAConnection *p, const QString &s, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    Q_ASSERT(p);
printf( "[%s][%s][%d] %s\n", __FILE__, __FUNCTION__, __LINE__, s.toUtf8().constData() );

    pConnection = p;
    stringDriver = s;

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( "DATAWDriverConnectDialog" );
    setWindowIcon( QIcon( ":W/Disconnected16x16" ) );
    setWindowTitle( "Connect using driver: " + stringDriver + "..." );

    QVBoxLayout *pLayoutTop = new QVBoxLayout;
    // top bit (widget)
    if ( stringDriver == "SQLITE3" )
    {
        pWidget = new DATAWDriverSQLiteWidget( this );
        pLayoutTop->addWidget( pWidget );
    }
    else if ( stringDriver == "MariaDB" )
    {
        pWidget = new DATAWDriverMariaWidget( this );
        pLayoutTop->addWidget( pWidget );
    }
    else
    {
        pWidget = new DATAWDriverOtherWidget( this );
        pLayoutTop->addWidget( pWidget );
    }
    // diagnostics
    {
        // output - in case the app output dock is not shown
        pDiagnostics = new DATAOutputDiagnosticsWidget( this );
        pLayoutTop->addWidget( pDiagnostics );
    }

    connect( pConnection, SIGNAL(signalMessage(DATAMessage*)), pDiagnostics, SLOT(slotMessage(DATAMessage*)) );
    connect( pConnection, SIGNAL(signalDiagnostic(DATADiagnostic*)), pDiagnostics, SLOT(slotDiagnostic(DATADiagnostic*)) );
        
    // bottom bit (button)
    {
        QDialogButtonBox *pButtonBox    = new QDialogButtonBox( this );
        pButtonConnect     = new QPushButton( tr("Connect") );
        pButtonDisconnect  = new QPushButton( tr("Disconnect") );
        QPushButton *pButtonDone        = new QPushButton( tr("Done") );
        pButtonBox->addButton( pButtonConnect, QDialogButtonBox::NoRole );
        pButtonBox->addButton( pButtonDisconnect, QDialogButtonBox::NoRole );
        pButtonBox->addButton( pButtonDone, QDialogButtonBox::NoRole );

        doSyncControls();

        connect( pButtonConnect, SIGNAL(clicked()), this, SLOT(slotConnect()) );
        connect( pButtonDisconnect, SIGNAL(clicked()), this, SLOT(slotDisconnect()) );
        connect( pButtonDone, SIGNAL(clicked()), this, SLOT(slotDone()) );

        pLayoutTop->addWidget( pButtonBox );
    }

    setLayout( pLayoutTop );

    doLoadState();
}

void DATAWDriverConnectDialog::doSyncControls()
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

void DATAWDriverConnectDialog::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    int nWidth = settings.value( s + "/width", width() ).toInt();
    int nHeight= settings.value( s + "/height", height() ).toInt();
    resize( nWidth, nHeight );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName() + "/";
    // QSettings settings;
    // restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void DATAWDriverConnectDialog::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    settings.setValue( s + "/width", width() );
    settings.setValue( s + "/height", height() );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName() + "/";
    // QSettings settings;
    // settings.setValue( s + "geometry", saveGeometry() );
}

void DATAWDriverConnectDialog::slotConnect()
{
    if ( pConnection->isConnected() ) return;
    pDiagnostics->slotClear(); 
    pConnection->doDriverConnect( "DRIVER=" + stringDriver + ";" + pWidget->getConnectString() );
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

void DATAWDriverConnectDialog::slotDisconnect()
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

void DATAWDriverConnectDialog::slotDone()
{
    doSaveState();
    done( QDialog::Accepted );
}



