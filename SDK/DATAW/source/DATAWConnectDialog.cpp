#include "LibInfo.h"
#include "DATAWConnectDialog.h"

#include <WPasswordWidget.h>

#include "DATAConnection.h"
#include "DATAOutputDiagnosticsWidget.h"

//
// DATAWConnectKeyValueWidget
//
DATAWConnectKeyValueWidget::DATAWConnectKeyValueWidget( const QMap<QString,QString> &mapKeyValues, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    this->mapKeyValues = mapKeyValues;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // build a form
    {
        QFormLayout *pLayout = new QFormLayout;
        QMapIterator<QString,QString> i(mapKeyValues);
        while ( i.hasNext() ) 
        {
            i.next();
            if ( i.value() == "?" )
            {
                QLineEdit *p = new QLineEdit( this );
                mapKeyEditors[i.key()] = p;
                pLayout->addRow( new QLabel( i.key(), this ), p );
            }
            else if ( i.value() == "??" )
            {
                WPasswordWidget *p = new WPasswordWidget( "", this );
                mapKeyEditors[i.key()] = p;
                pLayout->addRow( new QLabel( i.key(), this ), p );
            }
        }
        pLayoutTop->addLayout( pLayout );
    }
}

QMap<QString,QString> DATAWConnectKeyValueWidget::getKeyValues()
{
    // update our keyvalue list with widget values and return
    QMapIterator<QString,QWidget*> i(mapKeyEditors);
    while ( i.hasNext() ) 
    {
        i.next();
        QWidget *p = i.value();
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
    return mapKeyValues;
}

//
// DATAWConnectDialog
//
DATAWConnectDialog::DATAWConnectDialog( DATAConnection *pConnection, const QMap<QString,QString> &mapKeyValues, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    this->pConnection = pConnection;

    QString stringDataSourceName;
    if ( mapKeyValues.contains("DSN") ) stringDataSourceName = mapKeyValues.value( "DSN" );
    else if ( mapKeyValues.contains("DRIVER") ) stringDataSourceName = mapKeyValues.value( "DRIVER" );
    else if ( mapKeyValues.contains("FILEDSN") ) stringDataSourceName = mapKeyValues.value( "FILEDSN" );
    if ( stringDataSourceName.isEmpty() ) stringDataSourceName = tr("unknown");

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( "DATAWConnectDialog" );
    setWindowIcon( QIcon( ":W/Disconnected16x16" ) );
    setWindowTitle( "Connect: " + stringDataSourceName );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // main widget
    {
        pKeyValueWidget = new DATAWConnectKeyValueWidget( mapKeyValues, this );
        pLayoutTop->addWidget( pKeyValueWidget );
    }

    // diagnostics
    {
        // output - in case the app output dock is not shown
        pDiagnostics = new DATAOutputDiagnosticsWidget( this );
        pLayoutTop->addWidget( pDiagnostics );
        connect( pConnection, SIGNAL(signalMessage(DATAMessage*)), pDiagnostics, SLOT(slotMessage(DATAMessage*)) );
        connect( pConnection, SIGNAL(signalDiagnostic(DATADiagnostic*)), pDiagnostics, SLOT(slotDiagnostic(DATADiagnostic*)) );

        pLayoutTop->addWidget( new QLabel( tr("NOTE: Connecting with warnings or even errors may be unavoidable in detecting supported features."), this ) );
    }

    // buttons
    {
        QDialogButtonBox *pButtonBox = new QDialogButtonBox( this );
        pConnect = new QPushButton( QIcon( ":W/Disconnected16x16" ), "Connect", this );
        pButtonBox->addButton( pConnect, QDialogButtonBox::ApplyRole );
        pButtonBox->addButton( QDialogButtonBox::Close );

        pLayoutTop->addWidget( pButtonBox );
        connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
        connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
        connect( pConnect, SIGNAL(clicked()), this, SLOT(slotConnect()) );
    }

    connect( this, SIGNAL(finished(int)), this, SLOT(slotFinished(int)) );
    doLoadState();
}

void DATAWConnectDialog::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;
    restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void DATAWConnectDialog::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );
}

void DATAWConnectDialog::slotConnect()
{
    if ( pConnection->isConnected() ) return;
    pDiagnostics->slotClear();
    
    // get KeyValues with prompted values (ie UID, PWD etc)
    QMap<QString,QString> mapKeyValues = pKeyValueWidget->getKeyValues();

    // do it
    if ( mapKeyValues.count() == 3 && mapKeyValues.contains( "DSN" ) && mapKeyValues.contains( "UserName" ) && mapKeyValues.contains( "Authentication" ) )
        pConnection->doConnect( mapKeyValues.value( "DSN" ), mapKeyValues.value( "UserName" ), mapKeyValues.value( "Authentication" ) );
    else
        pConnection->doDriverConnect( nullptr, mapKeyValues );

    if ( pConnection->isConnected() )
    {
        DATAMessage Message = DATAMessage( DATAMessage::Success, tr("Connect"), tr("Connected") );
        pDiagnostics->slotMessage( &Message );
        pConnect->setIcon( QIcon( ":W/Connected16x16" ) );
        pConnect->setEnabled( false );
    }
    else
    {
        DATAMessage Message = DATAMessage( DATAMessage::Error, tr("Connect"), tr("Not Connected") );
        pDiagnostics->slotMessage( &Message );
    }
}

void DATAWConnectDialog::slotFinished( int )
{
    doSaveState();
}


