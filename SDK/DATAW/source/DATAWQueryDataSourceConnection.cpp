#include "LibInfo.h"
#include "DATAWQueryDataSourceConnection.h"

#include <DATAConnection.h>

DATAWQueryDataSourceConnection::DATAWQueryDataSourceConnection( ADObject *pParent, DATAEnvironment *pEnvironment, SQLUSMALLINT nDataSourceType, const QString &stringDataSourceName )
    : DATAWQueryConnection( pParent, pEnvironment )
{
    setObjectName( "DATAWQueryDataSourceConnection" );
    this->nDataSourceType       = nDataSourceType;
    this->stringDataSourceName  = stringDataSourceName;
}

DATAWQueryDataSourceConnection::~DATAWQueryDataSourceConnection()
{
}

void DATAWQueryDataSourceConnection::doConnect()
{
    if ( isConnected() ) return;

    DATAWDataSourceConnectDialog dialog( pConnection, stringDataSourceName, getEditorWidgetParent() );
    dialog.exec();
}

//
// DATAWDataSourceConnectWidget
//
DATAWDataSourceConnectWidget::DATAWDataSourceConnectWidget( QWidget *pParent )
    : QWidget( pParent )
{
    pUserName = new QLineEdit( this );
    pAuthentication = new QLineEdit( this );
    pShowAuthentication = new QCheckBox( this );

    pUserName->setToolTip( tr("User Name/User ID/UID\nNOTE: ignored when data source does not support it") );
    pAuthentication->setToolTip( tr("Authentication/Password/PWD or some other authentication appropriate for data source\nNOTE: ignored when data source does not support it") );
    pShowAuthentication->setToolTip( tr("toggle masking of Authentication") );

    pAuthentication->setEchoMode( QLineEdit::Password );
    pShowAuthentication->setChecked( false );

    connect( pShowAuthentication, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotShowAuthentication(Qt::CheckState)) );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("User Name"), pUserName );
    pLayout->addRow( tr("Authentication"), pAuthentication );
    pLayout->addRow( tr("Show Authentication"), pShowAuthentication );
}

void DATAWDataSourceConnectWidget::slotShowAuthentication( Qt::CheckState n )
{
    if ( n == Qt::Checked )
        pAuthentication->setEchoMode( QLineEdit::Normal );
    else
        pAuthentication->setEchoMode( QLineEdit::Password );
}

//
// DATAWDataSourceConnectDialog
//
DATAWDataSourceConnectDialog::DATAWDataSourceConnectDialog( DATAConnection *p, const QString &s, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    Q_ASSERT(p);

    pConnection = p;
    stringDataSourceName = s;

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( "DATAWDataSourceConnectDialog" );
    setWindowIcon( QIcon( ":W/Disconnected16x16" ) );
    setWindowTitle( "Connect to data source name: " + stringDataSourceName + "..." );

    QVBoxLayout *pLayoutTop = new QVBoxLayout;
    // top bit (widget)
    {
        pWidget = new DATAWDataSourceConnectWidget( this );
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

void DATAWDataSourceConnectDialog::doSyncControls()
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

void DATAWDataSourceConnectDialog::doLoadState()
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

void DATAWDataSourceConnectDialog::doSaveState()
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

void DATAWDataSourceConnectDialog::slotConnect()
{
    if ( pConnection->isConnected() ) return;
    pDiagnostics->slotClear(); 
    pConnection->doConnect( stringDataSourceName, pWidget->getUserName(), pWidget->getAuthentication() );
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

void DATAWDataSourceConnectDialog::slotDisconnect()
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

void DATAWDataSourceConnectDialog::slotDone()
{
    doSaveState();
    done( QDialog::Accepted );
}



