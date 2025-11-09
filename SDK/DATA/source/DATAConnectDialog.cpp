#include "LibInfo.h"
#include "DATAConnectDialog.h"

// #include "DATAProfile.h"
#include "DATAConnection.h"
#include "DATAOutputDiagnosticsWidget.h"

DATADataSourcesComboBox::DATADataSourcesComboBox( DATAConnection *pConnection, QWidget *pParent )
    : QWidget( pParent )
{
    this->pConnection = pConnection;

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pDataSources = new QComboBox( this );
    pDataSources->addItems( pConnection->getEnvironment()->getDataSources( true, true ) );
    pDataSources->setCurrentText( pConnection->getDataSourceName() );
    pLayout->addWidget( pDataSources );

    pRefresh     = new QToolButton( this );
    pRefresh->setIcon( QIcon( ":DATA/Refresh" ) );
    pLayout->addWidget( pRefresh );
    connect( pRefresh, SIGNAL(clicked()), SLOT(slotRefresh()) );

    pManageDataSources     = new QToolButton( this );
    pManageDataSources->setIcon( QIcon( ":DATA/Properties" ) );
    pLayout->addWidget( pManageDataSources );
    connect( pManageDataSources, SIGNAL(clicked()), SLOT(slotManageDataSources()) );

    pLayout->addStretch( 10 );
}

void DATADataSourcesComboBox::slotManageDataSources()
{
    pConnection->getEnvironment()->getSystem()->doManageDataSources( this );
    slotRefresh();
}

void DATADataSourcesComboBox::slotRefresh()
{
    pDataSources->clear();
    pDataSources->addItems( pConnection->getEnvironment()->getSystem()->getDataSourceNames() );
}

//
//
//
//
//
//

DATAConnectWidget::DATAConnectWidget( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    this->pConnection = pConnection;
   
    pDataSourceNames    = nullptr;
    pAlias              = nullptr;
    pUser               = nullptr;
    pPassword           = nullptr;
       
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );       
    QFormLayout *pLayout    = new QFormLayout();   
    if ( pConnection->getPromptDataSourceName() )
    { 
        pDataSourceNames        = new DATADataSourcesComboBox( pConnection, this );
        // DATADataSourcesComboBox will set current
        pLayout->addRow( tr("Data Source"), pDataSourceNames );
    }
    if ( pConnection->getPromptAlias() )
    { 
        pAlias                   = new QLineEdit( this );
        pAlias->setText( pConnection->getAlias() );
        pLayout->addRow( tr("Alias"), pAlias );
    }
    if ( pConnection->getPromptUserID() )
    { 
        pUser                   = new QLineEdit( this );
        pUser->setText( pConnection->getUserID() );
        pLayout->addRow( tr("User"), pUser );
    }
    if ( pConnection->getPromptPassword() )
    { 
        pPassword               = new QLineEdit( this );
        pPassword->setEchoMode( QLineEdit::Password );
        pLayout->addRow( tr("Password"), pPassword );
    }

    pLayoutTop->addLayout( pLayout );

    pDiagnostics = new DATAOutputDiagnosticsWidget( this );
    pLayoutTop->addWidget( pDiagnostics );

    connect( pConnection, SIGNAL(signalMessage(DATAMessage*)), pDiagnostics, SLOT(slotMessage(DATAMessage*)) );
    connect( pConnection, SIGNAL(signalDiagnostic(DATADiagnostic*)), pDiagnostics, SLOT(slotDiagnostic(DATADiagnostic*)) );
}

void DATAConnectWidget::slotManageDataSources()
{

}

QString DATAConnectWidget::getDataSource() 
{ 
    if ( pDataSourceNames ) return pDataSourceNames->getDataSource();
    return pConnection->getDataSourceName();
}

QString DATAConnectWidget::getAlias()       
{ 
    if ( pAlias ) return pAlias->text(); 
    return pConnection->getAlias();
}

QString DATAConnectWidget::getUser()       
{ 
    if ( pUser ) return pUser->text(); 
    return pConnection->getUserID();
}

QString DATAConnectWidget::getPassword()   
{ 
    if ( pPassword ) return pPassword->text(); 
    return QString();
}

void DATAConnectWidget::doClearDiagnostics() 
{ 
    pDiagnostics->slotClear(); 
}

//
//
//
//
//
//

DATAConnectDialog::DATAConnectDialog( DATAConnection *pConnection, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    this->pConnection = pConnection;

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( "DATAConnectDialog" );
    setWindowIcon( QPixmap( ":AW/Properties" ) );
    setWindowTitle( "Connect..." );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    pWidget = new DATAConnectWidget( pConnection, this );
    playoutTop->addWidget( pWidget );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( this );
    QPushButton *pButtonConnect = new QPushButton( tr("Connect") );
    playoutTop->addWidget( pButtonBox );
    pButtonBox->addButton( pButtonConnect, QDialogButtonBox::ApplyRole );
    pButtonBox->addButton( QDialogButtonBox::Cancel );

    connect( pButtonConnect, SIGNAL(clicked()), this, SLOT(slotConnect()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    setLayout( playoutTop );

    doLoadState();
}

void DATAConnectDialog::doLoadState()
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

void DATAConnectDialog::doSaveState()
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

void DATAConnectDialog::slotConnect()
{
    pWidget->doClearDiagnostics();
    pConnection->doConnect( pWidget->getDataSource(), pWidget->getUser(), pWidget->getPassword() );
    if ( !pConnection->isConnected() ) 
    {
        // setfocus on first field in play
        return;
    }

    doSaveState();
    done( QDialog::Accepted );
}

void DATAConnectDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}


